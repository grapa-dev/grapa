#!/bin/bash

# Grapa GitHub Release Creation Script
# Creates a GitHub release with all platform binaries

set -e  # Exit on any error

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="0.1.52"
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
    
    # Check for required binaries in new directory structure
    if [ ! -f "$REPO_ROOT/bin/mac-arm64/grapa" ]; then
        missing_binaries+=("grapa (Mac ARM64)")
    fi
    
    if [ ! -f "$REPO_ROOT/bin/win-amd64/grapa.exe" ]; then
        missing_binaries+=("grapa.exe (Windows AMD64)")
    fi
    
    if [ ! -f "$REPO_ROOT/bin/linux-amd64/grapa" ]; then
        missing_binaries+=("grapa (Linux AMD64)")
    fi
    
    if [ ! -f "$REPO_ROOT/bin/linux-arm64/grapa" ]; then
        missing_binaries+=("grapa (Linux ARM64)")
    fi
    
    if [ ! -f "$REPO_ROOT/bin/aws-amd64/grapa" ]; then
        missing_binaries+=("grapa (AWS AMD64)")
    fi
    
    if [ ! -f "$REPO_ROOT/bin/aws-arm64/grapa" ]; then
        missing_binaries+=("grapa (AWS ARM64)")
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
    
    # Create .zip archives for each platform folder
    cd "$REPO_ROOT/bin"
    
    # macOS ARM64
    if [ -d "mac-arm64" ]; then
        log_info "Creating macOS ARM64 package..."
        zip -r "$TEMP_DIR/grapa-$VERSION-mac-arm64.zip" mac-arm64/
    fi
    
    # Windows AMD64
    if [ -d "win-amd64" ]; then
        log_info "Creating Windows AMD64 package..."
        zip -r "$TEMP_DIR/grapa-$VERSION-win-amd64.zip" win-amd64/
    fi
    
    # Linux AMD64
    if [ -d "linux-amd64" ]; then
        log_info "Creating Linux AMD64 package..."
        zip -r "$TEMP_DIR/grapa-$VERSION-linux-amd64.zip" linux-amd64/
    fi
    
    # Linux ARM64
    if [ -d "linux-arm64" ]; then
        log_info "Creating Linux ARM64 package..."
        zip -r "$TEMP_DIR/grapa-$VERSION-linux-arm64.zip" linux-arm64/
    fi
    
    # AWS AMD64
    if [ -d "aws-amd64" ]; then
        log_info "Creating AWS AMD64 package..."
        zip -r "$TEMP_DIR/grapa-$VERSION-aws-amd64.zip" aws-amd64/
    fi
    
    # AWS ARM64
    if [ -d "aws-arm64" ]; then
        log_info "Creating AWS ARM64 package..."
        zip -r "$TEMP_DIR/grapa-$VERSION-aws-arm64.zip" aws-arm64/
    fi
    
    cd "$REPO_ROOT"
    
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
- Python integration via GrapaPy
- Improved build system with static library support

### Supported Platforms
- **macOS ARM64** (Apple Silicon): \`grapa-$VERSION-mac-arm64.zip\`
- **Windows AMD64**: \`grapa-$VERSION-win-amd64.zip\`
- **Linux AMD64**: \`grapa-$VERSION-linux-amd64.zip\`
- **Linux ARM64**: \`grapa-$VERSION-linux-arm64.zip\`
- **AWS AMD64**: \`grapa-$VERSION-aws-amd64.zip\`
- **AWS ARM64**: \`grapa-$VERSION-aws-arm64.zip\`

### Installation
Each package contains:
- Grapa executable and libraries
- Automated install script for your platform
- Platform-specific documentation

**Quick Start:**
1. Download the appropriate \`.zip\` file for your platform
2. Extract the archive
3. Run the included install script
4. Start using Grapa!

**Python users**: Install GrapaPy with \`pip install grapapy\`

### Documentation
- [GitHub Repository](https://github.com/grapa-dev/grapa)
- [Documentation](https://grapa-dev.github.io/grapa/)
- [Installation Guide](https://grapa-dev.github.io/grapa/installation/)

### Changes in this Release
- Version bump to $VERSION
- Improved build system with static library support
- Enhanced Python integration
- AWS platform support (AMD64 and ARM64)
- Cross-platform compatibility improvements
- Better error handling and build reliability
- Automated install scripts for all platforms

### SHA256 Checksums
\`\`\`
$(shasum -a 256 "$TEMP_DIR"/*.zip | sed 's|.*/||')
\`\`\`"
    
    # Create the release
    if gh release create "$TAG" \
        --title "Grapa $VERSION" \
        --notes "$RELEASE_BODY" \
        "$TEMP_DIR/grapa-$VERSION-mac-arm64.zip" \
        "$TEMP_DIR/grapa-$VERSION-win-amd64.zip" \
        "$TEMP_DIR/grapa-$VERSION-linux-amd64.zip" \
        "$TEMP_DIR/grapa-$VERSION-linux-arm64.zip" \
        "$TEMP_DIR/grapa-$VERSION-aws-amd64.zip" \
        "$TEMP_DIR/grapa-$VERSION-aws-arm64.zip"; then
        
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
