# $sys

The `$sys` object provides general utility functions that are useful but don't belong in the native language syntax. These functions offer system-level operations, environment access, script compilation, and dynamic evaluation capabilities.

## Overview

`$sys` contains utility functions for:
- **Type introspection** - Getting object types and descriptions
- **Environment management** - Accessing system variables and platform information
- **Script compilation** - Pre-compiling scripts for performance
- **Dynamic evaluation** - Running scripts with parameters (Grapa's core meta-programming capability)
- **System timing** - Sleep and delay operations

## Functions

### type(object)
Returns the type of the specified object.

**Parameters:**
- `object` - Any Grapa object or value

**Returns:** `$STR` - The type name (e.g., `$INT`, `$STR`, `$FLOAT`, `$LIST`)

**Example:**
```grapa
$sys().type(5);
$INT

$sys().type("hello");
$STR

$sys().type([1,2,3]);
$LIST
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

### getenv(name)
Gets environment variables and system information.

**Parameters:**
- `name` - Environment variable name or system constant

**Returns:** Value of the environment variable or system information

**Supported Environment Types:**

| Type | Description | Example Return |
|------|-------------|----------------|
| `PATH` | System PATH environment variable | `"C:\Windows\System32;C:\Windows"` |
| `$STATICLIB` | Static library path | `"lib/grapa"` |
| `$ARGCIN` | Stdin data (when using -S option) | `"data from stdin"` |
| `$ARGV` | Positional command line arguments only | `["a", "b", "c"]` |
| `$CLIARGV` | Full command line including flags and script | `["./grapa", "-c", "script", "a", "b", "c"]` |
| `$GRAPA_LIB` | Library directory path | `"lib"` |
| `$GRAPA_BIN` | Binary directory path (base installation directory) | `"/Users/matichuk/GitHub/grapa"` |
| `$NAME` | Program name | `"grapa"` |
| `$WORK` | Working directory | `"C:\Users\user\project"` |
| `$HOME` | Home directory | `"C:\Users\user"` |
| `$TEMP` | Temporary directory | `"C:\Users\user\AppData\Local\Temp"` |
| `$GRAPA_VERSION` | Grapa version information | `{"major":0,"minor":0,"micro":2,"releaselevel":"alpha","serial":63,"date":2020-04-24T16:30:37.000000}` |
| `$LICENCE` | License information | `"Apache License 2.0"` |
| `$PLATFORM` | Platform compilation flags | See platform values below |

**Note:** Any value not starting with `$` will be directed to the native OS `getenv()` function.

**Example:**
```grapa
$sys().getenv($GRAPA_VERSION);
/* Returns: {"major":0,"minor":0,"micro":2,"releaselevel":"alpha","serial":63,"date":2020-04-24T16:30:37.000000} */

$sys().getenv($HOME);
/* Returns: C:\Users\matichuk */

$sys().getenv("USERNAME");
/* Returns: matichuk */

$sys().getenv($ARGCIN);
/* Returns stdin data when using -S option */
/* Example: echo "data" | grapa -S -c "$sys().getenv('$ARGCIN').echo()" */
```

### Command Line Arguments

Grapa provides two ways to access command line arguments:

#### **$ARGV - Positional Arguments Only**
Returns only the positional arguments passed after the script, excluding flags and the script name itself.

**Example Usage:**
```bash
./grapa -c "script.grc" arg1 arg2 arg3
```

```grapa
/* Get positional arguments */
args = $sys().getenv($ARGV);
args.echo();
/* Output: ["arg1", "arg2", "arg3"] */

/* Access specific argument */
first_arg = args.get(0);
("First argument: " + first_arg).echo();
/* Output: First argument: arg1 */

/* Check number of arguments */
arg_count = args.len();
("Number of arguments: " + arg_count).echo();
/* Output: Number of arguments: 3 */
```

#### **$CLIARGV - Full Command Line**
Returns the complete command line including the executable name, flags, script, and all arguments.

**Example Usage:**
```bash
./grapa -c "script.grc" arg1 arg2 arg3
```

```grapa
/* Get full command line */
full_cmd = $sys().getenv($CLIARGV);
full_cmd.echo();
/* Output: ["./grapa", "-c", "script.grc", "arg1", "arg2", "arg3"] */

/* Access executable name */
exe_name = full_cmd.get(0);
("Executable: " + exe_name).echo();
/* Output: Executable: ./grapa */

/* Check if debug mode is enabled */
is_debug = full_cmd.grep("-d").len() > 0;
("Debug mode: " + is_debug).echo();
/* Output: Debug mode: false */
```

#### **Practical Examples**

**Basic Argument Processing:**
```grapa
/* script.grc */
args = $sys().getenv($ARGV);

if (args.len() == 0) {
    "Usage: ./grapa script.grc <filename>".echo();
    exit(1);
}

filename = args.get(0);
("Processing file: " + filename).echo();
```

**Command Line Flag Detection:**
```grapa
/* script.grc */
full_cmd = $sys().getenv($CLIARGV);

/* Check for verbose flag */
verbose = full_cmd.grep("--verbose").len() > 0 || full_cmd.grep("-v").len() > 0;

/* Check for debug flag */
debug = full_cmd.grep("-d").len() > 0;

if (verbose) {
    "Verbose mode enabled".echo();
}

if (debug) {
    "Debug mode enabled".echo();
}
```

**Argument Validation:**
```grapa
/* script.grc */
args = $sys().getenv($ARGV);

/* Validate required arguments */
if (args.len() < 2) {
    "Error: Requires at least 2 arguments".echo();
    "Usage: ./grapa script.grc <input_file> <output_file>".echo();
    exit(1);
}

input_file = args.get(0);
output_file = args.get(1);

/* Validate file existence */
if (!$file(input_file).exists()) {
    ("Error: Input file not found: " + input_file).echo();
    exit(1);
}

("Processing " + input_file + " -> " + output_file).echo();
```

**Environment-Based Configuration:**
```grapa
/* script.grc */
args = $sys().getenv($ARGV);
full_cmd = $sys().getenv($CLIARGV);

/* Set environment based on arguments */
if (args.grep("--production").len() > 0) {
    $sys().putenv("ENVIRONMENT", "production");
} else if (args.grep("--staging").len() > 0) {
    $sys().putenv("ENVIRONMENT", "staging");
} else {
    $sys().putenv("ENVIRONMENT", "development");
}

/* Check for debug mode */
if (full_cmd.grep("-d").len() > 0) {
    $sys().putenv("DEBUG_MODE", "true");
}

env = $sys().getenv("ENVIRONMENT");
debug = $sys().getenv("DEBUG_MODE");

("Environment: " + env).echo();
("Debug mode: " + debug).echo();
```

**Platform Values for $PLATFORM:**
- **Operating Systems:** `__APPLE__`, `_WIN32`, `_WIN64`, `__linux__`, `__ANDROID__`
- **Compilers:** `_MSC_VER`, `_MSC_FULL_VER`, `__GNUC__`, `__GNUC_MINOR__`, `__clang__`, `__MINGW32__`, `__MINGW64__`
- **Architectures:** `__i386__`, `__x86_64__`, `__arm__`, `__ARM_ARCH_5T__`, `__ARM_ARCH_7A__`, `__powerpc64__`, `__aarch64__`

### CLI Development with System Variables

These system environment variables are particularly useful for CLI script development:

#### **Essential for CLI Scripts**
- **`$WORK`**: Current working directory - essential for relative path operations
- **`$HOME`**: User's home directory - for user-specific configuration files
- **`$TEMP`**: Temporary directory - for temporary file operations
- **`$GRAPA_VERSION`**: Grapa version - for compatibility checking
- **`$PLATFORM`**: Platform information - for cross-platform script behavior

#### **Useful for Advanced CLI Scripts**
- **`$GRAPA_BIN`**: Base installation directory - for finding related executables and detecting installation type
- **`$GRAPA_LIB`**: Library directory - for library discovery and loading
- **`$NAME`**: Program name - for self-referencing in usage messages

#### **Installation Detection with $GRAPA_BIN**
Use `$GRAPA_BIN` to detect whether Grapa is running from a development or production installation:

```grapa
/* Detect installation type */
grapa_bin = $sys().getenv($GRAPA_BIN);
is_development = grapa_bin.grep("GitHub").len() > 0;
is_production = !is_development;

/* Set paths based on installation type */
if (is_development) {
    /* Development installation - running from source */
    user_lib = $sys().getenv($HOME) + "/.grapa/lib";
    config_dir = $sys().getenv($HOME) + "/.grapa";
} else {
    /* Production installation - installed via installer */
    user_lib = "/usr/local/lib";
    config_dir = "/etc/grapa";
}

("Installation type: " + (is_development ? "development" : "production")).echo();
("Base directory: " + grapa_bin).echo();
```

#### **CLI Script Example**
```grapa
/* cli_script.grc */
/* Get essential system information */
work_dir = $sys().getenv($WORK);
home_dir = $sys().getenv($HOME);
temp_dir = $sys().getenv($TEMP);
version = $sys().getenv($GRAPA_VERSION);
platform = $sys().getenv($PLATFORM);

/* Create cross-platform paths */
config_dir = home_dir + "/.grapa";
log_file = temp_dir + "/grapa_script.log";
output_dir = work_dir + "/output";

/* Version compatibility check */
if (version < "0.1.40") {
    ("Error: Requires Grapa version 0.1.40 or higher. Current: " + version).echo();
    exit(1);
}

/* Platform-specific behavior */
if (platform.grep("__APPLE__").len() > 0) {
    "Running on macOS".echo();
} else if (platform.grep("_WIN32").len() > 0) {
    "Running on Windows".echo();
} else {
    "Running on Linux/Other".echo();
}

/* Create directories if they don't exist */
if (!$file(config_dir).exists()) {
    $file(config_dir).mk();
}

if (!$file(output_dir).exists()) {
    $file(output_dir).mk();
}

("Config: " + config_dir).echo();
("Log: " + log_file).echo();
("Output: " + output_dir).echo();
```

### Automatic Configuration File Loading

Grapa automatically loads configuration files during startup to set up your environment:

**Configuration File Search Order:**
1. **`~/.grapa/config.grc`** - Plain text configuration file in user's home directory (recommended)
2. **`~/.grapa/config.grz`** - Compressed configuration file in user's home directory (for large configs)
3. **`$WORK/.grapa/config.grc`** - Plain text configuration file in current working directory (project-specific)

**Configuration File Format:**
Configuration files are standard Grapa scripts that execute during startup:

```grapa
/* ~/.grapa/config.grc */
"Loading user configuration...\n".echo();

/* Set custom environment variables */
$sys().putenv("CUSTOM_PATH", "/usr/local/myapp");
$sys().putenv("DEBUG_MODE", "true");

/* Define custom functions */
my_helper = op(x) { x * 2; };

/* Set up global variables */
$global.user_config = {
    theme: "dark",
    timeout: 30,
    auto_save: true
};

"Configuration loaded successfully\n".echo();
```

**Use Cases:**
- **Environment Setup**: Configure paths, debug settings, and environment variables
- **Custom Functions**: Define helper functions available in all sessions
- **Global Variables**: Set up shared configuration objects
- **User Preferences**: Store and load user-specific settings

**File Types:**
- **`.grc`**: Plain text Grapa script (recommended for readability and small configs)
- **`.grz`**: Compressed Grapa script (for larger configurations or faster startup)

**Compiling Configuration Files for Performance:**
For large configuration files that slow down startup, compile them to `.grz` format:

```grapa
/* Compile config.grc to config.grz for faster loading */
$sys().compilef("~/.grapa/config.grc", "~/.grapa/config.grz");
```

**When to Use Each Format:**
- **Use `.grc`** for: Small configs, development, easy editing
- **Use `.grz`** for: Large configs, production, faster startup times

**Example: Setting Up Custom Environment Variables:**
```grapa
/* ~/.grapa/config.grc */
/* Set up custom environment variables using regular variables */
GRAPA_HOME = $sys().getenv($WORK);
GRAPA_USER_LIB = $sys().getenv($HOME) + "/.grapa/lib";
GRAPA_PROJECT_LIB = $sys().getenv($WORK) + "/lib";

/* Add user library to search path if it exists */
if ($file().exists(GRAPA_USER_LIB)) {
    current_path = $sys().getenv($GRAPA_PATH);
    $sys().putenv($GRAPA_PATH, current_path + [GRAPA_USER_LIB]);
}

/* Set up custom functions */
my_helper = op(x) { x * 2; };
```

### putenv(name, value) / setenv(name, value)
Sets environment variables and system information. Both `putenv()` and `setenv()` are aliases for the same functionality.

### const(object)
Returns a read-only copy of the specified object.

**Parameters:**
- `object` - Any Grapa object or value

**Returns:** Read-only copy of the object

**Example:**
```grapa
data = [1, 2, 3];
readonly_data = $sys().const(data);
/* readonly_data is now read-only */
```

### setconst(object, readonly)
Makes an object read-only or writable.

**Parameters:**
- `object` - Any Grapa object or variable
- `readonly` - Boolean: `true` to make read-only, `false` to make writable

**Returns:** The modified object

**Example:**
```grapa
my_variable = "important data";
$sys().setconst(my_variable, true);  /* Make read-only */

/* Attempting to modify will fail */
my_variable = "new data";  /* Error: Cannot modify const variable */

/* Make writable again */
$sys().setconst(my_variable, false);
my_variable = "new data";  /* Now works */
```

**Important**: This feature helps protect critical system components from unintended changes, but should be used carefully as it can prevent legitimate modifications.

**Parameters:**
- `name` - Environment variable name or system constant
- `value` - New value to set

**Returns:** `$BOOL` - `true` if successful, `false` if failed

**Example:**
```grapa
// Both methods work identically
$sys().putenv("CUSTOM_VAR", "my_value");
$sys().setenv("ANOTHER_VAR", "another_value");

$sys().getenv("CUSTOM_VAR");
my_value

$sys().getenv("ANOTHER_VAR");
another_value
```

#### Debug Environment Variables
`putenv()` supports special debug-related environment variables for controlling Grapa's debugging system:

**System-Level Debug Variables:**
```grapa
// Enable system-level debug mode
$sys().putenv("GRAPA_DEBUG_MODE", "1");

// Set debug verbosity level (0-9)
$sys().putenv("GRAPA_DEBUG_LEVEL", "3");

// Enable debug for specific components
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "database,grep");
```

**Session-Level Debug Variables:**
```grapa
// Enable session-specific debug override
$sys().putenv("GRAPA_SESSION_DEBUG", "1");

