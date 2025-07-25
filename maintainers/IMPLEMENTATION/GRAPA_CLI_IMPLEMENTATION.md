---
tags:
  - maintainer
  - lowlevel
  - implementation
  - cli
---
# Grapa CLI Implementation

## Overview

The Grapa Command Line Interface (CLI) provides the primary user interface for executing Grapa scripts and commands. The CLI is implemented in C++ with a focus on Python-inspired simplicity and standard Unix conventions.

**Current Status**: Phase 1 implementation complete with standard options and smart input detection.

## Architecture

### Core Components

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Command Line  │───▶│  Option Parser  │───▶│  Execution      │
│     Arguments   │    │   (GrapaLink)   │    │   Engine        │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                │                        │
                                ▼                        ▼
                       ┌─────────────────┐    ┌─────────────────┐
                       │  Console I/O    │    │   Script State  │
                       │ (GrapaConsole)  │    │   Management    │
                       └─────────────────┘    └─────────────────┘
```

### Key Files
- **`source/grapa/GrapaLink.cpp`** - Main CLI entry point and option parsing
- **`source/grapa/GrapaConsole.cpp`** - Console I/O and script execution
- **`source/grapa/GrapaSystem.cpp`** - System integration and environment

## Currently Working Functionality

### Information Options

#### Help Display
```bash
grapa -h
grapa --help
```
**Implementation**: Sets `showHelp = true`, `needExit = true`
**Status**: ✅ **VERIFIED WORKING** - Displays version and list of options/execution methods
**Tested**: macOS - Shows "Version: 0.0.41" and available options

#### Version Display
```bash
grapa -v
grapa --version
```
**Implementation**: Sets `showVersion = true`, `needExit = true`
**Status**: ✅ **VERIFIED WORKING** - Displays "Version: 0.0.41"
**Tested**: macOS - Outputs version information correctly

#### Quiet Mode
```bash
grapa -q
grapa --quiet
```
**Implementation**: Sets `suppressHeader = true`
**Status**: ✅ **VERIFIED WORKING** - Suppresses header with all execution modes
**Tested**: macOS - Works with pipe input, -c, -f, and direct file execution
**Examples**: 
- `echo "'hello'.echo()" | ./grapa -q` outputs "hello" without header
- `./grapa -q -c "'hello world'.echo();"` outputs "hello world" without header
- `./grapa -q -f test_script.grc` outputs script results without header
- `./grapa -q test_script.grc` outputs script results without header

### Execution Modes

#### Interactive Mode
```bash
grapa
grapa -i
grapa --interactive
```
**Implementation**: Sets `interactiveMode = true`, `showConsole = true`
**Status**: ✅ Fully functional

#### Direct File Execution
```bash
grapa script.grc
grapa script.grz
```
**Implementation**: Auto-detects `.grc` and `.grz` files
**Status**: ✅ **VERIFIED WORKING** - Executes script files correctly
**Tested**: macOS - Requires proper Grapa syntax (semicolons after statements)
**Example**: `./grapa test_script.grc` executes file contents

#### Direct Command Execution
```bash
grapa "'hello'.echo()"
grapa '"hello world"'
```
**Implementation**: Detects quoted strings and method calls
**Status**: ✅ **VERIFIED WORKING** - Executes quoted commands correctly
**Tested**: macOS - `./grapa -c "'hello world'.echo()"` outputs "hello world"

### Smart Input Detection

#### Pipe Input Auto-Detection
```bash
echo "data" | grapa
cat file.txt | grapa
```
**Implementation**: Detects pipe input and reads from stdin
**Status**: ✅ **VERIFIED WORKING** - Automatically reads from stdin when piped
**Tested**: macOS - `echo "'hello'.echo()" | ./grapa` outputs "hello"

#### Method Call Detection
```bash
grapa 'hello'.echo()
grapa 'test'.len()
grapa 'data'.get(0)
```
**Implementation**: Detects common method patterns
**Status**: ⚠️ **NEEDS TESTING** - Not yet verified

## Known Broken Features

### Script Options (-s, -S)
```bash
grapa -s
grapa -S
```
**Status**: ❌ **BROKEN** - Hangs and requires process termination
**Issue**: Both options cause the CLI to hang and not respond
**Implementation**: Both read from stdin in infinite loops without proper termination
**Workaround**: Use `-f` option or direct file execution instead
**Priority**: Medium - These options appear to be legacy/unused

## Partially Implemented Features

### Debug Mode (-d)
```bash
grapa -d
```
**Status**: ⚠️ **PARTIAL** - Sets debug mode but drops to interactive
**Implementation**: Sets `gSystem->mDebugMode = true` but no debug instrumentation exists
**Behavior**: Drops into interactive mode (same as `-i`)
**Priority**: Low - Debug instrumentation not yet implemented

### Output File (-o)
```bash
grapa -o filename
```
**Status**: ⚠️ **PARTIAL** - Sets output file but drops to interactive
**Implementation**: Sets `gSystem->mOutputFile` but output redirection not fully implemented
**Behavior**: Drops into interactive mode
**Priority**: Low - Output redirection not yet implemented

### Append Mode (-a)
```bash
grapa -a
```
**Status**: ⚠️ **PARTIAL** - Sets append mode but drops to interactive
**Implementation**: Sets `gSystem->mAppendMode = true` but append functionality not implemented
**Behavior**: Drops into interactive mode
**Priority**: Low - Append functionality not yet implemented

## Implementation Details

### Option Parsing (GrapaLink.cpp)

#### Information Options
```cpp
if ((e->mValue.Cmp("-h") == 0) || (e->mValue.Cmp("--help") == 0))
{
    showHelp = true;
    needExit = true;
}
else if ((e->mValue.Cmp("-v") == 0) || (e->mValue.Cmp("--version") == 0))
{
    showVersion = true;
    needExit = true;
}
else if ((e->mValue.Cmp("-q") == 0) || (e->mValue.Cmp("--quiet") == 0))
{
    suppressHeader = true;
}
```

#### Interactive Mode
```cpp
else if ((e->mValue.Cmp("-i") == 0) || (e->mValue.Cmp("--interactive") == 0))
{
    interactiveMode = true;
    showConsole = true;
}
```

### Smart Detection Features

#### Direct Execution Detection
```cpp
if (e->mValue.mLength >= 2 && 
    ((e->mValue.mBytes[0] == '"' && e->mValue.mBytes[e->mValue.mLength - 1] == '"') ||
     (e->mValue.mBytes[0] == '\'' && e->mValue.mBytes[e->mValue.mLength - 1] == '\'')))
{
    GrapaCHAR cmd;
    cmd.FROM((char*)&e->mValue.mBytes[1], e->mValue.mLength - 2);
    runStr.FROM(cmd);
    needExit = true;
}
```

#### Method Call Detection
```cpp
else if (e->mValue.mLength > 0 && 
    (strstr((char*)e->mValue.mBytes, ".echo()") != NULL ||
     strstr((char*)e->mValue.mBytes, ".print()") != NULL ||
     strstr((char*)e->mValue.mBytes, ".len()") != NULL ||
     strstr((char*)e->mValue.mBytes, ".get(") != NULL))
{
    runStr.FROM(e->mValue);
    needExit = true;
}
```

#### File Detection
```cpp
else if (e->mValue.mLength > 4 && 
    (strstr((char*)e->mValue.mBytes, ".grc") != NULL ||
     strstr((char*)e->mValue.mBytes, ".grz") != NULL))
{
    inStr.FROM(e->mValue);
    needExit = true;
}
```

#### Pipe Input Detection
```cpp
if (isPipeInput && runStr.mLength == 0 && !interactiveMode && !showWidget)
{
    char c;
    runStr.SetLength(0);
    while (std::cin >> c && !std::cin.eof())
        runStr.Append((char)c);
    if (runStr.mLength > 0)
    {
        needExit = true;
    }
}
```

### Console I/O (GrapaConsole.cpp)

#### Script Execution
```cpp
GrapaCHAR GrapaConsoleSend::SendSync(GrapaCHAR& pIn, GrapaRuleEvent* pRule, u64 pRuleId, GrapaCHAR pProfile)
{
    GrapaCHAR s;
    if (pIn.mLength)
    {
        GrapaScriptExec tokenExec;
        tokenExec.vScriptState = &mScriptState;
        tokenExec.vScriptState->WaitCritical();
        
        GrapaScriptExec* saveTokenExec = tokenExec.vScriptState->vScriptExec;
        tokenExec.vScriptState->vScriptExec = &tokenExec;
        
        GrapaRuleEvent* result = tokenExec.Exec(tokenExec.vScriptState->GetNameSpace(), pRule, pRuleId, pProfile, pIn);
        
        if (result)
        {
            GrapaSystemSend send;
            send.isActive = false;
            GrapaRuleEvent* echo = result;
            while (echo && echo->mValue.mToken == GrapaTokenType::PTR)
                echo = echo->vRulePointer;
            
            if (echo && echo->mValue.mToken != GrapaTokenType::ERR)
            {
                if (echo->vQueue) tokenExec.EchoList(&send, echo, false, false, false);
                else tokenExec.EchoValue(&send, echo, false, false, false);
            }
            send.GetStr(s);
            if (s.mLength) std::cout << (char*)s.mBytes;
            result->CLEAR();
            delete result;
        }
        
        tokenExec.vScriptState->vScriptExec = saveTokenExec;
        tokenExec.vScriptState->LeaveCritical();
    }
    return s;
}
```

## Environment Integration

### System Variables
- **`$ARGCIN`** - Stdin content (when using `-argcin` option)
- **`$sys().getenv()`** - Environment variable access
- **`$sys().putenv()`** - Environment variable setting

### File System Integration
- **Unified path system** - Single API for files, databases, networks
- **Database integration** - Native database operations
- **Network access** - HTTP, FTP, and custom protocols

## Error Handling

### Current Error Handling
- **Syntax errors** - Displayed with line numbers
- **Runtime errors** - Displayed with context
- **System errors** - Displayed with error codes

### Error Output
- **Interactive mode** - Errors displayed immediately
- **Script mode** - Errors displayed and script exits
- **Pipe mode** - Errors displayed to stderr

## Performance Characteristics

### Execution Speed
- **Direct execution** - Immediate execution of simple commands
- **File execution** - Fast loading and execution of compiled scripts
- **Interactive mode** - Responsive command-line interface

### Memory Usage
- **Minimal overhead** - Lightweight console implementation
- **Efficient parsing** - Stream-based option parsing
- **Smart detection** - Minimal memory for input detection

## Platform Support

### Operating Systems
- **Linux** - Full support with native console
- **macOS** - Full support with native console
- **Windows** - Full support with console emulation

### Console Types
- **Terminal** - Standard terminal/console
- **Pipe** - Unix-style pipe input/output
- **Redirect** - File input/output redirection

## Integration Points

### System Functions
- **`shell()`** - Execute shell commands
- **`eval()`** - Evaluate expressions
- **`exec()`** - Execute commands
- **`include()`** - Include files

### Database Integration
- **Direct database access** - Native database operations
- **SQL-like queries** - High-level query interface
- **Transaction support** - ACID-compliant transactions

### Network Integration
- **HTTP client** - Web requests and responses
- **FTP client** - File transfer operations
- **Custom protocols** - Extensible protocol support

## Development Guidelines

### Adding New Options
1. **Define option** in GrapaLink.cpp option parsing
2. **Implement functionality** in appropriate system component
3. **Add help text** to option descriptions
4. **Test thoroughly** across platforms

### Adding New Execution Modes
1. **Define detection logic** in smart detection section
2. **Implement execution** in GrapaConsole.cpp
3. **Add error handling** for edge cases
4. **Update documentation** with examples

### Performance Considerations
- **Lazy evaluation** - Avoid unnecessary computation
- **Memory efficiency** - Minimize memory allocations
- **Fast parsing** - Optimize option parsing
- **Responsive UI** - Maintain interactive responsiveness

## Debugging

### Common Issues
- **Option conflicts** - Check for conflicting option names
- **Input detection** - Verify smart detection logic
- **Console output** - Check output buffering
- **Error handling** - Validate error propagation

### Debug Tools
- **Verbose mode** - Enable detailed output (planned)
- **Debug mode** - Enable debug information (planned)
- **Trace mode** - Enable execution tracing (planned)

## Related Documentation

- **[Grapa CLI Comprehensive Guide](../DEVELOPMENT/GRAPA_CLI_COMPREHENSIVE.md)** - Development roadmap and standards
- **[System Functions](SYSTEM_FUNCTIONS.md)** - Low-level system APIs
- **[Execution Trees](EXECUTION_TREES.md)** - Execution model details
- **[Include System](INCLUDE_SYSTEM.md)** - Module system implementation

---

**For users**: If you're looking for how to use the CLI, see the main [CLI Quickstart](../../docs/cli_quickstart/) and [Examples](../../docs/examples/). 