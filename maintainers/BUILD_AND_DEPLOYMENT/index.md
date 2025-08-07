---
tags:
  - maintainer
  - lowlevel
  - build
---

# Build & Deployment Documentation

This directory contains all build system and deployment documentation for the Grapa project.

## 🏗️ **Build System**

### **🎯 Single Source of Truth**
- **[BUILD_SYSTEM.md](BUILD_SYSTEM.md)** - **COMPREHENSIVE**: Complete build system reference with all platform instructions, dependencies, and testing commands
- **[MULTI_PLATFORM_BUILD_GUIDE.md](MULTI_PLATFORM_BUILD_GUIDE.md)** - **COMPREHENSIVE**: Multi-platform build guide with version bumping and deployment workflows
- **[THIRD_PARTY_LIBRARY_BUILD_GUIDE.md](THIRD_PARTY_LIBRARY_BUILD_GUIDE.md)** - **COMPREHENSIVE**: Complete guide for building third-party libraries across all platforms

## 🚀 **Deployment**

### **GitHub Actions Policy**
- **[GITHUB_ACTIONS_POLICY.md](GITHUB_ACTIONS_POLICY.md)** - **CRITICAL**: Policy for explicit workflow triggers only, preventing accidental deployments

### **Documentation Deployment**
- **[DOCUMENTATION_DEPLOYMENT.md](DOCUMENTATION_DEPLOYMENT.md)** - Documentation deployment process and procedures
- **[SETUPAWSDOCKER.md](SETUPAWSDOCKER.md)** - AWS and Docker setup instructions
- **[BUILD_DOCKER_MANUAL.md](BUILD_DOCKER_MANUAL.md)** - Docker build manual and containerization guide

## 📖 **Archived Documentation**

All deprecated files have been moved to:
- **[ARCHIVED_WIP/consolidation_2024/](../INTERNAL_NOTES/ARCHIVED_WIP/consolidation_2024/)**

## 🎯 **Quick Start**

1. **Build System**: Read [BUILD_SYSTEM.md](BUILD_SYSTEM.md) for complete build instructions
2. **Platform Support**: Check supported platforms and dependencies
3. **Troubleshooting**: Use the debugging guide for common issues

## 📞 **Maintenance**

- **Build Changes**: Update [BUILD_SYSTEM.md](BUILD_SYSTEM.md) for any build system changes
- **Platform Support**: Update platform support matrix as needed
- **Dependencies**: Update dependency versions and requirements

## How to Add or Update Documentation

- Before creating a new doc, check if it fits as a section in an existing canonical doc (e.g., BUILD_SYSTEM.md).
- Every new doc must be added to this index.md with a one-line summary.
- Use clear, descriptive filenames and place docs in the most relevant folder.
- Cross-link related docs in other folders as needed.
- Archive superseded or outdated docs in ARCHIVED_WIP/ and update the index.

**Contributor Checklist:**
- [ ] Purpose and scope stated at the top of the doc
- [ ] Cross-links to related docs
- [ ] Added to this index.md
- [ ] Filename and placement appropriate
- [ ] Old/superseded docs archived if needed

> For global guidelines, see the main maintainers/index.md.

---

**Last Updated**: July 19, 2024  
**Consolidation**: Complete - Single build system reference established 