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
- **Component-Specific Debugging**: Target specific system components (database, grep, vector, etc.)
- **Debug Levels**: Control verbosity from 0 (disabled) to 9 (maximum detail)
- **Session Isolation**: Debug settings are isolated between parallel sessions
- **Environment Variable Integration**: Configure debug settings via environment variables

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

## Script-Based Debug Control

### System-Level Debug Control

Enable or disable system-level debug settings that affect all sessions:

```grapa
// Enable system-level debug
$sys().putenv("GRAPA_DEBUG_MODE", "1");
$sys().putenv("GRAPA_DEBUG_LEVEL", "3");

// Disable system-level debug
$sys().putenv("GRAPA_DEBUG_MODE", "0");
```

### Session-Level Debug Control

Enable session-specific debug settings that override system settings for the current session:

```grapa
// Enable session-specific debug override
$sys().putenv("GRAPA_SESSION_DEBUG", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_LEVEL", "2");

// Disable session-specific debug
$sys().putenv("GRAPA_SESSION_DEBUG", "0");
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
| `GRAPA_SESSION_DEBUG` | Enable session debug override | `"0"` or `"1"` | `"1"` |
| `GRAPA_SESSION_DEBUG_LEVEL` | Session debug verbosity | `0-9` | `"2"` |
| `GRAPA_SESSION_DEBUG_COMPONENTS` | Session debug components | Component list | `"vector,filesystem"` |
| `GRAPA_SESSION_ID` | Current session ID (read-only) | Auto-generated | `"123"` |

## Available Debug Components

### High Priority Components
- **`database`**: Database operations, queries, and storage
- **`grep`**: Text search and pattern matching
- **`script`**: Script execution and parsing
- **`network`**: Network operations and connections

### Medium Priority Components
- **`vector`**: Vector operations and calculations
- **`filesystem`**: File system operations
- **`threading`**: Thread management and synchronization
- **`memory`**: Memory allocation and management

### Low Priority Components
- **`widget`**: GUI widget operations
- **`time`**: Time-related operations
- **`prime`**: Prime number operations
- **`crypto`**: Cryptographic operations
- **`object`**: Object management
- **`system`**: System-level operations
- **`encoding`**: Data encoding/decoding

## Debug Output Formats

### System-Level Debug Output
System-level debug output appears on stderr with the format:
```
[DEBUG] message
[DEBUG-component] message
```

### Session-Level Debug Output
Session-level debug output appears through the response system with the format:
```
[DEBUG-SESSION-{session_id}] message
[DEBUG-SESSION-{session_id}-{component}] message
```

## Practical Examples

### Example 1: Database Debugging
```grapa
// Enable database debugging
$sys().putenv("GRAPA_DEBUG_MODE", "1");
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "database");

// Database operations will now show debug output
db = $file().table('ROW');
db.mkfield('name', 'STR');
db.set('user1', 'Alice', 'name');
result = db.get('user1', 'name');

// Disable debug
$sys().putenv("GRAPA_DEBUG_MODE", "0");
```

### Example 2: Multi-Component Debugging
```grapa
// Enable debug for multiple components with different levels
$sys().putenv("GRAPA_DEBUG_MODE", "1");
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "database:3,grep:2,vector:1");

// Operations will show debug output based on component and level
db = $file().table('ROW');  // database level 3
"hello world".grep("hello");  // grep level 2
vec = [1, 2, 3];  // vector level 1
```

### Example 3: Session-Specific Debugging
```grapa
// Enable session-specific debug override
$sys().putenv("GRAPA_SESSION_DEBUG", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_LEVEL", "2");
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "database");

// This session will show debug output even if system debug is disabled
db = $file().table('ROW');
db.mkfield('name', 'STR');
```

### Example 4: Conditional Debugging
```grapa
// Enable debug only for specific operations
$sys().putenv("GRAPA_DEBUG_MODE", "1");
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "database");

// Database operations show debug output
db = $file().table('ROW');

// Disable debug for other operations
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "grep");

// Only grep operations show debug output
"hello world".grep("hello");
```

## Best Practices

### 1. Use Session-Specific Debug for Parallel Operations
When running multiple Grapa sessions in parallel, use session-specific debug settings to avoid interference:
```grapa
$sys().putenv("GRAPA_SESSION_DEBUG", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "database");
```

### 2. Target Specific Components
Instead of enabling all debug output, target specific components for focused debugging:
```grapa
// Good: Target specific component
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