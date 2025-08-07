#!/bin/bash
set -e

echo "Building manylinux wheels for GrapaPy..."

# Change to the mounted repository directory
cd /io

# Install build dependencies
yum install -y gcc gcc-c++ make cmake

# Install X11 development libraries in the manylinux container
yum install -y libX11-devel libXfixes-devel libXft-devel libXext-devel libXrender-devel libXinerama-devel fontconfig-devel libXcursor-devel

# Install Python build tools for all Python versions
for pyver in cp39-cp39 cp310-cp310 cp311-cp311 cp312-cp312; do
    echo "Installing build tools for Python $pyver..."
    /opt/python/$pyver/bin/pip install setuptools wheel build
done

# Build wheels for all Python versions
for pyver in cp39-cp39 cp310-cp310 cp311-cp311 cp312-cp312; do
    echo "Building wheel for Python $pyver..."
    /opt/python/$pyver/bin/python setup.py bdist_wheel
done

# Build source distribution
/opt/python/cp39-cp39/bin/python setup.py sdist

# Repair wheels to ensure manylinux compatibility
for wheel in dist/*.whl; do
    if [[ -f "$wheel" ]]; then
        echo "Repairing $wheel..."
        auditwheel repair "$wheel" --plat manylinux_2_28_x86_64 -w dist/
        # Remove the original wheel
        rm "$wheel"
    fi
done

echo "Built manylinux distributions:"
ls -la dist/ 