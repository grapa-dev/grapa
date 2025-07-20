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

## Current Context & Status (July 19, 2024)

### ✅ **Documentation System** - **FULLY OPERATIONAL**
- **Status**: All documentation issues resolved and deployed
- **Live Site**: https://grapa-dev.github.io/grapa/ - Fully functional
- **Navigation**: All links working correctly
- **Case Sensitivity**: All files use consistent lowercase naming
- **Build System**: Clean builds with no broken link warnings

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

### Documentation Consolidation (July 19, 2024)
- **Status**: ✅ **COMPLETED** - Maintainer documentation consolidated and cleaned up
- **Actions Taken**:
  - Removed duplicate `BINARY_GREP.md` file (kept in `ADVANCED_TOPICS/`)
  - Archived outdated WIP files to `INTERNAL_NOTES/ARCHIVED_WIP/`
  - Consolidated status tracking into `DEVELOPMENT/IMPLEMENTATION_PROGRESS.md`
  - Merged Windows debugging context into main debugging guide
  - Moved useful BNF notes to main `INTERNAL_NOTES/` directory
- **Benefits**: Reduced redundancy, improved organization, single source of truth for each topic

### Case Sensitivity Fixes (July 19, 2024)
- **Status**: ✅ **COMPLETED** - All documentation files and navigation links fixed
- **Problem**: Case sensitivity conflicts between navigation links and directory names
- **Actions Taken**:
  - Renamed all documentation files to lowercase (e.g., `API_REFERENCE.md` → `api_reference.md`)
  - Updated all migration files (e.g., `PYTHON_TO_GRAPA_MIGRATION.md` → `python_to_grapa_migration.md`)
  - Updated all type files (e.g., `ARRAY.md` → `array.md`)
  - Fixed MkDocs configuration to reference lowercase filenames
  - Updated all internal markdown links to use lowercase references
  - Deployed corrected site with lowercase directory names
- **Benefits**: All navigation links work correctly, no more "file not found" errors

## TODO & Future Work

### Documentation Maintenance
- [ ] **Monitor for broken links** - Check periodically after major changes
- [ ] **Review case sensitivity** - Ensure new files follow lowercase convention
- [ ] **Update CI/CD scripts** - If they reference old test file locations
- [ ] **Document test organization** - For future contributors

### Build System
- [ ] **Monitor build warnings** - Address any new MkDocs warnings
- [ ] **Update dependencies** - Keep MkDocs and plugins current
- [ ] **Performance optimization** - Monitor build times for large documentation

### Quality Assurance
- [ ] **Link validation** - Automated checking for broken internal links
- [ ] **Syntax validation** - Ensure all Grapa code examples are correct
- [ ] **Cross-reference validation** - Verify all internal references are accurate 