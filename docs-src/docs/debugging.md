---
tags:
  - user
  - highlevel
  - debugging
  - development
---

# Debugging in Grapa

Grapa provides a comprehensive debugging system that supports both command-line and script-based debug control. This system allows you to enable debug output, control verbosity levels, and target specific system components for detailed troubleshooting.

## Overview

Grapa's debug system operates at two levels:
- **System-level debugging**: Global debug settings that affect all sessions
- **Session-level debugging**: Session-specific debug settings that can override global settings

### Key Features
- **CLI Debug Control**: Enable debug mode with `-d` flag
- **Script Debug Control**: Dynamically enable/disable debug during script execution
- **Component-Specific Debugging**: Target specific system components (infrastructure ready, components planned)
- **Debug Levels**: Control verbosity from 0 (disabled) to 9 (maximum detail)
- **Session Isolation**: Debug settings are isolated between parallel sessions
- **Environment Variable Integration**: Configure debug settings via environment variables

### Current Status
The debug system infrastructure is fully implemented and ready for use. While no system components are currently instrumented with debug output, the framework is in place for future component integration. Users can set debug flags and configure the system, and debug output will appear once components are instrumented.

## CLI Debug Options

### Basic Debug Mode
```bash
# Enable debug mode
grapa -d script.grc

# Long form
grapa --debug script.grc
```

### Debug with Other Options
```bash
# Debug with command execution
grapa -d -c "'hello world'.echo();"

# Debug with quiet mode (suppresses header)
grapa -d -q script.grc

# Debug with verbose mode (shows version header)
grapa -d --verbose script.grc
```

## Script-Based Debug Output

### The `.debug()` Method

Grapa provides a `.debug()` method for script-based debug output that integrates with the debug system. This method allows you to output debug messages from your scripts with component-specific level control.

#### Basic Usage

```grapa
/* Basic debug output */
"Processing user data".debug(0);

/* Debug with custom component */
"Database connection established".debug(1, "db");

/* Debug with custom level and component */
"API response received".debug(2, "api");
```

#### Syntax

```grapa
value.debug(level, component?)
```

- **`level`** (required): Debug level (0-9). Only shows if component's debug level >= this level
- **`component`** (optional): Component name (defaults to "debug")

#### Debug Level Behavior

The `.debug()` method respects component-specific debug levels:

```bash
# Enable debug mode
grapa -d script.grc

# Enable specific component levels
grapa -d -d "debug:1,db:2,api:3" script.grc
```

**Examples:**

```grapa
/* These will show with -d -d "debug:1" */
"Basic info".debug(0);      /* Always shows with debug enabled */
"Detailed info".debug(1);   /* Shows if debug component level >= 1 */

/* These will show with -d -d "db:2" */
"DB connection".debug(1, "db");  /* Shows if db component level >= 1 */
"DB query".debug(2, "db");       /* Shows if db component level >= 2 */

/* These will show with -d -d "api:3" */
"API call".debug(1, "api");      /* Shows if api component level >= 1 */
"API response".debug(3, "api");  /* Shows if api component level >= 3 */
```

#### Output Format

Debug messages appear in the format:
```
[DEBUG-{session_id}-{component}] message
```

**Examples:**
```
[DEBUG-1-debug] Processing user data
[DEBUG-1-db] Database connection established
[DEBUG-1-api] API response received
```

#### Integration with Debug System

The `.debug()` method integrates seamlessly with Grapa's debug system:

- **Requires `-d` flag**: No output without debug mode enabled
- **Respects component levels**: Only shows if component's debug level >= message level
- **Session isolation**: Messages are tied to the current script session
- **Component filtering**: Can be filtered using `-d -d "component:level"`

#### Best Practices

```grapa
/* ✅ Good - Use meaningful levels */
"Starting process".debug(0);
"Processing step 1".debug(1);
"Detailed calculation".debug(2);

/* ✅ Good - Use descriptive components */
"User login".debug(1, "auth");
"Database query".debug(2, "db");
"API request".debug(1, "api");

/* ✅ Good - Use for troubleshooting */
if (error_condition) {
    "Error occurred: " + error_details.str().debug(1, "error");
}

/* ❌ Avoid - Too verbose for production */
"Every step".debug(0);  /* Too noisy */
```

#### Comparison with `.echo()`

| Method | Purpose | Debug Integration | Output Location |
|--------|---------|-------------------|-----------------|
| `.echo()` | General output | No | Standard output |
| `.debug()` | Debug output | Yes | Debug stream (stderr) |

