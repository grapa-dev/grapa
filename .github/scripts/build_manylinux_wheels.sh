#!/bin/bash

# Build manylinux wheels for GrapaPy
# This script runs inside the manylinux Docker container

set -e

echo "Building manylinux wheels for GrapaPy..."

# Install build dependencies
yum install -y gcc gcc-c++ make cmake

# Install Python build tools
/opt/python/cp39-cp39/bin/pip install setuptools wheel build

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