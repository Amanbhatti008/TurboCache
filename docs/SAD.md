# System Architecture Document (SAD)
**Project Mimir Infrastructure**

## 1. Deployment Topology
Mimir is designed to run in a clustered environment behind an API gateway or reverse proxy.
- **Node Topology:** 3 Active Nodes (e.g., US, EU, AP).
- **Reverse Proxy:** NGINX handles SSL termination, keep-alive connections, and distributes load via Round-Robin or Least-Connections to the Mimir instances.

## 2. Observability & Telemetry
SRE principles mandate strict observability. Mimir exposes two endpoints:
- `/stats`: Real-time JSON telemetry for the live HTML SRE Dashboard (RPS, Latency, Storage).
- `/metrics`: Prometheus-compatible exposition format.

## 3. Containerization
Mimir is packaged into a minimal Alpine/Ubuntu based Docker container. The `Dockerfile` multi-stage build compiles the C++ source and discards the toolchain, resulting in a binary image size under 15MB. Docker Compose manages the local Mimir node + NGINX cluster.
