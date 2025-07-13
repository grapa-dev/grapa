# $sys

The `$sys` object provides general utility functions that are useful but don't belong in the native language syntax. These functions offer system-level operations, environment access, script compilation, and dynamic evaluation capabilities.

## Overview

`$sys` contains utility functions for:
- **Type introspection** - Getting object types and descriptions
- **Environment management** - Accessing system variables and platform information
- **Script compilation** - Pre-compiling scripts for performance
- **Dynamic evaluation** - Running scripts with parameters
- **System timing** - Sleep and delay operations

## Functions

### type(object)
Returns the type of the specified object.

**Parameters:**
- `object` - Any Grapa object or value

**Returns:** `$STR` - The type name (e.g., `$INT`, `$STR`, `$FLOAT`, `$ARRAY`)

**Example:**
```grapa
$sys().type(5);
$INT

$sys().type("hello");
$STR

$sys().type([1,2,3]);
$ARRAY
```

### describe(object)
**Status:** Not yet implemented

Will provide detailed information about objects beyond just their type. For example:
- Table objects: field definitions, record counts, storage type
- File objects: current directory, file system type
- Array objects: dimensions, element types
- Custom objects: property descriptions, method signatures

**Parameters:**
- `object` - Any Grapa object

**Returns:** `$STR` - Detailed object description (when implemented)

### getenv(type)
Gets environment variables and system information.

**Parameters:**
- `type` - Environment variable name or system constant

**Returns:** Value of the environment variable or system information

**Supported Environment Types:**

| Type | Description | Example Return |
|------|-------------|----------------|
| `$PATH` | System PATH environment variable | `"C:\Windows\System32;C:\Windows"` |
| `$STATICLIB` | Static library path | `"lib/grapa"` |
| `$ARGCIN` | Command line argument count | `3` |
| `$ARGV` | Command line arguments array | `["grapa.exe", "script.grc", "param"]` |
| `$LIB` | Library directory path | `"lib"` |
| `$BIN` | Binary directory path | `"bin"` |
| `$NAME` | Program name | `"grapa"` |
| `$WORK` | Working directory | `"C:\Users\user\project"` |
| `$HOME` | Home directory | `"C:\Users\user"` |
| `$TEMP` | Temporary directory | `"C:\Users\user\AppData\Local\Temp"` |
| `$VERSION` | Grapa version information | `{"major":0,"minor":0,"micro":2,"releaselevel":"alpha","serial":63,"date":2020-04-24T16:30:37.000000}` |
| `$LICENCE` | License information | `"Apache License 2.0"` |
| `$PLATFORM` | Platform compilation flags | See platform values below |

**Note:** Any value not starting with `$` will be directed to the native OS `getenv()` function.

**Example:**
```grapa
> $sys().getenv($VERSION)
{"major":0,"minor":0,"micro":2,"releaselevel":"alpha","serial":63,"date":2020-04-24T16:30:37.000000}

> $sys().getenv($HOME)
C:\Users\matichuk

> $sys().getenv("USERNAME")
matichuk
```

**Platform Values for $PLATFORM:**
- **Operating Systems:** `__APPLE__`, `_WIN32`, `_WIN64`, `__linux__`, `__ANDROID__`
- **Compilers:** `_MSC_VER`, `_MSC_FULL_VER`, `__GNUC__`, `__GNUC_MINOR__`, `__clang__`, `__MINGW32__`, `__MINGW64__`
- **Architectures:** `__i386__`, `__x86_64__`, `__arm__`, `__ARM_ARCH_5T__`, `__ARM_ARCH_7A__`, `__powerpc64__`, `__aarch64__`

### putenv(type, value)
Sets environment variables and system information.

**Parameters:**
- `type` - Environment variable name or system constant
- `value` - New value to set

**Returns:** `$BOOL` - `true` if successful, `false` if failed

**Example:**
```grapa
> $sys().putenv("CUSTOM_VAR", "my_value")
true

> $sys().getenv("CUSTOM_VAR")
my_value
```

**Note:** Like `getenv()`, any value not starting with `$` will be directed to the native OS `putenv()` function.

### compilef(scriptfilename, compiledfilename)
Compiles a Grapa script file and saves the compiled version to disk.

**Parameters:**
- `scriptfilename` - Path to the source script file
- `compiledfilename` - Path where the compiled script will be saved

**Returns:** `$BOOL` - `true` if compilation successful, `false` if failed

**Purpose:** Pre-compiling scripts improves execution performance by avoiding parsing overhead at runtime.

**Example:**
```grapa
> $sys().compilef("script.grc", "script.grz")
true
```

### compile(script)
Compiles a Grapa script in memory.

**Parameters:**
- `script` - Script text to compile

**Returns:** Compiled script object that can be executed

**Example:**
```grapa
> compiled = $sys().compile("a = 5 + 3; a.echo();")
> compiled()
8
```

### eval(script, params={}, rule="", profile="")
Dynamically evaluates a Grapa script with optional parameters.

**Parameters:**
- `script` - Script text or compiled script object to execute
- `params` - `$LIST` containing parameters to pass to the script (default: empty list)
- `rule` - Starting rule name for text scripts (default: empty string)
- `profile` - Execution profile for token generation (not yet implemented)

**Returns:** Result of script execution

**Example:**
```grapa
> $sys().eval("a", {"a": 33}, "$function")
33

> $sys().eval("x + y", {"x": 10, "y": 20})
30

> $sys().eval("result = input * 2; result", {"input": 15})
30
```

### sleep(ms)
Pauses execution for the specified number of milliseconds.

**Parameters:**
- `ms` - Number of milliseconds to sleep (must be `$INT`)

**Returns:** `null`

**Purpose:** Useful for timing, rate limiting, and synchronization in multi-threaded applications.

**Example:**
```grapa
> "Starting...".echo()
Starting...
> $sys().sleep(1000)  // Sleep for 1 second
> "Finished!".echo()
Finished!
```

## Usage Patterns

### Environment Variable Management
```grapa
// Get system information
version = $sys().getenv($VERSION);
platform = $sys().getenv($PLATFORM);

// Set custom environment variables
$sys().putenv("DEBUG_MODE", "true");
$sys().putenv("LOG_LEVEL", "verbose");
```

### Script Compilation Workflow
```grapa
// Compile a script file for faster execution
if ($sys().compilef("my_script.grc", "my_script.grz")) {
    // Load and execute compiled script
    compiled = $file().get("my_script.grz");
    result = $sys().eval(compiled);
}
```

### Dynamic Script Evaluation
```grapa
// Evaluate user-provided expressions safely
user_input = "2 * (3 + 4)";
try {
    result = $sys().eval(user_input);
    ("Result: " + result).echo();
} catch (error) {
    "Invalid expression".echo();
}
```

### Performance Timing
```grapa
// Measure execution time
start_time = $TIME().utc();
// ... perform operations ...
$sys().sleep(100);  // Simulate work
end_time = $TIME().utc();
elapsed_ms = ((end_time - start_time) / 1000000).int();
("Execution time: " + elapsed_ms + " ms").echo();
```

## Error Handling

Most `$sys` functions return appropriate error values when operations fail:
- `getenv()` returns `null` for non-existent variables
- `putenv()` returns `false` for invalid operations
- `compilef()` and `compile()` return `false` for compilation errors
- `eval()` throws exceptions for script execution errors

## Performance Considerations

- **Compiled scripts** execute significantly faster than interpreted scripts
- **Environment variable access** is cached for performance
- **Dynamic evaluation** has overhead - use sparingly in performance-critical code
- **Sleep operations** are precise to millisecond resolution

