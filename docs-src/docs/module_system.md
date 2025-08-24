# Module System

Grapa provides a sophisticated module system that combines compile-time includes with runtime dynamic class loading and **automatic file loading**. This system is more flexible and powerful than traditional import/export mechanisms.

## Overview

Grapa's module system consists of three main components:

1. **Include System** - Compile-time file inclusion
2. **Dynamic Class Loading** - Runtime class resolution with search paths
3. **Automatic File Loading** - Automatic loading of `.grc` files for function calls
4. **Manual File Loading** - Manual file reading and execution using `$file()` commands

## Automatic File Loading

**Important**: This is **runtime loading** - files are loaded when the function or class is first called, not at compile time. This differs from the `include` command which loads files at compile time.

### Function Call Auto-Loading

When you call a function that doesn't exist in the current namespace, Grapa automatically searches for and loads the corresponding `.grc` file:

```grapa
/* File: lib/grapa/mycode.grc */
@global["mycode"] = class {
    test = op() {
        "Hello from mycode.grc!".echo();
        return "Success from mycode.grc";
    };
};

/* Usage - automatically loads mycode.grc */
x = mycode();  /* Searches for mycode.grc and loads it */
x.test();      /* Calls the test method */
```

### Class Assignment Auto-Loading

When you assign a class that doesn't exist, Grapa searches for a `.grc` file with the same name and looks for a class definition:

```grapa
/* File: myclass.grc */
@global["myclass"] = class {
    message = "Hello from myclass!";
    
    greet = op() {
        message.echo();
    };
};

/* Usage - automatically loads myclass.grc */
x = myclass();  /* Searches for myclass.grc and loads the myclass class */
x.greet();      /* Calls the greet method */
```

**Important**: The filename must match the class name. If `myclass.grc` doesn't define a class named `myclass`, the assignment will fail.

### Function Return Value Auto-Loading

If a `.grc` file doesn't define a class with the matching name, it can still be loaded as a function that returns a value:

```grapa
/* File: utils.grc */
utils = op() {
    return "Utility functions loaded";
};

/* Usage - automatically loads utils.grc */
result = utils();  /* Searches for utils.grc and calls the utils function */
result.echo();     /* Outputs: "Utility functions loaded" */
```

### Function-Only Files (Re-loading Behavior)

Files that only define functions (without defining `$global["filename"]`) will re-load every time they're called:

```grapa
/* File: $editor.grc - Example from lib/grapa/$editor.grc */
$global.createEditor = op(label) {
    /* Editor creation logic */
};

$global.newEditor = op(label) {
    /* Editor initialization logic */
};

/* No $global["$editor"] definition - will re-load every time */

/* Usage - re-loads every time */
$editor();  /* Re-loads and executes $editor.grc */
$editor();  /* Re-loads and executes $editor.grc again */
```

**Important**: To prevent re-loading, define `$global["filename"]` in your file:

```grapa
/* File: utils.grc - Cached version */
$global.utils = op() {
    return "Utility functions loaded";
};

/* Define the global to prevent re-loading */
$global["utils"] = $global.utils;

/* Usage - loads once, then cached */
result = utils();  /* Loads utils.grc and caches the function */
result = utils();  /* Uses cached version, doesn't re-load */
```

### Search Paths for Auto-Loading

The automatic file loading mechanism searches in this order:

1. **Current Working Directory** (`gSystem->mPath`) - Where you run the `grapa` command
2. **Library Directory** (`gSystem->mLibDir`) - Default: `{current_directory}/lib/grapa`
3. **Static Library** (`gSystem->mStaticLib`) - Built-in classes

### Default Library Directory Search Order

The library directory is determined in this order:

1. **`{current_working_directory}/lib/grapa`** (development/local)
2. **`/usr/lib/grapa`** (system-wide, package manager)
3. **`/usr/local/lib/grapa`** (system-wide, user-installed)
4. **`{binary_directory}/lib/grapa`** (fallback, relative to grapa executable)

> **Note**: The current implementation prioritizes development convenience by checking the current working directory first. This makes it easy for developers to place library files in their project's `lib/grapa/` directory.

## Runtime vs Compile-Time Loading

Grapa provides three different mechanisms for loading code:

