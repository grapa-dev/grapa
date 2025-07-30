# File-Based Database Operations Investigation

## 📋 **CURRENT STATUS: INVESTIGATING INDEX CORRUPTION IN FILE-BASED DATABASES**

### 🎯 **CURRENT TASK: Fix File-Based Database Operations**

**Status**: 🔄 **IN PROGRESS** - Investigating index corruption in file-based databases

**Latest Update**: Investigating why `GrapaDB::DumpThePointer` outputs 0 for `cursor.mValue` while `GrapaDB::DumpTheRowRec` outputs a value for `cursor.mValue`. This is related to the index corruption issue affecting file-based database operations.

### 📋 **INVESTIGATION PLAN**

**Phase 1: Class Hierarchy Analysis** 🔥 **CURRENT PRIORITY**
- [ ] **GrapaGroup → GrapaDB → GrapaBTree** inheritance chain investigation
- [ ] **Supporting classes and data structures** within these classes
- [ ] **Index corruption root cause** identification
- [ ] **DumpThePointer vs DumpTheRowRec** behavior analysis

**Phase 2: Specific Investigation Tasks**
- [ ] **Why DumpThePointer outputs 0 for cursor.mValue**
- [ ] **Why DumpTheRowRec outputs a value for cursor.mValue**
- [ ] **Index corruption patterns** in file-based operations
- [ ] **BTree traversal issues** in file-based storage

**Phase 3: Root Cause Analysis and Fix Implementation**
- [ ] **Identify specific root cause** of index corruption
- [ ] **Determine exact failure point** in file-based operations
- [ ] **Implement targeted fix** based on investigation findings
- [ ] **Test file-based read/write operations**

## 🚀 **AGENT ONBOARDING**

**⚠️ CRITICAL**: New agents MUST complete all onboarding steps before proceeding with any investigation or development work. This ensures proper environment setup and syntax understanding.

### 🔧 **PLATFORM-SPECIFIC TEST SETUP**

**Current Platform**: Windows AMD64
**Terminal**: x64 Native Tools Command Prompt for VS 2022

#### **Windows Test Setup**
```bash
# Build Command
python build.py --exe-only

# Test Commands
.\grapa.exe -h                    # Help
.\grapa.exe -c "script"           # Script execution
.\grapa.exe folder\file.grc       # File execution

# Primary Test File
.\grapa test\test_row_small.grc
```

#### **macOS Test Setup**
```bash
# Build Command
python3 build.py --exe-only

# Test Commands
./grapa -h                        # Help
./grapa -c "script"              # Script execution
./grapa folder/file.grc          # File execution

# Primary Test File
./grapa test/test_row_small.grc
```

#### **Linux Test Setup**
```bash
# Build Command
python3 build.py --exe-only

# Test Commands
./grapa -h                        # Help
./grapa -c "script"              # Script execution
./grapa folder/file.grc          # File execution

# Primary Test File
./grapa test/test_row_small.grc
```

### 🎯 **IMMEDIATE TASKS FOR NEW AGENTS**

**⚠️ CRITICAL FIRST STEP**: Run platform identification commands to determine your environment:
- **Windows**: `dir` (shows files with backslashes), `echo %OS%`
- **macOS/Linux**: `ls` (shows files with forward slashes), `uname -s`

**Use the appropriate shell syntax for your platform**:
- **Windows**: Backslash paths (`.\grapa.exe`), `dir` for listing
- **macOS/Linux**: Forward slash paths (`./grapa`), `ls` for listing

1. **Environment Setup**
   - [ ] Run platform identification commands (`dir` vs `ls`, `echo %OS%` vs `uname -s`)
   - [ ] Verify correct platform (Windows/macOS/Linux) based on command output
   - [ ] Use appropriate build command for platform (`python` vs `python3`)
   - [ ] Use correct executable path and syntax (backslash vs forward slash)
   - [ ] Test basic CLI functionality with platform-appropriate commands

2. **Investigation Setup**
   - [ ] Build Grapa from source using platform-specific command
   - [ ] Test executable with help command
   - [ ] Run primary test file: `test_row_small.grc`
   - [ ] Verify file-based database operations

3. **Current Investigation Focus**
   - [ ] Analyze `GrapaGroup → GrapaDB → GrapaBTree` inheritance chain
   - [ ] Investigate `DumpThePointer` vs `DumpTheRowRec` behavior differences
   - [ ] Identify root cause of index corruption in file-based databases
   - [ ] Document findings in investigation plan

### 📚 **REQUIRED GRAPA SYNTAX REFERENCE**

**⚠️ CRITICAL**: Before running any Grapa scripts, agents must load these syntax references:
- **Basic Syntax**: `docs-src/docs/syntax/basic_syntax.md` - Core Grapa language rules
- **Grammar Rules**: `lib/grapa/$grapa.grc` - Complete language grammar definition

**Required for all CLI testing** (both `-c` option and file execution):
- **Script Syntax**: Understanding Grapa script structure and commands
- **Database Operations**: Proper syntax for database creation, queries, and manipulation
- **Error Handling**: Correct `.iferr()` usage and error patterns
- **Data Types**: Proper handling of `$STR`, `$INT`, `$FLOAT`, `$TABLE`, etc.

### 📋 **INVESTIGATION PLAN**

### 🎯 **SUCCESS CRITERIA**
- ✅ **Root Cause Identified**: Why DumpThePointer vs DumpTheRowRec behave differently
- ✅ **Index Corruption Fixed**: File-based database operations work correctly
- ✅ **Performance Improved**: File-based operations match in-memory performance
- ✅ **Test Validation**: `test_row_small.grc` runs successfully

### 🔗 **RELATED DOCUMENTATION**
- **Backlog**: `BACKLOG.md` - Contains all other tasks and completed work
- **Test Files**: `test\test_row_small.grc` - Primary test file for investigation
- **Build System**: `build.py` - Windows build process

### 📊 **EXPECTED OUTCOMES**
- **File-Based Operations**: Working correctly without index corruption
- **Performance**: File-based operations comparable to in-memory
- **Index Usage**: Proper index utilization instead of table scan fallback
- **Test Results**: Successful execution of test_row_small.grc

## 📝 **DOCUMENTATION MAINTENANCE**

### ⚠️ **CRITICAL REQUIREMENT**: Keep Documentation Updated

**After ANY iteration with new information or status changes, agents MUST update**:
- [ ] **Current Status**: This document (`CURRENT_STATUS.md`) with latest findings
- [ ] **Investigation Progress**: Update investigation plan with completed tasks
- [ ] **Technical Findings**: Document any new discoveries in relevant sections
- [ ] **Platform-Specific Results**: Update any platform-specific findings
- [ ] **Related Documents**: Update `BACKLOG.md` or other maintainers documents as needed

### 🔄 **RESTART READINESS**

**This ensures that if a new agent takes over**:
- ✅ **Complete Context**: All findings and progress are captured
- ✅ **Onboarding Clarity**: New agents know exactly what to do first
- ✅ **No Information Loss**: Critical discoveries are preserved
- ✅ **Continuity**: Investigation can continue seamlessly

### 📋 **UPDATE CHECKLIST**

**Before completing any session, verify**:
- [ ] All investigation findings are documented in this file
- [ ] Investigation plan is updated with completed tasks
- [ ] Technical discoveries are captured with sufficient detail
- [ ] Platform-specific results are recorded
- [ ] Any relevant backlog items are updated
- [ ] Onboarding section remains clear and complete 