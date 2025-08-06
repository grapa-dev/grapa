#!/bin/bash

# Build Grapa for All Platforms
# This script builds Grapa application, static library, and shared library for all 5 platforms
# Assumes third-party libraries are already available in source/*/{platform}-{arch}/

set -e

echo "🚀 Building Grapa for All Platforms..."
echo "📋 Platforms: Windows AMD64, macOS ARM64, macOS AMD64, Linux AMD64, Linux ARM64"

# Build Docker image once
echo "🐳 Building Docker image..."
docker build -f Dockerfile.grapa-build -t grapa-build .

# Function to build for a specific platform
build_platform() {
    local platform=$1
    local arch=$2
    local docker_platform=$3
    
    echo ""
    echo "🔨 Building for $platform-$arch..."
    
    docker run --platform=$docker_platform -it --rm \
        -v $HOME:/data \
        grapa-build \
        bash -c "
            echo '🚀 Starting Grapa build for $platform-$arch...'
            cd /data/GitHub/grapa
            
            # Create output directories
            mkdir -p source/grapa-lib/$platform-$arch
            mkdir -p source/grapa-other/$platform-$arch
            mkdir -p bin
            
            # Build Grapa Application and create package using build.py
            echo '📦 Building Grapa Application and creating package for $platform-$arch using build.py...'
            python3 build.py
            echo '✅ Grapa Application and package built successfully for $platform-$arch'
            
            echo '🎉 All Grapa components built successfully for $platform-$arch!'
        "
    
    echo "✅ Completed build for $platform-$arch"
}

# Build for each platform
echo ""
echo "🔨 Starting builds for all platforms..."

# Linux ARM64
build_platform "linux" "arm64" "linux/arm64"

# Linux AMD64  
build_platform "linux" "amd64" "linux/amd64"

# Note: Windows and macOS builds would need different Docker containers
# For now, we'll focus on Linux builds which can be done with the current setup
echo ""
echo "⚠️  Note: Windows and macOS builds require different Docker containers"
echo "   - Windows: Requires Windows containers with Visual Studio"
echo "   - macOS: Requires macOS containers (limited Docker support)"
echo ""
echo "✅ Linux builds completed successfully!"
echo "📁 Applications: grapa (Linux ARM64 and AMD64)"
echo "📁 Static libraries: source/grapa-lib/linux-*/libgrapa.a"
echo "📁 Shared libraries: source/grapa-other/linux-*/libgrapa.so"
echo "📁 Compressed files: bin/grapa-linux-*.tar.gz" 