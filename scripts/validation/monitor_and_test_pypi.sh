#!/bin/bash

# GrapaPy PyPI Monitor and Test Script
# This script monitors PyPI for new versions and tests deployment across all platforms

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to get current version from setup.py
get_current_version() {
    grep 'grapapy_version = "' setup.py | sed 's/.*grapapy_version = "\([^"]*\)".*/\1/'
}

# Function to check if version is available on PyPI
check_pypi_version() {
    local version=$1
    print_status "Checking if version $version is available on PyPI..."
    
    # Clear pip cache to ensure we get the latest version
    pip cache purge 2>/dev/null || true
    
    # Check if version is available
    if pip index versions grapapy 2>/dev/null | grep -q "$version"; then
        print_success "Version $version is available on PyPI"
        return 0
    else
        print_warning "Version $version is not yet available on PyPI"
        return 1
    fi
}

# Function to wait for version to be available on PyPI
wait_for_pypi_version() {
    local version=$1
    local max_attempts=${2:-30}  # Default 30 attempts (5 minutes)
    local attempt=1
    
    print_status "Waiting for version $version to be available on PyPI..."
    
    while [[ $attempt -le $max_attempts ]]; do
        if check_pypi_version "$version"; then
            print_success "Version $version is now available on PyPI!"
            return 0
        fi
        
        print_status "Attempt $attempt/$max_attempts: Version not yet available, waiting 10 seconds..."
        sleep 10
        ((attempt++))
    done
    
    print_error "Version $version did not become available on PyPI after $max_attempts attempts"
    return 1
}

# Function to test GrapaPy installation on current platform
test_grapapy_installation() {
    local version=$1
    print_status "Testing GrapaPy installation on current platform..."
    
    # Clear pip cache
    pip cache purge 2>/dev/null || true
    
    # Install GrapaPy
    print_status "Installing GrapaPy version $version..."
    pip install --no-cache-dir grapapy --force-reinstall
    
    # Test basic functionality
    print_status "Testing basic functionality..."
    python3 -c "
import grapapy
xy = grapapy.grapa()
result = xy.eval('2 + 2;')
print(f'GrapaPy test: 2 + 2 = {result}')
assert result == 4, 'Basic functionality test failed'
print('✅ Basic functionality test passed!')

# Test file operations
xy.eval('\\$global.fs = \\$file();')
xy.eval('fs.set(\"test.txt\", \"Hello from GrapaPy!\");')
content = xy.eval('fs.get(\"test.txt\");')
if isinstance(content, bytes):
    content = content.decode('utf-8')
print(f'File content: {content}')
assert content == 'Hello from GrapaPy!', 'File operations test failed'
xy.eval('fs.remove(\"test.txt\");')
print('✅ File operations test passed!')

# Test functional methods
result = xy.eval('[1, 2, 3, 4, 5].map(op(x) { x * 2; });')
print(f'Map result: {result}')
assert result == [2, 4, 6, 8, 10], 'Map test failed'
print('✅ Functional methods test passed!')

print('✅ All tests passed!')
"
    
    print_success "GrapaPy installation test passed on current platform"
}

# Function to test on Linux AMD64
test_linux_amd64() {
    print_status "Testing on Linux AMD64..."
    
    # Check if we're on Linux AMD64
    if [[ "$(uname -s)" == "Linux" && "$(uname -m)" == "x86_64" ]]; then
        test_grapapy_installation "$1"
    else
        print_warning "Not on Linux AMD64, skipping test"
    fi
}

# Function to test on Linux ARM64
test_linux_arm64() {
    print_status "Testing on Linux ARM64..."
    
    # Check if we're on Linux ARM64
    if [[ "$(uname -s)" == "Linux" && "$(uname -m)" == "aarch64" ]]; then
        test_grapapy_installation "$1"
    else
        print_warning "Not on Linux ARM64, skipping test"
    fi
}

# Function to test on macOS AMD64
test_macos_amd64() {
    print_status "Testing on macOS AMD64..."
    
    # Check if we're on macOS AMD64
    if [[ "$(uname -s)" == "Darwin" && "$(uname -m)" == "x86_64" ]]; then
        test_grapapy_installation "$1"
    else
        print_warning "Not on macOS AMD64, skipping test"
    fi
}

# Function to test on macOS ARM64
test_macos_arm64() {
    print_status "Testing on macOS ARM64..."
    
    # Check if we're on macOS ARM64
    if [[ "$(uname -s)" == "Darwin" && "$(uname -m)" == "arm64" ]]; then
        test_grapapy_installation "$1"
    else
        print_warning "Not on macOS ARM64, skipping test"
    fi
}