// Set session debug level
$sys().putenv("GRAPA_SESSION_DEBUG_LEVEL", "2");

// Set session-specific debug components
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "vector,filesystem");
```

**For complete debugging documentation, see [Debugging in Grapa](../debugging.md).**

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
$sys().compilef("script.grc", "script.grz");
true
```

### compile(script)
Compiles a Grapa script in memory.

**Parameters:**
- `script` - Script text to compile

**Returns:** Compiled script object that can be executed with `$sys().eval()`

**Example:**
```grapa
/* Compile a script */
compiled = $sys().compile("a = 5 + 3; a.echo();");

/* Execute the compiled script using eval() */
$sys().eval(compiled);
/* Result: 8 */

/* Direct execution doesn't work */
compiled();
/* Result: {} (empty result) */
```

**Note:** Compiled objects must be executed using `$sys().eval()` rather than direct function calls.

### Relationship Between compile() and op()

Both `$sys().compile()` and `op()` create execution trees, but with different interfaces:

```grapa
/* Using $sys().compile() */
compiled = $sys().compile("a = 5 + 3; a.echo();");
$sys().eval(compiled);
/* Result: 8 */

/* Using op() - direct execution */
direct_op = op()("a = 5 + 3; a.echo();");
direct_op();
/* Result: 8 */

/* Both create similar execution trees */
compiled;
/* Result: @<[op,@[@<assign,{a,@<add,{5,3}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{}> */

direct_op;
/* Result: @<[op,@[@<assign,{a,@<add,{5,3}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{}> */
```

