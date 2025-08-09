# Grapa Project Onboarding Guide

This guide provides comprehensive onboarding information for new agents, developers, and contributors to the Grapa project.

---

## 🤖 Agent Onboarding

### ⚠️ CRITICAL SAFEGUARDS

**BEFORE making any changes to this project, agents MUST:**

1. **ALWAYS follow the discovery path** - Never skip the onboarding process
2. **NEVER modify onboarding files** without explicit user permission
3. **ALWAYS verify build commands** work before suggesting changes
4. **NEVER assume platform-specific behavior** - Always check Windows vs Mac/Linux differences
5. **ALWAYS test CLI functionality** before claiming features work
6. **NEVER modify canonical syntax** without empirical testing
7. **ALWAYS preserve the working onboarding process** - This is a critical system

### Discovery Path for New Agents

**⚠️ MANDATORY: You MUST complete ALL 6 steps in order before proceeding with any work:**

1. **Start Here**: Read this ONBOARD.md file (you're here!)
2. **Project Status**: Read `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md` for current priorities
3. **Directory Structure**: Review `maintainers/index.md` for complete directory organization
4. **Maintainer Guidance**: Check `maintainers/README.md` for maintainer-specific guidance
5. **Canonical Syntax**: Review `docs-src/docs/syntax/basic_syntax.md` for language rules
6. **Development Priorities**: Read `maintainers/PROJECT_MANAGEMENT/BACKLOG.md` for roadmap

**❌ DO NOT PROCEED until you have completed ALL 6 steps above**
**❌ DO NOT create new documentation until you have completed the discovery path**
**❌ DO NOT assume you understand the project without following this path**

### Discovery Path Validation Checklist

**After completing each step, you must acknowledge completion:**

- [ ] **Step 1**: ONBOARD.md read and understood
- [ ] **Step 2**: CURRENT_STATUS.md read - current priorities understood
- [ ] **Step 3**: index.md reviewed - directory structure understood
- [ ] **Step 4**: README.md checked - maintainer guidance understood
- [ ] **Step 5**: basic_syntax.md reviewed - language rules understood
- [ ] **Step 6**: BACKLOG.md read - development priorities understood

**Only proceed when all steps are complete and checked off.**

### Common Agent Mistakes to Avoid

❌ **Creating new TODO files** - Use existing CURRENT_STATUS.md and BACKLOG.md
❌ **Skipping the discovery path** - Always complete all 6 steps in order
❌ **Creating documentation without reading existing docs** - Check what already exists first
❌ **Assuming you understand the project** - Follow the discovery path first
❌ **Cherry-picking what to read** - Complete the entire discovery path systematically
❌ **Creating personal tracking files** - Work within the existing documentation structure

**Key Discovery Files:**
- `maintainers/index.md` - Complete directory navigation
- `maintainers/README.md` - Maintainer onboarding and guidance
- `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md` - **AUTHORITATIVE** project status
- `maintainers/PROJECT_MANAGEMENT/BACKLOG.md` - Development priorities
- `docs-src/docs/syntax/basic_syntax.md` - Canonical syntax reference

### Agent Validation Checklist

**Before responding to any user request, verify:**

- [ ] I have read ONBOARD.md completely
- [ ] I have followed the discovery path (all 6 steps completed)
- [ ] I understand the current project status from CURRENT_STATUS.md
- [ ] I know where to find key information in the documentation
- [ ] I will not create new documentation without checking existing docs first
- [ ] I will use the established response templates below

**If any item is unchecked, complete the discovery path first.**

### Quick Reference for New Agents

**If user says "onboard" or "load current status and tell me next steps":**

**⚠️ REQUIRED RESPONSE: Use this exact template after completing the discovery path:**

"Based on the current Grapa project status, here are the key areas of focus:

📋 **Current Priorities:**
1. Unicode Language Binding - Add case_fold(), Unicode-aware string methods
2. String Interpolation - Implement template literal-style interpolation
3. CLI Enhancement - Performance options, environment management, error handling
4. Optimization Implementation - Performance improvements for arithmetic, bitwise, comparison, and assignment operators
5. Core Language Features - Line comments, loop syntax, error handling improvements

🔗 **Key Resources:**
- `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md` - Complete project status
- `maintainers/PROJECT_MANAGEMENT/BACKLOG.md` - Development priorities
- `docs-src/` - User documentation
- `test/` - Test scripts and validation

**Next Steps:**
1. Choose a specific area to focus on from the current priorities
2. Review the current status and backlog for detailed information
3. Select a high-priority item to begin work on

Would you like me to focus on any specific area, or do you have questions about the current implementation?"

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
- `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md` - Complete project status
- `maintainers/PROJECT_MANAGEMENT/BACKLOG.md` - Development priorities and tasks
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
- Start with `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md` for current priorities
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
  - **Build Options**: `python build.py -h` to see all options
  - **Quick Build**: `python build.py --exe-only` for fast iterative development
  - **Full Build**: `python build.py` for complete build with libraries and packaging
  - **Clean Build**: `python build.py --clean` to clean build artifacts
  - **Test Build**: `python build.py --test` to run tests after build
- **Executable**: `grapa.exe` (not `./grapa`)
- **File paths**: Use backslashes `"test\grapa.exe"`
- **Shell**: Use PowerShell or x64 Native Tools Command Prompt for VS 2022
- **Test scripts**: `python test\script.grc` with backslashes

### Mac/Linux Commands
- **Build**: `python3 build.py` (not `python`)
  - **Build Options**: `python3 build.py -h` to see all options
  - **Quick Build**: `python3 build.py --exe-only` for fast iterative development
  - **Full Build**: `python3 build.py` for complete build with libraries and packaging
  - **Clean Build**: `python3 build.py --clean` to clean build artifacts
  - **Test Build**: `python3 build.py --test` to run tests after build
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

### 🛡️ Protecting the Onboarding Process

**This onboarding system is CRITICAL and must be preserved:**

- **DO NOT** modify `ONBOARD.md`, `CURRENT_STATUS.md`, or `BACKLOG.md` without explicit permission
- **DO NOT** change the discovery path order or structure
- **DO NOT** remove or modify the safeguards above
- **DO NOT** assume build commands work without testing them
- **DO NOT** claim CLI features work without verifying them
- **ALWAYS** test any suggested changes before implementing them
- **ALWAYS** preserve the working "onboard" command functionality

**If you need to update onboarding documentation:**
1. Get explicit user permission
2. Test all changes thoroughly
3. Verify the "onboard" command still works
4. Update this section if you modify the safeguards
5. Review [`ONBOARDING_SAFEGUARDS.md`](ONBOARDING_SAFEGUARDS.md) for additional protection guidelines

---

## 📚 Onboarding & Documentation Strategy

### Current Onboarding Resources

#### For Users
- **Getting Started**: `docs-src/docs/getting_started/`
- **Syntax Reference**: `docs-src/docs/syntax/basic_syntax.md`
- **Migration Guides**: `docs-src/docs/migrations/` (Python, JS, Go, etc.)
- **Examples**: `docs-src/docs/examples/`

#### For Developers
- **Project Status**: `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md`
- **Development Priorities**: `maintainers/PROJECT_MANAGEMENT/BACKLOG.md`
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
- [ ] Read `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md`
- [ ] Review `maintainers/PROJECT_MANAGEMENT/BACKLOG.md`
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

### 🔍 Agent Validation Checklist

**Before making any changes, agents MUST verify:**

- [ ] **Build System**: `python build.py --exe-only` works on Windows
- [ ] **CLI Functionality**: `.\grapa.exe -h` shows correct options
- [ ] **Discovery Path**: All 6 discovery steps are accessible
- [ ] **Cross-Platform**: Both Windows and Mac/Linux commands documented
- [ ] **Syntax Rules**: Canonical syntax reference is accurate
- [ ] **Documentation Links**: All internal links work correctly
- [ ] **Onboarding Process**: "onboard" command works as expected

**If any of these fail, STOP and ask for guidance before proceeding.**

### 📝 Documentation Update Rules

**When updating documentation:**

- [ ] **Search existing docs** before creating new ones
- [ ] **Update existing docs** rather than creating duplicates
- [ ] **Preserve `CURRENT_STATUS.md`** - only add status changes
- [ ] **Use proper directory structure** for content type
- [ ] **Cross-reference related documents**
- [ ] **Update index files** if adding new documents

**❌ NEVER create new documentation without:**
- [ ] Completing the full discovery path first
- [ ] Checking if the information already exists elsewhere
- [ ] Understanding the project's documentation structure
- [ ] Following the established naming and placement conventions

**Documentation Placement Guide:**
- **Investigation Results**: `maintainers/RESEARCH_AND_ANALYSIS/`
- **Technical Details**: `maintainers/IMPLEMENTATION/`
- **Analysis & Planning**: `maintainers/DEVELOPMENT/`
- **Status Changes**: `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md`
- **Personal Progress Tracking**: Use existing WIP files, don't create new TODO files

**Documentation Placement Guide:**
- **Investigation Results**: `maintainers/RESEARCH_AND_ANALYSIS/`
- **Technical Details**: `maintainers/IMPLEMENTATION/`
- **Analysis & Planning**: `maintainers/DEVELOPMENT/`
- **Status Changes**: `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md`

### 🚨 Agent Switching Protection

**CRITICAL: If you detect you are a new agent with no context:**

1. **STOP immediately** - Do not proceed with any actions
2. **Request onboarding** - Say "I need to onboard to understand the current context"
3. **Ask for context** - Request user to describe what was being worked on
4. **Verify state** - Check if system is in an incomplete state
5. **Wait for guidance** - Do not make assumptions about what should be done next

**Signs of incomplete context:**
- User mentions "continuing" or "next step" but you have no context
- User refers to previous work you don't know about
- User mentions specific files or changes you can't see
- User asks about progress on something you don't understand
- User seems frustrated or confused by your responses

**Safe response when context is missing:**
```
"I notice I may be missing context from previous work. To ensure I don't make incorrect assumptions or cause issues:

1. Could you briefly describe what was being worked on?
2. Are there any incomplete changes or processes I should be aware of?
3. Should I onboard first to understand the current project state?

This will help me avoid making rogue actions or incomplete changes."
```

**⚠️ CRITICAL: If you detect you are a new agent with no context:**

1. **STOP immediately** - Do not proceed with any actions
2. **Request onboarding** - Say "I need to onboard to understand the current context"
3. **Ask for context** - Request user to describe what was being worked on
4. **Verify state** - Check if system is in an incomplete state
5. **Wait for guidance** - Do not make assumptions about what should be done next
6. **Follow discovery path** - Complete all 6 steps before proceeding

**Signs of incomplete context:**
- User mentions "continuing" or "next step" but you have no context
- User refers to previous work you don't know about
- User mentions specific files or changes you can't see
- User asks about progress on something you don't understand
- User seems frustrated or confused by your responses

---

## 🔗 Related Documentation

- **Project Status**: `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md`
- **Development Priorities**: `maintainers/PROJECT_MANAGEMENT/BACKLOG.md`
- **Canonical Syntax**: `docs-src/docs/syntax/basic_syntax.md`
- **Migration Guides**: `docs-src/docs/migrations/`
- **Test Examples**: `test/` directory
- **Implementation Details**: `maintainers/IMPLEMENTATION/`

---

*Last Updated: December 2024*
*For complete project status, see `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md`* 