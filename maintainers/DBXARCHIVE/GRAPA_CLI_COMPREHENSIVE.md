# Grapa CLI Comprehensive Guide

**Status:** Authoritative reference for all CLI-related development and planning  
**Created:** January 2025  
**Purpose:** Single source of truth for Grapa CLI current state, industry standards, and enhancement roadmap

> **📋 CURRENT STATUS:** CLI redesign Phase 1 completed, Phase 2 in backlog. See **[CURRENT_STATUS.md](CURRENT_STATUS.md)** for project priorities.

---

## 🎯 Executive Summary

Grapa CLI follows a **Python-inspired design philosophy** while maintaining its own identity. The current implementation has completed Phase 1 (standard options) and is ready for Phase 2 enhancements (performance, environment, error handling).

**Key Principles:**
- **Python-like simplicity** with direct execution (`grapa script.grc`, `grapa "'hello'.echo()"`)
- **Standard Unix conventions** for option naming and behavior
- **Intuitive user experience** with auto-detection of input types
- **Separation of concerns** (CLI vs GUI tools)

---

## 📊 Current CLI State (As of January 2025)

### ✅ **Implemented & Working Well**

#### Standard Information Options
```bash
-h, --help          # Show help message ✅
-v, --version       # Show version ✅  
-q, --quiet         # Suppress header output ✅
```

#### Execution Modes
```bash
grapa               # Interactive mode (default) ✅
grapa -i            # Force interactive mode ✅
grapa script.grc    # Direct file execution ✅
grapa "'hello'"     # Direct quoted command ✅
grapa 'hello'.echo() # Direct method call ✅
```

#### Smart Input Detection
```bash
echo "data" | grapa  # Auto-detect pipe input ✅
cat file.txt | grapa # Automatic stdin reading ✅
```

### 🔄 **Needs Standardization**

#### Non-Standard Options (To Be Replaced)
```bash
-c script           # Execute command (standard)
-f file             # Execute file (standard)
-s                  # Read from stdin (standard)
-S                  # Store stdin in $ARGCIN (standard)
```

#### Conflicting Options (Removed)
```bash
# These options have been removed from the implementation
# -c, --console       # Conflicts with standard -c for command
# -e, --env           # Conflicts with standard -e for eval
```

### ❌ **GUI Options (To Be Moved)**
```bash
-w                  # Editor mode → move to separate GUI tool
-wfile file         # Editor with file → move to separate GUI tool
```

### 🔧 **Internal Options (Removed)**
```bash
# These options have been removed from the implementation
# -argv               # Reset argument list (internal use) - REMOVED
# -                   # Force non-exit (internal use) - REPURPOSED as standard stdin option
```

---

## 🐍 Python CLI Standards (Reference Model)

### Core Python CLI Patterns
```bash
# Command execution
python -c "print('hello')"           # Execute command
python -e "print('hello')"           # Execute expression (some versions)

# File execution  
python script.py                     # Direct execution
python -f script.py                  # File execution (some versions)

# Interactive mode
python -i                            # Interactive mode
python                               # Interactive mode (default)

# Input/Output
python -s                            # Read from stdin
python -o output.txt                 # Output to file (not standard)
```

### Python CLI Philosophy
1. **Simplicity first** - Minimal options, intuitive behavior
2. **Direct execution** - `python script.py` works without flags
3. **Standard conventions** - `-c`, `-i`, `-h`, `-v` follow Unix standards
4. **Pipe-friendly** - Works seamlessly with Unix pipes
5. **Interactive by default** - No args = interactive mode

### Python Options We Should Adopt
```bash
-c <command>         # Execute command (standard)
-i                   # Interactive mode (already implemented)
-s                   # Read from stdin (standard)
-h, --help           # Help (already implemented)
-v, --version        # Version (already implemented)
```

---

## 🚀 Proposed CLI Enhancements

### Phase 2: Focused Improvements (Backlog)

#### Enhanced Debug Mode
```bash
-d, --debug          # Enhanced debug mode with verbose output capabilities
--verbose            # User-friendly detailed information (different from debug)
```