**Key Differences:**
- **`$sys().compile()`**: Creates compiled object that requires `$sys().eval()` for execution
- **`op()`**: Creates function that can be called directly
- **Both**: Generate the same underlying execution tree structure
- **Performance**: Both benefit from compile-time optimization

### eval(script, sparams={}, srule="", sprofile="")
Evaluates a script with optional parameters, rules, and profile. This is one of Grapa's most powerful features, enabling dynamic code execution and meta-programming.

**Parameters:**
- `script` - Script string or compiled $OP object
- `sparams` - Parameter object (default: `{}`)
- `srule` - Custom rule set (default: `""`)
- `sprofile` - Profile configuration (default: `""`)

**Returns:** Result of script execution

**Examples:**
```grapa
/* Evaluate simple script */
result = $sys().eval("x = 5 + 3; x");
/* Result: 8 */

/* Evaluate with parameters */
result = $sys().eval("x + y", {"x": 5, "y": 3});
/* Result: 8 */

/* Evaluate complex expression */
result = $sys().eval("(a + b) * c", {"a": 2, "b": 3, "c": 4});
/* Result: 20 */

/* Evaluate compiled $OP object */
compiled = $sys().compile("a + b");
result = $sys().eval(compiled, {"a": 10, "b": 20});
/* Result: 30 */
```

