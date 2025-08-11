#!/bin/bash

# Manual Docker run script for Ubuntu 22.04 ARM64
# This matches the user's existing Docker pattern

echo "🐳 Starting Ubuntu 22.04 ARM64 Docker container..."
echo "📁 Your Mac $HOME is mapped to /data in the container"
echo "📁 Navigate to /data/GitHub/grapa to access your grapa project"

# Build the image if it doesn't exist
if ! docker image inspect grapa-ubuntu22-arm64 >/dev/null 2>&1; then
    echo "🔨 Building Docker image..."
    docker build -f Dockerfile.ubuntu22-arm64 -t grapa-ubuntu22-arm64 .
fi

# Run the container
echo "🚀 Starting container..."
docker run --platform=linux/arm64 -it --rm \
    -v $HOME:/data \
    grapa-ubuntu22-arm64

echo "✅ Container stopped" 