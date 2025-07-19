# Python Integration with Grapa

> **Tip:** Use the tabs below to switch between CLI and Python examples throughout this documentation.

## Overview

Grapa provides powerful Python integration capabilities through the GrapaPy extension, offering a unified interface for file system operations, database management, data processing, and advanced pattern matching. This integration combines Grapa's efficient storage systems with Python's rich ecosystem for data science, web development, and system administration.

## Installation

### Platform Support
GrapaPy supports the following platforms:
- **win-amd64** - Windows Intel 64bit
- **mac-amd64** - Mac Intel 64bit  
- **mac-arm64** - Mac Apple 64bit
- **linux-amd64** - Ubuntu 22.04 (and compatible with C++23) Intel 64bit
- **linux-arm64** - Ubuntu 24.04 (and compatible with C++23) Apple 64bit

### Installation Commands

=== "Windows"
    ```bash
    pip uninstall grapapy
    pip install grapapy
    ```

=== "macOS"
    ```bash
    pip3 uninstall grapapy
    pip3 install grapapy
    ```

=== "Linux"
    ```bash
    sudo apt update
    sudo apt install -y python3-pip
    sudo apt install -y x11-apps
    sudo apt install -y libx11-dev
    sudo apt install -y libxcursor-dev
    sudo apt install -y libxft-dev
    sudo apt install -y libxext-dev
    sudo apt install -y libxinerama-dev
    pip3 uninstall grapapy
    pip3 install grapapy
    ```

=== "Amazon Linux"
    ```bash
    dnf update -y
    dnf install -y libX*
    dnf install -y python3-devel
    python3 -m ensurepip --upgrade
    python3 -m pip install --upgrade pip setuptools
    pip3 install --user --upgrade packaging
    dnf install -y perl
    dnf install -y tar
    pip3 uninstall grapapy
    pip3 install grapapy
    ```

### Virtual Environment (Linux)
If pip3 install is being blocked, use a virtual environment:
```bash
sudo apt install -y python3-venv
python3 -m venv ~/.venvs/grapa-env
source ~/.venvs/grapa-env/bin/activate
pip3 install grapapy
```

## Basic Usage

=== "CLI"
    ```grapa
    /* Basic Grapa usage */
    x = 3.45;
    y = 4.32;
    result = x**y;
    result.echo();
    /* Output: 210.5612696743043090739172698565538965143180116634 */
    ```

=== "Python"
    ```python
    import grapapy
    xy = grapapy.grapa()
    
    # Execute simple Grapa code
    result = xy.eval("x = 3.45; y = 4.32; x**y;")
    print(result)  # 210.5612696743043090739172698565538965143180116634
    
    # Execute complex operations
    result = xy.eval("$file().pwd()")
    print(result)  # Current working directory
    ```

## Type Conversion

### Python ↔ Grapa Type Mapping

| Python | Grapa | Notes |
|--------|-------|-------|
| str | $STR | |
| bytes | $RAW | |
| int | $INT | Grapa $INT is unlimited precision |
| float | $FLOAT | Grapa $FLOAT is unlimited precision |
| list | $ARRAY | |
| dict | $LIST | In Grapa, a $LIST is a named version of $ARRAY |
| tuple | $TUPLE | In Grapa, a $TUPLE is the same as an $ARRAY |
| bool | $BOOL | |
| None | null | |

### Grapa → Python Conversion
GrapaPy converts the following to str when passing back to Python:

| Grapa | Notes |
|-------|-------|
| $ERR | Implemented as $LIST, returned on error |
| $XML, $EL, $TAG | Optionally convert to $LIST before returning to Python |
| $TIME | Supports unlimited years, high granularity |
| $VECTOR | Translation not yet implemented |
| $OP | Core Grapa language binding to internal libraries |
| $CODE | Core Grapa language supporting list of $OP values |
| $TABLE | Database type, no easy conversion |
| $WIDGET | Not yet supported in GrapaPy (based on FLTK) |
| $CLASS, $OBJ | |
| $RULE | Global variables that can be set, updated, etc. |
| $TOKEN | Controls how raw data is converted to tokens |
| $SYM, $SYSSYM, $SYSSTR, $SYSINT | |

### Type Conversion Best Practices

=== "CLI"
    ```grapa
    /* Convert to appropriate types */
    string_result = some_operation().str();
    int_result = some_operation().int();
    float_result = some_operation().float();
    ```