```grapa
/* General output */
"User logged in successfully".echo();

/* Debug output */
"Login attempt from IP 192.168.1.1".debug(1, "auth");
```

#### Using `.interpolate()` for Clean Output
For clean, readable output in both general and debug contexts, use `.interpolate()` instead of multiple `.echo()` calls:

```grapa
/* Avoid: Multiple .echo() calls */
"Processing item ".echo();
item_count.echo();
" of ".echo();
total_items.echo();
"\n".echo();

/* Prefer: Single .interpolate() call */
"Processing item ${item_count} of ${total_items}\n".interpolate().echo();

/* Debug with interpolation */
"Debug: Processing ${item_count} items, status: ${status}\n".interpolate().debug(1, "process");
```

## Script-Based Debug Control

### System-Level Debug Control

Enable or disable system-level debug settings that affect all sessions:

> **Note:** System-level debug settings only affect new sessions. Existing sessions that are already running will not be affected by changes to system-level debug settings.

```grapa
// Enable system-level debug
$sys().putenv("GRAPA_DEBUG_MODE", "1");
$sys().putenv("GRAPA_DEBUG_LEVEL", "3");

// Disable system-level debug
$sys().putenv("GRAPA_DEBUG_MODE", "0");
```

### Session-Level Debug Control

Enable session-specific debug settings that override system settings for the current session:

> **Note:** Session debug settings must be configured before the session starts. Setting environment variables during script execution will not affect the current session.

```grapa
// Enable session-specific debug override (set at script start)
$sys().putenv("GRAPA_SESSION_DEBUG_MODE", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_LEVEL", "2");

// Disable session-specific debug
$sys().putenv("GRAPA_SESSION_DEBUG_MODE", "0");
```

**Alternative: Use command-line environment variables**
```bash
GRAPA_SESSION_DEBUG_MODE=1 GRAPA_SESSION_DEBUG_COMPONENTS="debug:1" ./grapa -d script.grc
```

### Component-Specific Debugging

Target specific system components for focused debugging:

```grapa
// Enable debug for specific components
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "database,grep,vector");

// Enable all components
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "*");

// Session-specific component targeting
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "filesystem,network");
```

### Component-Specific Debug Levels

Set different debug levels for different components:

```grapa
// Set specific levels per component
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "grep:3,database:1,vector:0");

// Set default level for all components except specified ones
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "grep:3,*:1");

// Enable only one component at high level, disable others
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "database:3,*:0");
```

## Debug Environment Variables

### System-Level Variables

| Variable | Description | Values | Example |
|----------|-------------|--------|---------|
| `GRAPA_DEBUG_MODE` | Enable/disable system debug | `"0"` or `"1"` | `"1"` |
| `GRAPA_DEBUG_LEVEL` | System debug verbosity | `0-9` | `"3"` |
| `GRAPA_DEBUG_COMPONENTS` | System debug components | Component list | `"database,grep"` |

### Session-Level Variables

| Variable | Description | Values | Example |
|----------|-------------|--------|---------|
| `GRAPA_SESSION_DEBUG_MODE` | Enable session debug override | `"0"` or `"1"` | `"1"` |
| `GRAPA_SESSION_DEBUG_LEVEL` | Session debug verbosity | `0-9` | `"2"` |
| `GRAPA_SESSION_DEBUG_COMPONENTS` | Session debug components | Component list | `"vector,filesystem"` |
| `GRAPA_SESSION_ID` | Current session ID (read-only) | Auto-generated | `"123"` |

## Currently Instrumented Components

The following debug components are currently instrumented and available for use:

### **Lexer Component** (`lexer`)
- **Scope**: Tokenization process (bytes to tokens)
- **Debug Levels**: 1-5 (basic to detailed)
- **Example Output**:
  ```
  [DEBUG-SESSION-1-lexer] LEX: Created token type=4 value='x' quote=
  [DEBUG-SESSION-1-lexer] LEX: Created token type=10 value=' ' quote=
  [DEBUG-SESSION-1-lexer] LEX: Created token type=10 value='+' quote=
  ```

### **Parser Component** (`parser`)
- **Scope**: Grammar parsing (tokens to execution trees)
- **Debug Levels**: 1-5 (basic to detailed)
- **Example Output**:
  ```
  [DEBUG-SESSION-1-parser] PARSE: Rule matching attempt - rule='expression'
  [DEBUG-SESSION-1-parser] PARSE: Full token stream: [4:'x'] [10:'+'] [8:'5']
  ```

