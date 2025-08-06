#!/bin/bash

# Build Grapa Application and Libraries for Linux ARM64
# This script builds the Grapa executable, static library, and shared library
# Assumes third-party libraries are already available in source/*/linux-arm64/

set -e

echo "🐳 Building Grapa for Linux ARM64..."

# Build the Docker image
echo "Building Docker image..."
docker build -f Dockerfile.grapa-build -t grapa-build .

# Run the container and build Grapa
echo "Running container and building Grapa..."
docker run -it --rm \
    -v $HOME:/data \
    grapa-build \
    bash -c "
        echo '🚀 Starting Grapa build for Linux ARM64...'
        echo '📁 Working directory: $(pwd)'
        echo '📁 Grapa directory: /data/GitHub/grapa'
        
        # Navigate to grapa directory
        cd /data/GitHub/grapa
        
        # Create output directories if they don't exist
        mkdir -p source/grapa-lib/linux-arm64
        mkdir -p source/grapa-other/linux-arm64
        mkdir -p bin
        
        # Build Grapa using build.py with --bin-only to create compressed package
        echo '📦 Building Grapa Application and creating package using build.py...'
        python3 build.py --bin-only
        echo '✅ Grapa Application and package built successfully'
        
        echo '🎉 All Grapa components built successfully!'
        echo '📁 Application is in grapa'
        echo '📁 Static library is in source/grapa-lib/linux-arm64/libgrapa.a'
        echo '📁 Shared library is in source/grapa-other/linux-arm64/libgrapa.so'
        echo '📁 Compressed file is in bin/grapa-linux-arm64.tar.gz'
        
        # List built files
        echo '📋 Built files:'
        ls -la grapa
        ls -la source/grapa-lib/linux-arm64/libgrapa.a
        ls -la source/grapa-other/linux-arm64/libgrapa.so
        ls -la bin/grapa-linux-arm64.tar.gz
    "

echo "✅ Docker build completed successfully!"
echo "📁 Application is available as grapa"
echo "📁 Static library is available in source/grapa-lib/linux-arm64/"
echo "📁 Shared library is available in source/grapa-other/linux-arm64/"
echo "📁 Compressed file is available in bin/grapa-linux-arm64.tar.gz" 