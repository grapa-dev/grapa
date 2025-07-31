---
tags:
  - maintainer
  - lowlevel
  - implementation
  - modules
---

# Include System in Grapa

## Overview

Grapa's include system allows you to modularize code by including external files during compilation. The include system supports both source files (`.grc`) and pre-compiled files (`.grz`), providing flexibility for development and deployment optimization.

## Grammar Definition

The include functionality is defined in the Grapa grammar (`lib/grapa/$grapa.grc`):

```grapa
| $SYSID("INCLUDE") <$comp> {@<include,{$2}>}
| include <$comp> {@<include,{$2}>}
```

This creates an execution tree node `@<include,{filepath}>` that is processed by the Grapa C++ library during compilation.

## Include Syntax

### Basic Include Statement
```grapa
include "path/to/file.grc";
include "path/to/file.grz";
```

### Include with Variables
```grapa
library_path = "lib/grapa/";
include (library_path + "grapa.grc");
```

## File Types and Compilation Behavior

### 1. Source Files (.grc)

When you include a `.grc` file:

```grapa
include "test/test_basic.grc";
```

**Compilation Process:**
1. **Parse Phase**: The `.grc` file is parsed according to Grapa's BNF grammar
2. **Tree Building**: Execution trees are generated for all code in the file
3. **Optimization**: Compile-time optimizations are applied
4. **Integration**: The execution trees are integrated into the main script's compilation

**Performance Characteristics:**
- **Compilation Time**: Longer (file must be parsed and compiled)
- **Runtime Performance**: Same as inline code (fully optimized)
- **Development**: Best for active development and debugging

### 2. Pre-compiled Files (.grz)

When you include a `.grz` file:

```grapa
include "test/test_basic.grz";
```

**Compilation Process:**
1. **Load Phase**: The pre-compiled execution trees are loaded directly
2. **Integration**: The execution trees are integrated into the main script
3. **No Parsing**: No grammar parsing or tree building required

**Performance Characteristics:**
- **Compilation Time**: Very fast (no parsing overhead)
- **Runtime Performance**: Same as source files (fully optimized)
- **Deployment**: Best for production and distribution

## Creating Pre-compiled Files

### Using $sys().compilef()

You can create `.grz` files from `.grc` files using the system compilation function:

```grapa
/* Compile a source file to pre-compiled format */
$sys().compilef("test/test_basic.grc", "test/test_basic.grz");
```

### Build Process Integration

The `buildgrapalib.grc` script demonstrates the build process:

```grapa
/* From buildgrapalib.grc */
f = $file();
f.chd($sys().getenv($LIB));

/* Find all .grc files */
a = f.ls().reduce(op(a,b){
    if ((b.$KEY.left(1)=="$")&&(b.$KEY.right(1)=="c")) {
        a += b.$KEY.rtrim("c");
    }
}, {});

/* Compile each .grc file to .grz */
a.reduce(op(x,b,g){
    $sys().encode(op()(g.get(b+"c")), "ZIP-GRAPA").setfile(g,b+"z");
}, {}, f);
```

## Practical Examples

### Test Suite Organization

The `run_tests.grc` file demonstrates effective use of includes:

```grapa
/* Test Runner for Grapa Unicode Grep */
"=== GRAPA UNICODE GREP TEST SUITE ===\n".echo();
"Running comprehensive test suite for Unicode grep functionality\n".echo();

/* Include and run all test files */
include "test/test_current_capabilities.grc";
include "test/test_performance_optimizations.grc";
include "test/test_atomic_groups.grc";
include "test/test_lookaround_assertions.grc";
include "test/test_unicode_grapheme_clusters.grc";
include "test/test_parallel_grep_verification.grc";
include "test/test_option_combinations_matrix.grc";
include "test/test_error_handling.grc";

/* Include new comprehensive test files */
include "test/test_binary_mode.grc";
include "test/test_custom_delimiters.grc";
include "test/test_advanced_context.grc";
include "test/test_unicode_normalization.grc";
include "test/test_context_lines.grc";
include "test/test_output_formats.grc";
include "test/test_word_boundary.grc";
include "test/test_zero_length_bug.grc";
include "test/test_option_based_behavior.grc";

"=== ALL TESTS COMPLETED ===\n".echo();
"All test suites have been executed successfully!\n".echo();
```

### Library Development

For library development, you can use includes for modular organization:

```grapa
/* Main library file */
include "lib/grapa/grapa.grc";

/* Custom extensions */
include "lib/custom/string_utils.grc";
include "lib/custom/math_utils.grc";
include "lib/custom/data_processing.grc";
```

### Production Deployment

For production, use pre-compiled files for faster loading:

```grapa
/* Production deployment with pre-compiled files */
include "lib/grapa/grapa.grz";
include "lib/custom/string_utils.grz";
include "lib/custom/math_utils.grz";
include "lib/custom/data_processing.grz";
```

## Compilation Timing and Execution

