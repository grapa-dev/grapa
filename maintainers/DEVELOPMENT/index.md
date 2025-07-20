---
tags:
  - maintainer
  - lowlevel
  - development
---

# Development Documentation

This directory contains all development-related documentation for the Grapa project.

## 📋 **Current Status & Planning**

### **🎯 Single Source of Truth**
- **[CURRENT_STATUS.md](CURRENT_STATUS.md)** - **AUTHORITATIVE**: Single source of truth for all development status, priorities, and progress tracking
- **[LANGUAGE_ENHANCEMENT_ROADMAP.md](LANGUAGE_ENHANCEMENT_ROADMAP.md)** - **DEPRECATED**: Content consolidated into CURRENT_STATUS.md

### **📚 Reference Documentation**
- **[DOCUMENTATION_DEPLOYMENT_GUIDE.md](DOCUMENTATION_DEPLOYMENT_GUIDE.md)** - Complete documentation deployment process
- **[IMPLEMENTATION_PROGRESS.md](IMPLEMENTATION_PROGRESS.md)** - **DEPRECATED**: Moved to CURRENT_STATUS.md
- **[TODO.md](TODO.md)** - **DEPRECATED**: Moved to CURRENT_STATUS.md

## 🔧 **Build & Deployment**

### **🏗️ Build System**
- **[BUILD_SYSTEM.md](../BUILD_AND_DEPLOYMENT/BUILD_SYSTEM.md)** - **NEW**: Complete build system reference
- **[BUILD.md](../BUILD_AND_DEPLOYMENT/BUILD.md)** - **DEPRECATED**: Moved to BUILD_SYSTEM.md
- **[BUILD_DEBUGGING.md](../BUILD_AND_DEPLOYMENT/BUILD_DEBUGGING.md)** - **DEPRECATED**: Moved to BUILD_SYSTEM.md
- **[DEPENDENCIES.md](../BUILD_AND_DEPLOYMENT/DEPENDENCIES.md)** - **DEPRECATED**: Moved to BUILD_SYSTEM.md

## 📖 **Archived Documentation**

All deprecated files have been moved to:
- **[ARCHIVED_WIP/consolidation_2024/](../INTERNAL_NOTES/ARCHIVED_WIP/consolidation_2024/)**

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

---

**Last Updated**: July 19, 2024  
**Consolidation**: Complete - Single source of truth established 