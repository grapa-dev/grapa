#!/bin/bash
# Build Grapa Application and Libraries for Windows AMD64
# This script provides instructions for building on Windows
# Windows builds require a Windows machine with Visual Studio

echo "🪟 Building Grapa for Windows AMD64..."
echo ""
echo "⚠️  WINDOWS BUILD REQUIREMENTS:"
echo "   - Windows machine (cannot be done from macOS/Linux)"
echo "   - Visual Studio 2019 or later"
echo "   - Python 3.x installed"
echo "   - Git for Windows"
echo ""

# Check if we're on Windows
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    echo "✅ Running on Windows!"
    echo ""
    echo "🚀 Starting Windows build process..."
    
    # Create output directories
    mkdir -p source/grapa-lib/win-amd64
    mkdir -p source/grapa-other/win-amd64
    mkdir -p bin
    
    # Build Grapa using build.py with --bin-only
    echo "📦 Building Grapa Application and creating package using build.py..."
    python build.py --bin-only --clean
    echo "✅ Grapa Application and package built successfully"
    
    echo ""
    echo "🎉 Windows build completed successfully!"
    echo "📁 Application: grapa.exe"
    echo "📁 Static library: source/grapa-lib/win-amd64/grapa.lib"
    echo "📁 Shared library: source/grapa-other/win-amd64/grapa.dll"
    echo "📁 Compressed file: bin/grapa-win-amd64.tar.gz"
    
else
    echo "❌ This script must be run on a Windows machine"
    echo ""
    echo "📋 WINDOWS BUILD OPTIONS:"
    echo "   1. Use a Windows machine with Visual Studio"
    echo "   2. Use a Windows VM (VirtualBox, VMware, etc.)"
    echo "   3. Use a remote Windows server"
    echo "   4. Use GitHub Actions (Windows runner)"
    echo "   5. Use Azure DevOps (Windows agent)"
    echo ""
    echo "🔧 SETUP INSTRUCTIONS:"
    echo "   1. Install Visual Studio 2019 or later"
    echo "   2. Install Python 3.x"
    echo "   3. Clone the repository"
    echo "   4. Run: python build.py --bin-only"
    echo ""
    echo "📁 EXPECTED OUTPUT:"
    echo "   - grapa.exe (executable)"
    echo "   - source/grapa-lib/win-amd64/grapa.lib (static library)"
    echo "   - source/grapa-other/win-amd64/grapa.dll (shared library)"
    echo "   - bin/grapa-win-amd64.tar.gz (compressed package)"
    echo ""
    exit 1
fi 