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

**Current Status**: Phase 1 implementation complete with standard options and smart input detection. Phase 2 focused on enhanced debug mode, better error messages, and documentation updates.

## Phase 2 Scope (Revised)

### Focus Areas
1. **Enhanced Debug Mode**: Improve existing `-d` option with verbose output capabilities
2. **Better Error Messages**: Improve syntax error reporting with more descriptive messages
3. **Documentation Updates**: Reference existing language capabilities (performance, environment, timing)
4. **Cross-Reference Language Features**: Add examples showing built-in capabilities

### Existing Language Capabilities
Many features originally planned for CLI Phase 2 are already implemented in the Grapa language:

#### Performance and Timing
```grapa
/* Built-in performance profiling */
start_time = $TIME().utc();
/* ... operations ... */
end_time = $TIME().utc();
elapsed_ms = ((end_time - start_time) / 1000000).int();
```

**CLI Usage Examples:**
```bash
# Performance profiling from CLI
grapa -c "
start_time = \$TIME().utc();
/* ... operations ... */
end_time = \$TIME().utc();
elapsed_ms = ((end_time - start_time) / 1000000).int();
"Performance: ${elapsed_ms} ms".interpolate().echo();
"

# Parallel processing with timing
grapa -c "
start_time = \$TIME().utc();
data = (10000).range(0,1);
processed = data.map(op(x) { x * x + 1; }, 4);
end_time = \$TIME().utc();
elapsed_ms = ((end_time - start_time) / 1000000).int();
"Processed ${processed.len()} items in ${elapsed_ms} ms".interpolate().echo();
"
```

#### Environment Management
```grapa
/* Built-in environment management */
debug_mode = $sys().getenv("DEBUG_MODE");
$sys().putenv("CUSTOM_VAR", "my_value");
```

**CLI Usage Examples:**
```bash
# Environment variable access
grapa -c "\$sys().getenv('USERNAME').echo();"
grapa -c "\$sys().putenv('DEBUG_MODE', 'true');"

# Environment-based configuration
grapa -c "
env = \$sys().getenv('NODE_ENV');
if (env == 'production') {
    'Running in production mode'.echo();
} else {
    'Running in development mode'.echo();
}
"
```

#### Parallel Processing
```grapa
/* Built-in parallel processing */
large_data = (1000000).range(0,1);
squares = large_data.map(op(x) { x * x; }, 8);  /* 8 worker threads */
```

**CLI Usage Examples:**
```bash
# Parallel data processing
grapa -c "
data = (10000).range(0,1);
processed = data.map(op(x) { x * x + 1; }, 4);  /* 4 workers */
"Processed ${processed.len()} items".interpolate().echo();
"

# Concurrent operations
grapa -c "
tasks = [];
for (i in (5).range(0,1)) {
    task = op(id) {
        \$sys().sleep(100);  /* Simulate work */
        "Task ${id} completed".interpolate().echo();
    };
    tasks += task;
}
for (task in tasks) {
    task(tasks.index(task));
}
"
```

#### Output Control
```bash
# Standard shell piping already works
grapa -c "'Hello World'.echo()" > output.txt
grapa -c "'Hello World'.echo()" | grep "Hello"
```

**CLI Usage Examples:**
```bash
# Output formatting and control
grapa -c "
data = ['apple', 'banana', 'cherry'];
for (item in data) {
    "Item: ${item.upper()}".interpolate().echo();
}
"

# Output redirection with shell
grapa -c "'Hello World'.echo()" > output.txt
grapa -c "'Hello World'.echo()" | grep "Hello"
grapa -c "'Hello World'.echo()" >> log.txt
```

#### Dynamic Code Execution
```grapa
/* Built-in dynamic code execution */
template = 'name + "! You are " + age.str() + " years old."';
result = op()(template)();
```

**CLI Usage Examples:**
```bash
# String templates and dynamic construction
grapa -c "
greeting = op('name'=0, 'time'=0){
    'Good ' + time + ', ' + name + '!'
};
greeting('Alice', 'morning').echo();
"

# Dynamic code execution
grapa -c "
template = 'name + \"! You are \" + age.str() + \" years old.\"';
name = 'Alice';
age = 25;
result = op()(template)();
result.echo();
"
```

#### Error Handling
```grapa
/* Built-in error handling */
iferr {
    result = 10 / 0;
    'This won\'t print'.echo();
} {
    "Error occurred: ${$sys.error}".interpolate().echo();
}
```

**CLI Usage Examples:**
```bash
# Error handling with iferr
grapa -c "
iferr {
    result = 10 / 0;
    'This won\'t print'.echo();
} {
    "Error occurred: ${$sys.error}".interpolate().echo();
}
"
```

#### Database Operations
```grapa
/* Built-in database operations */
db = $file().table('ROW');
db.mkfield('name', 'STR');
db.set('user1', 'Alice', 'name');
```

**CLI Usage Examples:**
```bash
# Database operations
grapa -c "
db = \$file().table('ROW');
db.mkfield('name', 'STR');
db.mkfield('age', 'INT');
db.set('user1', 'Alice', 'name');
db.set('user1', 25, 'age');
result = db.get('user1', 'name');
"User name: ${result}".interpolate().echo();
"
```

#### File Operations
```grapa
/* Built-in file operations */
files = $file().ls('.');
for (file in files) {
    if (file.type == 'file') {
        "File: ${file.name}".interpolate().echo();
    }
}
```

**CLI Usage Examples:**
```bash
# File operations
grapa -c "
files = \$file().ls('.');
for (file in files) {
    if (file.type == 'file') {
        "File: ${file.name}".interpolate().echo();
    } else {
        "Directory: ${file.name}".interpolate().echo();
    }
}
"
```

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