**⚠️ Parameter Mutability:** Grapa provides two evaluation methods with different parameter handling:

- **`$sys().eval()`** - Parameters are **mutable** (pass-by-reference), like traditional function calls
- **`$sys().eval2()`** - Parameters are **immutable** (pass-by-value), using `$local++=pParams`

```grapa
/* $sys().eval() - parameters are mutable (pass-by-reference) */
x = 0;
result = $sys().eval("a = 10; a", {"a": x});
/* Result: 10 */
x;  /* 10 - parameter was mutated */

/* $sys().eval2() - parameters are immutable (pass-by-value) */
x = 0;
result = $sys().eval2("a = 10; a", {"a": x});
/* Result: 10 */
x;  /* Still 0 - parameter was not mutated */

/* op() - parameters are mutable (pass-by-reference) */
f = op(a:0)("a = 10; a");
x = 0;
result = f(x);  /* x is now 10 - parameter was mutated */
x;  /* 10 - original value was modified */

/* To prevent mutation with op(), use .copy() */
x = 0;
result = f(x.copy());  /* x remains 0 */
x;  /* Still 0 */
```

**⚠️ Important Note:** `$sys().eval()` does NOT work with `.grz` files. To execute `.grz` files, use:
```grapa
/* Correct method for .grz files */
$file().get("file.grz").decode("ZIP-GRAPA")["op"]();
```

