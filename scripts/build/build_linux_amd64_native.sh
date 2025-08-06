#!/bin/bash

# Build Linux AMD64 Libraries and Application on Native Linux AMD64 System
# This script should be run on a real Linux AMD64 system (not in Docker)
# to avoid architecture compatibility issues

set -e

echo "🐧 Building Linux AMD64 libraries and application on native system..."

# Check if we're on a real Linux AMD64 system
if [[ "$(uname -s)" != "Linux" ]]; then
    echo "❌ Error: This script must be run on a Linux system"
    exit 1
fi

if [[ "$(uname -m)" != "x86_64" ]]; then
    echo "❌ Error: This script must be run on an AMD64/x86_64 system"
    echo "Current architecture: $(uname -m)"
    exit 1
fi

echo "✅ Running on Linux AMD64 system: $(uname -a)"

# Install required dependencies
echo "📦 Installing required dependencies..."
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    python3 \
    python3-pip \
    x11-apps \
    libx11-dev \
    libxfixes-dev \
    libxft-dev \
    libxext-dev \
    libxrender-dev \
    libxinerama-dev \
    libfontconfig-dev \
    libxcursor-dev \
    libjpeg-dev \
    libbsd-dev \
    gcc \
    g++ \
    gdebi-core

echo "✅ Dependencies installed"

# Create output directories
echo "📁 Creating output directories..."
mkdir -p source/openssl-lib/linux-amd64
mkdir -p source/fl-lib/linux-amd64
mkdir -p source/pcre2-lib/linux-amd64
mkdir -p source/blst-lib/linux-amd64
mkdir -p source/grapa-lib/linux-amd64
mkdir -p source/grapa-other/linux-amd64
mkdir -p bin

# Extract dependencies if they don't exist
echo "📦 Extracting dependencies..."

# Extract OpenSSL
if [ ! -d 'dep/openssl-1.1.1w' ]; then
    echo 'Extracting OpenSSL...'
    cd dep
    tar -xzf openssl-1.1.1w.tar.gz
    cd ..
fi

# Extract FLTK
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

# Build Grapa application using the working reference command
echo '📦 Building Grapa application...'
cd ../..

# Remove old executable
rm -f grapa

# Build executable using the exact working reference command
echo 'Building executable using working reference command...'
g++ -Isource -DUTF8PROC_STATIC source/main.cpp source/grapa/*.cpp source/utf8proc/utf8proc.c source/openssl-lib/linux-amd64/*.a source/fl-lib/linux-amd64/*.a source/blst-lib/linux-amd64/*.a source/pcre2-lib/linux-amd64/libpcre2-8.a -Lsource/openssl-lib/linux-amd64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -o grapa

# Build static library
echo 'Building static library...'
g++ -c -Isource -DUTF8PROC_STATIC source/grapa/*.cpp source/utf8proc/utf8proc.c -std=c++17 -O3 -pthread -fPIC
ar -crs libgrapa.a *.o
rm *.o
cp libgrapa.a source/grapa-lib/linux-amd64/libgrapa.a
rm libgrapa.a

# Build shared library
echo 'Building shared library...'
g++ -shared -Isource -DUTF8PROC_STATIC source/grapa/*.cpp source/utf8proc/utf8proc.c source/openssl-lib/linux-amd64/*.a source/fl-lib/linux-amd64/*.a source/blst-lib/linux-amd64/*.a source/pcre2-lib/linux-amd64/libpcre2-8.a -Lsource/openssl-lib/linux-amd64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -fPIC -o libgrapa.so
cp libgrapa.so source/grapa-other/linux-amd64/libgrapa.so
rm libgrapa.so

# Create compressed package
echo 'Creating compressed package...'
tar -czvf bin/grapa-linux-amd64.tar.gz grapa source/grapa-lib/linux-amd64/* source/grapa-other/linux-amd64/*

echo '🎉 All Linux AMD64 components built successfully!'
echo '📁 Application is in grapa'
echo '📁 Static library is in source/grapa-lib/linux-amd64/libgrapa.a'
echo '📁 Shared library is in source/grapa-other/linux-amd64/libgrapa.so'
echo '📁 Compressed file is in bin/grapa-linux-amd64.tar.gz'

# Show library details
echo '📋 Library details:'
ls -la source/openssl-lib/linux-amd64/
ls -la source/fl-lib/linux-amd64/
ls -la source/pcre2-lib/linux-amd64/
ls -la source/blst-lib/linux-amd64/

echo '✅ Linux AMD64 build completed successfully on native system!' 