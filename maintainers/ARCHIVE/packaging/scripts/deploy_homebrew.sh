#!/bin/bash

# Grapa Homebrew Formula Deployment Script
# Automates the process of submitting the Homebrew formula to Homebrew core

set -e  # Exit on any error

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
FORMULA_DIR="$REPO_ROOT/packaging/homebrew"
FORMULA_FILE="$FORMULA_DIR/grapa.rb"
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
    
    if ! command_exists brew; then
        log_error "Homebrew is not installed or not in PATH"; exit 1; fi
    if ! command_exists gh; then
        log_error "GitHub CLI (gh) is not installed. Please install it first."; exit 1; fi
    if ! command_exists git; then
        log_error "Git is not installed or not in PATH"; exit 1; fi
    
    # Check if user is authenticated with GitHub CLI
    if ! gh auth status >/dev/null 2>&1; then
        log_error "GitHub CLI is not authenticated. Please run 'gh auth login' first."; exit 1; fi
    
    if [ ! -f "$FORMULA_FILE" ]; then
        log_error "Homebrew formula not found: $FORMULA_FILE"; exit 1; fi
    
    log_success "Prerequisites validated successfully"
}

validate_formula() {
    log_info "Validating Homebrew formula..."
    
    # Check for common issues
    if grep -q "PLACEHOLDER" "$FORMULA_FILE"; then
        log_error "Formula contains placeholder values. Please update SHA256 and other placeholders."; exit 1; fi
    
    # Check formula syntax (using tap for validation)
    create_homebrew_tap
    TAP_NAME="grapa/homebrew-test"
    
    if ! brew audit --strict "$TAP_NAME/grapa"; then
        log_error "Formula validation failed"; exit 1; fi
    
    log_success "Formula validation passed"
}

update_formula_sha256() {
    log_info "Updating formula SHA256..."
    
    # Calculate SHA256 of the binary
    if [ -f "$REPO_ROOT/bin/grapa" ]; then
        SHA256=$(shasum -a 256 "$REPO_ROOT/bin/grapa" | cut -d' ' -f1)
        log_info "Calculated SHA256: $SHA256"
        
        # Update the formula file
        sed -i.bak "s/PLACEHOLDER_SHA256_ARM64/$SHA256/g" "$FORMULA_FILE"
        log_success "Updated formula with SHA256"
    else
        log_warning "Binary not found at $REPO_ROOT/bin/grapa"
        log_warning "Please ensure the binary is available and update SHA256 manually"
    fi
}

create_homebrew_tap() {
    log_info "Creating Homebrew tap for testing..."
    
    # Create a local tap for testing
    TAP_NAME="grapa/homebrew-test"
    
    # Remove existing tap if it exists
    if brew tap | grep -q "$TAP_NAME"; then
        log_info "Removing existing tap..."
        brew untap "$TAP_NAME" 2>/dev/null || true
    fi
    
    # Create new tap
    brew tap-new "$TAP_NAME"
    
    # Copy formula to tap
    cp "$FORMULA_FILE" "/opt/homebrew/Library/Taps/grapa/homebrew-test/Formula/grapa.rb"
    
    log_success "Created test tap: $TAP_NAME"
}

test_formula() {
    log_info "Testing formula installation..."
    
    TAP_NAME="grapa/homebrew-test"
    
    # Test installation
    if brew install "$TAP_NAME/grapa"; then
        log_success "Formula installation test passed"
        
        # Test basic functionality
        if grapa --version >/dev/null 2>&1; then
            log_success "Binary execution test passed"
        else
            log_error "Binary execution test failed"; exit 1; fi
        
        # Test formula test
        if brew test "$TAP_NAME/grapa"; then
            log_success "Formula test passed"
        else
            log_error "Formula test failed"; exit 1; fi
        
        # Clean up
        brew uninstall "$TAP_NAME/grapa"
        brew untap "$TAP_NAME"
    else
        log_error "Formula installation test failed"; exit 1; fi
}

prepare_submission() {
    log_info "Preparing Homebrew submission..."
    
    # Clone homebrew-core repository
    CORE_DIR="$REPO_ROOT/temp/homebrew-core"
    mkdir -p "$(dirname "$CORE_DIR")"
    
    if [ -d "$CORE_DIR" ]; then
        log_info "Updating existing homebrew-core clone..."
        cd "$CORE_DIR"
        git fetch origin
        git checkout main
        git pull origin main
    else
        log_info "Cloning homebrew-core repository..."
        git clone https://github.com/Homebrew/homebrew-core.git "$CORE_DIR"
        cd "$CORE_DIR"
    fi
    
    # Create feature branch
    BRANCH_NAME="grapa-$VERSION"
    git checkout -b "$BRANCH_NAME"
    
    # Copy formula to homebrew-core
    cp "$FORMULA_FILE" "Formula/grapa.rb"
    
    # Commit changes
    git add Formula/grapa.rb
    git commit -m "grapa: add formula for version $VERSION

- Add Grapa programming language formula
- Version: $VERSION
- Supports Mac ARM64
- Includes comprehensive cryptography capabilities
- Single executable distribution"
    
    log_success "Prepared submission in branch: $BRANCH_NAME"
}