### Automatic File Loading (Runtime)
- **When**: Files are loaded when functions/classes are first called
- **How**: Searches for `.grc`/`.grz` files in search paths
- **Use case**: Dynamic loading, plugins, optional features
- **Example**: `mycode()` automatically loads `mycode.grc` when called

### Include Command (Compile-Time)
- **When**: Files are loaded during compilation/parsing
- **How**: Explicitly includes files with `include "file.grc"`
- **Use case**: Core dependencies, always-required modules
- **Example**: `include "lib/core/utils.grc"` loads file during compilation

### Manual File Loading (Runtime)
- **When**: Files are loaded manually using `$file()` commands
- **How**: Read file content and execute with `op()()` or `exec()`
- **Use case**: Dynamic file loading from any location, custom loading logic
- **Example**: `$file().read("path/to/file.grc").exec()` loads and executes file content

## Include System

### Basic Include Syntax

```grapa
include "lib/grapa/grapa.grc";
include "lib/custom/string_utils.grz";
```

### File Types

#### Source Files (.grc)
```grapa
/* Development - parsed and compiled during compilation */
include "lib/custom/string_utils.grc";
```

**Characteristics:**
- Parsed and compiled during compilation
- Best for development and debugging
- Full syntax checking and optimization
- Slower compilation time

#### Pre-compiled Files (.grz)
```grapa
/* Production - loaded directly for speed */
include "lib/custom/string_utils.grz";
```

**Characteristics:**
- Loaded directly without parsing
- Best for production deployment
- Fast compilation time
- Created using `$sys().compilef()`

### Creating Pre-compiled Files

```grapa
/* Compile a source file to pre-compiled format */
$sys().compilef("lib/custom/string_utils.grc", "lib/custom/string_utils.grz");
```

## Dynamic Class Loading

When you reference an undefined class, Grapa automatically searches for and loads class definitions.

### Automatic Class Loading

```grapa
/* Class will be automatically loaded from search paths */
custom = $CUSTOM_CLASS();
result = custom.some_method();
```

### Search Path Management

#### $PATH Environment Variable

The `$PATH` variable contains a queue of search locations:

```grapa
/* Set multiple search paths */
$sys().putenv($PATH, [
    "lib/custom",           /* Custom libraries */
    "lib/extensions",       /* Extensions */
    "lib/plugins",          /* Plugins */
    "/usr/local/lib/grapa"  /* System libraries */
]);

/* Add to existing path */
current_path = $sys().getenv($PATH);
new_path = current_path + ["lib/new"];
$sys().putenv($PATH, new_path);
```

#### Search Path Modification Command

You can modify the search paths using the `$sys().putenv()` command:

```grapa
/* Add a new search path */
$sys().putenv($PATH, $sys().getenv($PATH) + ["lib/new_feature"]);

/* Replace all search paths */
$sys().putenv($PATH, ["lib/prod", "lib/stable"]);

/* Remove a specific path */
current_paths = $sys().getenv($PATH);
filtered_paths = current_paths.filter(op(path) { path != "lib/old_feature"; });
$sys().putenv($PATH, filtered_paths);
```

**Note**: The automatic file loading mechanism uses these search paths, but there's no way to specify a custom location for individual file loads. For custom file locations, use manual file loading instead.

#### $LIB Environment Variable

The `$LIB` variable points to the main library directory:

```grapa
/* Set the main library directory */
$sys().putenv($LIB, "/usr/local/lib/grapa");

/* Get current library directory */
lib_dir = $sys().getenv($LIB);
```

### Search Order

When loading a class, Grapa searches in this order:

1. **Current namespace** - Check if class already exists
2. **$PATH locations** - Search each path in the queue
3. **$LIB directory** - Main library directory
4. **Static library** - Built-in classes

## Practical Examples

### Automatic Function Loading

```grapa
/* Create a utility function */
/* File: lib/grapa/utils.grc */
@global["utils"] = class {
    format_date = op(date) {
        /* Date formatting logic */
        return date.format("YYYY-MM-DD");
    };
    
    validate_email = op(email) {
        /* Email validation logic */
        return email.grep(r"^[^@]+@[^@]+\.[^@]+$", "x").len() > 0;
    };
};

/* Usage - automatically loads utils.grc */
formatted = utils().format_date($TIME());
is_valid = utils().validate_email("user@example.com");
```

