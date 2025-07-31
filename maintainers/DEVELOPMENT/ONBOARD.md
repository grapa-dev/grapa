# Grapa Project Onboarding Guide

This guide provides comprehensive onboarding information for new agents, developers, and contributors to the Grapa project.

---

## 🤖 Agent Onboarding

### Quick Reference for New Agents

**If user says "load current status and tell me next steps":**

#### 1. Current Project State
- **Project**: Grapa Programming Language with GrapaDB Database System
- **Status**: Core functionality stable, ongoing enhancements
- **Focus Areas**: Language features, CLI improvements, documentation

#### 2. Key Project Components
- **Language Core**: Grapa programming language with unified syntax
- **Database System**: GrapaDB with BTree-based storage
- **CLI Tools**: Command-line interface with debugging and development features
- **Documentation**: Comprehensive user and maintainer documentation

#### 3. Agent Response Template
When user asks for next steps, respond with:

"Based on the current Grapa project status, here are the key areas of focus:

📋 **Current Priorities:**
1. Language Enhancements - Unicode support, string interpolation, CLI improvements
2. Documentation Updates - API reference, examples, migration guides
3. Performance Optimization - Database operations, memory management
4. Testing and Quality - Comprehensive test suites, validation

🔗 **Key Resources:**
- `maintainers/DEVELOPMENT/CURRENT_STATUS.md` - Complete project status
- `maintainers/DEVELOPMENT/BACKLOG.md` - Development priorities
- `docs-src/` - User documentation
- `test/` - Test scripts and validation

Would you like me to focus on any specific area, or do you have questions about the current implementation?"

#### 4. Key Files to Review
- `maintainers/DEVELOPMENT/CURRENT_STATUS.md` - Complete project status
- `maintainers/DEVELOPMENT/BACKLOG.md` - Development priorities and tasks
- `docs-src/docs/syntax/basic_syntax.md` - Canonical syntax reference
- `test/` - Test scripts and validation tools

---

## 👨‍💻 Developer Onboarding

### Project Structure Overview

#### Core Components
- **Language Implementation**: `source/grapa/` - Core language features
- **Database System**: `source/grapa/GrapaDB.*` - Database implementation
- **Documentation**: `docs-src/` - User documentation, `maintainers/` - Internal docs
- **Testing**: `test/` - Comprehensive test suites
- **Build System**: `build.py`, `CMakeLists.txt` - Build configuration

#### Key Directories
- `maintainers/DEVELOPMENT/` - Development status and planning
- `maintainers/IMPLEMENTATION/` - Technical implementation details
- `maintainers/DBXARCHIVE/` - Database-specific archived documentation
- `docs-src/docs/` - User-facing documentation
- `test/` - Organized test suites by category

### Development Workflow

#### 1. Understanding the Codebase
- Start with `maintainers/DEVELOPMENT/CURRENT_STATUS.md` for current priorities
- Review `docs-src/docs/syntax/basic_syntax.md` for language rules
- Check `test/` directory for examples and validation

#### 2. Making Changes
- Follow the canonical syntax rules in `basic_syntax.md`
- Test all code changes with `.grc` scripts
- Update documentation for any language changes
- Ensure cross-platform compatibility

#### 3. Documentation Standards
- User docs in `docs-src/` (public-facing)
- Internal docs in `maintainers/` (implementation details)
- All code examples must be empirically tested
- Keep migration guides updated

---

## 🖥️ Cross-Platform Development

### Windows-Specific Commands
- **Build**: `python build.py` (not `python3`)
- **Executable**: `grapa.exe` (not `./grapa`)
- **File paths**: Use backslashes `"test\grapa.exe"`
- **Shell**: Use PowerShell or x64 Native Tools Command Prompt for VS 2022
- **Test scripts**: `python test\script.grc` with backslashes

### Mac/Linux Commands
- **Build**: `python3 build.py` (not `python`)
- **Executable**: `./grapa` (not `grapa.exe`)
- **File paths**: Use forward slashes `"./grapa"`
- **Shell**: Standard Mac/Linux shell
- **Test scripts**: `./grapa test/script.grc` with forward slashes

### Cross-Platform Testing
- **CLI testing**: Always load `basic_syntax.md` and `lib/grapa/$grapa.grc` for Grapa scripting syntax rules
- **Debug flag**: Use `-d` flag throughout for debug output
- **Script testing**: Use `-c` option for inline scripts (escape `$` characters)
- **File testing**: Use file option for `.grc` files

### Key Platform Differences
1. **Python command**: `python` (Windows) vs `python3` (Mac/Linux)
2. **Executable name**: `grapa.exe` (Windows) vs `grapa` (Mac/Linux)
3. **Path separators**: Backslashes (Windows) vs forward slashes (Mac/Linux)
4. **Shell environment**: PowerShell/Native Tools (Windows) vs standard shell (Mac/Linux)

---