### **Executor Component** (`executor`)
- **Scope**: Plan execution (execution trees to results)
- **Debug Levels**: 1-5 (basic to detailed)
- **Example Output**:
  ```
  [DEBUG-SESSION-1-executor] EXEC: Starting execution - operation type=21
  [DEBUG-SESSION-1-executor] EXEC: Processing operation type=21 (CODE=22, OP=21)
  [DEBUG-SESSION-1-executor] EXEC: Library execution - hasLibrary=true
  [DEBUG-SESSION-1-executor] EXEC: Execution completed - elapsed time=15.00 microseconds, hasResult=true
  ```

### **Combined Compiler Component** (`compiler`)
- **Scope**: Both lexer and parser (shorthand)
- **Debug Levels**: 1-5 (basic to detailed)
- **Usage**: `GRAPA_SESSION_DEBUG_COMPONENTS=compiler`
- **Equivalent to**: `GRAPA_SESSION_DEBUG_COMPONENTS=lexer,parser`

### **Runtime Component** (`runtime`)
- **Scope**: Complete execution pipeline (compiler + executor)
- **Debug Levels**: 1-5 (basic to detailed)
- **Usage**: `GRAPA_SESSION_DEBUG_COMPONENTS=runtime`
- **Equivalent to**: `GRAPA_SESSION_DEBUG_COMPONENTS=compiler,executor`

### Planned Debug Components
The following components are planned for debug instrumentation:

#### High Priority Components
- **`database`**: Database operations, queries, and storage
- **`grep`**: Text search and pattern matching
- **`network`**: Network operations and connections

#### Medium Priority Components
- **`vector`**: Vector operations and calculations
- **`filesystem`**: File system operations
- **`threading`**: Thread management and synchronization
- **`memory`**: Memory allocation and management

#### Low Priority Components
- **`widget`**: GUI widget operations
- **`time`**: Time-related operations
- **`prime`**: Prime number operations
- **`crypto`**: Cryptographic operations
- **`object`**: Object management
- **`system`**: System-level operations
- **`encoding`**: Data encoding/decoding

### Adding Debug to Components
To add debug output to a component, developers can use the existing debug infrastructure:

```cpp
// In component code
if (gSystem->mDebug.ShouldDebug("component_name", 1)) {
    gSystem->mDebug.DebugPrint("component_name", "Debug message", 1);
}

// For session-level debug
if (vScriptExec->vScriptState->mDebug.ShouldDebug("component_name", 1)) {
    vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "component_name", "Debug message", 1);
}
```

## Debug Output Formats and Stream Separation

### Stream Separation
Grapa's debug system properly separates normal program output from debug output:

- **Normal Program Output**: Goes to stdout (standard output)
- **Debug Output**: Goes to stderr (standard error)

This separation allows for proper stream redirection and filtering.

### System-Level Debug Output
System-level debug output appears on stderr with the format:
```
[DEBUG] message
[DEBUG-component] message
```

### Session-Level Debug Output
Session-level debug output appears on stderr with the format:
```
[DEBUG-SESSION-{session_id}] message
[DEBUG-SESSION-{session_id}-{component}] message
```

### Stream Redirection Examples
```bash
# Redirect normal output to file, debug output to separate file
grapa -d script.grc > normal_output.txt 2> debug_output.txt

# Redirect normal output to file, debug output to terminal
grapa -d script.grc > normal_output.txt

# Redirect normal output to terminal, debug output to file
grapa -d script.grc 2> debug_output.txt

# Suppress debug output entirely
grapa -d script.grc 2> /dev/null

# Suppress normal output, show only debug
grapa -d script.grc > /dev/null
```

## Practical Examples

### Example 1: Basic Debug Control
```grapa
// Enable system-level debug
$sys().putenv("GRAPA_DEBUG_MODE", "1");
$sys().putenv("GRAPA_DEBUG_LEVEL", "3");

// When components are instrumented, they will show debug output
// Currently, no components are instrumented, so no debug output will appear

// Disable debug
$sys().putenv("GRAPA_DEBUG_MODE", "0");
```

### Example 2: Compiler Debug Components
```grapa
// Enable debug for lexer only (tokenization)
$sys().putenv("GRAPA_SESSION_DEBUG", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_LEVEL", "2");
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "lexer");

// This will show token creation during script parsing
op()("x + y")();  // Shows: LEX: Created token type=4 value='x' quote=`

