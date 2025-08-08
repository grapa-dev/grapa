#!/bin/bash
set -e

echo "🧪 Linux PyPI Build Testing"
echo "=========================="

# Build the package in a Linux Docker container
echo "🐳 Building PyPI package in Linux Docker container..."
docker run --rm --platform linux/amd64 -v "$(pwd):/workspace" -w /workspace ubuntu:22.04 bash -c "
    apt-get update
    apt-get install -y python3 python3-pip python3-dev build-essential g++ gcc libc6-dev libstdc++-11-dev libgcc-s1 libstdc++6 libx11-dev libxfixes-dev libxft-dev libxext-dev libxrender-dev libxinerama-dev libfontconfig1-dev libxcursor-dev
    
    # Clean previous builds
    rm -rf dist/ build/ *.egg-info/
    
    # Build the package
    python3 setup.py bdist_wheel sdist
    
    echo '📦 Built distributions:'
    ls -la dist/
    
    # Test installation
    echo '🧪 Testing package installation...'
    pip3 install dist/grapapy-0.0.279-cp310-cp310-linux_x86_64.whl
    
    echo '❌ Testing grapapy import...'
    python3 -c \"import grapapy; print('✅ grapapy imports successfully')\" || echo '❌ grapapy import failed'
    
    # Debug X11 libraries
    echo '🔍 Debugging X11 libraries...'
    ls -la /usr/lib/x86_64-linux-gnu/libX11.so*
    nm -D /usr/lib/x86_64-linux-gnu/libX11.so.6 | grep XDrawArc
"

echo "✅ Linux PyPI build testing completed!" 