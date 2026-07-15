FROM alpine:latest AS builder

RUN apk add --no-cache build-base linux-headers

WORKDIR /app

# Copy the entire repository to preserve relative paths like ../../third_party
COPY . .

# Set working directory to the cpp-cache-engine folder
WORKDIR /app/cpp-cache-engine

# Build the server
RUN g++ src/epoll_server.cpp -o mimir_server -std=c++20 -Os -pthread

FROM alpine:latest

RUN apk add --no-cache libstdc++

WORKDIR /app

# Copy the built binary from the builder stage
COPY --from=builder /app/cpp-cache-engine/mimir_server .

EXPOSE 8080

CMD ["./mimir_server"]
