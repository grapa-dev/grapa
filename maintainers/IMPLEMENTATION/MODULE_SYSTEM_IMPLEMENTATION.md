---
tags:
  - maintainer
  - lowlevel
  - implementation
  - modules
  - path
  - lib
  - class-loading
---

# Grapa Module System Implementation

## Overview

Grapa's module system is more sophisticated than it initially appears. It consists of two main components:

1. **Include System**: Compile-time file inclusion with `.grc` and `.grz` support
2. **Dynamic Class Loading**: Runtime class resolution with search path management

## Core Components

### 1. Include System

The include system works at **compile time** and is defined in the BNF grammar:

```grapa
| $SYSID("INCLUDE") <$comp> {@<include,{$2}>}
| include <$comp> {@<include,{$2}>}
```

**Key Points:**
- Creates execution tree nodes `@<include,{filepath}>`
- Processed during compilation, not runtime
- Supports both `.grc` (source) and `.grz` (pre-compiled) files
- Integrated into the main execution tree

### 2. Dynamic Class Loading System

When you reference an undefined class (e.g., `x = xyz()`), Grapa automatically searches for class definitions using a sophisticated search mechanism.

## Search Path Management

### $PATH Environment Variable

The `$PATH` variable is managed through `gSystem->mPath` and contains a queue of search locations:

```cpp
// From GrapaLink.cpp - Initialization
if (gSystem->mPath) gSystem->mPath->CLEAR();
if (!gSystem->mPath) gSystem->mPath = new GrapaRuleQueue();
gSystem->mPath->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR()));
```

**Setting $PATH via $sys().putenv():**
```grapa
/* Set search paths */
$sys().putenv($PATH, ["lib/custom", "lib/extensions", "lib/plugins"]);

/* Add to existing path */
current_path = $sys().getenv($PATH);
new_path = current_path + ["lib/new"];
$sys().putenv($PATH, new_path);
```

### $LIB Environment Variable

The `$LIB` variable points to the main library directory (`gSystem->mLibDir`):

```cpp
// From GrapaLibRule.cpp - putenv handling
else if (r1.vVal->mValue.Cmp("$LIB") == 0 || (r1.vVal->mValue.mToken == GrapaTokenType::SYSID && r1.vVal->mValue.Cmp("LIB") == 0))
{
    err = 0;
    gSystem->mLibDir.FROM(r2.vVal? r2.vVal->mValue:GrapaCHAR());
}
```

**Setting $LIB:**
```grapa
$sys().putenv($LIB, "/usr/local/lib/grapa");
```

## Class Loading Process

### GetClass() Method

When a class is referenced but not found, `GrapaScriptState::GetClass()` is called:

```cpp
GrapaRuleEvent* GrapaScriptState::GetClass(GrapaNames* pNameSpace, const GrapaCHAR &name)
{
    // 1. Check if class already exists in namespace
    GrapaRuleEvent* result = vScriptExec->vScriptState->SearchVariable(pNameSpace, name);
    if (result && !result->mNull && result->mValue.mToken == GrapaTokenType::CLASS)
    {
        return(result);
    }
    
    // 2. Search for class definition files
    GrapaCHAR fNameOp(name);
    fNameOp.Append(".grz");  // Try pre-compiled first
    GrapaCHAR fNameTxt(name);
    fNameTxt.Append(".grc"); // Then try source
    
    // 3. Search through $PATH locations
    GrapaRuleEvent* e = (GrapaRuleEvent*)gSystem->mPath?gSystem->mPath->Head():NULL;
    while (e)
    {
        // Search each path location
        if (e->vDatabase)
        {
            e->vDatabase->FieldGet(fNameOp, GrapaCHAR(), setValue);
            if (setValue.mLength == 0)
                e->vDatabase->FieldGet(fNameTxt, GrapaCHAR(), setValue);
        }
        if (setValue.mLength)
            break;
        e = e->Next();
    }
    
    // 4. Fallback to $LIB directory
    if (setValue.mLength == 0 && gSystem->mLibDir.mLength)
    {
        GrapaLocalDatabase gd;
        gd.mHomeDir = gSystem->mLibDir;
        gd.FieldGet(fNameOp, GrapaCHAR(), setValue);
        if (setValue.mLength == 0)
            gd.FieldGet(fNameTxt, GrapaCHAR(), setValue);
    }
    
    // 5. Fallback to static library
    if (setValue.mLength == 0 && gSystem->mStaticLib && gSystem->mStaticLib->mCount)
    {
        s64 idx;
        GrapaRuleEvent * ev = gSystem->mStaticLib->Search(fNameOp,idx);
        if (ev)
            setValue.FROM(ev->mValue);
        if (setValue.mLength == 0)
        {
            ev = gSystem->mStaticLib->Search(fNameTxt,idx);
            if (ev)
                setValue.FROM(ev->mValue);
        }
    }
    
    // 6. Load and compile the class
    if (setValue.mLength > 0)
    {
        // Handle .grz files (pre-compiled)
        if ((setValue.mLength >= 2 && setValue.mBytes[0] == 0 && setValue.mBytes[1] == 0) || 
            (setValue.mLength > 4 && setValue.mBytes[0] == 'G' && setValue.mBytes[1] == 'R' && 
             setValue.mBytes[2] == 'Z' && (setValue.mBytes[3] & 0x80) == 0))
        {
            GrapaBYTE expanded;
            GrapaCompress::Expand(setValue, expanded);
            // Load pre-compiled class
        }
        else
        {
            // Parse and compile .grc file
        }
    }
}
```

