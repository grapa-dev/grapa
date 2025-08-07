#!/bin/bash

# GrapaPy PyPI Deployment Script
# This script deploys to PyPI after build_all_platforms.sh has completed successfully
# Assumes all platform artifacts are committed to GitHub

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

# Function to monitor GitHub Actions workflow
monitor_workflow() {
    print_status "Monitoring GitHub Actions workflow..."
    
    # Wait for workflow to start
    print_status "Waiting for workflow to start..."
    sleep 10
    
    # Monitor workflow status
    local max_attempts=60  # 10 minutes
    local attempt=1
    
    while [[ $attempt -le $max_attempts ]]; do
        print_status "Checking workflow status (attempt $attempt/$max_attempts)..."
        
        # Check if workflow is running
        if gh run list --limit=1 --json status,conclusion,displayTitle | grep -q "deploy-pypi"; then
            local status=$(gh run list --limit=1 --json status,conclusion,displayTitle | jq -r '.[0].status')
            local conclusion=$(gh run list --limit=1 --json status,conclusion,displayTitle | jq -r '.[0].conclusion')
            
            if [[ "$status" == "completed" ]]; then
                if [[ "$conclusion" == "success" ]]; then
                    print_success "GitHub Actions workflow completed successfully!"
                    return 0
                else
                    print_error "GitHub Actions workflow failed"
                    return 1
                fi
            elif [[ "$status" == "in_progress" ]]; then
                print_status "Workflow is still running..."
            fi
        else
            print_status "Waiting for workflow to appear..."
        fi
        
        sleep 10
        ((attempt++))
    done
    
    print_error "Workflow monitoring timed out after $max_attempts attempts"
    return 1
}

# Function to monitor PyPI for version availability
monitor_pypi_version() {
    local version=$1
    local max_attempts=30  # 5 minutes
    local attempt=1
    
    print_status "Monitoring PyPI for version $version availability..."
    
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

# Function to test PyPI installation on all platforms
test_pypi_installation() {
    local version=$1
    print_status "Testing PyPI installation on current platform..."
    
    # Test current platform
    print_status "Testing on current platform..."
    
    # Clear pip cache
    pip cache purge 2>/dev/null || true
    
    # Install from PyPI
    print_status "Installing grapapy from PyPI..."
    pip install --no-cache-dir grapapy --force-reinstall
    
    # Test basic functionality
    print_status "Testing basic functionality..."
    python3 -c "
import grapapy
xy = grapapy.grapa()
result = xy.eval('2 + 2;')
print(f'GrapaPy test: 2 + 2 = {result}')
assert result == 4, 'Basic functionality test failed'
print('✅ PyPI installation test passed!')
"
    
    print_success "PyPI installation test passed on current platform"
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --monitor-only           Monitor PyPI only (don't deploy)"
    echo "  --test-only              Test PyPI installation only"
    echo "  -h, --help              Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                       # Deploy current version (auto-detected from setup.py)"
    echo "  $0 --monitor-only        # Monitor PyPI only"
    echo "  $0 --test-only           # Test PyPI installation only"
    echo ""
    echo "Prerequisites:"
    echo "  - build_all_platforms.sh must have completed successfully"
    echo "  - All platform artifacts must be committed to GitHub"
    echo "  - Version must be set in setup.py"
    echo "  - GitHub CLI (gh) must be installed and authenticated"
    echo "  - PyPI API token must be configured in GitHub secrets"
}

# Main script
main() {
    local monitor_only=false
    local test_only=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --monitor-only)
                monitor_only=true
                shift
                ;;
            --test-only)
                test_only=true
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
    
    # Get current version from setup.py for monitoring/testing
    print_status "Auto-detecting current version from setup.py..."
    version=$(get_current_version)
    
    if [[ -z "$version" ]]; then
        print_error "Could not determine version from setup.py"
        print_status "Make sure build_all_platforms.sh has completed successfully"
        exit 1
    fi
    
    print_status "Current version: $version"
    
    # Validate version format
    if [[ ! $version =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        print_error "Invalid version format in setup.py: $version"
        exit 1
    fi
    
    # Check prerequisites
    print_status "Checking prerequisites..."
    
    # Check if GitHub CLI is installed
    if ! command_exists gh; then
        print_error "GitHub CLI (gh) is not installed. Please install it first."
        exit 1
    fi
    
    # Check if we're authenticated with GitHub
    if ! gh auth status >/dev/null 2>&1; then
        print_error "Not authenticated with GitHub. Please run 'gh auth login' first."
        exit 1
    fi
    
    # Check if version is already on PyPI
    if check_pypi_version "$version"; then
        print_warning "Version $version already exists on PyPI"
        print_status "Continuing with testing..."
    fi
    
    # Execute requested operations
    if [[ "$test_only" == true ]]; then
        test_pypi_installation "$version"
    elif [[ "$monitor_only" == true ]]; then
        monitor_pypi_version "$version"
    else
        # Full deployment process
        print_status "Starting PyPI deployment process for version $version..."
        
        # Step 1: Trigger PyPI deployment workflow
        print_status "Triggering PyPI deployment workflow..."
        if gh workflow run "Deploy to PyPI" --field confirm="YES"; then
            print_success "PyPI deployment workflow triggered successfully"
            print_status "The workflow will auto-detect the version from setup.py"
        else
            print_error "Failed to trigger PyPI deployment workflow"
            exit 1
        fi
        
        # Step 2: Monitor GitHub Actions workflow
        monitor_workflow
        
        # Step 3: Monitor PyPI for version availability
        monitor_pypi_version "$version"
        
        # Step 4: Test PyPI installation
        test_pypi_installation "$version"
    fi
    
    print_success "All operations completed successfully!"
}

# Run main function with all arguments
main "$@" 