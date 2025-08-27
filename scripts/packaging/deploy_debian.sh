#!/bin/bash

# Grapa Debian Package Deployment Script
# Automates the process of submitting Debian packages to Ubuntu/Debian repositories

set -e  # Exit on any error

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
PACKAGE_DIR="$REPO_ROOT/packaging/debian"
DEBIAN_DIR="$PACKAGE_DIR/grapa"
VERSION="0.1.51"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

command_exists() { command -v "$1" >/dev/null 2>&1; }

validate_prerequisites() {
    log_info "Validating prerequisites..."
    
    if ! command_exists dpkg-deb; then
        log_error "dpkg-deb is not installed. Please install dpkg-dev package."; exit 1; fi
    if ! command_exists lintian; then
        log_warning "lintian is not installed. Package quality checks will be skipped."; fi
    if ! command_exists git; then
        log_error "Git is not installed or not in PATH"; exit 1; fi
    
    # Check if required files exist
    if [ ! -d "$DEBIAN_DIR" ]; then
        log_error "Debian package directory not found: $DEBIAN_DIR"; exit 1; fi
    if [ ! -f "$DEBIAN_DIR/DEBIAN/control" ]; then
        log_error "Debian control file not found: $DEBIAN_DIR/DEBIAN/control"; exit 1; fi
    
    log_success "Prerequisites validated successfully"
}

build_packages() {
    log_info "Building Debian packages for all architectures..."
    
    # Create binary control file for ARM64
    cat > "$DEBIAN_DIR/DEBIAN/control.binary" << 'EOF'
Package: grapa
Version: 0.1.51
Section: devel
Priority: optional
Architecture: arm64
Depends: 
Maintainer: Grapa Team <team@grapa-dev>
Description: Grapa Programming Language
 A modern, high-performance programming language with unlimited precision arithmetic,
 advanced vector operations, and comprehensive cryptographic capabilities.
 .
 Features:
  * Unlimited precision arithmetic for integers and floating-point numbers
  * Advanced vector and matrix operations with optimized algorithms
  * Comprehensive cryptography with OpenSSL 3.5.2 integration
  * Machine learning capabilities including linear regression
  * Unicode string support and advanced text processing
  * Cross-platform compatibility (Linux, macOS, Windows)
  * Python integration via GrapaPy
  * Command-line interface with script execution
  * Database integration with unified file/database API
 .
 Homepage: https://github.com/grapa-dev/grapa
EOF
    
    # Build ARM64 package
    log_info "Building ARM64 package..."
    # Clean up package directory
    rm -rf "$DEBIAN_DIR/usr/bin/grapa"
    rm -rf "$DEBIAN_DIR/source" 2>/dev/null || true
    rm -rf "$DEBIAN_DIR/docs-src" 2>/dev/null || true
    rm -f "$DEBIAN_DIR/README.md" 2>/dev/null || true
    rm -f "$DEBIAN_DIR/LICENSE" 2>/dev/null || true
    
    cp "$REPO_ROOT/bin/grapa-linux-arm64" "$DEBIAN_DIR/usr/bin/grapa"
    chmod +x "$DEBIAN_DIR/usr/bin/grapa"
    cp "$DEBIAN_DIR/DEBIAN/control.binary" "$DEBIAN_DIR/DEBIAN/control"
    sed -i "s/Architecture: arm64/Architecture: arm64/g" "$DEBIAN_DIR/DEBIAN/control"
    
    cd "$PACKAGE_DIR"
    dpkg-deb --build grapa
    mv grapa.deb "grapa_${VERSION}_arm64.deb"
    
    # Build AMD64 package
    log_info "Building AMD64 package..."
    # Clean up package directory
    rm -rf "$DEBIAN_DIR/usr/bin/grapa"
    rm -rf "$DEBIAN_DIR/source" 2>/dev/null || true
    rm -rf "$DEBIAN_DIR/docs-src" 2>/dev/null || true
    rm -f "$DEBIAN_DIR/README.md" 2>/dev/null || true
    rm -f "$DEBIAN_DIR/LICENSE" 2>/dev/null || true
    
    cp "$REPO_ROOT/bin/grapa-linux-amd64" "$DEBIAN_DIR/usr/bin/grapa"
    chmod +x "$DEBIAN_DIR/usr/bin/grapa"
    cp "$DEBIAN_DIR/DEBIAN/control.binary" "$DEBIAN_DIR/DEBIAN/control"
    sed -i "s/Architecture: arm64/Architecture: amd64/g" "$DEBIAN_DIR/DEBIAN/control"
    
    dpkg-deb --build grapa
    mv grapa.deb "grapa_${VERSION}_amd64.deb"
    
    log_success "Built packages:"
    log_info "  - grapa_${VERSION}_arm64.deb"
    log_info "  - grapa_${VERSION}_amd64.deb"
}