#### Better Error Messages
```bash
# Improved syntax error reporting with more descriptive and helpful error messages
# Better error context and suggestions for common issues
```

#### Documentation Updates
```bash
# Update CLI documentation to reference existing language capabilities:
# - Performance: $TIME().utc() for timing, .map() with thread counts for parallel processing
# - Environment: $sys().getenv() and $sys().putenv() for environment management
# - Output Control: Standard shell piping (>, >>, |) already works
# - Profiling: Built-in timing capabilities with nanosecond precision
```

#### Cross-Reference Language Features
```bash
# Add examples showing how to use built-in capabilities from CLI:

# Performance profiling and timing
grapa -c "
start_time = \$TIME().utc();
/* ... operations ... */
end_time = \$TIME().utc();
elapsed_ms = ((end_time - start_time) / 1000000).int();
('Performance: ' + elapsed_ms + ' ms').echo();
"

# Environment management
grapa -c "\$sys().getenv('USERNAME').echo();"
grapa -c "\$sys().putenv('DEBUG_MODE', 'true');"

# Parallel processing
grapa -c "large_data = (1000000).range(0,1); squares = large_data.map(op(x) { x * x; }, 8);"

# Output control with shell piping
grapa -c "'Hello World'.echo()" > output.txt
grapa -c "'Hello World'.echo()" | grep "Hello"

# Dynamic code execution
grapa -c "template = 'name + \"! You are \" + age.str() + \" years old.\"'; result = op()(template)();"

# String templates
grapa -c "greeting = op('name'=0, 'time'=0){ 'Good ' + time + ', ' + name + '!' }; greeting('Alice', 'morning').echo();"

# Error handling
grapa -c "iferr { result = 10 / 0; } { ('Error: ' + \$sys.error).echo(); }"

# Database operations
grapa -c "db = \$file().table('ROW'); db.mkfield('name', 'STR'); db.set('user1', 'Alice', 'name');"

# File operations
grapa -c "files = \$file().ls('.'); for (file in files) { ('File: ' + file.name).echo(); }"
```

### Database-Specific CLI Features
```bash
--db-index <name>    # Create custom database index
--db-optimize        # Optimize database performance
--db-backup <file>   # Backup database
--db-restore <file>  # Restore database
```

---

## 🔧 Implementation Details

### Current Implementation Analysis

#### Option Parsing (GrapaLink.cpp)
```cpp
// Information options (working well)
if ((e->mValue.Cmp("-h") == 0) || (e->mValue.Cmp("--help") == 0))
{
    showHelp = true;
    needExit = true;
}

// Non-standard options (need replacement)
else if (e->mValue.Cmp("-ccmd") == 0)
{
    e = e->Next();
    if (e)
        runStr.FROM(e->mValue);
    needExit = true;
}
```

#### Smart Detection Features
```cpp
// Direct execution detection
if (e->mValue.mLength >= 2 && 
    ((e->mValue.mBytes[0] == '"' && e->mValue.mBytes[e->mValue.mLength - 1] == '"') ||
     (e->mValue.mBytes[0] == '\'' && e->mValue.mBytes[e->mValue.mLength - 1] == '\'')))
{
    // Execute quoted command
}

// Method call detection
else if (strstr((char*)e->mValue.mBytes, ".echo()") != NULL)
{
    // Execute method call
}

// File detection
else if (strstr((char*)e->mValue.mBytes, ".grc") != NULL)
{
    // Execute script file
}
```

### Migration Strategy

#### Backward Compatibility
**Decision:** No backward compatibility needed
**Rationale:** 
- Current options are non-standard and confusing
- Better to have clean, standard interface
- Users will adapt quickly to standard options

#### Implementation Priority
1. **Replace non-standard options** (easy, immediate impact)
2. **Add missing standard options** (moderate effort, high value)
3. **Remove conflicting options** (easy, reduces confusion)
4. **Add advanced features** (complex, future enhancement)

---

