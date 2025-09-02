#!/bin/bash
# Grapa Installation Script for Mac arm64
# Run this script with sudo for system-wide installation

set -e

INSTALL_PATH="/usr/local"
BIN_PATH="$INSTALL_PATH/bin"
LIB_PATH="$INSTALL_PATH/lib"
INCLUDE_PATH="$INSTALL_PATH/include"

echo "Installing Grapa for Mac arm64..."

# Create directories
sudo mkdir -p $BIN_PATH $LIB_PATH $INCLUDE_PATH

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Copy executable
sudo cp "$SCRIPT_DIR/grapa" $BIN_PATH/
sudo chmod +x $BIN_PATH/grapa

# Copy libraries
if [ -f "$SCRIPT_DIR/libgrapa_static.a" ]; then
    sudo cp "$SCRIPT_DIR/libgrapa_static.a" $LIB_PATH/
fi

if [ -f "$SCRIPT_DIR/libgrapa.so" ]; then
    sudo cp "$SCRIPT_DIR/libgrapa.so" $LIB_PATH/
    # Note: ldconfig is not needed on macOS
fi

echo "Installation complete!"
echo "Grapa executable: $BIN_PATH/grapa"
if [ -f "$LIB_PATH/libgrapa_static.a" ]; then
    echo "Static library: $LIB_PATH/libgrapa_static.a"
fi
if [ -f "$LIB_PATH/libgrapa.so" ]; then
    echo "Shared library: $LIB_PATH/libgrapa.so"
fi