=== "Python"
    ```python
    # Convert to appropriate Python types
    string_result = xy.eval('some_operation().str()')
    int_result = xy.eval('some_operation().int()')
    float_result = xy.eval('some_operation().float()')
    
    # Common patterns
    content = xy.eval('fs.get("file.txt").str()')
    name = xy.eval('table.get("key", "field").str()')
    age = xy.eval('table.get("key", "field").int()')
    value = xy.eval('table.get("key", "field").float()')
    ```

## Namespace Behavior

### Local vs Global Namespaces

GrapaPy maintains two separate namespaces:

1. **Local Namespace**: Cleared between each `eval()` call
2. **Global Namespace**: Persists across multiple `eval()` call

### The `$global` Pattern

For objects that need to persist across multiple calls, use the `$global` namespace:

=== "CLI"
    ```grapa
    /* Global namespace usage */
    $global.my_table = $file().table("ROW");
    $global.my_file = $file();
    
    /* Subsequent calls use variable name directly */
    my_table.mkfield("name", "STR", "VAR");
    my_file.set("test.txt", "Hello World");
    ```

=== "Python"
    ```python
    # First assignment - use $global
    xy.eval('$global.my_table = $file().table("ROW")')
    xy.eval('$global.my_file = $file()')
    
    # Subsequent calls - use variable name directly
    xy.eval('my_table.mkfield("name", "STR", "VAR")')
    xy.eval('my_file.set("test.txt", "Hello World")')
    ```

### Why `$global` is Needed

Without `$global`, objects created in one call are lost in the next:

=== "CLI"
    ```grapa
    /* ❌ This will fail - object lost between calls */
    table = $file().table("ROW");
    table.mkfield("name", "STR");  /* Error: table not found */
    
    /* ✅ This works - object persists in global namespace */
    $global.table = $file().table("ROW");
    table.mkfield("name", "STR");  /* Success */
    ```

=== "Python"
    ```python
    # ❌ This will fail - object lost between calls
    xy.eval('table = $file().table("ROW")')
    xy.eval('table.mkfield("name", "STR")')  # Error: table not found
    
    # ✅ This works - object persists in global namespace
    xy.eval('$global.table = $file().table("ROW")')
    xy.eval('table.mkfield("name", "STR")')  # Success
    ```

## File Operations

### File System Access

=== "CLI"
    ```grapa
    /* File system operations */
    fs = $file();
    fs.set("test.txt", "Hello World");
    content = fs.get("test.txt").str();
    fs.rm("test.txt");
    ```

=== "Python"
    ```python
    # Create file object in global namespace
    xy.eval('$global.fs = $file()')
    
    # Basic operations
    xy.eval('fs.set("test.txt", "Hello World")')
    content = xy.eval('fs.get("test.txt").str()')
    xy.eval('fs.rm("test.txt")')
    ```

### Directory Operations

=== "CLI"
    ```grapa
    /* Directory operations */
    fs.mk("test_dir");
    fs.cd("test_dir");
    files = fs.ls();
    fs.cd("..");
    fs.rm("test_dir");
    ```

=== "Python"
    ```python
    # Create and navigate directories
    xy.eval('fs.mk("test_dir")')
    xy.eval('fs.cd("test_dir")')
    files = xy.eval('fs.ls()')
    xy.eval('fs.cd("..")')
    xy.eval('fs.rm("test_dir")')
    ```

## Table Operations

### Creating Tables

=== "CLI"
    ```grapa
    /* Create table in global namespace */
    $global.table = $file().table("ROW");
    
    /* Create fields with correct syntax */
    table.mkfield("name", "STR", "VAR");
    table.mkfield("age", "INT");  /* Note: no explicit mode for INT */
    table.mkfield("salary", "FLOAT", "FIX", 8);
    ```

=== "Python"
    ```python
    # Create table in global namespace
    xy.eval('$global.table = $file().table("ROW")')
    
    # Create fields with correct syntax
    xy.eval('table.mkfield("name", "STR", "VAR")')
    xy.eval('table.mkfield("age", "INT")')  # Note: no explicit mode for INT
    xy.eval('table.mkfield("salary", "FLOAT", "FIX", 8)')
    ```

### Working with Data