validate_packages() {
    log_info "Validating Debian packages..."
    
    cd "$PACKAGE_DIR"
    
    # Check package structure
    for arch in arm64 amd64; do
        PACKAGE_FILE="grapa_${VERSION}_${arch}.deb"
        
        if [ ! -f "$PACKAGE_FILE" ]; then
            log_error "Package not found: $PACKAGE_FILE"; exit 1; fi
        
        log_info "Validating $PACKAGE_FILE..."
        
        # Check package contents
        dpkg-deb -I "$PACKAGE_FILE" >/dev/null 2>&1 || {
            log_error "Invalid package structure: $PACKAGE_FILE"; exit 1; }
        
        # Check control file
        dpkg-deb -I "$PACKAGE_FILE" control >/dev/null 2>&1 || {
            log_error "Invalid control file: $PACKAGE_FILE"; exit 1; }
        
        # Run lintian if available
        if command_exists lintian; then
            log_info "Running lintian checks on $PACKAGE_FILE..."
            if lintian "$PACKAGE_FILE"; then
                log_success "Lintian checks passed for $PACKAGE_FILE"
            else
                log_warning "Lintian found issues with $PACKAGE_FILE (non-critical)"
            fi
        fi
    done
    
    log_success "Package validation completed"
}

test_package_installation() {
    log_info "Testing package installation..."
    
    cd "$PACKAGE_DIR"
    
    # Test ARM64 package on ARM64 system
    if [ "$(uname -m)" = "aarch64" ]; then
        log_info "Testing ARM64 package installation..."
        
        # Remove existing package if installed
        sudo dpkg -r grapa 2>/dev/null || true
        
        # Install package
        if sudo dpkg -i "grapa_${VERSION}_arm64.deb"; then
            log_success "ARM64 package installation successful"
            
            # Test basic functionality
            if grapa --version | grep -q "$VERSION"; then
                log_success "Version check passed"
            else
                log_error "Version check failed"; exit 1; fi
            
            if echo "2+2" | grapa -c | grep -q "4"; then
                log_success "Basic functionality test passed"
            else
                log_error "Basic functionality test failed"; exit 1; fi
            
            # Clean up
            sudo dpkg -r grapa
        else
            log_error "ARM64 package installation failed"; exit 1; fi
    else
        log_warning "Skipping ARM64 installation test (not on ARM64 system)"
    fi
    
    log_success "Package installation testing completed"
}

