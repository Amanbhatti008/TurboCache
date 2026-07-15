#pragma once

#include <string>
#include <vector>
#include <memory_resource>
#include <unordered_map>
#include <shared_mutex>
#include <string_view>
#include <cstddef>

// Define XXH_INLINE_ALL to compile xxhash functions in this header
#define XXH_INLINE_ALL
#include "../../third_party/xxhash.h"

namespace mimir {

// C++20 Transparent Hash for zero-copy lookups
struct XXHash3Hasher {
    using is_transparent = void;

    size_t operator()(const std::pmr::string& key) const noexcept {
        return XXH3_64bits(key.data(), key.size());
    }
    size_t operator()(std::string_view key) const noexcept {
        return XXH3_64bits(key.data(), key.size());
    }
};

class CacheShard {
public:
    // Initialize the shard with a 16MB pre-allocated contiguous slab to fit in 512MB RAM
    CacheShard(size_t slab_size = 16 * 1024 * 1024) 
        : buffer_(slab_size, std::byte{0}),
          pmr_resource_(buffer_.data(), buffer_.size(), std::pmr::null_memory_resource()),
          store_(&pmr_resource_) 
    {
        // Pre-allocate buckets to avoid rehashing
        store_.reserve(100000); 
    }

    // Zero-copy get using string_view and transparent hashing
    std::string_view get(std::string_view key) {
#ifdef MULTI_THREAD
        std::shared_lock lock(mtx_);
#endif
        auto it = store_.find(key); // uses transparent hash
        if (it != store_.end()) {
            return it->second;
        }
        return "";
    }

    void set(std::string_view key, std::string_view value) {
#ifdef MULTI_THREAD
        std::unique_lock lock(mtx_);
#endif
        // Allocate both key and value from the PMR pool
        std::pmr::string p_key(key, &pmr_resource_);
        std::pmr::string p_value(value, &pmr_resource_);
        store_.insert_or_assign(std::move(p_key), std::move(p_value));
    }

private:
#ifdef MULTI_THREAD
    std::shared_mutex mtx_;
#endif
    std::vector<std::byte> buffer_;
    std::pmr::monotonic_buffer_resource pmr_resource_;
    
    // Both map nodes (buckets) and strings use the monotonic buffer resource
    std::pmr::unordered_map<
        std::pmr::string, 
        std::pmr::string, 
        XXHash3Hasher,
        std::equal_to<> // C++20 transparent equality
    > store_;
};

class ShardedCache {
public:
    ShardedCache(size_t num_shards = 8) : shards_(num_shards) {}

    CacheShard& get_shard(std::string_view key) {
        size_t hash = XXH3_64bits(key.data(), key.size());
        return shards_[hash % shards_.size()];
    }

    void set(std::string_view key, std::string_view value) {
        get_shard(key).set(key, value);
    }

    std::string_view get(std::string_view key) {
        return get_shard(key).get(key);
    }

private:
    // 16 shards pre-initialized
    std::vector<CacheShard> shards_;
};

} // namespace mimir
