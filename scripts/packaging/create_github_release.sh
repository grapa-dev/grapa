#!/bin/bash

# Grapa GitHub Release Creation Script
# Creates a GitHub release with all platform binaries

set -e  # Exit on any error

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="0.1.51"
TAG="v$VERSION"

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
    
    if ! command_exists gh; then
        log_error "GitHub CLI (gh) is not installed. Please install it first."; exit 1; fi
    
    # Check if user is authenticated with GitHub CLI
    if ! gh auth status >/dev/null 2>&1; then
        log_error "GitHub CLI is not authenticated. Please run 'gh auth login' first."; exit 1; fi
    
    if ! command_exists git; then
        log_error "Git is not installed or not in PATH"; exit 1; fi
    
    log_success "Prerequisites validated successfully"
}

check_binaries() {
    log_info "Checking for required binaries..."
    
    local missing_binaries=()
    
    # Check for required binaries
    if [ ! -f "$REPO_ROOT/bin/grapa" ]; then
        missing_binaries+=("grapa (Mac ARM64)")
    fi
    
    if [ ! -f "$REPO_ROOT/bin/grapa-win-amd64.exe" ]; then
        missing_binaries+=("grapa-win-amd64.exe")
    fi
    
    if [ ! -f "$REPO_ROOT/bin/grapa-linux-amd64" ]; then
        missing_binaries+=("grapa-linux-amd64")
    fi
    
    if [ ! -f "$REPO_ROOT/bin/grapa-linux-arm64" ]; then
        missing_binaries+=("grapa-linux-arm64")
    fi
    
    if [ ${#missing_binaries[@]} -gt 0 ]; then
        log_error "Missing required binaries:"
        for binary in "${missing_binaries[@]}"; do
            log_error "  - $binary"
        done
        log_error "Please build all binaries before creating a release"
        exit 1
    fi
    
    log_success "All required binaries found"
}

prepare_release_assets() {
    log_info "Preparing release assets..."
    
    # Create temporary directory for release assets
    TEMP_DIR="$REPO_ROOT/temp/release-assets"
    mkdir -p "$TEMP_DIR"
    
    # Copy binaries with correct names
    cp "$REPO_ROOT/bin/grapa" "$TEMP_DIR/grapa-mac-arm64"
    cp "$REPO_ROOT/bin/grapa-win-amd64.exe" "$TEMP_DIR/grapa-win-amd64.exe"
    cp "$REPO_ROOT/bin/grapa-linux-amd64" "$TEMP_DIR/grapa-linux-amd64"
    cp "$REPO_ROOT/bin/grapa-linux-arm64" "$TEMP_DIR/grapa-linux-arm64"
    
    # Make binaries executable
    chmod +x "$TEMP_DIR/grapa-mac-arm64"
    chmod +x "$TEMP_DIR/grapa-linux-amd64"
    chmod +x "$TEMP_DIR/grapa-linux-arm64"
    
    log_success "Release assets prepared in $TEMP_DIR"
}

create_github_release() {
    log_info "Creating GitHub release..."
    
    cd "$REPO_ROOT"
    
    # Check if tag already exists
    if git tag -l | grep -q "^$TAG$"; then
        log_warning "Tag $TAG already exists"
        read -p "Do you want to delete and recreate the tag? (y/N): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            git tag -d "$TAG"
            git push origin ":refs/tags/$TAG" 2>/dev/null || true
        else
            log_error "Release creation cancelled"; exit 1; fi
    fi
    
    # Create and push tag
    git tag "$TAG"
    git push origin "$TAG"
    
    # Create release with assets
    RELEASE_BODY="## Grapa $VERSION Release

### Features
- Modern, high-performance programming language
- Unlimited precision arithmetic
- Comprehensive cryptography with OpenSSL 3.5.2
- Advanced vector and matrix operations
- Machine learning capabilities
- Cross-platform compatibility

### Supported Platforms
- **macOS ARM64** (Apple Silicon): \`grapa-mac-arm64\`
- **Windows AMD64**: \`grapa-win-amd64.exe\`
- **Linux AMD64**: \`grapa-linux-amd64\`
- **Linux ARM64**: \`grapa-linux-arm64\`

### Installation
- **macOS**: \`brew install grapa\` (after Homebrew formula is merged)
- **Other platforms**: Download the appropriate binary and make it executable

### Documentation
- [GitHub Repository](https://github.com/grapa-dev/grapa)
- [Documentation](https://github.com/grapa-dev/grapa/tree/main/docs-src)

### Changes in this Release
- Initial public release
- Comprehensive cryptographic capabilities
- Cross-platform support
- Single executable distribution

### SHA256 Checksums
\`\`\`
$(shasum -a 256 "$TEMP_DIR"/* | sed 's|.*/||')
\`\`\`"
    
    # Create the release
    if gh release create "$TAG" \
        --title "Grapa $VERSION" \
        --notes "$RELEASE_BODY" \
        "$TEMP_DIR/grapa-mac-arm64" \
        "$TEMP_DIR/grapa-win-amd64.exe" \
        "$TEMP_DIR/grapa-linux-amd64" \
        "$TEMP_DIR/grapa-linux-arm64"; then
        
        log_success "GitHub release created successfully!"
        log_info "Release URL: https://github.com/grapa/grapa/releases/tag/$TAG"
    else
        log_error "Failed to create GitHub release"
        exit 1
    fi
}

cleanup() {
    log_info "Cleaning up temporary files..."
    
    if [ -d "$TEMP_DIR" ]; then
        rm -rf "$TEMP_DIR"
    fi
    
    log_success "Cleanup completed"
}

show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Create a GitHub release for Grapa"
    echo ""
    echo "OPTIONS:"
    echo "  --dry-run     Show what would be done without creating release"
    echo "  -h, --help    Show this help message"
    echo ""
    echo "EXAMPLES:"
    echo "  $0 --dry-run     Preview release creation"
    echo "  $0              Create GitHub release"
    echo ""
    echo "Note: This requires GitHub CLI authentication and all binaries to be built."
}

main() {
    DRY_RUN=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --dry-run)
                DRY_RUN=true
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
    
    log_info "Starting Grapa GitHub release creation..."
    log_info "Version: $VERSION"
    log_info "Tag: $TAG"
    
    validate_prerequisites
    check_binaries
    
    if [[ "$DRY_RUN" == "true" ]]; then
        log_info "DRY RUN: Would create release $TAG with all binaries"
        log_info "Release would be available at: https://github.com/grapa/grapa/releases/tag/$TAG"
        exit 0
    fi
    
    prepare_release_assets
    create_github_release
    cleanup
    
    log_success "GitHub release creation completed!"
    log_info "Next steps:"
    log_info "1. Test the Homebrew formula: ./scripts/packaging/deploy_homebrew.sh --test-only"
    log_info "2. Submit to Homebrew: ./scripts/packaging/deploy_homebrew.sh --submit"
}

main "$@"