# Function to test on Windows AMD64 (using Wine if available)
test_windows_amd64() {
    print_status "Testing on Windows AMD64..."
    
    # Check if we're on Windows
    if [[ "$(uname -s)" == "MINGW"* || "$(uname -s)" == "MSYS"* ]]; then
        test_grapapy_installation "$1"
    elif command_exists wine; then
        print_status "Using Wine to test Windows compatibility..."
        
        # Install Python for Windows via Wine
        # Note: This is a simplified test - in practice, you'd need a proper Windows Python installation
        print_warning "Windows testing via Wine requires additional setup"
        print_warning "Skipping Windows test - requires native Windows environment"
    else
        print_warning "Not on Windows and Wine not available, skipping Windows test"
    fi
}

# Function to test all platforms
test_all_platforms() {
    local version=$1
    print_status "Testing GrapaPy on all supported platforms..."
    
    # Test current platform
    test_grapapy_installation "$version"
    
    # Test other platforms if we're on them
    test_linux_amd64 "$version"
    test_linux_arm64 "$version"
    test_macos_amd64 "$version"
    test_macos_arm64 "$version"
    test_windows_amd64 "$version"
    
    print_success "Platform testing completed"
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -v, --version VERSION    Specify version to monitor and test"
    echo "  --wait                   Wait for version to become available on PyPI"
    echo "  --test-only              Test existing installation only"
    echo "  --monitor-only           Monitor PyPI only (don't test)"
    echo "  -h, --help              Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 -v 0.0.252                    # Monitor and test version 0.0.252"
    echo "  $0 -v 0.0.252 --wait            # Wait for version and test"
    echo "  $0 --test-only                   # Test current installation"
    echo "  $0 --monitor-only                # Monitor PyPI only"
}

# Function to check minimum requirements
check_minimum_requirements() {
    print_status "Checking minimum requirements for GrapaPy installation..."
    
    # Check Python version
    python_version=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
    print_status "Python version: $python_version"
    
    # Check pip
    if command_exists pip; then
        print_success "pip is available"
    else
        print_error "pip is not available"
        exit 1
    fi
    
    # Check platform-specific requirements
    if [[ "$(uname -s)" == "Linux" ]]; then
        print_status "Linux detected - checking for development tools..."
        if command_exists gcc; then
            print_success "GCC is available"
        else
            print_warning "GCC not found - may need to install build-essential"
        fi
    elif [[ "$(uname -s)" == "Darwin" ]]; then
        print_status "macOS detected - checking for Xcode Command Line Tools..."
        if command_exists clang; then
            print_success "Clang is available"
        else
            print_warning "Clang not found - may need to install Xcode Command Line Tools"
        fi
    elif [[ "$(uname -s)" == "MINGW"* || "$(uname -s)" == "MSYS"* ]]; then
        print_status "Windows detected - checking for Visual Studio Build Tools..."
        if command_exists cl; then
            print_success "MSVC compiler is available"
        else
            print_warning "MSVC compiler not found - may need Visual Studio Build Tools"
        fi
    fi
    
    print_success "Minimum requirements check completed"
}

# Main script
main() {
    local version=""
    local wait_for_version=false
    local test_only=false
    local monitor_only=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -v|--version)
                version="$2"
                shift 2
                ;;
            --wait)
                wait_for_version=true
                shift
                ;;
            --test-only)
                test_only=true
                shift
                ;;
            --monitor-only)
                monitor_only=true
                shift
                ;;
            -h|--help)
                show_usage
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    # Get current version if not specified
    if [[ -z "$version" ]]; then
        version=$(get_current_version)
        print_status "Using current version: $version"
    fi
    
    # Check minimum requirements
    check_minimum_requirements
    
    # Execute requested operations
    if [[ "$test_only" == true ]]; then
        test_all_platforms "$version"
    elif [[ "$monitor_only" == true ]]; then
        if [[ "$wait_for_version" == true ]]; then
            wait_for_pypi_version "$version"
        else
            check_pypi_version "$version"
        fi
    else
        # Full monitor and test
        if [[ "$wait_for_version" == true ]]; then
            wait_for_pypi_version "$version"
        else
            check_pypi_version "$version"
        fi
        
        if [[ $? -eq 0 ]]; then
            test_all_platforms "$version"
        else
            print_error "Version $version is not available on PyPI"
            exit 1
        fi
    fi
    
    print_success "All operations completed successfully!"
}

# Run main function with all arguments
main "$@" 