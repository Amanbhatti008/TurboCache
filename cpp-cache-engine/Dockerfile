FROM alpine:latest

# Install build dependencies
RUN apk add --no-cache build-base linux-headers

# Set working directory
WORKDIR /app

# Copy the source code
COPY . .

# Compile the C++ Mimir Server
# Using -D_linux_ is not strictly needed since epoll_server.cpp uses #ifndef _WIN32
RUN g++ src/epoll_server.cpp -o mimir_server -std=c++17 -O3 -pthread

# Expose port
EXPOSE 8080

# Run the server
CMD ["./mimir_server"]