## 📋 Success Criteria

### Phase 1 Success Criteria ✅ COMPLETED
- [x] `grapa -c "'hello'.echo()"` works ✅ **COMPLETED**
- [x] `grapa -f script.grc` works ✅ **COMPLETED**
- [x] `grapa -s` works ✅ **COMPLETED**
- [x] `grapa -S` works ✅ **COMPLETED**
- [x] Help text updated with new options
- [x] All existing functionality preserved
- [x] No GUI options in CLI help

### Phase 2 Success Criteria 📋 BACKLOG
- [ ] `grapa -j 4` uses 4 parallel workers
- [ ] `grapa --no-parallel` disables parallelism
- [ ] `grapa -E "VAR=value"` sets environment
- [ ] `grapa --strict` fails on warnings
- [ ] `grapa --continue` continues on errors
- [ ] `grapa -o file.txt` redirects output
- [ ] `grapa -d` shows debug output
- [ ] Performance profiling works
- [ ] Execution tracing works

---

## 🎯 Industry Standards Reference

### Unix/Linux CLI Conventions
```bash
-c <command>         # Execute command (bash, python, node)
-f <file>            # Execute file (bash, python)
-i                    # Interactive mode (python, node, bash)
-s                    # Read from stdin (bash, python)
-h, --help           # Help (universal)
-v, --version        # Version (universal)
-q, --quiet          # Quiet mode (universal)
-d, --debug          # Debug mode (common)
-j, --jobs <N>       # Parallel jobs (make, xargs)
```

### Language-Specific Patterns

#### Python
```bash
python -c "print('hello')"     # Command execution
python -i                       # Interactive mode
python script.py               # Direct execution
python -m module               # Module execution
```

#### Node.js
```bash
node -e "console.log('hello')" # Expression execution
node -i                        # Interactive mode
node script.js                 # Direct execution
node -p "2+2"                  # Print result
```

#### Ruby
```bash
ruby -e "puts 'hello'"         # Expression execution
ruby -i                        # Interactive mode
ruby script.rb                 # Direct execution
ruby -n                        # Process each line
```

---

## 🚨 Risk Assessment

### Low Risk
- Option name changes (standard conventions)
- Adding verbose/debug options
- File output redirection
- Removing internal options

### Medium Risk  
- Parallel worker control (affects performance)
- Environment variable handling
- Error handling changes
- Removing GUI options (may break existing workflows)

### High Risk
- Performance profiling (complex implementation)
- AST/bytecode dumping (exposes internals)
- Database-specific features (domain complexity)

---

## 📚 References

### External Standards
- [CLI Design Guidelines](https://clig.dev/) - Standard CLI conventions
- [Python CLI Options](https://docs.python.org/3/using/cmdline.html) - Reference implementation
- [Node.js CLI Options](https://nodejs.org/api/cli.html) - Reference implementation
- [Bash CLI Options](https://www.gnu.org/software/bash/manual/html_node/Invoking-Bash.html) - Reference implementation

### Internal Documentation
- **[CURRENT_STATUS.md](CURRENT_STATUS.md)** - Project priorities and status
- **[BACKLOG.md](BACKLOG.md)** - Phase 2 CLI enhancements
- **GrapaLink.cpp** - Current implementation source

---

## 🎯 Next Steps

### Immediate (Phase 1 - Completed ✅)
- Standard option replacement
- Basic missing options
- GUI option removal
- Documentation updates

### Future (Phase 2 - Backlog 📋)
- Performance options
- Environment management
- Error handling
- Advanced debugging
- Database-specific features

### Long-term Vision
- **Python-inspired simplicity** with Grapa's unique strengths
- **Standard Unix conventions** for maximum compatibility
- **Intuitive user experience** that "just works"
- **Separation of concerns** between CLI and GUI tools
- **Extensible architecture** for future enhancements

---

**Last Updated:** January 2025  
**Status:** Phase 1 Complete, Phase 2 Ready for Implementation  
**Focus:** Python-inspired CLI with standard Unix conventions 