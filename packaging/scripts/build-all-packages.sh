#!/bin/bash

# Build All Packages Script for Grapa
# This script builds packages for all supported platforms

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
VERSION="0.1.51"

echo "🔨 Building Grapa packages for version $VERSION"

# Function to calculate SHA256
calculate_sha256() {
    local file="$1"
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$file" | cut -d' ' -f1
    elif command -v shasum >/dev/null 2>&1; then
        shasum -a 256 "$file" | cut -d' ' -f1
    else
        echo "Error: No SHA256 calculator found"
        exit 1
    fi
}

# Function to update SHA256 in files
update_sha256() {
    local file="$1"
    local sha256="$2"
    local platform="$3"
    
           case $platform in
           "mac-arm64")
               sed -i.bak "s/PLACEHOLDER_SHA256_ARM64/$sha256/g" "$PROJECT_ROOT/packaging/homebrew/grapa.rb"
               ;;
           "win-amd64")
               sed -i.bak "s/PLACEHOLDER_SHA256/$sha256/g" "$PROJECT_ROOT/packaging/chocolatey/tools/chocolateyinstall.ps1"
               ;;
       esac
}

# Build Homebrew formula
build_homebrew() {
    echo "🍎 Building Homebrew formula..."
    
                  # Calculate SHA256 for mac-arm64
       if [ -f "$PROJECT_ROOT/bin/grapa" ]; then
           SHA256_ARM64=$(calculate_sha256 "$PROJECT_ROOT/bin/grapa")
           update_sha256 "$PROJECT_ROOT/bin/grapa" "$SHA256_ARM64" "mac-arm64"
           echo "✅ Updated Homebrew formula with mac-arm64 SHA256: $SHA256_ARM64"
       fi
    
    echo "✅ Homebrew formula ready"
}

# Build Debian package
build_debian() {
    echo "🐧 Building Debian package..."
    
    DEBIAN_DIR="$PROJECT_ROOT/packaging/debian/grapa"
    
    # Copy the appropriate binary
    if [ -f "$PROJECT_ROOT/bin/grapa-linux-amd64" ]; then
        cp "$PROJECT_ROOT/bin/grapa-linux-amd64" "$DEBIAN_DIR/usr/bin/grapa"
        chmod +x "$DEBIAN_DIR/usr/bin/grapa"
        echo "✅ Copied grapa-linux-amd64 to Debian package"
    else
        echo "❌ grapa-linux-amd64 not found"
        return 1
    fi
    
    # Build the .deb package
    cd "$PROJECT_ROOT/packaging/debian"
    dpkg-deb --build grapa
    mv grapa.deb "grapa_${VERSION}_amd64.deb"
    
    echo "✅ Debian package built: grapa_${VERSION}_amd64.deb"
}

# Build RPM package
build_rpm() {
    echo "🔴 Building RPM package..."
    
    RPM_DIR="$PROJECT_ROOT/packaging/rpm/grapa"
    
    # Copy the appropriate binary
    if [ -f "$PROJECT_ROOT/bin/grapa-linux-amd64" ]; then
        cp "$PROJECT_ROOT/bin/grapa-linux-amd64" "$RPM_DIR/usr/bin/grapa"
        chmod +x "$RPM_DIR/usr/bin/grapa"
        echo "✅ Copied grapa-linux-amd64 to RPM package"
    else
        echo "❌ grapa-linux-amd64 not found"
        return 1
    fi
    
    # Build the .rpm package (requires rpmbuild)
    if command -v rpmbuild >/dev/null 2>&1; then
        cd "$PROJECT_ROOT/packaging/rpm"
        rpmbuild -bb grapa.spec
        echo "✅ RPM package built"
    else
        echo "⚠️  rpmbuild not found, skipping RPM package"
    fi
}

# Build Chocolatey package
build_chocolatey() {
    echo "🪟 Building Chocolatey package..."
    
    # Calculate SHA256 for Windows binary
    if [ -f "$PROJECT_ROOT/bin/grapa-win-amd64.exe" ]; then
        SHA256_WIN=$(calculate_sha256 "$PROJECT_ROOT/bin/grapa-win-amd64.exe")
        update_sha256 "$PROJECT_ROOT/bin/grapa-win-amd64.exe" "$SHA256_WIN" "win-amd64"
        echo "✅ Updated Chocolatey install script with SHA256: $SHA256_WIN"
    else
        echo "❌ grapa-win-amd64.exe not found"
        return 1
    fi
    
    echo "✅ Chocolatey package ready"
}

# Main build process
main() {
    echo "🚀 Starting package build process..."
    
    # Check if binaries exist
    if [ ! -d "$PROJECT_ROOT/bin" ]; then
        echo "❌ bin directory not found"
        exit 1
    fi
    
    # Build all packages
    build_homebrew
    build_debian
    build_rpm
    build_chocolatey
    
    echo "🎉 All packages built successfully!"
    echo ""
    echo "📦 Generated packages:"
    echo "  - Homebrew: $PROJECT_ROOT/packaging/homebrew/grapa.rb"
    echo "  - Debian: $PROJECT_ROOT/packaging/debian/grapa_${VERSION}_amd64.deb"
    echo "  - Chocolatey: $PROJECT_ROOT/packaging/chocolatey/"
    echo ""
    echo "📋 Next steps:"
    echo "  1. Test packages on target platforms"
    echo "  2. Upload to package repositories"
    echo "  3. Update documentation"
}

main "$@"
