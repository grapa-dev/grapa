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
$sys().getenv($VERSION);
/* Returns: {"major":0,"minor":0,"micro":2,"releaselevel":"alpha","serial":63,"date":2020-04-24T16:30:37.000000} */

$sys().getenv($HOME);
/* Returns: C:\Users\matichuk */

$sys().getenv("USERNAME");
/* Returns: matichuk */
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
$sys().putenv("CUSTOM_VAR", "my_value");
true

$sys().getenv("CUSTOM_VAR");
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

**⚠️ Important Note:** `$sys().eval()` does NOT work with `.grz` files. To execute `.grz` files, use:
```grapa
/* Correct method for .grz files */
$file().get("file.grz").decode("ZIP-GRAPA")["op"]();
```

### sleep(ms)
Pauses execution for the specified number of milliseconds.

**Parameters:**
- `ms` - Number of milliseconds to sleep (must be `$INT`)

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
version = $sys().getenv($VERSION);
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

