#!/bin/bash

# Ubuntu Official Repository Submission Helper Script
# This script helps prepare and guide the submission to Ubuntu Universe repository

set -e

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
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

show_ubuntu_submission_guide() {
    cat << 'EOF'

# Ubuntu Universe Repository Submission Guide

## Prerequisites

1. **Ubuntu MOTU Team Membership**
   - Join Ubuntu MOTU (Masters of the Universe) team
   - Apply at: https://wiki.ubuntu.com/MOTU
   - This gives you permission to upload packages to Ubuntu Universe

2. **Ubuntu Developer Account**
   - Create account at: https://launchpad.net/
   - Enable Ubuntu Developer permissions
   - Set up GPG key for package signing

## Submission Process

### Step 1: Prepare Source Package
```bash
# The source package is already prepared in:
# temp/debian-official-submission/
```

### Step 2: Upload to Ubuntu Universe
```bash
# Upload source package to Ubuntu Universe
dput ubuntu grapa_0.1.51-1_source.changes

# Or use the Ubuntu package upload interface
# https://launchpad.net/ubuntu/+ppas
```

### Step 3: Monitor Review Process
- Package will be reviewed by Ubuntu maintainers
- Address any feedback or issues
- Timeline: 2-4 weeks for initial review

### Step 4: Package Acceptance
- Once approved, package will be available in Ubuntu repositories
- Users can install with: `sudo apt install grapa`

## Alternative: Work with Ubuntu MOTU Team

If you're not a MOTU member, you can:

1. **Contact Ubuntu MOTU Team**
   - IRC: #ubuntu-motu on Freenode
   - Mailing list: ubuntu-motu@lists.ubuntu.com
   - Forum: https://ubuntuforums.org/forumdisplay.php?f=123

2. **Submit Package Request**
   - Create bug report requesting package inclusion
   - Provide all submission materials
   - MOTU team will review and potentially sponsor the package

## Files Ready for Submission

The following files are prepared in `temp/debian-official-submission/`:
- `grapa_0.1.51_arm64.deb` - ARM64 binary package
- `grapa_0.1.51_amd64.deb` - AMD64 binary package
- `DEBIAN_SUBMISSION.md` - Package documentation
- `SUBMISSION_CHECKLIST.md` - Submission checklist

## Next Steps

1. **Join Ubuntu MOTU team** or contact them
2. **Upload source package** to Ubuntu Universe
3. **Monitor review process** and respond to feedback
4. **Package will be available** in Ubuntu repositories once approved

EOF
}

show_debian_submission_guide() {
    cat << 'EOF'

# Debian Repository Submission Guide

## Prerequisites

1. **Debian Developer Account**
   - Become a Debian Developer (DD) or work with one
   - Apply at: https://www.debian.org/devel/join/
   - This gives you permission to upload packages to Debian

2. **Debian Package Maintainer**
   - Set up Debian package maintenance tools
   - Configure GPG key for package signing
   - Join Debian packaging team

## Submission Process

### Step 1: Prepare Source Package
```bash
# The source package is already prepared in:
# temp/debian-official-submission/
```

### Step 2: Upload to Debian
```bash
# Upload source package to Debian
dput ftp-master grapa_0.1.51-1_source.changes

# Or use the Debian package upload interface
# https://ftp-master.debian.org/
```

### Step 3: Monitor Review Process
- Package will be reviewed by Debian maintainers
- Address any feedback or issues
- Timeline: 4-8 weeks for initial review

### Step 4: Package Acceptance
- Once approved, package will be available in Debian repositories
- Users can install with: `sudo apt install grapa`

## Alternative: Work with Debian Maintainers

If you're not a Debian Developer, you can:

1. **Contact Debian Maintainers**
   - Mailing list: debian-devel@lists.debian.org
   - IRC: #debian on OFTC
   - Forum: https://www.debian.org/support

2. **Submit Package Request**
   - Create ITP (Intent to Package) bug report
   - Provide all submission materials
   - Debian maintainers will review and potentially sponsor the package

## Files Ready for Submission

The following files are prepared in `temp/debian-official-submission/`:
- `grapa_0.1.51_arm64.deb` - ARM64 binary package
- `grapa_0.1.51_amd64.deb` - AMD64 binary package
- `DEBIAN_SUBMISSION.md` - Package documentation
- `SUBMISSION_CHECKLIST.md` - Submission checklist

## Next Steps

1. **Become a Debian Developer** or contact maintainers
2. **Upload source package** to Debian
3. **Monitor review process** and respond to feedback
4. **Package will be available** in Debian repositories once approved

EOF
}

main() {
    log_info "Ubuntu/Debian Official Repository Submission Helper"
    log_info "Version: $VERSION"
    
    echo ""
    log_info "This script provides guidance for submitting Grapa to official repositories."
    echo ""
    
    log_warning "Note: You need appropriate permissions to submit to official repositories."
    echo ""
    
    log_info "Choose your submission target:"
    echo "1. Ubuntu Universe Repository"
    echo "2. Debian Repository"
    echo "3. Both"
    echo "4. Exit"
    echo ""
    
    read -p "Enter your choice (1-4): " choice
    
    case $choice in
        1)
            show_ubuntu_submission_guide
            ;;
        2)
            show_debian_submission_guide
            ;;
        3)
            show_ubuntu_submission_guide
            echo ""
            echo "=========================================="
            echo ""
            show_debian_submission_guide
            ;;
        4)
            log_info "Exiting..."
            exit 0
            ;;
        *)
            log_error "Invalid choice. Please run the script again."
            exit 1
            ;;
    esac
    
    log_success "Submission guides displayed successfully!"
    log_info "Review the guides above and follow the appropriate process."
}

main "$@" 