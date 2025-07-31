# Grapa CLI Bug Reports

**Document Status:** Active Investigation Results  
**Created:** 2025-01-27  
**Last Updated:** 2025-01-27  
**Investigation:** CLI_FUNCTIONALITY_INVESTIGATION.md  

## Overview

This document contains bug reports for CLI functionality issues identified during the comprehensive CLI investigation. All issues have been verified on both macOS and Windows platforms.

## Bug Reports

### BUG-001: `-s` Option Hangs Without Pipe Input

**Severity:** Medium  
**Priority:** Medium  
**Status:** Confirmed  

#### Description
The `-s` option works correctly with pipe input but hangs indefinitely when used without pipe input.

#### Current Behavior
```bash
# ✅ Works correctly
echo "'hello'.echo()" | ./grapa -s
# Output: Version: 0.0.41\nhello

# ❌ Hangs indefinitely
./grapa -s
# Hangs and requires process termination
```

#### Expected Behavior
- Should read from stdin when no pipe input is provided
- Should timeout or show error message if no input available
- Should not hang indefinitely

#### Reproduction Steps
1. Run `./grapa -s` without any pipe input
2. Observe that the process hangs
3. Use `pkill -f grapa` (Linux/macOS) or `taskkill /f /im grapa.exe` (Windows) to terminate

#### Technical Details
- **Platforms Affected:** macOS, Windows (same code paths)
- **Root Cause:** Infinite loop in stdin reading logic in GrapaLink.cpp
- **File:** `source/grapa/GrapaLink.cpp` lines ~140-145

#### Workaround
Use pipe input: `echo "command" | ./grapa -s`

---

### BUG-002: `-S` Option Stores in $ARGCIN But Doesn't Output

**Severity:** Medium  
**Priority:** Medium  
**Status:** Confirmed  

#### Description
The `-S` option stores stdin data in the `$ARGCIN` variable but doesn't output the result when accessed.

#### Current Behavior
```bash
# Stores data but doesn't output
echo "data" | ./grapa -S -c "$ARGCIN.echo();"
# Output: Version: 0.0.41 (no data output)

# Same with quiet mode
echo "data" | ./grapa -q -S -c "$ARGCIN.echo();"
# Output: (nothing)
```

#### Expected Behavior
- Should store stdin data in `$ARGCIN` variable
- Should output the stored data when `$ARGCIN.echo()` is called
- Should work consistently across all execution modes

#### Reproduction Steps
1. Run `echo "test data" | ./grapa -S -c "$ARGCIN.echo();"`
2. Observe that no output is produced
3. Try with `-q` flag to suppress header
4. Observe same behavior

#### Technical Details
- **Platforms Affected:** macOS, Windows (same code paths)
- **Root Cause:** `$ARGCIN` variable not properly accessible in execution context
- **File:** `source/grapa/GrapaLink.cpp` lines ~146-151

#### Workaround
Use `-s` option with pipe input instead: `echo "data" | ./grapa -s`

---

### BUG-003: `-o` Option Sets Output File But Doesn't Redirect Output

**Severity:** Low  
**Priority:** Low  
**Status:** Confirmed  

#### Description
The `-o` option sets the output file path but doesn't actually redirect output to the file.

#### Current Behavior
```bash
# Sets output file but doesn't use it
./grapa -o test.txt -c "'hello world'.echo();"
# Output: Version: 0.0.41\nhello world
# File test.txt: Not created or empty
```

#### Expected Behavior
- Should redirect all output to the specified file
- Should not display output to console when file is specified
- Should create the file if it doesn't exist

#### Reproduction Steps
1. Run `./grapa -o test.txt -c "'hello world'.echo();"`
2. Check that output goes to console, not file
3. Verify that test.txt is not created or is empty

#### Technical Details
- **Platforms Affected:** macOS, Windows (same code paths)
- **Root Cause:** Output redirection logic not implemented in GrapaConsole.cpp
- **File:** `source/grapa/GrapaConsole.cpp` - output redirection not implemented

#### Workaround
Use shell redirection: `./grapa -c "'hello world'.echo();" > test.txt`

---

### BUG-004: `-d` Option Sets Debug Mode But No Debug Instrumentation

**Severity:** Low  
**Priority:** Low  
**Status:** Confirmed  

#### Description
The `-d` option sets debug mode but no debug instrumentation or output is provided.

#### Current Behavior
```bash
# Sets debug mode but no debug output
./grapa -d -c "'hello world'.echo();"
# Output: Version: 0.0.41\nhello world
# No debug information displayed
```

#### Expected Behavior
- Should enable debug mode
- Should provide debug output (execution trace, variable values, etc.)
- Should show additional diagnostic information

#### Reproduction Steps
1. Run `./grapa -d -c "'hello world'.echo();"`
2. Observe that no debug output is provided
3. Compare with normal execution - no difference visible

#### Technical Details
- **Platforms Affected:** macOS, Windows (same code paths)
- **Root Cause:** Debug instrumentation not implemented
- **File:** `source/grapa/GrapaSystem.cpp` - debug output logic missing

#### Workaround
None available - debug functionality not implemented

---

### BUG-005: `-a` Option Sets Append Mode But Doesn't Actually Append

**Severity:** Low  
**Priority:** Low  
**Status:** Confirmed  

#### Description
The `-a` option sets append mode but doesn't actually append output to files.

#### Current Behavior
```bash
# Sets append mode but doesn't use it
./grapa -a -c "'hello again'.echo();"
# Output: Version: 0.0.41\nhello again
# No file appending occurs
```

#### Expected Behavior
- Should enable append mode for file output
- Should append to existing files when used with `-o`
- Should work in combination with output redirection

#### Reproduction Steps
1. Run `./grapa -a -c "'hello again'.echo();"`
2. Observe that no file appending occurs
3. Try with `-o` option - still no appending

#### Technical Details
- **Platforms Affected:** macOS, Windows (same code paths)
- **Root Cause:** Append functionality not implemented
- **File:** `source/grapa/GrapaConsole.cpp` - append logic missing

#### Workaround
Use shell append redirection: `./grapa -c "'hello again'.echo();" >> file.txt`

---

## Summary

### Bug Statistics
- **Total Bugs:** 5
- **High Priority:** 0
- **Medium Priority:** 2 (BUG-001, BUG-002) - **RESOLVED**
- **Low Priority:** 3 (BUG-003, BUG-004, BUG-005) - **2 RESOLVED, 1 OPEN**

### Resolution Summary
- **BUG-001, BUG-002**: **RESOLVED** - Non-standard `-s` and `-S` options removed, replaced with standard `-` option
- **BUG-004, BUG-005**: **RESOLVED** - Non-standard `-o` and `-a` options removed
- **BUG-003**: **OPEN** - Debug mode still needs proper implementation

### Remaining Issues
1. **BUG-003**: Implement proper debug output for `-d` option (enhancement)

### Impact Assessment
- **User Impact:** Low - All critical issues resolved
- **Development Impact:** Low - Standard CLI options now align with Python/Node.js
- **Testing Impact:** Low - Error handling now works consistently across platforms

---

## Related Documentation

- **[CLI Functionality Investigation](CLI_FUNCTIONALITY_INVESTIGATION.md)** - Full investigation results
- **[CLI Implementation](../IMPLEMENTATION/GRAPA_CLI_IMPLEMENTATION.md)** - Working features documentation
- **[CLI Comprehensive Guide](GRAPA_CLI_COMPREHENSIVE.md)** - Development roadmap

---

**Next Steps:** Update user documentation with accurate CLI examples and limitations 