### Custom Class Library

```grapa
/* Create custom class */
/* File: lib/custom/math_utils.grc */
@global["$MATH_UTILS"] = class {
    add = op(x, y) { x + y; };
    multiply = op(x, y) { x * y; };
    factorial = op(n) { 
        if (n <= 1) 1 else n * factorial(n - 1);
    };
};
```

### Using Custom Classes

```grapa
/* Set up search path */
$sys().putenv($PATH, ["lib/custom"]);

/* Class will be automatically loaded */
math = $MATH_UTILS();
result = math.add(5, 3);  /* 8 */
fact = math.factorial(5); /* 120 */
```

### Plugin System

```grapa
/* Dynamic plugin loading */
plugin_paths = [
    "plugins/network",
    "plugins/database", 
    "plugins/ui"
];

$sys().putenv($PATH, plugin_paths);

/* Plugins are loaded on demand */
network = $NETWORK_PLUGIN();
db = $DATABASE_PLUGIN();
ui = $UI_PLUGIN();
```

### Development vs Production Setup

#### Development Environment
```grapa
/* Use .grc files for easy debugging */
$sys().putenv($PATH, [
    "lib/dev",
    "lib/custom"
]);
```

#### Production Environment
```grapa
/* Use .grz files for performance */
$sys().putenv($PATH, [
    "lib/prod/optimized",
    "lib/prod/stable"
]);
$sys().putenv($LIB, "/opt/grapa/lib");
```

## Manual File Loading

For cases where you need to load files from specific locations or implement custom loading logic, you can use manual file loading with `$file()` commands.

### Basic Manual File Loading

```grapa
/* Read and execute a file from any location */
file_content = $file().read("/path/to/custom/file.grc");
result = file_content.exec();

/* Load and execute with error handling */
load_file = op(file_path) {
    if ($file().exists(file_path)) {
        content = $file().read(file_path);
        return content.exec();
    } else {
        return {"error": "File not found: " + file_path};
    };
};

/* Usage */
result = load_file("/custom/path/utils.grc");
```

### Dynamic File Loading

```grapa
/* Load files based on configuration */
config = {
    "dev_mode": true,
    "custom_paths": ["/dev/lib", "/prod/lib"]
};

load_from_config = op() {
    if (config.dev_mode) {
        /* Load development files */
        dev_content = $file().read(config.custom_paths[0] + "/dev_utils.grc");
        return dev_content.exec();
    } else {
        /* Load production files */
        prod_content = $file().read(config.custom_paths[1] + "/prod_utils.grc");
        return prod_content.exec();
    };
};
```

### File Loading with Parameters

```grapa
/* Load and execute with parameters */
load_with_params = op(file_path, params) {
    content = $file().read(file_path);
    
    /* Create a function that accepts parameters */
    func = op()(content)();
    
    /* Call with parameters */
    return func(params);
};

/* Usage */
result = load_with_params("/path/to/script.grc", {"name": "Alice", "age": 30});
```

### Comparison of Loading Methods

| Method | When to Use | Advantages | Limitations |
|--------|-------------|------------|-------------|
| **Include** | Compile-time dependencies | Fast, always available | Fixed at compile time |
| **Automatic** | Runtime dynamic loading | Convenient, automatic discovery | Limited to search paths |
| **Manual** | Custom locations, dynamic logic | Full control, any location | Requires explicit code |

## Advanced Usage

### Dynamic Path Management

```grapa
/* Add paths dynamically */
add_search_path = op(new_path) {
    current = $sys().getenv($PATH);
    updated = current + [new_path];
    $sys().putenv($PATH, updated);
};

add_search_path("lib/new_feature");
```

### Conditional Loading

```grapa
/* Load different libraries based on environment */
if ($sys().getenv("DEBUG_MODE") == "true") {
    $sys().putenv($PATH, ["lib/debug", "lib/dev"]);
} else {
    $sys().putenv($PATH, ["lib/prod"]);
}
```

### Library Organization

```grapa
/* Organize libraries by category */
$sys().putenv($PATH, [
    "lib/core",           /* Core functionality */
    "lib/extensions",     /* Extensions */
    "lib/plugins",        /* Plugins */
    "lib/experimental"    /* Experimental features */
]);
```

