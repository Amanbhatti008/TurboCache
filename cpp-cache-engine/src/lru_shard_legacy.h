#pragma once

#if defined(__INTELLISENSE__) || defined(__clang__)
#undef __SSE__
#undef __SSE2__
#define XXH_VECTOR 0
#endif

#include <string>
#include <vector>
#include <unordered_map>
#ifdef MULTI_THREAD
#include <mutex>
#endif
#include <cstddef>
#if __has_include(<string_view>)
#include <string_view>
using string_view = std::string_view;
#elif __has_include(<experimental/string_view>)
#include <experimental/string_view>
using string_view = std::experimental::string_view;
#else
#include <string>
using string_view = std::string;
#endif

#define XXH_INLINE_ALL
#if defined(__clang__) || defined(__INTELLISENSE__)
#define XXH_VECTOR 0
#endif
#include "../../third_party/xxhash.h"

namespace mimir {

struct XXHash3Hasher {
    using is_transparent = void; // Enable heterogeneous lookup if compiler supports it
    std::size_t operator()(string_view key) const noexcept {
        return XXH64(key.data(), key.size(), 0);
    }
    std::size_t operator()(const std::string& key) const noexcept {
        return XXH64(key.data(), key.size(), 0);
    }
};

class CacheShard {
public:
    CacheShard() {
        store_.reserve(10000); 
    }

    std::string get(string_view key) {
#ifdef MULTI_THREAD
        std::lock_guard<std::mutex> lock(mtx_);
#endif
        // For C++14/17 we have to convert string_view to string for find,
        // but this limits scope of copy
        auto it = store_.find(std::string(key));
        if (it != store_.end()) {
            return it->second;
        }
        return "";
    }

    void set(string_view key, string_view value) {
#ifdef MULTI_THREAD
        std::lock_guard<std::mutex> lock(mtx_);
#endif
        store_.emplace(std::string(key), std::string(value));
    }

private:
#ifdef MULTI_THREAD
    std::mutex mtx_;
#endif
    std::unordered_map<std::string, std::string, XXHash3Hasher> store_;
};

class ShardedCache {
public:
    ShardedCache(size_t num_shards = 16) : shards_(num_shards) {}

    CacheShard& get_shard(string_view key) {
        size_t hash = XXH64(key.data(), key.size(), 0);
        return shards_[hash % shards_.size()];
    }

    void set(string_view key, string_view value) {
        get_shard(key).set(key, value);
    }

    std::string get(string_view key) {
        return get_shard(key).get(key);
    }

private:
    std::vector<CacheShard> shards_;
};

} // namespace mimir