create_ppa_submission() {
    log_info "Preparing PPA submission..."
    
    # Create submission directory
    SUBMISSION_DIR="$REPO_ROOT/temp/debian-submission"
    mkdir -p "$SUBMISSION_DIR"
    
    cd "$SUBMISSION_DIR"
    
    # Create source package structure
    mkdir -p "grapa-${VERSION}"
    cd "grapa-${VERSION}"
    
    # Copy source files (excluding source directory to avoid conflicts)
    cp -r "$REPO_ROOT/docs-src" .
    cp "$REPO_ROOT/README.md" .
    cp "$REPO_ROOT/LICENSE" .
    
    # Create a minimal source structure for PPA
    mkdir -p src
    echo "# Grapa Source Package" > src/README.md
    
    # Create debian directory
    mkdir -p debian
    cp -r "$DEBIAN_DIR/DEBIAN"/* debian/
    
    # Create source control file for PPA
    cat > debian/control << 'EOF'
Source: grapa
Section: devel
Priority: optional
Maintainer: Grapa Team <team@grapa-dev>
Homepage: https://github.com/grapa-dev/grapa

Package: grapa
Architecture: any
Depends: ${shlibs:Depends}, ${misc:Depends}
Description: Grapa Programming Language
 A modern, high-performance programming language with unlimited precision arithmetic,
 advanced vector operations, and comprehensive cryptographic capabilities.
 .
 Features:
  * Unlimited precision arithmetic for integers and floating-point numbers
  * Advanced vector and matrix operations with optimized algorithms
  * Comprehensive cryptography with OpenSSL 3.5.2 integration
  * Machine learning capabilities including linear regression
  * Unicode string support and advanced text processing
  * Cross-platform compatibility (Linux, macOS, Windows)
  * Python integration via GrapaPy
  * Command-line interface with script execution
  * Database integration with unified file/database API
 .
 Homepage: https://github.com/grapa-dev/grapa
EOF
    
    # Create source format file
    mkdir -p debian/source
    echo "3.0 (quilt)" > debian/source/format
    
    # Create source package
    tar -czf "../grapa_${VERSION}.orig.tar.gz" .
    
    # Create debian source package
    cd ..
    dpkg-source -b "grapa-${VERSION}"
    
    log_success "PPA submission package created"
    log_info "Files ready for PPA upload:"
    log_info "  - grapa_${VERSION}.orig.tar.gz"
    log_info "  - grapa_${VERSION}-1.dsc"
    log_info "  - grapa_${VERSION}-1.debian.tar.xz"
}

create_github_release_assets() {
    log_info "Preparing GitHub release assets..."
    
    cd "$PACKAGE_DIR"
    
    # Create release directory
    RELEASE_DIR="$REPO_ROOT/temp/debian-release"
    mkdir -p "$RELEASE_DIR"
    
    # Copy packages to release directory
    cp "grapa_${VERSION}_arm64.deb" "$RELEASE_DIR/"
    cp "grapa_${VERSION}_amd64.deb" "$RELEASE_DIR/"
    
    # Create installation instructions
    cat > "$RELEASE_DIR/INSTALL.md" << 'EOF'
# Grapa Debian Package Installation

## Quick Install

### Ubuntu/Debian ARM64
```bash
# Download and install
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.51/grapa_0.1.51_arm64.deb
sudo dpkg -i grapa_0.1.51_arm64.deb
```

### Ubuntu/Debian AMD64
```bash
# Download and install
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.51/grapa_0.1.51_amd64.deb
sudo dpkg -i grapa_0.1.51_amd64.deb
```

## Verify Installation
```bash
grapa --version
grapa -c "2+2"
```

## Uninstall
```bash
sudo dpkg -r grapa
```

## Package Information
- **Version**: 0.1.51
- **Architectures**: ARM64, AMD64
- **Dependencies**: None (static binary)
- **Size**: ~4.6MB (ARM64), ~4.2MB (AMD64)

## Features
- Unlimited precision arithmetic
- Advanced vector and matrix operations
- Comprehensive cryptography with OpenSSL 3.5.2
- Machine learning capabilities
- Cross-platform compatibility
- Python integration via GrapaPy

## Documentation
- Homepage: https://github.com/grapa-dev/grapa
- Documentation: https://github.com/grapa-dev/grapa/tree/main/docs-src
EOF
    
    log_success "GitHub release assets prepared"
    log_info "Release directory: $RELEASE_DIR"
}

show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Deploy Grapa Debian packages"
    echo ""
    echo "OPTIONS:"
    echo "  --build-only      Only build packages (no testing or submission)"
    echo "  --test-only       Build and test packages locally"
    echo "  --create-ppa      Create PPA submission package"
    echo "  --create-release  Create GitHub release assets"
    echo "  --full            Full deployment (build, test, create assets)"
    echo "  -h, --help        Show this help message"
    echo ""
    echo "EXAMPLES:"
    echo "  $0 --build-only     Build packages only"
    echo "  $0 --test-only      Build and test packages"
    echo "  $0 --create-ppa     Create PPA submission package"
    echo "  $0 --create-release Create GitHub release assets"
    echo "  $0 --full           Full deployment process"
    echo ""
    echo "Note: PPA submission requires manual upload to Launchpad."
}

cleanup() {
    log_info "Cleaning up temporary files..."
    
    # Remove backup files
    find "$PACKAGE_DIR" -name "*.bak" -delete
    
    # Remove temporary directories
    if [ -d "$REPO_ROOT/temp/debian-submission" ]; then
        rm -rf "$REPO_ROOT/temp/debian-submission"
    fi
    
    if [ -d "$REPO_ROOT/temp/debian-release" ]; then
        rm -rf "$REPO_ROOT/temp/debian-release"
    fi
    
    log_success "Cleanup completed"
}

main() {
    BUILD_ONLY=false
    TEST_ONLY=false
    CREATE_PPA=false
    CREATE_RELEASE=false
    FULL=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --build-only)
                BUILD_ONLY=true
                shift
                ;;
            --test-only)
                TEST_ONLY=true
                shift
                ;;
            --create-ppa)
                CREATE_PPA=true
                shift
                ;;
            --create-release)
                CREATE_RELEASE=true
                shift
                ;;
            --full)
                FULL=true
                shift
                ;;
            -h|--help)
                show_usage
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    log_info "Starting Grapa Debian package deployment..."
    log_info "Package directory: $PACKAGE_DIR"
    log_info "Version: $VERSION"
    
    validate_prerequisites
    build_packages
    
    if [[ "$BUILD_ONLY" == "true" ]]; then
        log_success "Package building completed!"
        cleanup
        exit 0
    fi
    
    validate_packages
    
    if [[ "$TEST_ONLY" == "true" ]]; then
        test_package_installation
        log_success "Package testing completed!"
        cleanup
        exit 0
    fi
    
    if [[ "$CREATE_PPA" == "true" || "$FULL" == "true" ]]; then
        create_ppa_submission
    fi
    
    if [[ "$CREATE_RELEASE" == "true" || "$FULL" == "true" ]]; then
        test_package_installation
        create_github_release_assets
    fi
    
    if [[ "$FULL" == "true" ]]; then
        log_success "Full Debian deployment completed!"
        log_info "Next steps:"
        log_info "1. Upload PPA files to Launchpad (if --create-ppa was used)"
        log_info "2. Upload release assets to GitHub (if --create-release was used)"
        log_info "3. Submit package to Ubuntu/Debian repositories"
    else
        log_success "Deployment completed successfully!"
    fi
    
    cleanup
}

main "$@" 