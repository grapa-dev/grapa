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
- **[CURRENT_STATUS.md](CURRENT_STATUS.md)** - **AUTHORITATIVE**: Single source of truth for all development status, priorities, and progress tracking
- **[LANGUAGE_ENHANCEMENT_ROADMAP.md](LANGUAGE_ENHANCEMENT_ROADMAP.md)** - **DEPRECATED**: Content consolidated into CURRENT_STATUS.md

### **📚 Reference Documentation**
- **[DOCUMENTATION_DEPLOYMENT_GUIDE.md](../BUILD_AND_DEPLOYMENT/DOCUMENTATION_DEPLOYMENT.md)** - Complete documentation deployment process
- **[IMPLEMENTATION_PROGRESS.md](IMPLEMENTATION_PROGRESS.md)** - **DEPRECATED**: Moved to CURRENT_STATUS.md
- **[TODO.md](TODO.md)** - **DEPRECATED**: Moved to CURRENT_STATUS.md
- **[PROGRAMMER_FRIENDLINESS_AND_ADOPTION_PLAN.md](PROGRAMMER_FRIENDLINESS_AND_ADOPTION_PLAN.md)** - Comprehensive audit, migration review, pain points, and proposals to improve comment handling, loop syntax, string interpolation, onboarding, and overall language adoption.
- **BACKLOG.md**: Historical or long-term backlog. Now located in this folder.

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

- **CLI Functionality Investigation** – See [CLI_FUNCTIONALITY_INVESTIGATION.md](CLI_FUNCTIONALITY_INVESTIGATION.md) for systematic testing plan to verify current CLI features and update implementation documentation.
- **ROW Table Index Corruption Investigation** – See [ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md](ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md) and [ROW_INDEX_BUG_TODO.md](ROW_INDEX_BUG_TODO.md) for full debug log, root cause analysis, and current status.

## 📦 Archived/Closed Investigations

- [ROW Table Index Corruption Investigation (Closed 2025-07-22)](archive/ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md)
- [ROW Table Index Bug TODO Tracker (Closed 2025-07-22)](archive/ROW_INDEX_BUG_TODO.md)

## How to Add or Update Documentation

- Before creating a new doc, check if it fits as a section in an existing canonical doc (e.g., CURRENT_STATUS.md).
- Every new doc must be added to this index.md with a one-line summary.
- Use clear, descriptive filenames and place docs in the most relevant folder.
- Cross-link related docs in other folders as needed.
- Archive superseded or outdated docs in ARCHIVED/ and update the index.

### How to Add a New Investigation or Debug Log
- Check if your topic fits as a section in an existing doc (especially CURRENT_STATUS.md).
- If not, create a new file in this folder, add a one-line summary to index.md, and cross-link from any relevant implementation docs.
- Use a descriptive name like FEATURE_OR_AREA_BUG_INVESTIGATION.md (e.g., ROW_TABLE_INDEX_CORRUPTION_INVESTIGATION.md).
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