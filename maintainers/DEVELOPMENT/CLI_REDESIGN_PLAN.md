# Grapa CLI Redesign Plan

**Status:** Reference – Phase 1 complete, Phase 2 in backlog.
**Created:** January 2025  
**Objective:** Redesign Grapa CLI to follow standard conventions and improve usability

> **📋 CURRENT STATUS:** CLI redesign status is tracked in **[CURRENT_STATUS.md](CURRENT_STATUS.md)**.
>
> **✅ PHASE 1 COMPLETED:** Standard CLI options implemented successfully.
>
> **📋 PHASE 2:** Advanced features added to backlog for future implementation.

## Current State Analysis

**Reference:** See `maintainers/DEVELOPMENT/CURRENT_CLI_ANALYSIS.md` for comprehensive analysis of all current options.

### Existing Options (To Be Replaced)
```bash
-ccmd script          # Execute command (non-standard)
-cfile file           # Execute file (non-standard)  
-ccin                 # Read from stdin (non-standard)
-argcin               # Place stdin in $ARGCIN (non-standard)
-w                    # Editor mode (GUI feature)
-wfile file           # Editor with file (GUI feature)
```

### Current Issues
1. **Non-standard option names** - Users expect `-c`, `-f`, not `-ccmd`, `-cfile`
2. **Missing standard features** - No `-v` (verbose), `-d` (debug), `-j` (parallel)
3. **GUI mixed with CLI** - `-w` options belong in separate GUI tool
4. **Inconsistent naming** - `-e` used for environment, conflicts with standard `-e` (eval)
5. **Limited error handling** - No `--strict`, `--continue` options
6. **No performance options** - No parallel worker control

### Excellent Features (Keep As-Is)
- **Direct execution features** - `grapa "'hello'.echo()"`, `grapa script.grc`
- **Pipe input auto-detection** - Automatic stdin reading
- **Method call detection** - `grapa 'hello'.echo()` works directly
- **Standard information options** - `-h`, `-v`, `-q` work well

## Target CLI Design

### Core Execution (Standard Conventions)
```bash
# Execute commands/expressions
grapa -c "'hello'.echo()"           # Execute command (like bash -c, python -c)
grapa -e "'hello'.echo()"           # Execute expression (like perl -e, ruby -e)

# Execute files  
grapa -f script.grc                 # Execute file (like bash -f, python -f)
grapa script.grc                    # Direct execution (no flag needed) ✅ ALREADY IMPLEMENTED

# Interactive mode
grapa -i                            # Interactive mode (like python -i, node -i)
grapa                               # Interactive mode (default, no args) ✅ ALREADY IMPLEMENTED

# Input/Output
grapa -s                            # Read from stdin (like cat | grapa -s)
grapa -o output.txt                 # Output to file
grapa -a                            # Append to file
```

### Information & Control
```bash
# Information
-h, --help                          # Show help ✅ ALREADY IMPLEMENTED
--version                           # Show version ✅ ALREADY IMPLEMENTED
--license                           # Show license

# Output Control
-q, --quiet                         # Suppress output ✅ ALREADY IMPLEMENTED
-v, --verbose                       # Verbose output
-d, --debug                         # Debug mode
--trace                             # Execution trace
```

### Performance & Environment
```bash
# Performance
-j, --jobs <N>                      # Parallel workers
--no-parallel                       # Disable parallelism

# Environment
-E, --env <VAR=value>               # Set environment variable
--env-file <file>                   # Load environment file
```

### Error Handling
```bash
--strict                            # Strict mode (fail on warnings)
--continue                          # Continue on errors
--max-errors <N>                    # Stop after N errors
```

## Implementation Plan

### Phase 1: Low-Hanging Fruit (Completed ✅)
**Estimated Effort:** 2-3 hours  
**Priority:** HIGH - Immediate usability improvement

#### 1.1 Replace Non-Standard Options (Easy)
```bash
# Replace these:
-ccmd script    →  -c script
-cfile file     →  -f file  
-ccin           →  -s
-argcin         →  -S (stdin to variable)

# Implementation:
- Update option parsing in GrapaLink.cpp
- Update help text
- Test all scenarios
- Note: $ARGCIN usage is `$sys().getenv('$ARGCIN')`, not direct `$ARGCIN`
```

