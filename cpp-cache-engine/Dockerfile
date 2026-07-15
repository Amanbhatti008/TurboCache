FROM alpine:latest AS builder

RUN apk add --no-cache build-base linux-headers

WORKDIR /app

# Copy only the source directory (not the whole repo)
COPY cpp-cache-engine/ .

RUN g++ src/epoll_server.cpp -o mimir_server -std=c++17 -O3 -pthread

FROM alpine:latest

RUN apk add --no-cache libstdc++

WORKDIR /app

COPY --from=builder /app/mimir_server .

EXPOSE 8080

CMD ["./mimir_server"]
