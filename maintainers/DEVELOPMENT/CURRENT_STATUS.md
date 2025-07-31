# Grapa Database Development Status

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

**COMMAND COMPLETION RECOGNITION**:
- **Windows `dir`**: Look for directory listing with `d----` folders, `-a---` files, backslash paths, and Windows-style timestamps
- **macOS/Linux `ls`**: Look for directory listing with `d` folders, `-` files, forward slash paths, and Unix-style timestamps
- **Windows OS Check**: 
  - **Command Prompt**: `echo %OS%` should return "Windows_NT" or similar
  - **PowerShell**: `$env:OS` should return "Windows_NT" or similar
- **`uname -s` (macOS/Linux)**: Should return "Darwin" (macOS) or "Linux"

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

---

## **Current Issue: RPTR Corruption During Leaf Shifting** 🔴 **INVESTIGATION ONGOING**

### **Root Cause Analysis** 🔍
- **Primary Issue**: RPTR_ITEM entries appear to point to `RREC (0)` instead of the correct record
- **Corruption Point**: The issue occurs during **record lookup** in the `PtrToRec` method, not in RPTR entry creation
- **Evidence**: Debug shows RPTR entries are created correctly with proper values, but when PtrToRec tries to follow the pointer, it gets a record with value 0
- **Index Ordering**: The RPTR entries are an index that must maintain their alphabetical/key order during leaf shifting

### **Critical New Finding: PtrToRec Function Issue** 🔴
- **Exact Issue Point**: The `PtrToRec` function is returning `recCursor.mValue=0` when accessing RPTR entries
- **Evidence**: Debug output shows:
  ```
  DEBUG: PtrToRec Search - err=-1 recCursor.mValue=0 recCursor.mKey=0
  DEBUG: PtrToRec ENTER - ptrCursor.mValue=0 ptrCursor.mKey=0 ptrCursor.mValueType=0
  DEBUG: PtrToRec - Unknown value type 0, returning error
  ```
- **Root Cause**: The RPTR entry is being created correctly, but when `PtrToRec` tries to follow the pointer, it gets a corrupted value

### **UpdateLeafInfo Logging Implementation** ✅
- **Virtual Function**: Made `UpdateLeafInfo` virtual in GrapaBtree.h
- **Override Added**: Added override in GrapaDB.h and implementation in GrapaDB.cpp
- **Logging Target**: Specifically logs operations on tree 84, first leaf (index 0)
- **Logging Details**: Captures before/after write operations with key values and types
- **Status**: Implementation complete, but no leaf operations for tree 84, first leaf detected in current tests

### **Investigation Progress** 📊
1. ✅ **Identified corruption pattern**: First RPTR entry always points to RREC (0)
2. ✅ **Added comprehensive logging**: SetRecordField, CreateRecord, PtrToRec operations
3. ✅ **Implemented UpdateLeafInfo override**: Ready to capture leaf writing operations
4. 🔍 **Root cause identified**: Issue is in PtrToRec function, not in RPTR creation
5. 🔍 **Next steps**: Investigate why PtrToRec returns value 0 for first RPTR entry

### **Technical Details** 🔧
- **RPTR Entry Creation**: ✅ Working correctly - entries are created with proper values
- **SetRecordField Operations**: ✅ Working correctly - delete/insert operations succeed
- **PtrToRec Function**: 🔴 **ISSUE** - Returns value 0 when accessing first RPTR entry
- **Leaf Operations**: ✅ MoveLeaf operations detected and working correctly

### **Next Investigation Steps** 📋
1. **Analyze PtrToRec function**: Why does it return value 0 for first RPTR entry?
2. **Check RPTR entry structure**: Verify the actual data stored in RPTR entries
3. **Test with larger datasets**: Trigger more leaf operations to see UpdateLeafInfo logging
4. **Fix PtrToRec logic**: Correct the pointer resolution for RPTR entries

### **Current Test Results** 📈
- **1-Record Test**: RPTR corruption confirmed
- **2-Record Test**: RPTR corruption confirmed  
- **UpdateLeafInfo Logging**: Implemented but no tree 84, first leaf operations detected
- **MoveLeaf Operations**: Detected and working correctly
- **PtrToRec Function**: Identified as the root cause of corruption 