## Search Order

The class loading system searches in this order:

1. **Current namespace** - Check if class already exists
2. **$PATH locations** - Search each path in the queue
3. **$LIB directory** - Main library directory
4. **Static library** - Built-in classes

### Path Search Details

Each path in `$PATH` can be:
- **String**: Directory path (e.g., `"lib/custom"`)
- **Object**: Database object with custom search logic

```cpp
switch (e->mValue.mToken)
{
case GrapaTokenType::STR:
    if (e->vDatabase == NULL)
    {
        e->vDatabase = new GrapaLocalDatabase(this);
        e->vDatabase->mHomeDir = e->mValue;
    }
    break;
case GrapaTokenType::OBJ:
    if (e->vDatabase == NULL)
        e->vDatabase = new GrapaLocalDatabase(this);
    break;
}
```

## File Type Support

### .grc Files (Source)
- Parsed and compiled during class loading
- Full BNF grammar parsing
- Optimized execution tree generation
- Best for development and debugging

### .grz Files (Pre-compiled)
- Loaded directly without parsing
- Compressed execution tree format
- Fast loading for production
- Created using `$sys().compilef()`

## Environment Variable Management

### Setting Search Paths

```grapa
/* Set custom search paths */
$sys().putenv($PATH, [
    "lib/custom",           /* Custom libraries */
    "lib/extensions",       /* Extensions */
    "lib/plugins",          /* Plugins */
    "/usr/local/lib/grapa"  /* System libraries */
]);

/* Set library directory */
$sys().putenv($LIB, "/usr/local/lib/grapa");
```

### Getting Current Settings

```grapa
/* Get current search paths */
current_path = $sys().getenv($PATH);

/* Get library directory */
lib_dir = $sys().getenv($LIB);
```

## Practical Examples

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

## Performance Considerations

### Development vs Production

**Development:**
```grapa
/* Use .grc files for easy debugging */
$sys().putenv($PATH, ["lib/dev"]);
```

**Production:**
```grapa
/* Use .grz files for performance */
$sys().putenv($PATH, ["lib/prod"]);
```

### Caching

- Classes are cached after first load
- Subsequent references use cached version
- No re-parsing or re-compilation needed

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
```

## Advanced Usage

### Custom Search Providers

```grapa
/* Create custom search provider */
custom_provider = {
    search: op(filename) {
        /* Custom search logic */
        if (filename == "special.grc") {
            return "custom implementation";
        }
        return null;
    }
};

/* Add to search path */
$sys().putenv($PATH, [custom_provider, "lib/default"]);
```

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

## Conclusion

Grapa's module system provides:

1. **Compile-time includes** for code organization
2. **Runtime class loading** with flexible search paths
3. **Environment variable management** for configuration
4. **Performance optimization** with pre-compiled files
5. **Extensible architecture** for custom search providers

This system is more sophisticated than traditional import/export modules and provides greater flexibility for dynamic loading and configuration.

## Related Documentation

- [Include System](INCLUDE_SYSTEM.md) - Detailed include system documentation
- [Execution Trees](EXECUTION_TREES.md) - Understanding the underlying execution model
- [System Functions](SYSTEM_FUNCTIONS.md) - Environment variable management
- [Base Types](BASETYPES.md) - Foundation of the execution model
