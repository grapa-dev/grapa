#!/bin/bash
# Build Grapa Application and Libraries for macOS AMD64
# This script builds the Grapa executable, static library, and shared library
# Uses cross-compilation from ARM64 Mac to AMD64
# Assumes third-party libraries are already available in source/*/mac-amd64/
set -e

echo "🍎 Building Grapa for macOS AMD64 (cross-compilation from ARM64)..."
echo "⚠️  Note: This requires running on an ARM64 Mac with Xcode"

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "❌ This script must be run on macOS"
    exit 1
fi

# Check if we're on ARM64 Mac
if [[ $(uname -m) != "arm64" ]]; then
    echo "❌ This script requires an ARM64 Mac for cross-compilation"
    echo "   On AMD64 Mac, you can build natively with: python3 build.py --bin-only"
    exit 1
fi

echo "✅ Running on ARM64 Mac, using cross-compilation..."

# Create output directories
mkdir -p source/grapa-lib/mac-amd64
mkdir -p source/grapa-other/mac-amd64
mkdir -p bin

# Build Grapa using build.py with --bin-only and target platform
echo "📦 Building Grapa Application and creating package for macOS AMD64..."
python3 build.py --bin-only --target-platform mac-amd64 --clean
echo "✅ Grapa Application and package built successfully"

echo "🎉 All Grapa components built successfully!"
echo "📁 Application is in grapa"
echo "📁 Static library is in source/grapa-lib/mac-amd64/libgrapa.a"
echo "📁 Shared library is in source/grapa-other/mac-amd64/libgrapa.so"
echo "📁 Compressed file is in bin/grapa-mac-amd64.tar.gz"

echo "✅ macOS AMD64 build completed successfully!"
echo "📁 Application is available as grapa"
echo "📁 Static library is available in source/grapa-lib/mac-amd64/"
echo "📁 Shared library is available in source/grapa-other/mac-amd64/"
echo "📁 Compressed file is available in bin/grapa-mac-amd64.tar.gz" 