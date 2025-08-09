---
tags:
  - maintainer
  - lowlevel
  - development
---

# Development Documentation

This folder is for all in-progress, investigation, planning, and debug documentation for the Grapa project. For stable, canonical implementation references, see IMPLEMENTATION/.

## 📋 **Current Status & Planning**

### **🎯 Single Source of Truth**
- **[CURRENT_STATUS.md](../PROJECT_MANAGEMENT/CURRENT_STATUS.md)** - **AUTHORITATIVE**: Single source of truth for all development status, priorities, and progress tracking
- **[ONBOARD.md](../PROJECT_MANAGEMENT/ONBOARD.md)** - **NEW**: Agent onboarding and developer workflow documentation
- **[MIGRATION_SUMMARY.md](../PROJECT_MANAGEMENT/MIGRATION_SUMMARY.md)** - **NEW**: Summary of GrapaDBX migration and content organization

### **📚 Reference Documentation**
- **[DOCUMENTATION_DEPLOYMENT_GUIDE.md](../BUILD_AND_DEPLOYMENT/DOCUMENTATION_DEPLOYMENT.md)** - Complete documentation deployment process
- **[BACKLOG.md](../PROJECT_MANAGEMENT/BACKLOG.md)** - Historical or long-term backlog
- **[LANGUAGE_IMPROVEMENT_PLAN.md](LANGUAGE_IMPROVEMENT_PLAN.md)** - Language improvement roadmap and proposals
- **[CLI_IMPROVEMENT_PLAN.md](CLI_IMPROVEMENT_PLAN.md)** - CLI enhancement plans and proposals
- **[LANGUAGE_ANALYSIS_AND_IMPROVEMENT_PLAN.md](LANGUAGE_ANALYSIS_AND_IMPROVEMENT_PLAN.md)** - Comprehensive audit, migration review, pain points, and proposals to improve comment handling, loop syntax, string interpolation, onboarding, and overall language adoption.

## 🔧 **Build & Deployment**

### **🏗️ Build System**
- **[BUILD_SYSTEM.md](../BUILD_AND_DEPLOYMENT/BUILD_SYSTEM.md)** - **NEW**: Complete build system reference
- **[BUILD.md](../BUILD_AND_DEPLOYMENT/BUILD.md)** - **DEPRECATED**: Moved to BUILD_SYSTEM.md
- **[BUILD_DEBUGGING.md](../BUILD_AND_DEPLOYMENT/BUILD_DEBUGGING.md)** - **DEPRECATED**: Moved to BUILD_SYSTEM.md
- **[DEPENDENCIES.md](../BUILD_AND_DEPLOYMENT/DEPENDENCIES.md)** - **DEPRECATED**: Moved to BUILD_SYSTEM.md

## 📖 **Archived Documentation**

All deprecated files have been moved to:
- **[ARCHIVED/consolidation_2024/](../INTERNAL_NOTES/ARCHIVED/consolidation_2024/)**

## 🎯 **Quick Start**

1. **Check Current Status**: Read [CURRENT_STATUS.md](CURRENT_STATUS.md) for immediate priorities
2. **Review Language Plans**: See [CURRENT_STATUS.md](CURRENT_STATUS.md) for language development roadmap
3. **Build System**: Use [BUILD_SYSTEM.md](../BUILD_AND_DEPLOYMENT/BUILD_SYSTEM.md) for build instructions

## 📞 **Maintenance**

- **Status Updates**: Update [CURRENT_STATUS.md](CURRENT_STATUS.md) when priorities change
- **Language Planning**: Update [CURRENT_STATUS.md](CURRENT_STATUS.md) for new features
- **Build Changes**: Update [BUILD_SYSTEM.md](../BUILD_AND_DEPLOYMENT/BUILD_SYSTEM.md) for build system changes

## 🛠️ **Code Creation Guidelines**

### **📝 Grapa Code Creation Checklist**
**CRITICAL**: Before creating any Grapa code (scripts, tests, examples), always:

1. **Review Syntax Rules**: Check `docs/docs/syntax/basic_syntax.md` for current Grapa syntax
2. **Use Block Comments**: Use `/* */` format, not `#` for comments
3. **String Concatenation**: Wrap concatenation expressions in parentheses: `("text" + var.str()).echo()`
4. **Escape Sequences**: Use proper escaping for special characters
5. **Operator Precedence**: Use parentheses to ensure correct evaluation order
6. **Test Syntax**: Validate syntax before running complex scripts

**Reference**: The canonical syntax reference is `docs/docs/syntax/basic_syntax.md` - always consult this before writing Grapa code.

## 🔎 Quick Links to Major Investigations

### **Database & Analysis**
- **[GRAPADB_ANALYSIS.md](GRAPADB_ANALYSIS.md)** - Comprehensive GrapaDB analysis and implementation details
- **[GRAPA_IN_MEMORY_DATABASE_DISCOVERY.md](GRAPA_IN_MEMORY_DATABASE_DISCOVERY.md)** - Discovery of Grapa's in-memory database mechanism using `$` path pattern

### **Language & CLI Analysis**
- **[LANGUAGE_ANALYSIS_AND_IMPROVEMENT_PLAN.md](LANGUAGE_ANALYSIS_AND_IMPROVEMENT_PLAN.md)** - Comprehensive analysis of Grapa language features, migration patterns, pain points, and improvement plan
- **[CLI_ANALYSIS_AND_IMPROVEMENT_PLAN.md](CLI_ANALYSIS_AND_IMPROVEMENT_PLAN.md)** - Comprehensive analysis of CLI functionality, testing requirements, and improvement roadmap

### **CLI & Interface**
- **[CLI_FUNCTIONALITY_INVESTIGATION.md](../RESEARCH_AND_ANALYSIS/CLI_FUNCTIONALITY_INVESTIGATION.md)** - Comprehensive CLI functionality investigation
- **[CLI_BUG_REPORTS.md](CLI_BUG_REPORTS.md)** - CLI bug reports and systematic testing plan

### **Language & Documentation**
- **[DOCUMENTATION_SEARCH_BUG.md](../RESEARCH_AND_ANALYSIS/DOCUMENTATION_SEARCH_BUG.md)** - Documentation search bug investigation

### **Bug Investigations**
- **[GRAPA_DB_IMPLEMENTATION.md](../IMPLEMENTATION/GRAPA_DB_IMPLEMENTATION.md)** - GrapaDB implementation details
- **[GRAPA_BTREE_IMPLEMENTATION.md](../IMPLEMENTATION/GRAPA_BTREE_IMPLEMENTATION.md)** - BTree implementation details

## How to Add or Update Documentation

- Before creating a new doc, check if it fits as a section in an existing canonical doc (e.g., CURRENT_STATUS.md).
- Every new doc must be added to this index.md with a one-line summary.
- Use clear, descriptive filenames and place docs in the most relevant folder.
- Cross-link related docs in other folders as needed.
- Archive superseded or outdated docs in ARCHIVED/ and update the index.

### How to Add a New Investigation or Debug Log
- Check if your topic fits as a section in an existing doc (especially CURRENT_STATUS.md).
- If not, create a new file in this folder, add a one-line summary to index.md, and cross-link from any relevant implementation docs.
- Use a descriptive name like FEATURE_OR_AREA_BUG_INVESTIGATION.md (e.g., DATABASE_PERFORMANCE_INVESTIGATION.md).
- Follow the contributor checklist below for naming, placement, and cross-linking.

**Contributor Checklist:**
- [ ] Purpose and scope stated at the top of the doc
- [ ] Cross-links to related docs
- [ ] Added to this index.md
- [ ] Filename and placement appropriate
- [ ] Old/superseded docs archived if needed

> For global guidelines, see the main maintainers/index.md.

---

**Last Updated**: July 19, 2024  
**Consolidation**: Complete - Single source of truth established 