## 📚 Onboarding & Documentation Strategy

### Current Onboarding Resources

#### For Users
- **Getting Started**: `docs-src/docs/getting_started/`
- **Syntax Reference**: `docs-src/docs/syntax/basic_syntax.md`
- **Migration Guides**: `docs-src/docs/migrations/` (Python, JS, Go, etc.)
- **Examples**: `docs-src/docs/examples/`

#### For Developers
- **Project Status**: `maintainers/DEVELOPMENT/CURRENT_STATUS.md`
- **Development Priorities**: `maintainers/DEVELOPMENT/BACKLOG.md`
- **Implementation Details**: `maintainers/IMPLEMENTATION/`
- **Testing**: `test/` directory with organized test suites

### Known Pain Points & Improvements

#### Language Learning Curve
- **No line comments** - Only block comments (`/* */`) supported
- **No for/foreach loops** - Use `while` or `.range()`+functional methods
- **No string interpolation** - Use concatenation (`"Hello " + name`)
- **No try/catch** - Use `.iferr()` for error handling
- **No implicit truthy/falsy** - Use explicit boolean checks

#### Proposed Improvements
- **Comment Handling**: Consider adding line comments (`//`, `#`)
- **Loop Syntax**: Explore `for`/`foreach` as syntactic sugar
- **String Interpolation**: Add template literal support
- **Module System**: Design import/include system for code reuse
- **Error Handling**: Improve `.iferr()` ergonomics

### Migration Support

#### Available Migration Guides
- **Python**: `docs-src/docs/migrations/python_to_grapa_migration.md`
- **JavaScript**: `docs-src/docs/migrations/js_to_grapa_migration.md`
- **Go**: `docs-src/docs/migrations/go_to_grapa_migration.md`
- **Rust**: `docs-src/docs/migrations/rust_to_grapa_migration.md`
- **TypeScript**: `docs-src/docs/migrations/typescript_to_grapa_migration.md`
- **Ruby**: `docs-src/docs/migrations/ruby_to_grapa_migration.md`
- **Kotlin**: `docs-src/docs/migrations/kotlin_to_grapa_migration.md`
- **Swift**: `docs-src/docs/migrations/swift_to_grapa_migration.md`

#### Key Migration Differences
- **Loops**: Use `while` or `.range()`+functional methods instead of `for`/`foreach`
- **Error Handling**: Use `.iferr()` instead of `try/catch`
- **Comments**: Only block comments, always on their own line
- **Property Access**: `.get()` only for `$file`/`$TABLE`
- **String Operations**: Use concatenation instead of interpolation

---

## 🔄 Documentation Maintenance Policy

### Update Requirements
Any change to the Grapa language requires:
1. **Immediate update** of `docs-src/docs/syntax/basic_syntax.md`
2. **Full scan** of all `.md` files in `docs-src` for code samples
3. **Empirical testing** of all `.grc` code samples
4. **Migration guide updates** if new features address pain points
5. **Cross-linking** all relevant documentation

### Maintenance Checklist
- [ ] Update canonical syntax reference
- [ ] Scan and update all code samples in documentation
- [ ] Re-test all `.grc` scripts for compliance
- [ ] Update migration guides if applicable
- [ ] Cross-link all relevant documentation
- [ ] Update this onboarding guide if needed

### Automation Recommendations
- Consider creating a script to extract all Grapa code blocks from `docs-src`
- Run extracted code as `.grc` scripts and report failures
- Implement CI job for documentation validation

---

## 📋 Onboarding Checklist

### For New Agents
- [ ] Read `maintainers/DEVELOPMENT/CURRENT_STATUS.md`
- [ ] Review `maintainers/DEVELOPMENT/BACKLOG.md`
- [ ] Understand the agent response template above
- [ ] Familiarize with key file locations
- [ ] Know how to handle "load current status" requests

### For New Developers
- [ ] Review project structure and key directories
- [ ] Read canonical syntax reference
- [ ] Understand development workflow
- [ ] Familiarize with testing procedures
- [ ] Know documentation maintenance requirements

### For New Contributors
- [ ] Read user documentation in `docs-src/`
- [ ] Review migration guides for your language
- [ ] Test examples with `.grc` scripts
- [ ] Understand contribution guidelines
- [ ] Know where to find help and resources

---

## 🔗 Related Documentation

- **Project Status**: `maintainers/DEVELOPMENT/CURRENT_STATUS.md`
- **Development Priorities**: `maintainers/DEVELOPMENT/BACKLOG.md`
- **Canonical Syntax**: `docs-src/docs/syntax/basic_syntax.md`
- **Migration Guides**: `docs-src/docs/migrations/`
- **Test Examples**: `test/` directory
- **Implementation Details**: `maintainers/IMPLEMENTATION/`

---

*Last Updated: December 2024*
*For complete project status, see `maintainers/DEVELOPMENT/CURRENT_STATUS.md`* 