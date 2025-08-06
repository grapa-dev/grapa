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
                   python3 build.py --bin-only
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

# macOS ARM64 (native build on Mac)
echo ""
echo "🍎 Building for macOS ARM64 (native)..."
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "✅ Running on macOS, building natively..."
    mkdir -p source/grapa-lib/mac-arm64
    mkdir -p source/grapa-other/mac-arm64
    mkdir -p bin
    python3 build.py --bin-only
    echo "✅ macOS ARM64 build completed"
else
    echo "⚠️  macOS ARM64 build skipped (not on macOS)"
fi

# macOS AMD64 (cross-compilation from ARM64 Mac)
echo ""
echo "🍎 Building for macOS AMD64 (cross-compilation)..."
if [[ "$OSTYPE" == "darwin"* && "$(uname -m)" == "arm64" ]]; then
    echo "✅ Running on ARM64 Mac, using cross-compilation..."
    mkdir -p source/grapa-lib/mac-amd64
    mkdir -p source/grapa-other/mac-amd64
    mkdir -p bin
    python3 build.py --bin-only --target-platform mac-amd64
    echo "✅ macOS AMD64 build completed"
else
    echo "⚠️  macOS AMD64 build skipped (requires ARM64 Mac)"
fi

# Windows AMD64 (GitHub Actions workflow)
echo ""
echo "🪟 Building for Windows AMD64..."
echo "📋 Checking if GitHub CLI is available for Windows workflow..."

if command -v gh &> /dev/null && gh auth status &> /dev/null; then
    echo "✅ GitHub CLI available and authenticated"
    echo "🚀 Triggering Windows build workflow..."
    
    # Trigger the workflow
    gh workflow run "Build Windows AMD64.yml"
    
    if [[ $? -eq 0 ]]; then
        echo "✅ Windows workflow triggered successfully!"
        echo "🔄 Monitoring workflow and downloading artifacts..."
        
        # Use the monitoring script to wait for completion and download
        ./scripts/monitor_and_download_windows.sh
        
        if [[ $? -eq 0 ]]; then
            echo "✅ Windows AMD64 build completed and artifacts downloaded"
        else
            echo "❌ Windows build failed or download failed"
            echo "   Please check the workflow at: https://github.com/grapa-dev/grapa/actions"
        fi
    else
        echo "❌ Failed to trigger Windows workflow"
        echo "   Please check GitHub CLI permissions and try again"
    fi
else
    echo "⚠️  GitHub CLI not available or not authenticated"
    echo "   Manual options:"
    echo "   1. Install GitHub CLI: https://cli.github.com/"
    echo "   2. Run: gh auth login"
    echo "   3. Or trigger manually: https://github.com/grapa-dev/grapa/actions"
    echo "   4. Then run: ./scripts/monitor_and_download_windows.sh"
fi

echo ""
echo "✅ All available builds completed successfully!"
echo "📁 Applications: grapa (Linux ARM64/AMD64, macOS ARM64/AMD64), grapa.exe (Windows AMD64)"
echo "📁 Static libraries: source/grapa-lib/*/libgrapa.a, source/grapa-lib/win-amd64/grapa.lib"
echo "📁 Shared libraries: source/grapa-other/*/libgrapa.so"
echo "📁 Compressed files: bin/grapa-*.tar.gz, bin/grapa-win-amd64.zip"
echo ""
echo "🎯 Next steps:"
echo "   1. Run: ./scripts/check_platform_status.sh (verify all builds)"
echo "   2. Build Python distribution: python3 setup.py sdist bdist_wheel"
echo "   3. Deploy to PyPI: twine upload dist/*" 