// Enable debug for parser only (grammar parsing)
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "parser");

// This will show rule matching and error context
op()("invalid syntax here")();  // Shows parser error details

// Enable debug for both lexer and parser
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "compiler");
// Equivalent to: $sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "lexer,parser");
```

### Example 3: Syntax Error Debugging
Debug output is particularly useful for identifying syntax issues in Grapa scripts:

#### Scenario 1: CLI Environment Debug (for .grc files with syntax errors)
```bash
# Set debug flags via environment variables before running the script
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_LEVEL=2 GRAPA_SESSION_DEBUG_COMPONENTS=lexer ./grapa script_with_errors.grc
```

#### Scenario 2: op() with bad syntax (where .grc has good syntax)
```grapa
// Set debug flags within the .grc script, then use op() to execute problematic code
$sys().putenv("GRAPA_SESSION_DEBUG", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "lexer");

// Test different syntax variations dynamically
op()("x = 5 + 3")();  // Test expression
op()("message = 'test'")();  // Test string syntax (shows quote=')
op()("echo('This should use .echo() method')")();  // Test incorrect echo syntax
```

#### Debug Output Analysis
The lexer debug output reveals:

**Token Types:**
- **Type 4**: Identifiers (`x`, `message`, `echo`)
- **Type 5**: String literals (with quote character shown)
- **Type 8**: Numbers
- **Type 10**: Operators and punctuation (`=`, `+`, `(`, `)`, `;`, `'`, `"`)
- **Type 11**: Newlines
- **Type 18**: System identifiers (`sys`)

**Quote Character Analysis:**
- **`quote=`**: No quote character (identifiers, operators)
- **`quote='`**: Single quotes detected (syntax issue!)
- **`quote="`**: Double quotes (correct)

**Common Syntax Issues Identified:**
1. **Missing semicolons**: Token stream ends abruptly
2. **Single quotes**: Shows `quote='` instead of `quote="`
3. **Incomplete expressions**: Tokens stop mid-expression
4. **Incorrect function calls**: Shows `echo` as identifier instead of method

**Example Debug Output:**
```
[DEBUG-SESSION-1-lexer] LEX: Created token type=4 value='x' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value=' ' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value='=' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value=' ' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=8 value='5' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value=' ' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value='+' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value=' ' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=8 value='3' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=11 value='' quote=
```

### Example 4: Complete Execution Pipeline
Debug the entire execution pipeline from text to results:

```bash
# Enable runtime debug (compiler + executor)
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_LEVEL=2 GRAPA_SESSION_DEBUG_COMPONENTS=runtime ./grapa script.grc

# Or enable specific components
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_LEVEL=2 GRAPA_SESSION_DEBUG_COMPONENTS=lexer,parser,executor ./grapa script.grc
```

**Complete Pipeline Output**:
```
[DEBUG-SESSION-1-lexer] LEX: Created token type=4 value='x' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value=' ' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value='+' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=10 value=' ' quote=
[DEBUG-SESSION-1-lexer] LEX: Created token type=8 value='5' quote=
[DEBUG-SESSION-1-parser] PARSE: Full token stream: [4:'x'] [10:'+'] [8:'5']
[DEBUG-SESSION-1-executor] EXEC: Starting execution - operation type=21
[DEBUG-SESSION-1-executor] EXEC: Processing operation type=21 (CODE=22, OP=21)
[DEBUG-SESSION-1-executor] EXEC: Library execution - hasLibrary=true
[DEBUG-SESSION-1-executor] EXEC: Execution completed - elapsed time=15.00 microseconds, hasResult=true
```

### Example 5: Component-Specific Debug (Future)
```grapa
// Enable debug for specific components (when instrumented)
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "database,grep,vector");

// When these components are instrumented, they will show debug output
db = {}.table("ROW");  // database operations
"hello world".grep("hello");  // grep operations
```

### Example 6: Session-Specific Debug
```grapa
// Enable session-specific debug override
$sys().putenv("GRAPA_SESSION_DEBUG", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_LEVEL", "2");
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "compiler");

// This session will show debug output even if system debug is disabled
// (when components are instrumented)
db = {}.table("ROW");
```

### Example 7: Component-Specific Levels (Future)
```grapa
// Set different debug levels for different components
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "database:3,grep:2,vector:1");

// When instrumented, components will show debug output based on their level
db = {}.table("ROW");  // database level 3 (most verbose)
"hello world".grep("hello");  // grep level 2 (medium verbose)
vec = [1, 2, 3];  // vector level 1 (basic debug)
```