## Performance Considerations

### Caching

- Classes are cached after first load
- Subsequent references use cached version
- No re-parsing or re-compilation needed

### Function Caching Behavior

**Files with `$global["filename"]` definition:**
- Loaded once and cached
- Subsequent calls use cached version
- Best performance for frequently used functions

**Files without `$global["filename"]` definition:**
- Re-loaded every time they're called
- Each call re-executes the entire file
- Suitable for initialization scripts or one-time setup

### Performance Examples

```grapa
/* Cached function (good performance) */
/* File: cached_utils.grc */
$global.cached_utils = op() { return "cached"; };
$global["cached_utils"] = $global.cached_utils;

/* Non-cached function (re-loads every time) */
/* File: uncached_utils.grc */
$global.uncached_utils = op() { return "uncached"; };
/* No $global["uncached_utils"] definition */

/* Usage */
cached_utils();  /* Loads once, then cached */
cached_utils();  /* Uses cache - fast */

uncached_utils();  /* Loads and executes file */
uncached_utils();  /* Loads and executes file again - slower */
```

### File Type Selection

- **Development**: Use `.grc` files for debugging
- **Production**: Use `.grz` files for speed
- **Mixed**: Use `.grz` for stable libraries, `.grc` for active development

### Search Path Optimization

```grapa
/* Optimize search order - most used first */
$sys().putenv($PATH, [
    "lib/frequently_used",  /* Most common libraries */
    "lib/standard",         /* Standard libraries */
    "lib/rare"              /* Rarely used libraries */
]);
```

## Error Handling

### Missing Classes

If a class cannot be found:
1. Search through all `$PATH` locations
2. Check `$LIB` directory
3. Check static library
4. Return error if not found

### File Loading Errors

- Invalid `.grz` files cause decompression errors
- Syntax errors in `.grc` files cause parsing errors
- Missing files are handled gracefully

## Integration with Include System

The include system and class loading work together:

```grapa
/* Include system - compile time */
include "lib/core/grapa.grc";

/* Class loading - runtime */
custom = $CUSTOM_CLASS();  /* Searches $PATH automatically */

/* Automatic file loading - runtime */
utils = utils();           /* Automatically loads utils.grc */
```

## Best Practices

1. **Use automatic file loading for simple utilities and functions**
2. **Use $PATH for custom libraries and plugins**
3. **Use $LIB for system-wide library directory**
4. **Use .grc files during development**
5. **Use .grz files in production**
6. **Organize libraries by category**
7. **Optimize search path order**
8. **Cache frequently used libraries**

## Comparison with Traditional Modules

Grapa's module system is more flexible than traditional import/export:

| Traditional Modules | Grapa Module System |
|-------------------|-------------------|
| Static imports | Dynamic class loading + automatic file loading |
| Fixed dependencies | Runtime path configuration |
| Compile-time resolution | Runtime resolution |
| Limited flexibility | High flexibility |
| Single search location | Multiple search paths |
| Explicit imports | Automatic discovery |

## Conclusion

Grapa's module system provides:

- **Compile-time includes** for code organization
- **Runtime class loading** with flexible search paths
- **Automatic file loading** for seamless function calls
- **Manual file loading** for custom locations and dynamic logic
- **Environment variable management** for configuration
- **Performance optimization** with pre-compiled files
- **Extensible architecture** for custom search providers

This system is more sophisticated than traditional import/export modules and provides greater flexibility for dynamic loading and configuration.

### Key Behaviors

1. **Filename Matching**: For automatic loading, the filename must match the class name (e.g., `myclass.grc` must define `@global["myclass"]`)
2. **Class vs Function**: Files can define either classes (for assignment) or functions (for function calls)
3. **Search Paths**: Automatic loading uses predefined search paths that can be modified with `$sys().putenv($PATH, ...)`
4. **Manual Control**: For files outside search paths, use `$file().read()` and manual execution
5. **Caching Behavior**: Files with `$global["filename"]` are cached; files without it re-load every time

## Related Documentation

- [Command Operators](operators/command.md) - Include syntax details
- [System Functions](sys/sys.md) - Environment variable management
- [Object Methods](type/obj_methods.md) - Class and method usage