#### Verbose Mode
```bash
grapa --verbose
```
**Implementation**: Sets `showVersion = true` to display version header
**Status**: ✅ **VERIFIED WORKING** - Shows version header (default: hidden)
**Tested**: macOS - `./grapa --verbose -c "'hello'.echo()"` shows version header
**Behavior**: Displays "Version: 0.0.41" before command output

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

#### Standard Stdin Execution
```bash
echo "data" | grapa -
cat file.txt | grapa -
```
**Implementation**: Standard `-` option for reading from stdin
**Status**: ✅ **VERIFIED WORKING** - Standard Python/Node.js behavior
**Tested**: macOS - `echo "'hello'.echo()" | ./grapa -` outputs "hello"
**Error Handling**: Shows clear error when used without pipe input

### Smart Input Detection

#### Pipe Input Auto-Detection
```bash
echo "data" | grapa
cat file.txt | grapa
```
**Implementation**: Detects pipe input and reads from stdin
**Status**: ✅ **VERIFIED WORKING** - Automatically reads from stdin when piped
**Tested**: macOS - `echo "'hello'.echo()" | ./grapa` outputs "hello"

#### Direct File Execution (No -f flag)
```bash
grapa script.grc
grapa script.grz
```
**Implementation**: Automatically detects .grc/.grz files as scripts
**Status**: ✅ **VERIFIED WORKING** - Executes script files without -f flag
**Behavior**: Looks for first positional argument ending in .grc/.grz and executes it
**Note**: Grapa does not support direct quoted command execution like `grapa 'hello'.echo()` - use `grapa -c "'hello'.echo()"` instead

## Error Handling

### Unknown Options
```bash
grapa -s
grapa --invalid-option
```
**Status**: ✅ **VERIFIED WORKING** - Shows clear error message and exits cleanly
**Implementation**: Detects unknown flags starting with `-` and shows helpful error
**Behavior**: Prints error to stderr and exits with error code
**Example**: `./grapa -s` outputs "Error: Unknown option '-s'" and exits

### Invalid Stdin Usage
```bash
grapa -
```
**Status**: ✅ **VERIFIED WORKING** - Shows helpful error when used without pipe input
**Implementation**: Checks for pipe input before reading from stdin
**Behavior**: Shows clear usage example and exits cleanly
**Example**: `./grapa -` outputs "Error: - option requires pipe input (e.g., echo 'command' | grapa -)"

## Removed Features (Non-Standard)

### Script Options (-s, -S)
```bash
grapa -s
grapa -S
```
**Status**: ❌ **REMOVED** - Replaced with standard `-` option
**Reason**: Non-standard options that don't align with Python/Node.js CLI conventions
**Replacement**: Use `grapa -` for stdin reading (standard behavior)

## Partially Implemented Features

### Debug Mode (-d)
```bash
grapa -d
```
**Status**: ✅ **VERIFIED WORKING** - Provides enhanced debug output for CLI operations
**Implementation**: Sets `gSystem->mDebugMode = true` and calls `gSystem->DebugPrint()`
**Behavior**: Shows debug information for command execution, file loading, and pipe detection
**Examples**:
- `./grapa -d -c "'hello'.echo()"` shows "[DEBUG] Debug mode enabled" and "[DEBUG] Executing command: ..."
- `./grapa -d -f script.grc` shows debug info for file execution
- `./grapa -d -` shows debug info for stdin execution
**Output**: Debug messages prefixed with "[DEBUG]" sent to stderr

### Enhanced Debug System Architecture

#### **System-Level Debug** (`GrapaDebug`)
- **Scope**: Pre-session and session-agnostic operations
- **Output Method**: Direct output (`fprintf(stderr)`)
- **Use Cases**: CLI initialization, system startup, global operations
- **Environment Variables**: `GRAPA_DEBUG_MODE`, `GRAPA_DEBUG_LEVEL`, `GRAPA_DEBUG_COMPONENTS`

#### **Session-Level Debug** (`GrapaScriptExecStateDebug`)
- **Scope**: Session-specific operations within Grapa script execution
- **Output Method**: Response system (`pNameSpace->GetResponse()->Send()`)
- **Use Cases**: Script execution, compiler operations, database operations
- **Environment Variables**: `GRAPA_SESSION_DEBUG`, `GRAPA_SESSION_DEBUG_LEVEL`, `GRAPA_SESSION_DEBUG_COMPONENTS`

#### **Component-Specific Debugging**
- **Filtering**: Debug output can be filtered by specific components
- **Components**: `lexer`, `parser`, `compiler`, `database`, `grep`, `vector`, etc.
- **Levels**: Different verbosity levels per component (e.g., "lexer:2,parser:1")
- **Syntax**: Comma-separated list with optional level specification

**Examples**:
```bash
# Enable specific components
GRAPA_SESSION_DEBUG_COMPONENTS=lexer,parser ./grapa script.grc

# Enable component-specific levels
GRAPA_SESSION_DEBUG_COMPONENTS=lexer:2,parser:1 ./grapa script.grc

# Enable all components
GRAPA_SESSION_DEBUG_COMPONENTS=* ./grapa script.grc

# Enable only one component at high level
GRAPA_SESSION_DEBUG_COMPONENTS=lexer:3,*:0 ./grapa script.grc
```

#### **Session Isolation**
- **Multi-Processing Support**: Each session has isolated debug output
- **Session IDs**: Unique identifiers prevent output interference
- **Parallel Execution**: Multiple sessions can run with different debug configurations
- **Context Preservation**: Debug context is maintained per session

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
- **`$ARGCIN`** - Stdin content (when using `-S` option)
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