## Complete Execution Pipeline Overview

Grapa's debug system provides comprehensive visibility into the entire execution pipeline:

### **Pipeline Stages**

1. **Lexer** (`lexer`): Raw text → Tokens
   - Token creation and classification
   - State machine transitions
   - Quote character analysis
   - Performance metrics

2. **Parser** (`parser`): Tokens → Execution Trees
   - Rule matching and grammar parsing
   - Error context and token streams
   - Execution tree construction
   - Performance metrics

3. **Executor** (`executor`): Execution Trees → Results
   - Operation execution flow
   - Library loading and function calls
   - Parameter binding and scope management
   - Performance metrics

### **Debug Component Shorthands**

- **`compiler`**: `lexer` + `parser` (compilation phase)
- **`runtime`**: `compiler` + `executor` (complete pipeline)

### **Performance Impact**

The debug system uses `ShouldDebug()` checks to ensure minimal performance impact:
- Debug output is only generated when explicitly enabled
- Component-specific filtering prevents unnecessary output
- Level-based filtering provides granular control
- Session isolation prevents debug interference

## Best Practices

### 1. Use Session-Specific Debug for Parallel Operations
When running multiple Grapa sessions in parallel, use session-specific debug settings to avoid interference:
```grapa
$sys().putenv("GRAPA_SESSION_DEBUG", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "database");
```

### 2. Target Specific Components
When components are instrumented, target specific components for focused debugging:
```grapa
// Good: Target specific component (when available)
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "database");

// Avoid: Enable all components unless needed
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "*");
```

### 3. Use Appropriate Debug Levels
- **Level 0**: Disabled (no output)
- **Level 1**: Basic operations
- **Level 2**: Detailed operations
- **Level 3**: Verbose operations
- **Levels 4-9**: Very detailed (use sparingly)

### 4. Clean Up Debug Settings
Remember to disable debug output when not needed:
```grapa
// Enable debug for troubleshooting
$sys().putenv("GRAPA_DEBUG_MODE", "1");

// ... perform operations ...

// Disable debug when done
$sys().putenv("GRAPA_DEBUG_MODE", "0");
```

### 5. Use Debug for Syntax Troubleshooting
Debug output is particularly valuable for identifying syntax issues:

```bash
# For scripts with syntax errors, use CLI environment variables
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_COMPONENTS=lexer ./grapa problematic_script.grc
```

```grapa
// For testing syntax variations, use op() with debug enabled
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "lexer");

// Test different syntax patterns
op()("x = 5 + 3")();  // Valid expression
op()("message = 'test'")();  // Shows single quote issue
op()("echo('wrong')")();  // Shows incorrect echo syntax
```

**Key Debug Insights:**
- **Token type 4**: Identifiers (variables, functions)
- **Token type 5**: String literals (check quote character)
- **Token type 8**: Numbers
- **Token type 10**: Operators and punctuation
- **Token type 11**: Newlines
- **Quote analysis**: `quote='` indicates single quotes (syntax error)

## Troubleshooting

### Debug Output Not Appearing
1. **Check debug mode**: Ensure `GRAPA_DEBUG_MODE` is set to `"1"`
2. **Check debug level**: Ensure the operation's debug level is within your set level
3. **Check components**: Ensure the component is included in `GRAPA_DEBUG_COMPONENTS`
4. **Check session override**: If using session debug, ensure `GRAPA_SESSION_DEBUG` is set to `"1"`

### Too Much Debug Output
1. **Reduce debug level**: Lower the `GRAPA_DEBUG_LEVEL` value
2. **Target specific components**: Use `GRAPA_DEBUG_COMPONENTS` to limit output
3. **Use component-specific levels**: Set different levels for different components

### Debug Output in Wrong Session
1. **Use session-specific settings**: Set `GRAPA_SESSION_DEBUG` to `"1"`
2. **Check session isolation**: Ensure you're setting debug for the correct session
3. **Verify session ID**: Use `$sys().getenv("GRAPA_SESSION_ID")` to confirm session

## See Also
- [CLI Quickstart](cli_quickstart.md) - Basic CLI usage including debug options
- [System Functions](sys/sys.md) - `$sys().putenv()` and `$sys().getenv()` functions
- [Advanced Topics](advanced_topics.md) - Advanced debugging techniques
- [Troubleshooting](troubleshooting.md) - General troubleshooting guide
