#!/bin/bash

# Grapa VSCode Extension Deployment Script
# Builds and publishes the VSCode extension to the Visual Studio Marketplace.
# Requires VSCE_PAT GitHub secret to be configured.

set -e  # Exit on any error

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
EXTENSION_DIR="$REPO_ROOT/extensions/vscode-grapa"

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
    if ! command_exists node; then
        log_error "Node.js is not installed or not in PATH"; exit 1; fi
    if ! command_exists npm; then
        log_error "npm is not installed or not in PATH"; exit 1; fi
    if ! command_exists git; then
        log_error "git is not installed or not in PATH"; exit 1; fi
    if [ ! -d "$EXTENSION_DIR" ]; then
        log_error "Extension directory not found: $EXTENSION_DIR"; exit 1; fi
    if [ ! -f "$EXTENSION_DIR/package.json" ]; then
        log_error "Extension package.json not found: $EXTENSION_DIR/package.json"; exit 1; fi
    log_success "Prerequisites validated successfully"
}

build_extension() {
    log_info "Building VSCode extension..."
    cd "$EXTENSION_DIR"
    
    log_info "Installing dependencies..."
    if npm ci; then
        log_success "Dependencies installed successfully"
    else
        log_error "Failed to install dependencies"; exit 1; fi
    
    log_info "Compiling TypeScript..."
    if npm run compile; then
        log_success "TypeScript compilation completed"
    else
        log_error "Failed to compile TypeScript"; exit 1; fi
    
    log_info "Installing VSCE..."
    if npm install -g @vscode/vsce; then
        log_success "VSCE installed successfully"
    else
        log_error "Failed to install VSCE"; exit 1; fi
    
    log_info "Packaging extension..."
    if vsce package; then
        log_success "Extension packaged successfully"
    else
        log_error "Failed to package extension"; exit 1; fi
    
    cd "$REPO_ROOT"
}

validate_extension() {
    log_info "Validating extension package..."
    cd "$EXTENSION_DIR"
    
    # Check if .vsix file was created
    local vsix_files=(*.vsix)
    if [ ${#vsix_files[@]} -eq 0 ]; then
        log_error "No .vsix file found after packaging"; exit 1; fi
    
    local vsix_file="${vsix_files[0]}"
    log_info "Found extension package: $vsix_file"
    
    # Check file size (should be reasonable)
    local file_size=$(stat -f%z "$vsix_file" 2>/dev/null || stat -c%s "$vsix_file" 2>/dev/null)
    if [ "$file_size" -lt 1000 ]; then
        log_error "Extension package seems too small ($file_size bytes)"; exit 1; fi
    
    log_success "Extension validation passed"
    cd "$REPO_ROOT"
}

trigger_github_workflow() {
    log_info "Triggering GitHub Actions workflow for VSCode extension publishing..."
    
    if command_exists gh; then
        if gh workflow run "Publish VSCode Extension" --field confirm="YES"; then
            log_success "GitHub Actions workflow triggered successfully."
            log_info "Monitor deployment at: https://github.com/grapa-dev/grapa/actions"
        else
            log_error "Failed to trigger GitHub Actions workflow."
            log_info "You may need to trigger it manually from the GitHub Actions tab."
        fi
    else
        log_warning "GitHub CLI (gh) not found. Please trigger the workflow manually:"
        log_info "1. Go to https://github.com/grapa-dev/grapa/actions"
        log_info "2. Select 'Publish VSCode Extension' workflow"
        log_info "3. Click 'Run workflow'"
    fi
}

show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Builds and publishes the VSCode extension to Visual Studio Marketplace."
    echo ""
    echo "OPTIONS:"
    echo "  --publish      Trigger GitHub Actions workflow to publish to marketplace"
    echo "  --build-only   Build and package extension without publishing"
    echo "  -h, --help     Show this help message"
    echo ""
    echo "EXAMPLES:"
    echo "  $0 --build-only    Build and package extension (no publish)"
    echo "  $0 --publish       Build, package, and trigger publishing workflow"
    echo ""
    echo "Note: Publishing requires VSCE_PAT GitHub secret to be configured."
}

main() {
    BUILD_ONLY=true
    PUBLISH=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --publish)
                BUILD_ONLY=false
                PUBLISH=true
                shift
                ;;
            --build-only)
                BUILD_ONLY=true
                PUBLISH=false
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
    
    log_info "Starting VSCode extension deployment..."
    log_info "Extension directory: $EXTENSION_DIR"
    validate_prerequisites
    build_extension
    validate_extension
    
    if [[ "$PUBLISH" == "true" ]]; then
        trigger_github_workflow
        log_success "VSCode extension deployment completed!"
        log_info "Extension will be published to Visual Studio Marketplace via GitHub Actions."
    else
        log_success "VSCode extension build completed!"
        log_info "Extension packaged successfully. Use --publish to deploy to marketplace."
    fi
}

main "$@"
