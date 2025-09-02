#!/bin/bash
# Grapa Installation Script for Linux arm64
# Run this script with sudo for system-wide installation

set -e

INSTALL_PATH="/usr/local"
BIN_PATH="$INSTALL_PATH/bin"
LIB_PATH="$INSTALL_PATH/lib"
INCLUDE_PATH="$INSTALL_PATH/include"

echo "Installing Grapa for Linux arm64..."

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Check if files exist
REQUIRED_FILES=("grapa" "libgrapa_static.a")
MISSING_FILES=()

for file in "${REQUIRED_FILES[@]}"; do
    if [ ! -f "$SCRIPT_DIR/$file" ]; then
        MISSING_FILES+=("$file")
    fi
done

if [ ${#MISSING_FILES[@]} -gt 0 ]; then
    echo "❌ Missing required files:" >&2
    for file in "${MISSING_FILES[@]}"; do
        echo "   - $file" >&2
    done
    echo "Please ensure all required files are in the same directory as this script." >&2
    exit 1
fi

# Check for existing installation
if [ -d "$INSTALL_PATH/grapa" ] || [ -f "$BIN_PATH/grapa" ]; then
    echo "⚠️  Grapa is already installed at: $INSTALL_PATH"
    read -p "Do you want to replace it? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Installation cancelled."
        exit 0
    fi
    echo "🔄 Replacing existing installation..."
    
    # Remove old installation completely to ensure clean replacement
    if [ -d "$INSTALL_PATH/grapa" ]; then
        sudo rm -rf "$INSTALL_PATH/grapa"
        echo "✅ Removed old installation directory"
    fi
    
    if [ -f "$BIN_PATH/grapa" ]; then
        sudo rm -f "$BIN_PATH/grapa"
        echo "✅ Removed old executable"
    fi
    
    if [ -f "$LIB_PATH/libgrapa_static.a" ]; then
        sudo rm -f "$LIB_PATH/libgrapa_static.a"
        echo "✅ Removed old static library"
    fi
    
    if [ -f "$LIB_PATH/libgrapa.so" ]; then
        sudo rm -f "$LIB_PATH/libgrapa.so"
        echo "✅ Removed old shared library"
    fi
else
    echo "🆕 Installing Grapa..."
fi

# Create directories
sudo mkdir -p $BIN_PATH $LIB_PATH $INCLUDE_PATH
echo "✅ Created installation directories"

# Copy executable
sudo cp "$SCRIPT_DIR/grapa" $BIN_PATH/
sudo chmod +x $BIN_PATH/grapa
echo "✅ Copied grapa executable to $BIN_PATH"

# Copy static library
sudo cp "$SCRIPT_DIR/libgrapa_static.a" $LIB_PATH/
echo "✅ Copied static library to $LIB_PATH"

# Copy shared library if it exists
if [ -f "$SCRIPT_DIR/libgrapa.so" ]; then
    sudo cp "$SCRIPT_DIR/libgrapa.so" $LIB_PATH/
    sudo ldconfig
    echo "✅ Copied shared library to $LIB_PATH and updated library cache"
else
    echo "ℹ️  Shared library not found (shared library not available)"
fi

# Add to PATH if not already present
if ! echo "$PATH" | grep -q "$BIN_PATH"; then
    # Check if we can modify the system PATH
    if [ -w /etc/environment ]; then
        echo "export PATH=\"\$PATH:$BIN_PATH\"" | sudo tee -a /etc/environment > /dev/null
        echo "✅ Added Grapa to system PATH in /etc/environment"
    elif [ -w /etc/profile.d ]; then
        echo "export PATH=\"\$PATH:$BIN_PATH\"" | sudo tee /etc/profile.d/grapa.sh > /dev/null
        sudo chmod +x /etc/profile.d/grapa.sh
        echo "✅ Added Grapa to system PATH in /etc/profile.d/grapa.sh"
    else
        echo "⚠️  Could not automatically add to system PATH"
        echo "Please manually add $BIN_PATH to your PATH"
    fi
else
    echo "ℹ️  Grapa already in system PATH"
fi

echo ""
echo "🎉 Installation complete!"
echo ""
echo "Installation details:"
echo "  Executable: $BIN_PATH/grapa"
echo "  Static Library: $LIB_PATH/libgrapa_static.a"
if [ -f "$LIB_PATH/libgrapa.so" ]; then
    echo "  Shared Library: $LIB_PATH/libgrapa.so"
fi
echo "  System PATH: Updated"
echo ""
echo "Next steps:"
echo "  1. Restart your terminal for PATH changes to take effect"
echo "  2. Verify installation: grapa --version"
echo "  3. Test functionality: grapa -c '2+2'"
echo ""
echo "To uninstall: sudo rm -rf $INSTALL_PATH/grapa && sudo rm -f $BIN_PATH/grapa"
