# Technical Architecture Document (TAD)
**Project Mimir**

## 1. Concurrency Model
Mimir uses a Thread-per-Core (Share-Nothing) event loop architecture for I/O, coupled with a Thread-Safe Sharded memory map.
- Linux leverages `epoll` with `EPOLLONESHOT` and Edge Triggered (`EPOLLET`) modes.
- Windows leverages Winsock `select`/`WSAAsyncSelect` (currently utilizing non-blocking polling loops).

## 2. Memory Architecture
To combat heap fragmentation and overhead from frequent `std::string` allocations, Mimir utilizes C++17 Polymorphic Memory Resources (`std::pmr`).
- A `std::pmr::synchronized_pool_resource` sits atop a massive `std::pmr::monotonic_buffer_resource` pre-allocated at startup.
- All internal string keys and values within the cache map use `std::pmr::string`.

## 3. Sharding Strategy
Lock contention is the enemy of throughput. Mimir divides the global cache into 16 distinct, independent shards.
- `Shard ID = xxhash(key) % 16`
- Each shard contains its own `std::shared_mutex`, allowing concurrent reads across the entire dataset, and localized writes that do not block unrelated shards.
