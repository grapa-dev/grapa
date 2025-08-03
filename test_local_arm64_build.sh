#!/bin/bash

echo "=== Local ARM64 Build Test ==="
echo "Current directory: $(pwd)"
echo "Architecture: $(uname -m)"
echo "Python version: $(python3 --version)"

echo ""
echo "=== Checking required libraries ==="
echo "Grapa libraries:"
ls -la source/grapa-lib/linux-arm64/ || echo "Grapa libraries not found"

echo ""
echo "OpenSSL libraries:"
ls -la source/openssl-lib/linux-arm64/ || echo "OpenSSL libraries not found"

echo ""
echo "FL libraries:"
ls -la source/fl-lib/linux-arm64/ || echo "FL libraries not found"

echo ""
echo "BLST libraries:"
ls -la source/blst-lib/linux-arm64/ || echo "BLST libraries not found"

echo ""
echo "PCRE2 libraries:"
ls -la source/pcre2-lib/linux-arm64/ || echo "PCRE2 libraries not found"

echo ""
echo "=== Testing wheel build ==="
export GRAPA_PLATFORM=arm64
echo "Setting GRAPA_PLATFORM=$GRAPA_PLATFORM"

echo ""
echo "Building wheel..."
python3 setup.py bdist_wheel

if [ $? -eq 0 ]; then
    echo ""
    echo "=== Build successful! ==="
    echo "Wheel files created:"
    ls -la dist/
    
    echo ""
    echo "=== Testing wheel installation ==="
    pip3 install dist/*.whl --force-reinstall
    
    echo ""
    echo "=== Testing grapapy import ==="
    python3 -c "import grapapy; print('grapapy imported successfully')"
    
    echo ""
    echo "=== Testing basic functionality ==="
    python3 -c "
import grapapy
print('GrapaPy version:', grapapy.__version__)
print('Basic functionality test passed')
"
else
    echo ""
    echo "=== Build failed ==="
    exit 1
fi 