#!/bin/bash

# GrapaPy Build and Deploy to PyPI Script
# This script builds the Python package and deploys it to PyPI

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

# Function to check if we're in a git repository
check_git_repo() {
    if ! git rev-parse --git-dir > /dev/null 2>&1; then
        print_error "Not in a git repository"
        exit 1
    fi
}

# Function to get current version from setup.py
get_current_version() {
    grep 'grapapy_version = "' setup.py | sed 's/.*grapapy_version = "\([^"]*\)".*/\1/'
}

# Function to check if version is already on PyPI
check_pypi_version() {
    local version=$1
    print_status "Checking if version $version is already on PyPI..."
    
    if pip index versions grapapy 2>/dev/null | grep -q "$version"; then
        print_warning "Version $version already exists on PyPI"
        return 0
    else
        print_success "Version $version is not on PyPI"
        return 1
    fi
}

# Function to build Python package
build_package() {
    local version=$1
    print_status "Building Python package for version $version..."
    
    # Clean previous builds
    rm -rf dist/ build/ *.egg-info/
    
    # Update version in setup.py if provided
    if [[ -n "$version" ]]; then
        print_status "Updating version to $version in setup.py..."
        sed -i.bak "s/grapapy_version = \"[^\"]*\"/grapapy_version = \"$version\"/" setup.py
        rm setup.py.bak
    fi
    
    # Build wheel and source distribution
    python3 setup.py bdist_wheel sdist
    
    print_success "Package built successfully"
    echo "Built distributions:"
    ls -la dist/
}

# Function to test package locally
test_package() {
    print_status "Testing package locally..."
    
    # Install the built package
    pip install dist/*.whl --force-reinstall
    
    # Test basic functionality
    python3 -c "
import grapapy
xy = grapapy.grapa()
result = xy.eval('2 + 2;')
print(f'GrapaPy test: 2 + 2 = {result}')
assert result == 4, 'Basic functionality test failed'
print('✅ Local package test passed!')
"
    
    print_success "Local package test passed"
}

# Function to deploy to PyPI
deploy_to_pypi() {
    local version=$1
    print_status "Deploying version $version to PyPI..."
    
    # Check if twine is installed
    if ! command_exists twine; then
        print_error "twine is not installed. Please install it with: pip install twine"
        exit 1
    fi
    
    # Upload to PyPI
    twine upload dist/*
    
    print_success "Package uploaded to PyPI successfully"
}

# Function to verify PyPI deployment
verify_pypi_deployment() {
    local version=$1
    print_status "Verifying PyPI deployment for version $version..."
    
    # Wait for PyPI to process the upload
    print_status "Waiting for PyPI to process upload..."
    sleep 30
    
    # Check if the package is available on PyPI
    pip install --upgrade pip
    pip install --no-cache-dir grapapy --force-reinstall
    
    # Test basic functionality
    python3 -c "
import grapapy
xy = grapapy.grapa()
result = xy.eval('2 + 2;')
print(f'GrapaPy test: 2 + 2 = {result}')
assert result == 4, 'Basic functionality test failed'
print('✅ PyPI deployment verified successfully!')
"
    
    print_success "PyPI deployment verified successfully"
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -v, --version VERSION    Specify version to build and deploy"
    echo "  --build-only             Build package only (don't deploy)"
    echo "  --test-only              Test existing package only"
    echo "  --deploy-only            Deploy existing package only"
    echo "  --verify-only            Verify PyPI deployment only"
    echo "  -h, --help              Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 -v 0.0.252                    # Build and deploy version 0.0.252"
    echo "  $0 --build-only                  # Build package only"
    echo "  $0 --test-only                   # Test existing package"
    echo "  $0 --deploy-only                 # Deploy existing package"
    echo "  $0 --verify-only                 # Verify PyPI deployment"
}

# Main script
main() {
    local version=""
    local build_only=false
    local test_only=false
    local deploy_only=false
    local verify_only=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -v|--version)
                version="$2"
                shift 2
                ;;
            --build-only)
                build_only=true
                shift
                ;;
            --test-only)
                test_only=true
                shift
                ;;
            --deploy-only)
                deploy_only=true
                shift
                ;;
            --verify-only)
                verify_only=true
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
    
    # Check if we're in a git repository
    check_git_repo
    
    # Get current version if not specified
    if [[ -z "$version" ]]; then
        version=$(get_current_version)
        print_status "Using current version: $version"
    fi
    
    # Check if version is already on PyPI
    if check_pypi_version "$version"; then
        print_warning "Version $version already exists on PyPI"
        read -p "Do you want to continue anyway? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_status "Aborted by user"
            exit 0
        fi
    fi
    
    # Execute requested operations
    if [[ "$test_only" == true ]]; then
        test_package
    elif [[ "$verify_only" == true ]]; then
        verify_pypi_deployment "$version"
    elif [[ "$deploy_only" == true ]]; then
        deploy_to_pypi "$version"
        verify_pypi_deployment "$version"
    elif [[ "$build_only" == true ]]; then
        build_package "$version"
        test_package
    else
        # Full build and deploy
        build_package "$version"
        test_package
        deploy_to_pypi "$version"
        verify_pypi_deployment "$version"
    fi
    
    print_success "All operations completed successfully!"
}

# Run main function with all arguments
main "$@" 