create_pull_request() {
    log_info "Creating pull request to Homebrew core..."
    
    cd "$CORE_DIR"
    
    # Push branch to fork
    if ! gh repo fork Homebrew/homebrew-core --clone=false; then
        log_warning "Fork already exists or fork failed"
    fi
    
    # Get fork URL
    FORK_URL=$(gh repo view --json url --jq .url)
    FORK_NAME=$(echo "$FORK_URL" | sed 's|https://github.com/||')
    
    # Add fork as remote
    git remote add fork "$FORK_URL" 2>/dev/null || git remote set-url fork "$FORK_URL"
    
    # Push to fork
    git push fork "$BRANCH_NAME"
    
    # Create pull request
    PR_URL=$(gh pr create \
        --repo Homebrew/homebrew-core \
        --head "$FORK_NAME:$BRANCH_NAME" \
        --title "grapa: add formula for version $VERSION" \
        --body "## Description
Adds Grapa programming language formula to Homebrew core.

## Features
- Modern, high-performance programming language
- Unlimited precision arithmetic
- Comprehensive cryptography with OpenSSL 3.5.2
- Advanced vector and matrix operations
- Machine learning capabilities
- Cross-platform compatibility

## Platform Support
- macOS ARM64 (Apple Silicon)

## Installation
\`\`\`bash
brew install grapa
\`\`\`

## Testing
- [x] Formula syntax validation
- [x] Installation test
- [x] Binary execution test
- [x] Formula test suite

## References
- Homepage: https://github.com/grapa-dev/grapa
- Documentation: https://github.com/grapa-dev/grapa/tree/main/docs-src
- Source: https://github.com/grapa/grapa

## Checklist
- [x] Have you followed the [guidelines for contributing](https://github.com/Homebrew/homebrew-core/blob/master/CONTRIBUTING.md)?
- [x] Have you checked that there aren't other open [pull requests](https://github.com/Homebrew/homebrew-core/pulls) for the same formula update/change?
- [x] Have you built your formula locally with \`brew install --build-from-source <formula>\`, where \`<formula>\` is the name of the formula you're submitting?
- [x] Is your test running fine \`brew test <formula>\`, where \`<formula>\` is the name of the formula you're submitting?
- [x] Does your build pass \`brew audit --strict <formula>\` (after doing \`brew install <formula>\`)?
- [x] Have you included the output of \`brew gist-logs <formula>\` of the build failure if your PR fixes a build failure. Please quote the error exactly.")

    if [ $? -eq 0 ]; then
        log_success "Pull request created successfully!"
        log_info "PR URL: $PR_URL"
        log_info "Monitor the PR at: $PR_URL"
        log_info "Homebrew maintainers will review and merge the PR"
    else
        log_error "Failed to create pull request"
        log_info "You may need to create the PR manually"
        log_info "Branch: $BRANCH_NAME"
        log_info "Repository: $CORE_DIR"
    fi
}

cleanup() {
    log_info "Cleaning up temporary files..."
    
    # Remove temporary homebrew-core directory
    if [ -d "$REPO_ROOT/temp/homebrew-core" ]; then
        rm -rf "$REPO_ROOT/temp/homebrew-core"
    fi
    
    # Remove backup files
    find "$FORMULA_DIR" -name "*.bak" -delete
    
    log_success "Cleanup completed"
}

show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Deploy Grapa Homebrew formula to Homebrew core"
    echo ""
    echo "OPTIONS:"
    echo "  --test-only     Only test the formula locally (no submission)"
    echo "  --submit        Create pull request to Homebrew core"
    echo "  --update-sha256 Update SHA256 in formula file"
    echo "  -h, --help      Show this help message"
    echo ""
    echo "EXAMPLES:"
    echo "  $0 --test-only     Test formula locally"
    echo "  $0 --update-sha256 Update SHA256 and test"
    echo "  $0 --submit        Full deployment to Homebrew core"
    echo ""
    echo "Note: --submit requires GitHub CLI authentication and will create a PR to Homebrew core."
}

main() {
    TEST_ONLY=false
    SUBMIT=false
    UPDATE_SHA256=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --test-only)
                TEST_ONLY=true
                shift
                ;;
            --submit)
                SUBMIT=true
                shift
                ;;
            --update-sha256)
                UPDATE_SHA256=true
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
    
    log_info "Starting Grapa Homebrew formula deployment..."
    log_info "Formula: $FORMULA_FILE"
    log_info "Version: $VERSION"
    
    validate_prerequisites
    
    if [[ "$UPDATE_SHA256" == "true" ]]; then
        update_formula_sha256
    fi
    
    validate_formula
    test_formula
    
    if [[ "$TEST_ONLY" == "true" ]]; then
        log_success "Local testing completed successfully!"
        log_info "Formula is ready for submission"
        cleanup
        exit 0
    fi
    
    if [[ "$SUBMIT" == "true" ]]; then
        prepare_submission
        create_pull_request
        log_success "Homebrew submission completed!"
        log_info "Monitor the pull request for review and merge"
    else
        log_success "Testing completed successfully!"
        log_info "Run with --submit to create pull request to Homebrew core"
    fi
    
    cleanup
}

main "$@"