=== "CLI"
    ```grapa
    /* Insert data */
    table.set("user1", "John Doe", "name");
    table.set("user1", 30, "age");
    table.set("user1", 75000.50, "salary");
    
    /* Retrieve data with type conversion */
    name = table.get("user1", "name").str();
    age = table.get("user1", "age").int();
    salary = table.get("user1", "salary").float();
    ```

=== "Python"
    ```python
    # Insert data
    xy.eval('table.set("user1", "John Doe", "name")')
    xy.eval('table.set("user1", 30, "age")')
    xy.eval('table.set("user1", 75000.50, "salary")')
    
    # Retrieve data with type conversion
    name = xy.eval('table.get("user1", "name").str()')
    age = xy.eval('table.get("user1", "age").int()')
    salary = xy.eval('table.get("user1", "salary").float()')
    ```

## Advanced Database Capabilities

### Column Store for Analytics
Optimized for analytical workloads and data science:

=== "CLI"
    ```grapa
    /* Python data science workflows with column store */
    f = $file();
    f.mk("analytics_db", "COL");  /* Column store for analytical queries */
    f.cd("analytics_db");
    
    /* Define schema optimized for analytics */
    f.mkfield("timestamp", "TIME", "FIX", 8);
    f.mkfield("user_id", "INT", "FIX", 4);
    f.mkfield("metric_value", "FLOAT", "FIX", 8);
    f.mkfield("category", "STR", "VAR");
    ```

=== "Python"
    ```python
    # Python data science workflows with column store
    f = grapa.file()
    f.mk("analytics_db", "COL")  # Column store for analytical queries
    f.cd("analytics_db")
    
    # Define schema optimized for analytics
    f.mkfield("timestamp", "TIME", "FIX", 8)
    f.mkfield("user_id", "INT", "FIX", 4)
    f.mkfield("metric_value", "FLOAT", "FIX", 8)
    f.mkfield("category", "STR", "VAR")
    ```

### Row Store for Transactional Data
Optimized for transactional workloads and frequent updates:

=== "CLI"
    ```grapa
    /* Transactional data with row store */
    f.mk("user_sessions", "ROW");
    f.cd("user_sessions");
    
    /* Fast point queries and updates */
    f.set("session_123", "user_456", "user_id");
    f.set("session_123", "2024-01-15", "login_time");
    f.set("session_123", "active", "status");
    ```

=== "Python"
    ```python
    # Transactional data with row store
    f.mk("user_sessions", "ROW")
    f.cd("user_sessions")
    
    # Fast point queries and updates
    f.set("session_123", "user_456", "user_id")
    f.set("session_123", "2024-01-15", "login_time")
    f.set("session_123", "active", "status")
    ```

## Grep Examples

### Method 1: Embedded Parameters
Pass parameters directly in the Grapa string:

=== "CLI"
    ```grapa
    /* Basic grep with embedded string */
    "apple 123 pear 456\nbanana 789".grep(r"\d+","o");
    /* Result: ["123", "456", "789"] */
    
    /* With all parameters embedded */
    r"data1\x00data2\x00data3".grep(r"data\d+", "o", r"\x00", "NONE", "BINARY");
    /* Result: ["data1", "data2", "data3"] */
    ```

=== "Python"
    ```python
    import grapapy
    xy = grapapy.grapa()
    
    # Basic grep with embedded string
    xy.eval('"apple 123 pear 456\nbanana 789".grep(r"\\d+","o");')
    # Result: ['123', '456', '789']
    
    # With all parameters embedded
    xy.eval('r"data1\\x00data2\\x00data3".grep(r"data\\d+", "o", r"\\x00", "NONE", "BINARY")')
    # Result: ['data1', 'data2', 'data3']
    ```

### Method 2: Parameter Variables
Pass parameters as Python variables to be used in the Grapa string:

=== "CLI"
    ```grapa
    /* Basic grep with parameter variables */
    input.grep(pattern,options);
    /* Result: ["123", "456", "789"] */
    
    /* With all parameters as variables */
    input.grep(pattern,options,delimiter,normalization,mode);
    /* Result: ["data1", "data2", "data3"] */
    ```

=== "Python"
    ```python
    # Basic grep with parameter variables
    xy.eval("input.grep(pattern,options);", {
        "input": "apple 123 pear 456\nbanana 789",
        "pattern": "\\d+",
        "options": "o"
    })
    # Result: ['123', '456', '789']
    
    # With all parameters as variables
    xy.eval("input.grep(pattern,options,delimiter,normalization,mode);", {
        "input": "data1\\x00data2\\x00data3",
        "pattern": "data\\d+",
        "options": "o",
        "delimiter": "\\x00",
        "normalization": "NONE",
        "mode": "BINARY"
    })
    # Result: ['data1', 'data2', 'data3']
    ```

