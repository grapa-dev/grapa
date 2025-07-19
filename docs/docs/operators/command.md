# Command Operators

## include

Loads referenced file during compile time. Also useful for testing a library or class file by using "include" to reload the file. If included in a script, this is completed during compile time.

### Basic Syntax

```grapa
include "lib/grapa/grapa.grc";
include "examples/basic_example.grc";
```

### File Types and Compilation Behavior

#### Source Files (.grc)
When you include a `.grc` file, it is compiled at the same time as the script that called the include:

```grapa
/* This .grc file will be parsed and compiled during compilation */
include "lib/custom/string_utils.grc";

/* The included code is available immediately */
result = some_function_from_included_file();
```

**Compilation Process:**
1. **Parse Phase**: The `.grc` file is parsed according to Grapa's BNF grammar
2. **Tree Building**: Execution trees are generated for all code in the file
3. **Optimization**: Compile-time optimizations are applied
4. **Integration**: The execution trees are integrated into the main script

**Performance Characteristics:**
- **Compilation Time**: Longer (file must be parsed and compiled)
- **Runtime Performance**: Same as inline code (fully optimized)
- **Development**: Best for active development and debugging

#### Pre-compiled Files (.grz)
When you include a `.grz` file, it is already compiled and will shorten the runtime compilation:

```grapa
/* This .grz file is pre-compiled and loads faster */
include "lib/custom/string_utils.grz";

/* The included code is available immediately */
result = some_function_from_included_file();
```

**Compilation Process:**
1. **Load Phase**: The pre-compiled execution trees are loaded directly
2. **Integration**: The execution trees are integrated into the main script
3. **No Parsing**: No grammar parsing or tree building required

**Performance Characteristics:**
- **Compilation Time**: Very fast (no parsing overhead)
- **Runtime Performance**: Same as source files (fully optimized)
- **Deployment**: Best for production and distribution

### Creating Pre-compiled Files

You can create `.grz` files from `.grc` files using the system compilation function:

```grapa
/* Compile a source file to pre-compiled format */
$sys().compilef("lib/custom/string_utils.grc", "lib/custom/string_utils.grz");
```

### Practical Examples

#### Test Suite Organization
```grapa
/* Include multiple test files */
include "examples/basic_example.grc";
include "examples/advanced_example.grc";
include "examples/performance_example.grc";
```

#### Library Development
```grapa
/* Include core library */
include "lib/grapa/grapa.grc";

/* Include custom extensions */
include "lib/custom/string_utils.grc";
include "lib/custom/math_utils.grc";
```

#### Production Deployment
```grapa
/* Use pre-compiled files for production */
include "lib/grapa/grapa.grz";
include "lib/custom/string_utils.grz";
include "lib/custom/math_utils.grz";
```

### Compilation Timing

Includes are processed **during compilation**, not at runtime:

```grapa
/* This include is processed when the script is compiled */
include "lib/grapa/grapa.grc";

/* The included code is available immediately */
result = some_function_from_included_file();
```

### Performance Optimization

#### Development Phase
```grapa
/* Use .grc files for development */
include "lib/development/debug_utils.grc";
include "lib/development/test_helpers.grc";
```

#### Production Phase
```grapa
/* Use .grz files for production */
include "lib/production/core.grz";
include "lib/production/utils.grz";
```

### Error Handling

#### File Not Found
```grapa
/* Include with error handling */
try {
    include "lib/missing_file.grc";
} catch (error) {
    ("Warning: Could not include missing_file.grc: " + error).echo();
}
```

#### Compilation Errors
```grapa
/* Compilation errors in included files are reported during compilation */
include "lib/syntax_error.grc";  /* Will fail during compilation */
```

### Best Practices

1. **Development**: Use `.grc` files for easy debugging and modification
2. **Production**: Use `.grz` files for optimal performance
3. **Organization**: Use clear, descriptive names for included files
4. **Dependencies**: Document include dependencies clearly
5. **Build Process**: Automate compilation from `.grc` to `.grz` for deployment

## exit

Causes the command line shell / console to exit. Primarily used for a script that is initiated from the command line.

If used in the console, the exit will not happen until another command is issued from the shell. This is because everything is handled async.

To exit while in the console, enter a '.' character.
