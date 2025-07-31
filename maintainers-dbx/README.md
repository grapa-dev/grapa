# Maintainers & News

**Note for contributors:**

> For onboarding, project status, and maintainer documentation, see [`maintainers/README.md`](maintainers/README.md) and [`maintainers/DEVELOPMENT/CURRENT_STATUS.md`](maintainers/DEVELOPMENT/CURRENT_STATUS.md).

**Project Status and Priorities**

> **Always consult [`maintainers/DEVELOPMENT/CURRENT_STATUS.md`](DEVELOPMENT/CURRENT_STATUS.md) for the authoritative, up-to-date project status, priorities, and progress tracking.**  
> This file is the single source of truth for all ongoing work, TODOs, and development focus. All maintainers and contributors should review CURRENT_STATUS.md before starting new work or making major changes.

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

### 🚨 CRITICAL: Documentation Search Bug (IMMEDIATE PRIORITY)
- [ ] **🚨 CRITICAL ISSUE IDENTIFIED** - Material theme search highlighting corrupting function names across entire documentation
- [ ] **Deploy enhanced JavaScript fix** - Enhanced `search-fix.js` with comprehensive patterns ready for deployment
- [ ] **Test other function names** - Verify if `int()`, `str()`, `len()`, etc. are also affected
- [ ] **Consider disabling search highlighting** - Temporary fix by commenting out `search.highlight` in mkdocs.yml
- [ ] **Investigate Material theme version** - Check if this is a known bug in current version
- [ ] **Systematic testing** - Test search for all major function categories
- **Reference**: `maintainers/DEVELOPMENT/DOCUMENTATION_SEARCH_BUG.md` - Comprehensive documentation of the issue

### Scientific Notation Support (Future Enhancement)
- [ ] **Add scientific notation parsing** - Support for `1e-10`, `1.5e+3`, `2.3E-5` format
- [ ] **Implement at tokenization level** - Add parsing in base-level byte parsing and tokenization step
- [ ] **Alternative: Higher-level rules** - Or implement in `$grapa.grc` rules
- [ ] **Update GrapaFloat integration** - Ensure proper conversion to GrapaFloat objects
- [ ] **Add test cases** - Comprehensive testing of scientific notation parsing
- [ ] **Update documentation** - Remove limitation notes once implemented 

## Maintainer Checklist for Language Changes

After any change to the Grapa language (syntax, semantics, features, or deprecations), you must:
- Update [Basic Syntax Guide](../docs-src/docs/syntax/basic_syntax.md) and migration docs.
- Scan and update all code samples in documentation (`docs-src`).
- Empirically re-test all `.grc` scripts.
- Update the [Programmer Friendliness & Adoption Plan](DEVELOPMENT/PROGRAMMER_FRIENDLINESS_AND_ADOPTION_PLAN.md) if new features address previous pain points.
- Cross-link all relevant docs (basic_syntax.md, migration docs, comment handling guide, this plan).
- Ensure a sign-off step in the PR/release process for documentation and sample updates. 

## Documentation Separation Policy

- All user-facing documentation must reside in `docs-src` (and ultimately `docs/` for the site build).
- User-facing docs in `docs-src` must **never** link to or reference anything outside of `docs-src` (including `maintainers/`).
- Maintainer/internal documentation must reside in `maintainers/` and must **not** be placed in `docs-src`.
- This strict separation is mandatory for all contributors and agents to ensure clean user/maintainer boundaries and prevent accidental exposure of internal content.
- The only exception: clickable links to maintainer docs are allowed in docs-src/docs/deep_expert_implementation_overview.md, which is a dedicated bridge for deep expert users. All other docs in docs-src must not link outside docs-src. 