### eval2(script, sparams={}, srule="")
Evaluates a script with **immutable parameters** (pass-by-value). This is the safe version of `eval()` that prevents accidental modification of passed parameters.

**Parameters:**
- `script` - Script string to evaluate
- `sparams` - Parameter object (default: `{}`) - **These parameters cannot be modified**
- `srule` - Custom rule set (default: `""`)

**Returns:** Result of script execution

**Key Differences from `eval()`:**
- **Immutable Parameters**: Parameters are copied using `$local++=pParams`, preventing modification
- **Safer for User Input**: Ideal for evaluating user-provided scripts or dynamic code
- **No Compiled Object Support**: Only works with script strings, not compiled `$OP` objects

**Examples:**
```grapa
/* Safe evaluation with immutable parameters */
x = 0;
result = $sys().eval2("a = 10; a", {"a": x});
/* Result: 10 */
x;  /* Still 0 - parameter was not mutated */

/* User input processing (safe) */
user_script = "input * 2 + offset";
result = $sys().eval2(user_script, {"input": 5, "offset": 3});
/* Result: 13 */
/* Original parameters remain unchanged */

/* Template processing */
template = "result = base * multiplier";
result = $sys().eval2(template, {"base": 10, "multiplier": 2});
/* Result: 20 */
/* Template cannot modify the base or multiplier values */
```

