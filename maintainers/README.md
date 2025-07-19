# Maintainers & News

This section is for Grapa maintainers and advanced contributors. It contains documentation and resources related to:

- Building Grapa (C++ and Python) from source
- Managing dependencies and build environments
- Deployment and packaging
- Internal design notes, roadmaps, and work-in-progress docs
- Advanced topics (e.g., grammar/BNF development, low-level system functions)
- Documentation examples and dependency management

**General users do not need to reference these files.**

## Contents
- Build instructions and environment setup
- Dependency management
- Documentation examples management (`docs/docs/examples/`)
- Docker and cloud setup
- Internal design and roadmap docs
- System function and grammar/BNF documentation
- Work-in-progress and internal notes

## Key Maintenance Areas

### ✅ **Build System** - **COMPLETED**
- **Status**: Automated build system working across all 7 platforms
- **Platforms**: Windows, Mac ARM64/AMD64, Linux ARM64/AMD64, AWS ARM64/AMD64
- **Features**: Auto-detection, Python integration, package creation
- **Usage**: `python3 build.py` (see [BUILD_README.md](../BUILD_README.md))
- **Documentation**: [BUILD_AND_DEPLOYMENT/](BUILD_AND_DEPLOYMENT/) for detailed guides

### Documentation Examples (`docs/docs/examples/`)
- **Purpose**: Centralized location for documentation test examples
- **Dependency System**: Files are referenced by documentation - do not delete/move without updating links
- **Current Examples**: Basic, advanced, performance, and Python integration examples
- **Maintenance**: See `DEVELOPMENT/DOCUMENTATION_DEPLOYMENT_GUIDE.md` for detailed procedures

### Dependency Management
- **External Links**: All documentation examples are self-contained within docs
- **Test Files**: No dependencies on external test files in `test/` directory
- **Link Validation**: Automated checking for broken external links
- **Deployment**: Comprehensive guide for maintaining link integrity 