### Unicode Examples

=== "CLI"
    ```grapa
    /* Unicode characters */
    "Привет мир".grep("мир");
    /* Result: ["Привет мир"] */
    
    /* Diacritic-insensitive matching */
    "café résumé naïve".grep("cafe", "d");
    /* Result: ["café résumé naïve"] */
    
    /* Case-insensitive Unicode */
    "Café RÉSUMÉ Naïve".grep("café", "i");
    /* Result: ["Café RÉSUMÉ Naïve"] */
    
    /* Unicode properties */
    "Hello 世界 123 €".grep(r"\p{L}+", "o");
    /* Result: ["Hello", "世界"] */
    ```

=== "Python"
    ```python
    # Unicode characters
    xy.eval('"Привет мир".grep("мир")')
    # Result: ['Привет мир']
    
    # Diacritic-insensitive matching
    xy.eval('"café résumé naïve".grep("cafe", "d")')
    # Result: ['café résumé naïve']
    
    # Case-insensitive Unicode
    xy.eval('"Café RÉSUMÉ Naïve".grep("café", "i")')
    # Result: ['Café RÉSUMÉ Naïve']
    
    # Unicode properties
    xy.eval('"Hello 世界 123 €".grep(r"\\p{L}+", "o")')
    # Result: ['Hello', '世界']
    ```

### Performance Examples

=== "CLI"
    ```grapa
    /* Large input performance comparison */
    large_text = "..."; /* 50MB of text data */
    
    /* Single-threaded (like Python re module) */
    result = input.grep(pattern, options, delimiter, normalization, mode, 1);
    /* Takes ~9.6 seconds */
    
    /* Parallel processing (massive speedup) */
    result = input.grep(pattern, options, delimiter, normalization, mode, 16);
    /* Takes ~0.85 seconds (11.3x speedup!) */
    ```

=== "Python"
    ```python
    # Large input performance comparison
    large_text = "..." # 50MB of text data
    
    # Single-threaded (like Python re module)
    result = xy.eval("input.grep(pattern, options, delimiter, normalization, mode, num_workers);", {
        "input": large_text,
        "pattern": "test",
        "options": "n",
        "num_workers": 1  # Single thread
    })
    # Takes ~9.6 seconds
    
    # Parallel processing (massive speedup)
    result = xy.eval("input.grep(pattern, options, delimiter, normalization, mode, num_workers);", {
        "input": large_text,
        "pattern": "test", 
        "options": "n",
        "num_workers": 16  # 16 threads
    })
    # Takes ~0.85 seconds (11.3x speedup!)
    ```

## Use Cases

### 1. Data Science and Analytics

#### Scientific Computing
=== "CLI"
    ```grapa
    /* Scientific computing with Grapa */
    f = $file();
    f.mk("experiment_data", "COL");
    f.cd("experiment_data");
    
    /* Store experimental results efficiently */
    f.set("exp_123", experiment.timestamp, "time");
    f.set("exp_123", experiment.results, "data");
    f.set("exp_123", experiment.parameters, "config");
    ```

=== "Python"
    ```python
    # Scientific computing with Grapa
    f = grapa.file()
    f.mk("experiment_data", "COL")
    f.cd("experiment_data")
    
    # Store experimental results efficiently
    for experiment in experiments:
        f.set(f"exp_{experiment.id}", experiment.timestamp, "time")
        f.set(f"exp_{experiment.id}", experiment.results, "data")
        f.set(f"exp_{experiment.id}", experiment.parameters, "config")
    ```

### 2. Web Application Development

#### Backend Data Management
=== "CLI"
    ```grapa
    /* Web app with Grapa storage */
    f = $file();
    f.chd("/app/data");
    
    /* User management */
    f.cd("users");
    f.set(user_id, user_data, "profile");
    f.set(user_id, session_data, "session");
    
    /* Content management */
    f.cd("../content");
    f.set(content_id, content_data, "body");
    f.set(content_id, metadata, "meta");
    ```

