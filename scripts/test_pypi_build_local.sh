#!/bin/bash
set -e

echo "🧪 Local PyPI Build Testing"
echo "=========================="

# Clean previous builds
echo "🧹 Cleaning previous builds..."
rm -rf dist/ build/ *.egg-info/

# Build the package locally
echo "🔨 Building PyPI package locally..."
python3 setup.py bdist_wheel sdist

echo "📦 Built distributions:"
ls -la dist/

# Test in Docker container without X11
echo "🐳 Testing in Docker container without X11..."
docker run --rm -v "$(pwd)/dist:/dist" ubuntu:22.04 bash -c "
    apt-get update
    apt-get install -y python3 python3-pip
    pip3 install /dist/*.whl
    python3 -c 'import grapapy; print(\"✅ Import successful\")'
"

# Test in Docker container with X11
echo "🐳 Testing in Docker container with X11..."
docker run --rm -v "$(pwd)/dist:/dist" ubuntu:22.04 bash -c "
    apt-get update
    apt-get install -y python3 python3-pip libx11-dev libxfixes-dev libxft-dev libxext-dev libxrender-dev libxinerama-dev libfontconfig1-dev libxcursor-dev
    pip3 install /dist/*.whl
    python3 -c 'import grapapy; print(\"✅ Import successful with X11\")'
"

echo "✅ Local PyPI build testing completed!" 