**When to Use `eval2()`:**
- Processing user input or untrusted scripts
- Template evaluation where parameters should remain unchanged
- Any scenario where parameter immutability is important
- Debugging or testing where you want to ensure no side effects

### sleep(milliseconds)
Pauses execution for the specified number of milliseconds.

**Parameters:**
- `milliseconds` - Number of milliseconds to sleep (must be `$INT`)

**Returns:** `null`

**Purpose:** Useful for timing, rate limiting, and synchronization in multi-threaded applications.

**Example:**
```grapa
"Starting...".echo();
Starting...
$sys().sleep(1000);  /* Sleep for 1 second */
"Finished!".echo();
Finished!
```

## Usage Patterns

### Environment Variable Management
```grapa
/* Get system information */
version = $sys().getenv($GRAPA_VERSION);
platform = $sys().getenv($PLATFORM);

/* Set custom environment variables */
$sys().putenv("DEBUG_MODE", "true");
$sys().putenv("LOG_LEVEL", "verbose");
```

### Script Compilation Workflow
```grapa
/* Compile a script file for faster execution */
if ($sys().compilef("my_script.grc", "my_script.grz")) {
    /* Load and execute compiled script */
    compiled = $file().get("my_script.grz");
    result = $sys().eval(compiled);
}

/* Compile and execute in-memory scripts */
compiled = $sys().compile("x = 10; y = 20; x + y;");
result = $sys().eval(compiled);
/* Result: 30 */
```

### Dynamic Script Evaluation
```grapa
/* Evaluate user-provided expressions safely */
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
/* Measure execution time */
start_time = $TIME().utc();
/* ... perform operations ... */
$sys().sleep(100);  /* Simulate work */
end_time = $TIME().utc();
elapsed_ms = ((end_time - start_time) / 1000000).int();
("Execution time: " + elapsed_ms + " ms").echo();
```

### Execution Tree Creation and Optimization
```grapa
/* Create execution trees with different methods */
script = "result = input * 2 + 1; result";

/* Method 1: Using $sys().compile() */
compiled = $sys().compile(script);
$sys().eval(compiled, {"input": 5});
/* Result: 11 */

/* Method 2: Using op() */
direct_func = op("input"=0)(script);
direct_func(5);
/* Result: 11 */

/* Method 3: Using op() with block syntax */
block_func = op("input"=0){result = input * 2 + 1; result};
block_func(5);
/* Result: 11 */

/* View execution trees */
compiled;
/* Shows: @<[op,@[@<assign,{result,@<add,{@<mul,{@<var,{input}>},2}>},1}>],@<var,{result}>]],{}> */

direct_func;
/* Shows: @<[op,@[@<assign,{result,@<add,{@<mul,{@<var,{input}>},2}>},1}>],@<var,{result}>]],{"input":0}> */
```

### Advanced Compilation Patterns
```grapa
/* Compile-time optimization examples */
simple = $sys().compile("5 + 3");
$sys().eval(simple);
/* Result: 8 (constant folding applied) */

complex = $sys().compile("x = 5; y = 3; x + y");
$sys().eval(complex);
/* Result: 8 (assignment and addition preserved) */

/* Dynamic compilation with parameters */
template = "result = base * multiplier + offset";
dynamic_compiled = $sys().compile(template);
$sys().eval(dynamic_compiled, {"base": 10, "multiplier": 2, "offset": 5});
/* Result: 25 */
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

