# Functional Architecture Document (FAD)
**Project Mimir**

## 1. System Overview
Mimir operates as a standalone TCP daemon that accepts connections, parses a custom text-based protocol, and interfaces with a sharded memory backend.

## 2. Core Components
- **Network Listener:** Binds to port 8080 and handles incoming TCP connections asynchronously.
- **Zero-Copy Parser:** Reads directly from the socket buffer using `std::string_view` to extract commands without memory allocations.
- **Command Router:** Routes `SET`, `GET`, `ASK`, `/stats`, and `/metrics` commands to their respective handlers.
- **LLM Gateway:** Intercepts `ASK` commands, checks the cache for the prompt hash, and makes asynchronous non-blocking API calls to Gemini on a cache miss.

## 3. Data Flow
1. Client sends `GET mykey\r\n`.
2. Network Listener receives bytes.
3. Zero-Copy Parser identifies `GET` and `mykey`.
4. Command Router invokes `Cache::Get("mykey")`.
5. Shard Manager hashes `mykey`, locks the specific shard, and retrieves the value.
6. Value is queued for asynchronous write back to the client socket.
