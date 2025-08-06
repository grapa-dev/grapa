#!/bin/bash

# Build Libraries and Grapa Application in Docker (Ubuntu 22.04 ARM64)
# This script builds all third-party libraries and Grapa application in a Docker container
# to ensure compatibility with GitHub Actions runners

set -e

echo "🐳 Building Grapa libraries and application in Docker (Ubuntu 22.04 ARM64)..."

# Build the Docker image
echo "Building Docker image..."
docker build -f Dockerfile.ubuntu22-arm64 -t grapa-ubuntu22-arm64 .

# Run the container and build libraries
echo "Running container and building libraries and application..."
docker run --platform=linux/arm64 -it --rm \
    -v $HOME:/data \
    grapa-ubuntu22-arm64 \
    bash -c "
        echo '🚀 Starting library and application builds...'
        echo '📁 Working directory: \$(pwd)'
        echo '📁 Grapa directory: /data/GitHub/grapa'
        
        # Navigate to grapa directory
        cd /data/GitHub/grapa
        
        # Create output directories if they don't exist
        mkdir -p source/openssl-lib/linux-arm64
        mkdir -p source/fl-lib/linux-arm64
        mkdir -p source/pcre2-lib/linux-arm64
        mkdir -p source/blst-lib/linux-arm64
        mkdir -p source/grapa-lib/linux-arm64
        mkdir -p source/grapa-other/linux-arm64
        mkdir -p bin
        
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
        
        # Build OpenSSL
        echo '📦 Building OpenSSL...'
        cd dep/openssl-1.1.1w
        ./config -fPIC -std=c++17 no-shared
        make
        cp libcrypto.a libssl.a ../../source/openssl-lib/linux-arm64/
        echo '✅ OpenSSL built successfully'
        
        # Build FLTK (using correct directory name)
        echo '📦 Building FLTK...'
        cd ../../dep/fltk-1.3.11
        NOCONFIGURE=1 ./autogen.sh
        ./configure --with-optim='-fPIC -std=c++17' --disable-shared
        make
        cp lib/*.a ../../source/fl-lib/linux-arm64/
        echo '✅ FLTK built successfully'
        
        # Build PCRE2
        echo '📦 Building PCRE2...'
        cd ../../dep/pcre2-10.45
        rm -rf build_static
        mkdir build_static
        cd build_static
        cmake -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
        make
        cp libpcre2-8.a ../../../source/pcre2-lib/linux-arm64/
        echo '✅ PCRE2 built successfully'
        
        # Build BLST
        echo '📦 Building BLST...'
        cd ../../../prj/blst
        ./build.sh
        cp libblst.a ../../source/blst-lib/linux-arm64/
        echo '✅ BLST built successfully'
        
        # Build Grapa Application
        echo '📦 Building Grapa Application...'
        cd ../../source
        g++ -Isource -DUTF8PROC_STATIC source/main.cpp source/grapa/GrapaObject.cpp source/grapa/GrapaTinyAES.cpp source/grapa/GrapaFloat.cpp source/grapa/GrapaLibRule.cpp source/grapa/GrapaWidget.cpp source/grapa/GrapaConsole.cpp source/grapa/GrapaLink.cpp source/grapa/GrapaBtree.cpp source/grapa/GrapaCompress.cpp source/grapa/GrapaFileCache.cpp source/grapa/GrapaDatabase.cpp source/grapa/GrapaGroup.cpp source/grapa/GrapaBtreeBlock.cpp source/grapa/GrapaInt.cpp source/grapa/GrapaHash.cpp source/grapa/GrapaThread.cpp source/grapa/GrapaNetConnect.cpp source/grapa/GrapaPrime.cpp source/grapa/GrapaValue.cpp source/grapa/GrapaFileIO.cpp source/grapa/GrapaDB.cpp source/grapa/GrapaEncode.cpp source/grapa/GrapaTime.cpp source/grapa/GrapaSystem.cpp source/grapa/GrapaVector.cpp source/grapa/GrapaNet.cpp source/grapa/GrapaState.cpp source/grapa/GrapaFileTree.cpp source/utf8proc/utf8proc.c source/openssl-lib/linux-arm64/libcrypto.a source/openssl-lib/linux-arm64/libssl.a source/fl-lib/linux-arm64/libfltk_jpeg.a source/fl-lib/linux-arm64/libfltk.a source/fl-lib/linux-arm64/libfltk_gl.a source/fl-lib/linux-arm64/libfltk_forms.a source/blst-lib/linux-arm64/libblst.a source/pcre2-lib/linux-arm64/libpcre2-8.a -Lsource/openssl-lib/linux-arm64 -std=c++17 -O3 -pthread -o grapa -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ljpeg -ldl -lm
        echo '✅ Grapa Application built successfully'
        
        # Build Grapa Static Library
        echo '📦 Building Grapa Static Library...'
        mkdir -p source/grapa-lib/linux-arm64
        ar -crs source/grapa-lib/linux-arm64/libgrapa.a source/openssl-lib/linux-arm64/libcrypto.a source/openssl-lib/linux-arm64/libssl.a source/fl-lib/linux-arm64/libfltk.a source/fl-lib/linux-arm64/libfltk_gl.a source/fl-lib/linux-arm64/libfltk_forms.a source/fl-lib/linux-arm64/libfltk_images.a source/blst-lib/linux-arm64/libblst.a source/pcre2-lib/linux-arm64/libpcre2-8.a
        echo '✅ Grapa Static Library built successfully'
        
        # Build Grapa Shared Library
        echo '📦 Building Grapa Shared Library...'
        mkdir -p source/grapa-other/linux-arm64
        g++ -shared -fPIC -Isource -DUTF8PROC_STATIC source/grapa/GrapaObject.cpp source/grapa/GrapaTinyAES.cpp source/grapa/GrapaFloat.cpp source/grapa/GrapaLibRule.cpp source/grapa/GrapaWidget.cpp source/grapa/GrapaConsole.cpp source/grapa/GrapaLink.cpp source/grapa/GrapaBtree.cpp source/grapa/GrapaCompress.cpp source/grapa/GrapaFileCache.cpp source/grapa/GrapaDatabase.cpp source/grapa/GrapaGroup.cpp source/grapa/GrapaBtreeBlock.cpp source/grapa/GrapaInt.cpp source/grapa/GrapaHash.cpp source/grapa/GrapaThread.cpp source/grapa/GrapaNetConnect.cpp source/grapa/GrapaPrime.cpp source/grapa/GrapaValue.cpp source/grapa/GrapaFileIO.cpp source/grapa/GrapaDB.cpp source/grapa/GrapaEncode.cpp source/grapa/GrapaTime.cpp source/grapa/GrapaSystem.cpp source/grapa/GrapaVector.cpp source/grapa/GrapaNet.cpp source/grapa/GrapaState.cpp source/grapa/GrapaFileTree.cpp source/utf8proc/utf8proc.c source/openssl-lib/linux-arm64/libcrypto.a source/openssl-lib/linux-arm64/libssl.a source/fl-lib/linux-arm64/libfltk.a source/fl-lib/linux-arm64/libfltk_gl.a source/fl-lib/linux-arm64/libfltk_forms.a source/fl-lib/linux-arm64/libfltk_images.a source/blst-lib/linux-arm64/libblst.a source/pcre2-lib/linux-arm64/libpcre2-8.a -Lsource/openssl-lib/linux-arm64 -std=c++17 -O3 -pthread -o source/grapa-other/linux-arm64/libgrapa.so -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ljpeg -ldl -lm
        echo '✅ Grapa Shared Library built successfully'
        
        # Create compressed file
        echo '📦 Creating compressed file...'
        mkdir -p bin
        tar -czf bin/grapa-linux-arm64.tar.gz grapa source/grapa-lib/linux-arm64/libgrapa.a source/grapa-other/linux-arm64/libgrapa.so
        echo '✅ Compressed file created successfully'
        
        echo '🎉 All libraries and application built successfully!'
        echo '📁 Libraries are in source/*/linux-arm64/'
        echo '📁 Application is in grapa'
        echo '📁 Compressed file is in bin/grapa-linux-arm64.tar.gz'
        
        # List built libraries and files
        echo '📋 Built libraries:'
        ls -la source/openssl-lib/linux-arm64/
        ls -la source/fl-lib/linux-arm64/
        ls -la source/pcre2-lib/linux-arm64/
        ls -la source/blst-lib/linux-arm64/
        ls -la source/grapa-lib/linux-arm64/
        ls -la source/grapa-other/linux-arm64/
        echo '📋 Application:'
        ls -la grapa
        echo '📋 Compressed file:'
        ls -la bin/grapa-linux-arm64.tar.gz
    "

echo "✅ Docker build completed successfully!"
echo "📁 Libraries are now available in source/*/linux-arm64/"
echo "📁 Application is available as grapa"
echo "📁 Compressed file is available in bin/grapa-linux-arm64.tar.gz" 