=== "Python"
    ```python
    # Web app with Grapa storage
    f = grapa.file()
    f.chd("/app/data")
    
    # User management
    f.cd("users")
    f.set(user_id, user_data, "profile")
    f.set(user_id, session_data, "session")
    
    # Content management
    f.cd("../content")
    f.set(content_id, content_data, "body")
    f.set(content_id, metadata, "meta")
    ```

### 3. System Administration

#### Log File Management
=== "CLI"
    ```grapa
    /* System admin tools with Grapa */
    f = $file();
    
    /* Log file management */
    f.cd("/var/log");
    log_files = f.ls();
    for (log_file in log_files) {
        info = f.info(log_file);
        if (info["$BYTES"] > 1000000) { /* 1MB */
            /* Split large log files */
            f.split(4, log_file, "log_chunks", "\n", "");
        };
    };
    ```

=== "Python"
    ```python
    # System admin tools with Grapa
    f = grapa.file()
    
    # Log file management
    f.cd("/var/log")
    log_files = f.ls()
    for log_file in log_files:
        info = f.info(log_file)
        if info["$BYTES"] > 1000000:  # 1MB
            # Split large log files
            f.split(4, log_file, "log_chunks", "\n", "")
    ```

## Best Practices

### 1. Use Global Namespace for Persistent Objects

=== "CLI"
    ```grapa
    /* ✅ Good: Objects persist across calls */
    $global.table = $file().table("ROW");
    table.mkfield("name", "STR");
    
    /* ❌ Bad: Objects lost between calls */
    table = $file().table("ROW");
    table.mkfield("name", "STR");  /* Will fail */
    ```

=== "Python"
    ```python
    # ✅ Good: Objects persist across calls
    xy.eval('$global.table = $file().table("ROW")')
    xy.eval('table.mkfield("name", "STR")')
    
    # ❌ Bad: Objects lost between calls
    xy.eval('table = $file().table("ROW")')
    xy.eval('table.mkfield("name", "STR")')  # Will fail
    ```

### 2. Convert Binary Data Appropriately

=== "CLI"
    ```grapa
    /* ✅ Good: Convert to appropriate types */
    name = table.get("user1", "name").str();
    age = table.get("user1", "age").int();
    
    /* ❌ Bad: Use raw binary data */
    name = table.get("user1", "name");  /* Binary data */
    ```

=== "Python"
    ```python
    # ✅ Good: Convert to Python types
    name = xy.eval('table.get("user1", "name").str()')
    age = xy.eval('table.get("user1", "age").int()')
    
    # ❌ Bad: Use raw binary data
    name = xy.eval('table.get("user1", "name")')  # Binary data
    ```

### 3. Use Correct Field Creation Syntax

=== "CLI"
    ```grapa
    /* ✅ Good: Correct integer field creation */
    table.mkfield("age", "INT");
    
    /* ❌ Bad: May create string fields instead */
    table.mkfield("age", "INT", "FIX", 4);
    ```

=== "Python"
    ```python
    # ✅ Good: Correct integer field creation
    xy.eval('table.mkfield("age", "INT")')
    
    # ❌ Bad: May create string fields instead
    xy.eval('table.mkfield("age", "INT", "FIX", 4)')
    ```

### 4. Error Handling

=== "CLI"
    ```grapa
    /* Error handling */
    result = some_operation();
    if (result.type() == $ERR) {
        "Operation failed".echo();
    } else {
        ("Success: " + result.str()).echo();
    };
    ```

=== "Python"
    ```python
    try:
        result = xy.eval('some_operation()')
        if result == {"error":-1}:
            print("Operation failed")
        else:
            print("Success:", result)
    except Exception as e:
        print("Exception:", e)
    ```

## Quickstart for Python Users

> **Essentials:**
> - Always use `$global` for objects that need to persist across eval calls (file handles, tables, etc.).
> - Use `.str()`, `.int()`, `.float()` for type conversion of results.
> - Check for `$ERR` return values—no try/catch, handle errors explicitly.
> - Use `[]` for list/array access, not `.get()`.

## Jupyter Notebook Example

For a comprehensive Jupyter notebook example, see [grapapy_test.ipynb](../grapapy_test.ipynb).

## Next Steps

- Learn about [Testing](TESTING.md) your Grapa code
- Check out the [Grep functionality](grep.md) for advanced pattern matching
- Review the [Syntax Quick Reference](syntax/basic_syntax.md) for more syntax rules and tips
- Explore [Migration Guides](migrations/) for language-specific guidance 