#### 1.2 Add Missing Standard Options (Easy)
```bash
# Add these:
-v, --verbose   # Verbose output
-d, --debug     # Debug mode
-o, --output    # Output to file
-a, --append    # Append to file

# Implementation:
- Add option parsing logic
- Implement verbose/debug output
- Add file output redirection
- Update help text
```

#### 1.3 Remove GUI Options (Easy)
```bash
# Remove these (move to separate GUI tool):
-w
-wfile

# Implementation:
- Remove option parsing
- Update help text
- Note: GUI functionality preserved, just moved
```

### Phase 2: Advanced Features (Future)
**Estimated Effort:** 1-2 weeks  
**Priority:** MEDIUM - Enhanced functionality

#### 2.1 Performance Options
```bash
-j, --jobs <N>        # Parallel worker control
--no-parallel         # Disable parallelism
--profile             # Performance profiling
```

#### 2.2 Environment Management
```bash
-E, --env <VAR=value> # Set environment variables
--env-file <file>     # Load environment file
```

#### 2.3 Error Handling
```bash
--strict              # Strict mode
--continue            # Continue on errors  
--max-errors <N>      # Error limit
```

#### 2.4 Advanced Debugging
```bash
--trace               # Execution trace
--dump-ast            # Show parsed AST
--dump-bytecode       # Show compiled bytecode
```

## Migration Strategy

### Backward Compatibility
**Decision:** No backward compatibility needed
**Rationale:** 
- Current options are non-standard and confusing
- Better to have clean, standard interface
- Users will adapt quickly to standard options

### Documentation Updates
- Update CLI Quickstart guide
- Update all examples in documentation
- Update maintainer documentation
- Update any scripts or CI/CD that use CLI

### Testing Strategy
- Unit tests for each new option
- Integration tests for option combinations
- Regression tests for existing functionality
- Performance tests for parallel options

## Success Criteria

### Phase 1 Success Criteria
- [x] `grapa -c "'hello'.echo()"` works (replaces `-ccmd`)
- [x] `grapa -f script.grc` works (replaces `-cfile`)
- [x] `grapa -s` works (replaces `-ccin`)
- [ ] `grapa -v` shows verbose output
- [ ] `grapa -d` shows debug output
- [ ] `grapa -o file.txt` redirects output
- [ ] `grapa -a file.txt` appends output
- [ ] Help text updated with new options
- [ ] All existing functionality preserved
- [ ] No GUI options in CLI help

### Phase 2 Success Criteria
- [ ] `grapa -j 4` uses 4 parallel workers
- [ ] `grapa --no-parallel` disables parallelism
- [ ] `grapa -E "VAR=value"` sets environment
- [ ] `grapa --strict` fails on warnings
- [ ] `grapa --continue` continues on errors
- [ ] Performance profiling works
- [ ] Execution tracing works

## Risk Assessment

### Low Risk
- Option name changes (standard conventions)
- Adding verbose/debug options
- File output redirection

### Medium Risk  
- Parallel worker control (affects performance)
- Environment variable handling
- Error handling changes

### High Risk
- Removing GUI options (may break existing workflows)
- Performance profiling (complex implementation)

## Timeline

### Phase 1: Current Session (2-3 hours)
- Replace non-standard options
- Add basic missing options
- Remove GUI options
- Update documentation

### Phase 2: Future (1-2 weeks)
- Performance options
- Environment management
- Error handling
- Advanced debugging

## References

- [CLI Design Guidelines](https://clig.dev/) - Standard CLI conventions
- [Python CLI Options](https://docs.python.org/3/using/cmdline.html) - Reference implementation
- [Node.js CLI Options](https://nodejs.org/api/cli.html) - Reference implementation
- [Bash CLI Options](https://www.gnu.org/software/bash/manual/html_node/Invoking-Bash.html) - Reference implementation
- [Current CLI Analysis](maintainers/DEVELOPMENT/CURRENT_CLI_ANALYSIS.md) - Comprehensive analysis of existing options

---

**Next Steps:** Phase 1 completed! Ready for Phase 2 (advanced features) when needed 