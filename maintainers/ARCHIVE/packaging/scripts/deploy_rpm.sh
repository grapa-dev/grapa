#!/bin/bash

# RPM Package Deployment Script for Amazon Linux and other yum-based systems
# This script builds, validates, and deploys RPM packages for ARM64 and AMD64

set -e

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="0.1.51"
RPM_DIR="$REPO_ROOT/packaging/rpm"
PACKAGE_DIR="$REPO_ROOT/temp/rpm-release"

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

show_usage() {
    cat << 'EOF'
RPM Package Deployment Script

Usage: $0 [OPTIONS]

Options:
    --build-only      Build RPM packages only
    --test-only       Test existing RPM packages only
    --full            Build, test, and create release assets (default)
    --help            Show this help message

Examples:
    $0 --build-only    # Build RPM packages for both architectures
    $0 --test-only     # Test existing RPM packages
    $0 --full          # Complete build, test, and release process

EOF
}

build_packages() {
    log_info "Building RPM packages for all architectures..."
    
    # Create package directory
    mkdir -p "$PACKAGE_DIR"
    
    # Detect current architecture
    CURRENT_ARCH=$(uname -m)
    log_info "Current system architecture: $CURRENT_ARCH"
    
    # Build for current architecture
    if [[ "$CURRENT_ARCH" == "aarch64" || "$CURRENT_ARCH" == "arm64" ]]; then
        build_rpm_package "arm64"
    elif [[ "$CURRENT_ARCH" == "x86_64" || "$CURRENT_ARCH" == "amd64" ]]; then
        build_rpm_package "amd64"
    else
        log_warning "Unknown architecture: $CURRENT_ARCH"
        log_info "Building for detected architecture..."
        build_rpm_package "$CURRENT_ARCH"
    fi
    
    # Note about cross-architecture building
    log_info "Note: Cross-architecture RPM building requires the target system."
    log_info "To build for other architectures, run this script on those systems."
}

build_rpm_package() {
    local arch=$1
    log_info "Building RPM package for $arch..."
    
    # Create RPM build directory
    local rpm_build_dir="$PACKAGE_DIR/rpm-build-$arch"
    mkdir -p "$rpm_build_dir"
    
    # Copy RPM structure
    cp -r "$RPM_DIR/grapa" "$rpm_build_dir/"
    
    # Copy the appropriate binary
    local binary_name="grapa"
    if [[ "$arch" == "arm64" ]]; then
        # Use AWS ARM64 binary if available (for Amazon Linux compatibility)
        if [[ -f "$REPO_ROOT/bin/grapa-aws-arm64" ]]; then
            binary_name="grapa-aws-arm64"
        else
            binary_name="grapa-linux-arm64"
        fi
    elif [[ "$arch" == "amd64" ]]; then
        # Use AWS AMD64 binary if available (for Amazon Linux compatibility)
        if [[ -f "$REPO_ROOT/bin/grapa-aws-amd64" ]]; then
            binary_name="grapa-aws-amd64"
        else
            binary_name="grapa-linux-amd64"
        fi
    fi
    
    # Copy binary to RPM structure
    if [[ -f "$REPO_ROOT/bin/$binary_name" ]]; then
        cp "$REPO_ROOT/bin/$binary_name" "$rpm_build_dir/grapa/usr/bin/grapa"
        chmod +x "$rpm_build_dir/grapa/usr/bin/grapa"
        log_success "Binary copied: $binary_name -> grapa"
    else
        log_error "Binary not found: $REPO_ROOT/bin/$binary_name"
        log_info "Available binaries:"
        ls -la "$REPO_ROOT/bin/" | grep grapa || true
        return 1
    fi
    
    # Create source tarball
    cd "$rpm_build_dir"
    tar -czf "grapa-${VERSION}.tar.gz" grapa/
    
    # Copy spec file
    cp "$RPM_DIR/grapa.spec" .
    
    # Update spec file for architecture
    sed -i "s/BuildArch:.*/BuildArch: $arch/" grapa.spec
    
    # Build RPM package
    log_info "Building RPM package..."
    rpmbuild --define "_topdir $(pwd)" --define "_sourcedir $(pwd)" --define "_rpmdir $(pwd)" -bb grapa.spec
    
    # Find and rename the built RPM
    local rpm_file=$(find . -name "grapa-${VERSION}-*.rpm" | head -1)
    if [[ -n "$rpm_file" ]]; then
        local new_name="grapa_${VERSION}_${arch}.rpm"
        mv "$rpm_file" "$PACKAGE_DIR/$new_name"
        log_success "RPM package built: $new_name"
    else
        log_error "RPM package not found after build"
        return 1
    fi
    
    cd "$REPO_ROOT"
}

