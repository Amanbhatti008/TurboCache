cmake_minimum_required(VERSION 3.15)
project(MimirCache CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Aggressive optimization flags
set(CMAKE_CXX_FLAGS "-Wall -O3 -march=native -DNDEBUG")

# Route the output binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(WIN32)
    add_executable(mimir_cache src/windows_server.cpp)
    target_link_libraries(mimir_cache wsock32 ws2_32)
else()
    add_executable(mimir_cache src/epoll_server.cpp)
    target_link_libraries(mimir_cache pthread)
endif()
