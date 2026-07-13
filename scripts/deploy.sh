#!/bin/bash
# TurboCache - AWS EC2 Free Tier Deploy Script

set -e

echo "🚀 Starting TurboCache Deployment on EC2..."

# 1. System Update & Docker Install
sudo apt update -y
sudo apt install -y docker.io git

# 2. Start Docker & Enable Auto-Start
sudo systemctl start docker
sudo systemctl enable docker
sudo usermod -aG docker ubuntu || true

# 3. Clone/Update Repo (Replace with YOUR GitHub URL)
# NOTE: Make sure your repo is public, or use SSH keys.
git clone https://github.com/YOUR_USERNAME/mimir-cache.git /tmp/mimir || (cd /tmp/mimir && git pull)

# 4. Build Docker Image
cd /tmp/mimir/cpp-cache-engine
sudo docker build -t mimir-cache .

# 5. Run Container (Port 8080, Auto-Restart)
# Stop existing container if it's running
sudo docker rm -f mimir || true
sudo docker run -d \
  --name mimir \
  --restart always \
  -p 8080:8080 \
  mimir-cache

# 6. Health Check
sleep 2
if sudo docker ps | grep -q mimir; then
    echo "✅ Mimir is running on port 8080!"
    echo "🌍 Public IP: $(curl -s http://checkip.amazonaws.com)"
    echo "🩺 Health Check URL: http://$(curl -s http://checkip.amazonaws.com):8080/health"
else
    echo "❌ Container failed. Check logs: sudo docker logs mimir"
fi