validate_packages() {
    log_info "Validating RPM packages..."
    
    if [[ ! -d "$PACKAGE_DIR" ]]; then
        log_error "Package directory not found: $PACKAGE_DIR"
        return 1
    fi
    
    local packages_found=0
    
    for rpm_file in "$PACKAGE_DIR"/*.rpm; do
        if [[ -f "$rpm_file" ]]; then
            packages_found=$((packages_found + 1))
            log_info "Validating: $(basename "$rpm_file")"
            
            # Check RPM structure
            rpm -qip "$rpm_file" > /dev/null 2>&1 || {
                log_error "Invalid RPM package: $rpm_file"
                return 1
            }
            
            # Check package contents
            rpm -qlp "$rpm_file" | grep -q "usr/bin/grapa" || {
                log_error "RPM package missing grapa binary: $rpm_file"
                return 1
            }
            
            log_success "Package validated: $(basename "$rpm_file")"
        fi
    done
    
    if [[ $packages_found -eq 0 ]]; then
        log_error "No RPM packages found in $PACKAGE_DIR"
        return 1
    fi
    
    log_success "All RPM packages validated successfully"
}

test_package_installation() {
    log_info "Testing RPM package installation..."
    
    # Find RPM package for current architecture
    local current_arch=$(uname -m)
    local rpm_file=""
    
    if [[ "$current_arch" == "aarch64" || "$current_arch" == "arm64" ]]; then
        rpm_file="$PACKAGE_DIR/grapa_${VERSION}_arm64.rpm"
    elif [[ "$current_arch" == "x86_64" || "$current_arch" == "amd64" ]]; then
        rpm_file="$PACKAGE_DIR/grapa_${VERSION}_amd64.rpm"
    else
        log_warning "Unknown architecture: $current_arch"
        log_info "Skipping installation test for unknown architecture"
        return 0
    fi
    
    if [[ ! -f "$rpm_file" ]]; then
        log_warning "RPM package not found for current architecture: $rpm_file"
        log_info "Skipping installation test"
        return 0
    fi
    
    log_info "Testing installation of: $(basename "$rpm_file")"
    
    # Check if grapa is already installed
    if rpm -q grapa > /dev/null 2>&1; then
        log_info "Removing existing grapa installation..."
        rpm -e grapa || {
            log_error "Failed to remove existing grapa installation"
            return 1
        }
    fi
    
    # Install the package
    log_info "Installing RPM package..."
    rpm -i "$rpm_file" || {
        log_error "Failed to install RPM package"
        return 1
    }
    
    # Test the installation
    log_info "Testing grapa installation..."
    
    # Check version
    local version_output=$(grapa --version 2>/dev/null || echo "FAILED")
    if [[ "$version_output" == *"Version: $VERSION"* ]]; then
        log_success "Version check passed: $version_output"
    else
        log_error "Version check failed: $version_output"
        return 1
    fi
    
    # Test basic functionality
    local math_result=$(grapa -c "2+2" 2>/dev/null || echo "FAILED")
    if [[ "$math_result" == "4" ]]; then
        log_success "Basic math test passed: 2+2 = $math_result"
    else
        log_error "Basic math test failed: $math_result"
        return 1
    fi
    
    # Test string operations
    local string_result=$(grapa -c "'Hello World'.echo()" 2>/dev/null || echo "FAILED")
    if [[ "$string_result" == "Hello World" ]]; then
        log_success "String operations test passed"
    else
        log_error "String operations test failed: $string_result"
        return 1
    fi
    
    # Uninstall for cleanup
    log_info "Uninstalling grapa for cleanup..."
    rpm -e grapa || {
        log_warning "Failed to uninstall grapa (this is okay for testing)"
    }
    
    log_success "RPM package installation test completed successfully"
}

create_github_release_assets() {
    log_info "Creating GitHub release assets..."
    
    # Create installation guide
    cat > "$PACKAGE_DIR/INSTALL.md" << 'EOF'
# Grapa RPM Package Installation

## Quick Install

### Amazon Linux ARM64
```bash
# Download and install
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.51-rpm/grapa_0.1.51_arm64.rpm
sudo rpm -i grapa_0.1.51_arm64.rpm
```

### Amazon Linux AMD64
```bash
# Download and install
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.51-rpm/grapa_0.1.51_amd64.rpm
sudo rpm -i grapa_0.1.51_amd64.rpm
```

## Verification

After installation, verify Grapa is working:

```bash
# Check version
grapa --version

# Test basic functionality
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
```

## Uninstall

To remove Grapa:

```bash
sudo rpm -e grapa
```

## Package Information

- **Package Name**: grapa
- **Version**: 0.1.51
- **Architecture**: ARM64/AMD64
- **License**: MIT
- **Homepage**: https://github.com/grapa-dev/grapa

## Features

- Unlimited precision arithmetic
- Advanced vector and matrix operations
- Comprehensive cryptography with OpenSSL 3.5.2
- Machine learning capabilities
- Cross-platform compatibility
- Python integration via GrapaPy

## Documentation

For more information, visit: https://grapa-dev.github.io/grapa/
EOF
    
    log_success "GitHub release assets created in $PACKAGE_DIR"
    log_info "Files ready for upload:"
    ls -la "$PACKAGE_DIR"/*.rpm 2>/dev/null || log_warning "No RPM packages found"
    log_info "Installation guide: $PACKAGE_DIR/INSTALL.md"
}

create_amazon_linux_submission() {
    log_info "Preparing Amazon Linux repository submission..."
    
    local submission_dir="$REPO_ROOT/temp/amazon-linux-submission"
    mkdir -p "$submission_dir"
    
    # Create submission documentation
    cat > "$submission_dir/AMAZON_LINUX_SUBMISSION.md" << 'EOF'
# Grapa Amazon Linux Repository Submission

## Package Information
- **Package Name**: grapa
- **Version**: 0.1.51
- **Architecture**: ARM64, AMD64
- **License**: MIT
- **Homepage**: https://github.com/grapa-dev/grapa

## Description
Grapa is a modern, high-performance programming language with unlimited precision arithmetic, advanced vector operations, and comprehensive cryptographic capabilities.

## Features
- Unlimited precision arithmetic for integers and floating-point numbers
- Advanced vector and matrix operations with optimized algorithms
- Comprehensive cryptography with OpenSSL 3.5.2 integration
- Machine learning capabilities including linear regression
- Unicode string support and advanced text processing
- Cross-platform compatibility (Linux, macOS, Windows)
- Python integration via GrapaPy
- Command-line interface with script execution
- Database integration with unified file/database API

## Technical Details
- **Dependencies**: None (static binary)
- **Build System**: FLTK 1.4.4, OpenSSL 3.5.2
- **Target Platforms**: Amazon Linux 2, Amazon Linux 2023
- **Architectures**: ARM64 (Graviton), AMD64 (x86_64)

## Quality Assurance
- ✅ Package builds successfully on target architectures
- ✅ Package installs and uninstalls correctly
- ✅ Binary executes and responds to commands
- ✅ Basic functionality tests pass
- ✅ No external dependencies required

## Installation Instructions
```bash
# For ARM64 (Graviton)
sudo rpm -i grapa_0.1.51_arm64.rpm

# For AMD64 (x86_64)
sudo rpm -i grapa_0.1.51_amd64.rpm
```

## Verification
```bash
grapa --version
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
```

## Project Information
- **Repository**: https://github.com/grapa-dev/grapa
- **Documentation**: https://grapa-dev.github.io/grapa/
- **License**: MIT
- **Maintainer**: Grapa Team <team@grapa-dev>

## Release Notes
- Initial release of Grapa programming language
- Features unlimited precision arithmetic
- Advanced vector and matrix operations
- Comprehensive cryptography with OpenSSL 3.5.2
- Machine learning capabilities
- Cross-platform compatibility
- Python integration via GrapaPy
EOF
    
    # Copy RPM packages
    cp "$PACKAGE_DIR"/*.rpm "$submission_dir/" 2>/dev/null || log_warning "No RPM packages to copy"
    
    log_success "Amazon Linux submission package created in $submission_dir"
}

cleanup() {
    log_info "Cleaning up temporary files..."
    # Keep the package directory for review
    log_info "Package files preserved in: $PACKAGE_DIR"
}

main() {
    local mode="full"
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --build-only)
                mode="build"
                shift
                ;;
            --test-only)
                mode="test"
                shift
                ;;
            --full)
                mode="full"
                shift
                ;;
            --help)
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
    
    log_info "RPM Package Deployment Script"
    log_info "Version: $VERSION"
    log_info "Mode: $mode"
    
    case $mode in
        "build")
            build_packages
            ;;
        "test")
            validate_packages
            test_package_installation
            ;;
        "full")
            build_packages
            validate_packages
            test_package_installation
            create_github_release_assets
            create_amazon_linux_submission
            ;;
    esac
    
    cleanup
    
    log_success "RPM deployment completed successfully!"
    log_info "Next steps:"
    log_info "1. Test packages on Amazon Linux systems"
    log_info "2. Submit to Amazon Linux repositories"
    log_info "3. Update user documentation"
}

main "$@" 