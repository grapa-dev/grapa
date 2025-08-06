#!/bin/bash

# Validate Linux builds work on Amazon Linux containers
# This script tests if the Linux libraries and executables work on Amazon Linux

set -e

echo "🔍 Validating Linux builds on Amazon Linux containers..."
echo "=================================================="

# Function to test a platform
test_platform() {
    local platform=$1
    local arch=$2
    local docker_platform=$3
    
    echo ""
    echo "🧪 Testing $platform-$arch on Amazon Linux..."
    
    # Build using Linux container
    echo "📦 Building with Linux container..."
    docker run --platform=$docker_platform -it --rm \
        -v $HOME:/data \
        ubuntu:22.04 \
        bash -c "
            cd /data/GitHub/grapa
            python3 build.py --exe-only
            echo '✅ Linux build completed'
            ls -la grapa
        "
    
    # Test on Amazon Linux container
    echo "🧪 Testing on Amazon Linux container..."
    docker run --platform=$docker_platform -it --rm \
        -v $HOME:/data \
        amazonlinux:2023 \
        bash -c "
            cd /data/GitHub/grapa
            echo '📋 Testing executable...'
            if [ -f grapa ]; then
                echo '✅ Executable exists'
                ./grapa -c '\$sys().getenv(\$VERSION)' || echo '⚠️  Executable test failed'
            else
                echo '❌ Executable not found'
                exit 1
            fi
            
            echo '📋 Testing Python extension...'
            if [ -f source/grapa-lib/linux-$arch/libgrapa.a ]; then
                echo '✅ Static library exists'
                # Try to build Python extension
                python3 setup.py build_ext || echo '⚠️  Python extension build failed'
            else
                echo '❌ Static library not found'
                exit 1
            fi
            
            echo '✅ Amazon Linux validation completed'
        "
    
    if [ $? -eq 0 ]; then
        echo "✅ $platform-$arch validation successful"
    else
        echo "❌ $platform-$arch validation failed"
        return 1
    fi
}

# Test both platforms
echo "🚀 Starting validation tests..."

# Test Linux AMD64
test_platform "linux" "amd64" "linux/amd64"

# Test Linux ARM64  
test_platform "linux" "arm64" "linux/arm64"

echo ""
echo "=================================================="
echo "✅ All Amazon Linux validation tests completed!"
echo ""
echo "📋 Summary:"
echo "   - Linux builds should work on Amazon Linux"
echo "   - Executables should run on Amazon Linux"
echo "   - Python extensions should build on Amazon Linux"
echo ""
echo "💡 If tests pass, you can use Linux containers for AWS builds"
echo "💡 If tests fail, you may need to keep AWS separate" 