### Compile-Time Processing

Includes are processed **during compilation**, not at runtime:

```grapa
/* This include is processed when the script is compiled */
include "lib/grapa/grapa.grc";

/* The included code is available immediately */
result = some_function_from_included_file();
```

### Execution Tree Integration

When a file is included, its execution trees are integrated into the main script:

```grapa
/* Main script */
a = 5;

/* Include file: math_utils.grc */
/* function add(x, y) { x + y; } */

/* After inclusion, the function is available */
result = add(a, 3);  /* Result: 8 */
```

The execution tree becomes:
```grapa
@<[op,@[
  @<assign,{a,5}>,
  @<include,{"math_utils.grc"}>,
  @<call,{@<name,{add,null}>,@[a,3]}>
]],{}>;
```

## Performance Optimization Strategies

### Development Phase
```grapa
/* Use .grc files for development */
include "lib/development/debug_utils.grc";
include "lib/development/test_helpers.grc";
```

### Testing Phase
```grapa
/* Mix .grc and .grz for testing */
include "lib/production/core.grz";        /* Pre-compiled core */
include "lib/development/test_utils.grc"; /* Source for debugging */
```

### Production Phase
```grapa
/* Use .grz files for production */
include "lib/production/core.grz";
include "lib/production/utils.grz";
include "lib/production/extensions.grz";
```

## Build Process Integration

### Automated Compilation

Create a build script to compile all source files:

```grapa
/* build_library.grc */
f = $file();
f.chd("lib/source");

/* Find all .grc files */
source_files = f.ls().reduce(op(a,b){
    if (b.$KEY.right(3)=="grc") {
        a += b.$KEY;
    }
}, {});

/* Compile each file */
source_files.reduce(op(x,filename){
    output_file = filename.rtrim("c") + "z";
    $sys().compilef(filename, output_file);
    ("Compiled " + filename + " to " + output_file).echo();
}, {});
```

### Conditional Compilation

Use includes for conditional compilation:

```grapa
/* Check environment and include appropriate files */
if ($sys().getenv("DEBUG_MODE") == "true") {
    include "lib/debug/debug_utils.grc";
} else {
    include "lib/production/optimized_utils.grz";
}
```

## Best Practices

### 1. File Organization
- **Source files**: Use `.grc` extension for source code
- **Compiled files**: Use `.grz` extension for pre-compiled code
- **Clear naming**: Use descriptive names for included files

### 2. Development Workflow
- **Development**: Use `.grc` files for easy debugging and modification
- **Testing**: Mix `.grc` and `.grz` files as needed
- **Production**: Use `.grz` files for optimal performance

### 3. Build Process
- **Automate compilation**: Use build scripts to compile `.grc` to `.grz`
- **Version control**: Keep `.grc` files in version control, distribute `.grz` files
- **Dependency management**: Document include dependencies clearly

### 4. Performance Considerations
- **Large libraries**: Pre-compile frequently used libraries
- **Small utilities**: Keep as `.grc` for flexibility during development
- **Critical paths**: Use `.grz` files for performance-critical code

## Error Handling

### File Not Found
```grapa
/* Include with error handling */
try {
    include "lib/missing_file.grc";
} catch (error) {
    ("Warning: Could not include missing_file.grc: " + error).echo();
}
```

### Compilation Errors
```grapa
/* Compilation errors in included files are reported during compilation */
include "lib/syntax_error.grc";  /* Will fail during compilation */
```

## Integration with Execution Trees

The include system integrates seamlessly with Grapa's execution tree architecture:

1. **Compilation**: Included files are compiled to execution trees
2. **Integration**: Trees are merged into the main script's execution tree
3. **Optimization**: All trees are optimized together
4. **Execution**: The combined tree is executed as a single unit

This provides the benefits of modular code organization while maintaining the performance advantages of a unified execution tree.

## Conclusion

Grapa's include system provides a powerful mechanism for code modularization and performance optimization. By supporting both source and pre-compiled files, it offers flexibility for development while enabling optimal performance for production deployments.

The system's integration with the execution tree architecture ensures that included code benefits from the same optimizations as inline code, while the build process allows for efficient distribution of pre-compiled libraries.

## Related Documentation

- [Execution Trees](EXECUTION_TREES.md) - Understanding the underlying execution model
- [System Functions](sys/sys.md) - Compilation and evaluation functions
- [Base Types](BASETYPES.md) - Foundation of the execution model
- [Build Process](BUILD.md) - Building and packaging Grapa applications 

## Running Grapa Tests and Scripts

- Use **-cfile <filename>** to run a Grapa script file (e.g., .grc test):
  
  ```powershell
  .\grapa.exe -q -cfile "test/my_test.grc"
  ```
- Use **-ccmd <code>** for inline code only:
  
  ```powershell
  .\grapa.exe -q -ccmd "'Hello'.echo();"
  ```

**Do not use -ccmd for .grc files.** 