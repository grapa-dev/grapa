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
-ccmd script        # Execute command → should be -c
-cfile file         # Execute file → should be -f  
-ccin               # Read from stdin → should be -s
-argcin             # Store stdin in $ARGCIN → should be -S
```

#### Conflicting Options (To Be Removed)
```bash
-c, --console       # Conflicts with standard -c for command
-e, --env           # Conflicts with standard -e for eval
```

### ❌ **GUI Options (To Be Moved)**
```bash
-w                  # Editor mode → move to separate GUI tool
-wfile file         # Editor with file → move to separate GUI tool
```

### 🔧 **Internal Options (To Be Removed)**
```bash
-argv               # Reset argument list (internal use)
-                   # Force non-exit (internal use)
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
-s                   # Read from stdin (replace -ccin)
-h, --help           # Help (already implemented)
-v, --version        # Version (already implemented)
```

---

## 🚀 Proposed CLI Enhancements

### Phase 2: Advanced Features (Backlog)

#### Performance Options
```bash
-j, --jobs <N>       # Parallel workers (like make -j)
--no-parallel        # Disable parallelism
--profile            # Performance profiling
```

#### Environment Management
```bash
-E, --env <VAR=value> # Set environment variable
--env-file <file>     # Load environment file
```

#### Error Handling
```bash
--strict             # Strict mode (fail on warnings)
--continue           # Continue on errors
--max-errors <N>     # Stop after N errors
```

#### Advanced Debugging
```bash
-d, --debug          # Debug mode
--trace              # Execution trace
--dump-ast           # Show parsed AST
--dump-bytecode      # Show compiled bytecode
```

#### Output Control
```bash
-o, --output <file>  # Redirect output to file
-a, --append         # Append to file
--verbose            # Verbose output (different from -v version)
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
- [x] `grapa -c "'hello'.echo()"` works (replaces `-ccmd`)
- [x] `grapa -f script.grc` works (replaces `-cfile`)
- [x] `grapa -s` works (replaces `-ccin`)
- [x] `grapa -S` works (replaces `-argcin`)
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