#!/bin/bash

# Rebuild Linux AMD64 Libraries in Docker
# This script rebuilds all third-party libraries for Linux AMD64 in the current Docker environment
# to fix the "file in wrong format" compatibility issue

set -e

echo "🐳 Rebuilding Linux AMD64 libraries in Docker (Ubuntu 24.04)..."

# Build the Docker image
echo "Building Docker image..."
docker build -f Dockerfile.grapa-build -t grapa-build .

# Run the container and rebuild libraries
echo "Running container and rebuilding libraries..."
docker run --rm \
    -v $HOME:/data \
    grapa-build \
    bash -c "
        echo '🚀 Starting Linux AMD64 library rebuild...'
        echo '📁 Working directory: \$(pwd)'
        echo '📁 Grapa directory: /data/GitHub/grapa'
        
        # Navigate to grapa directory
        cd /data/GitHub/grapa
        
        # Create output directories if they don't exist
        mkdir -p source/openssl-lib/linux-amd64
        mkdir -p source/fl-lib/linux-amd64
        mkdir -p source/pcre2-lib/linux-amd64
        mkdir -p source/blst-lib/linux-amd64
        
        # Extract dependencies if they don't exist
        echo '📦 Extracting dependencies...'
        
        # Extract OpenSSL (already extracted)
        if [ ! -d 'dep/openssl-1.1.1w' ]; then
            echo 'Extracting OpenSSL...'
            cd dep
            tar -xzf openssl-1.1.1w.tar.gz
            cd ..
        fi
        
        # Extract FLTK (already extracted as fltk-1.3.11)
        if [ ! -d 'dep/fltk-1.3.11' ]; then
            echo 'Extracting FLTK...'
            cd dep
            tar -xzf fltk-1.3.11-source.tar.gz
            cd ..
        fi
        
        # Extract PCRE2
        if [ ! -d 'dep/pcre2-10.45' ]; then
            echo 'Extracting PCRE2...'
            cd dep
            unzip -q pcre2-10.45.zip
            cd ..
        fi
        
        # Extract BLST
        if [ ! -d 'dep/blst-master' ]; then
            echo 'Extracting BLST...'
            cd dep
            unzip -q blst-master.zip
            cd ..
        fi
        
        echo '✅ Dependencies extracted'
        echo '📋 Extracted directories:'
        ls -la dep/
        
        # Build OpenSSL for AMD64
        echo '📦 Building OpenSSL for AMD64...'
        cd dep/openssl-1.1.1w
        ./config -fPIC -std=c++17 no-shared
        make clean
        make
        cp libcrypto.a libssl.a ../../source/openssl-lib/linux-amd64/
        echo '✅ OpenSSL built successfully for AMD64'
        
        # Build FLTK for AMD64
        echo '📦 Building FLTK for AMD64...'
        cd ../../dep/fltk-1.3.11
        NOCONFIGURE=1 ./autogen.sh
        ./configure --with-optim='-fPIC -std=c++17' --disable-shared
        make clean
        make
        cp lib/*.a ../../source/fl-lib/linux-amd64/
        echo '✅ FLTK built successfully for AMD64'
        
        # Build PCRE2 for AMD64
        echo '📦 Building PCRE2 for AMD64...'
        cd ../../dep/pcre2-10.45
        rm -rf build_static
        mkdir build_static
        cd build_static
        cmake -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
        make clean
        make
        cp libpcre2-8.a ../../../source/pcre2-lib/linux-amd64/
        echo '✅ PCRE2 built successfully for AMD64'
        
        # Build BLST for AMD64
        echo '📦 Building BLST for AMD64...'
        cd ../../../dep/blst-master
        make clean
        make
        cp libblst.a ../../source/blst-lib/linux-amd64/
        echo '✅ BLST built successfully for AMD64'
        
        echo '🎉 All Linux AMD64 libraries rebuilt successfully!'
        echo '📁 Libraries available in:'
        echo '  - source/openssl-lib/linux-amd64/'
        echo '  - source/fl-lib/linux-amd64/'
        echo '  - source/pcre2-lib/linux-amd64/'
        echo '  - source/blst-lib/linux-amd64/'
        
        # Show library details
        echo '📋 Library details:'
        ls -la source/openssl-lib/linux-amd64/
        ls -la source/fl-lib/linux-amd64/
        ls -la source/pcre2-lib/linux-amd64/
        ls -la source/blst-lib/linux-amd64/
    "

echo "✅ Linux AMD64 libraries rebuilt successfully!"
echo "📁 Libraries are now compatible with the current Docker environment"
echo "🔄 You can now run: ./scripts/build_grapa_linux_amd64.sh" 