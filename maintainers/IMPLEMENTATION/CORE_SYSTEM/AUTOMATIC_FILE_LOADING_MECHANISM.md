# Automatic File Loading Mechanism

## Overview

Grapa includes a sophisticated automatic file loading mechanism that allows functions and classes to be automatically loaded from `.grc` and `.grz` files without explicit import statements. This mechanism works for both function calls and class instantiation.

**Important**: This is **runtime loading** - files are loaded when the function or class is first called, not at compile time. This differs from the `include` command which loads files at compile time.

## How It Works

### Function Call Flow

When you call a function like `mycode()`, the following process occurs:

1. **Function Resolution**: The call goes through `ProcessPlan` → `LoadLib` → `ItemSearchCall`
2. **Variable Search**: First, `SearchVariable` is called to look for `mycode` in the current namespace
3. **Class Loading Fallback**: If not found, `GetClass("mycode")` is called
4. **File Search**: `GetClass` searches for files in the following order:
   - `gSystem->mPath` (current working directory)
   - `gSystem->mLibDir` (library directory, typically `lib/grapa/`)
   - `gSystem->mStaticLib` (static library directory)
5. **File Loading**: If `mycode.grc` or `mycode.grz` is found, it's loaded and parsed
6. **Class Return**: The loaded class is returned and can be instantiated

### Class Instantiation Flow

When you instantiate a class like `mycode = $class()`, the process is similar:

1. **Class Resolution**: `GetClass("mycode")` is called directly
2. **File Search**: Same search process as function calls
3. **File Loading**: The `.grc` file is loaded and parsed
4. **Class Return**: The class definition is returned for instantiation

## Code Implementation

### Key Functions

- **`ProcessPlan`** (`source/grapa/GrapaState.cpp:4418`): Main execution engine
- **`LoadLib`** (`source/grapa/GrapaState.cpp:4395`): Library loading mechanism
- **`ItemSearchCall`** (`source/grapa/GrapaLibRule.cpp:6223`): Function call resolution
- **`GetClass`** (`source/grapa/GrapaState.cpp:2971`): Class loading and file search

### Critical Code Path

```cpp
// In ItemSearchCall (GrapaLibRule.cpp:6286)
if (pCmd->mValue.mBytes && pCmd->mValue.mLength)
{
    GrapaCHAR name;
    if ((pCmd->mValue.mToken == GrapaTokenType::SYSID || pCmd->mValue.mToken == GrapaTokenType::SYSSTR) && pCmd->mValue.mBytes && pCmd->mValue.mBytes[0] != '$')
        name.FROM("$");
    name.Append(pCmd->mValue);
    o = vScriptExec->vScriptState->GetClass(pNameSpace, name);  // <-- This triggers file loading
    if (o && o->mValue.mToken == GrapaTokenType::OP)
    {
        // Execute the loaded function/class
        if (pParam && pParam->vQueue)
            result = vScriptExec->ProcessPlan(pNameSpace, o, (GrapaRuleEvent*)pParam->vQueue->Head(), pParam->vQueue->mCount);
        else
            result = vScriptExec->ProcessPlan(pNameSpace, o);
        break;
    }
}
```

## File Search Locations

The mechanism searches for files in these directories (in order):

1. **Current Working Directory** (`gSystem->mPath`) - Where you run the `grapa` command
2. **Library Directory** (`gSystem->mLibDir`) - Determined by priority:
   - `{current_working_directory}/lib/grapa` (development/local)
   - `/usr/lib/grapa` (system-wide, package manager)
   - `/usr/local/lib/grapa` (system-wide, user-installed)
   - `{binary_directory}/lib/grapa` (fallback, relative to grapa executable)
3. **Static Library Directory** (`gSystem->mStaticLib`) - Built-in compressed library files

## File Extensions

The system looks for files with these extensions:
- `.grc` - Grapa source files
- `.grz` - Grapa compiled/compressed files

## Usage Examples

### Function Call Example

```grapa
/* File: lib/grapa/mycode.grc */
@global["mycode"] = class {
    test = op() {
        "Hello from mycode.grc!".echo();
        return "Success from mycode.grc";
    };
};

/* Usage in REPL or script */
x = mycode();  /* Automatically loads mycode.grc */
x.test();      /* Calls the test method */
```

### Class Instantiation Example

```grapa
/* File: lib/grapa/myclass.grc */
@global["myclass"] = class {
    constructor = op() {
        "Constructor called".echo();
    };
    method = op() {
        return "Method called";
    };
};

/* Usage */
obj = myclass();  /* Automatically loads myclass.grc and instantiates */
obj.method();     /* Calls the method */
```

## Benefits

1. **No Explicit Imports**: Functions and classes are automatically available
2. **Modular Design**: Code can be organized into separate files
3. **Dynamic Loading**: Files are loaded only when needed
4. **Backward Compatibility**: Existing code continues to work
5. **Flexible Organization**: Files can be placed in multiple search paths

## Limitations

1. **File Naming**: File names must match the function/class name exactly
2. **Search Paths**: Limited to predefined search directories
3. **Performance**: File I/O occurs on first use
4. **Error Handling**: Missing files result in errors rather than graceful fallbacks

## Implementation Details

### File Loading Process

1. **Path Construction**: Builds search paths from system configuration
2. **File Existence Check**: Checks if `.grc` or `.grz` files exist
3. **File Parsing**: Parses the Grapa source code
4. **Class Registration**: Registers the loaded class in the system
5. **Return**: Returns the class definition for use

### Error Handling

- If no file is found, `GetClass` returns `NULL`
- This results in a function call error or class instantiation failure
- No automatic fallback to other naming conventions

## Future Enhancements

Potential improvements to consider:

1. **Multiple File Extensions**: Support for additional file types
2. **Subdirectory Support**: Recursive search in subdirectories
3. **Caching**: Cache loaded files to improve performance
4. **Error Recovery**: Graceful handling of missing or corrupted files
5. **Namespace Support**: Support for namespaced file organization

## Related Documentation

- [Grapa Rule Event Implementation](GRAPA_RULE_EVENT_MVAR_IMPLEMENTATION.md)
- [Property Assignment Hack Implementation](PROPERTY_ASSIGNMENT_HACK_IMPLEMENTATION.md)
- [Core System Overview](../CORE_SYSTEM_OVERVIEW.md)
