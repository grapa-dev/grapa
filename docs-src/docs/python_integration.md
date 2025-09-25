# Python Integration with Grapa

## Overview

Grapa provides powerful Python integration capabilities, offering a unified interface for file system operations, database management, and data processing. This integration combines Grapa's efficient storage systems with Python's rich ecosystem for data science, web development, and system administration.

## Execution Model

GrapaPy has two execution modes that affect variable persistence, and differs significantly from the CLI `-c` option:

### Direct Execution (No Parameters)
When calling `eval()` without parameters, code runs at the global level and variables persist between calls:

```python
import grapapy
g = grapapy.grapa()

# Variables persist automatically - no $global needed
g.eval("f = $file(); f.cd('project_data');")
g.eval("f.mk('database', 'COL');")  # f persists from previous call
g.eval("f.cd('database');")         # f still available
result = g.eval("f.ls();")          # Returns database contents
```

### Parameterized Execution (With Parameters)
When calling `eval()` with parameters, code runs in a local scope and variables don't persist. **Parameters are automatically available as variables:**

```python
# Parameters are automatically available as variables
g.eval("result = x + y;", {'x': 5, 'y': 3})
result = g.eval("result;")  # Returns {"error":-1} - variable not found (local scope)

# Parameters persist and are available in the local scope
g.eval("sum = x + y;", {'x': 10, 'y': 20})  # x and y are automatically available
g.eval("product = x * y;", {'x': 10, 'y': 20})  # x and y still available

# To persist variables with parameters, use $global
g.eval("$global.persistent_var = 10;", {'x': 5})
result = g.eval("persistent_var;")  # Returns 10
```

### Comparison with CLI `-c` Option

**GrapaPy `.eval()` (with persistence):**
```python
g = grapapy.grapa()
g.eval("f = $file(); f.cd('data');")     # f is created
g.eval("f.mk('users', 'ROW');")          # f persists, can use it
g.eval("f.cd('users');")                 # f still available
g.eval("f.setfield('user1', 'name', 'Alice');")  # f persists
```

**CLI `-c` (no persistence):**
```bash
./grapa -c "f = $file(); f.cd('data');"           # f is created
./grapa -c "f.mk('users', 'ROW');"                # f is gone, error!
./grapa -c "f = $file(); f.cd('data/users');"     # Must recreate f
./grapa -c "f.setfield('user1', 'name', 'Alice');" # f is gone, error!
```

### Parameter Handling Differences

**GrapaPy `.eval()` (automatic parameter variables):**
```python
g = grapapy.grapa()
# Parameters are automatically available as variables
g.eval("result = x + y;", {'x': 5, 'y': 3})  # x and y are automatically available
g.eval("name = user_name;", {'user_name': 'Alice'})  # user_name is automatically available
```

**CLI `-c` (manual parameter access):**
```bash
# Must access parameters through $sys().getenv($ARGV)
./grapa -c "args = \$sys().getenv(\$ARGV); x = args.get(0).int(); y = args.get(1).int(); result = x + y; result.echo();" 5 3
./grapa -c "args = \$sys().getenv(\$ARGV); name = args.get(0); ('Hello ' + name).echo();" Alice
```

### Practical Example: Parameter Handling

**GrapaPy (automatic parameter variables):**
```python
import grapapy

g = grapapy.grapa()

# Simple calculation with automatic parameter access
result = g.eval("x + y", {'x': 10, 'y': 20})
print(result)  # 30

# Complex operation with multiple parameters
g.eval("f = $file(); f.mk('users', 'ROW');", {'db_name': 'users'})
g.eval("f.setfield('user1', 'name', user_name);", {'user_name': 'Alice'})
g.eval("f.setfield('user1', 'age', user_age);", {'user_age': 25})
name = g.eval("f.getfield('user1', 'name');")
print(name)  # Alice
```

**CLI equivalent (manual parameter access):**
```bash
# Simple calculation - must parse arguments manually
./grapa -c "args = \$sys().getenv(\$ARGV); x = args.get(0).int(); y = args.get(1).int(); (x + y).echo();" 10 20

# Complex operation - much more verbose
./grapa -c "f = \$file(); f.mk('users', 'ROW');"
./grapa -c "args = \$sys().getenv(\$ARGV); f = \$file(); f.cd('users'); f.setfield('user1', 'name', args.get(0));" Alice
./grapa -c "args = \$sys().getenv(\$ARGV); f = \$file(); f.cd('users'); f.setfield('user1', 'age', args.get(0));" 25
./grapa -c "f = \$file(); f.cd('users'); f.getfield('user1', 'name').echo();"
```

### Key Points
- **GrapaPy persistence**: Variables like `f = $file()` persist across `.eval()` calls
- **CLI limitation**: Each `-c` call is isolated - no variable persistence
- **Parameter handling**: GrapaPy parameters are automatically available as variables; CLI requires `$sys().getenv($ARGV)`
- **Top-level execution**: Variables persist automatically (no `$global` needed)
- **Parameterized execution**: Variables are local and don't persist (need `$global` for persistence)
- **`$file()` objects**: Persist naturally in GrapaPy, need recreation in CLI
- **Scope behavior**: Parameters create a local scope that shadows global variables

## Key Benefits

### 1. **Unified File System and Database Access**
Grapa provides a single API that seamlessly navigates between file systems and databases:

```python
import grapapy

g = grapapy.grapa()
# Navigate from file system into database seamlessly
g.eval('f = $file(); f.cd("project_data")')  # f persists across calls
g.eval('f.cd("users")')                      # f still available - could be database table
g.eval('f.cd("profile")')                    # f still available - could be nested structure
```

**Advantages:**
- **Single API** for both file system and database operations
- **Transparent navigation** between different data sources
- **Consistent interface** regardless of underlying storage

### 2. **Advanced Database Capabilities**

#### **Column Store for Analytics**
Optimized for analytical workloads and data science:

```python
# Python data science workflows with column store
g = grapapy.grapa()
g.eval('f = $file(); f.mk("analytics_db", "COL")')  # f persists across calls
g.eval('f.cd("analytics_db")')                      # f still available

# Define schema optimized for analytics - f persists throughout
g.eval('f.mkfield("timestamp", "TIME", "FIX", 8)')
g.eval('f.mkfield("user_id", "INT", "FIX", 4)')
g.eval('f.mkfield("metric_value", "FLOAT", "FIX", 8)')
g.eval('f.mkfield("category", "STR", "VAR")')
```

# Efficient column scans for aggregations
# Python can process the results for statistical analysis
```

**Benefits:**
- **Native column store** for analytical workloads
- **Better performance** than row stores for aggregations
- **Memory efficient** for large datasets

#### **Row Store for Transactional Data**
Optimized for transactional workloads and frequent updates:

```python
# Transactional data with row store
g = grapapy.grapa()
g.eval('f = $file(); f.mk("user_sessions", "ROW")')
g.eval('f.cd("user_sessions")')
```

# Fast point queries and updates
g.eval('f.setfield("session_123", "user_id", "user_456")')
g.eval('f.setfield("session_123", "login_time", "2024-01-15")')
g.eval('f.setfield("session_123", "status", "active")')
```

### 3. **File Processing Capabilities**

#### **Large File Management**
Built-in support for handling large files efficiently:

```python
# Python can orchestrate large file operations
g = grapapy.grapa()

# Split large files for parallel processing
result = g.eval('f = $file(); f.split(8, "large_dataset.csv", "chunks", "", "csv")')
# Python can then process each chunk in parallel

# Get file metadata efficiently
file_info = g.eval('f = $file(); f.info("large_file.txt")')
if file_info["$TYPE"] == "FILE" and file_info["$BYTES"] > 1000000:
    # Handle large files appropriately
    pass
```

#### **Unified Path System**
Seamless navigation across complex data structures:

```python
# Navigate complex data structures
g = grapapy.grapa()
g.eval('f = $file(); f.chd("/project/data")')  # Set home directory
g.eval('f.cd("database")')                     # Enter database
g.eval('f.cd("users")')                        # Navigate to users table
g.eval('f.cd("..")')                           # Go back up
g.eval('f.cd("../logs")')                      # Navigate to logs directory
```

## Use Cases

### 1. **Data Science and Analytics**

#### **Scientific Computing**
```python
# Scientific computing with Grapa
g = grapapy.grapa()
g.eval('f = $file(); f.mk("experiment_data", "COL")')
g.eval('f.cd("experiment_data")')

# Store experimental results efficiently
for experiment in experiments:
    g.eval(f'f.setfield("exp_{experiment.id}", "time", "{experiment.timestamp}")')
    g.eval(f'f.setfield("exp_{experiment.id}", "data", "{experiment.results}")')
    g.eval(f'f.setfield("exp_{experiment.id}", "config", "{experiment.parameters}")')

# Fast column scans for statistical analysis
```

#### **Machine Learning Workflows**
```python
# ML data management with Grapa
g = grapapy.grapa()
g.eval('f = $file(); f.mk("ml_dataset", "COL")')
g.eval('f.cd("ml_dataset")')

# Store features and labels
for sample in training_data:
    g.eval(f'f.setfield("sample_{sample.id}", "features", "{sample.features}")')
    g.eval(f'f.setfield("sample_{sample.id}", "label", "{sample.label}")')
    g.eval(f'f.setfield("sample_{sample.id}", "meta", "{sample.metadata}")')

# Efficient feature extraction for model training
```

### 2. **Web Application Development**

#### **Backend Data Management**
```python
# Web app with Grapa storage
g = grapapy.grapa()
g.eval('f = $file(); f.chd("/app/data")')

# User management
g.eval('f.cd("users")')
g.eval(f'f.setfield("{user_id}", "profile", "{user_data}")')
g.eval(f'f.setfield("{user_id}", "session", "{session_data}")')

# Content management
g.eval('f.cd("../content")')
g.eval(f'f.setfield("{content_id}", "body", "{content_data}")')
g.eval(f'f.setfield("{content_id}", "meta", "{metadata}")')
```

#### **API Development**
```python
# FastAPI with Grapa backend
from fastapi import FastAPI
import grapapy

app = FastAPI()
g = grapapy.grapa()

@app.get("/users/{user_id}")
async def get_user(user_id: str):
    g.eval('f = $file(); f.cd("users")')
    profile = g.eval(f'f.getfield("{user_id}", "profile")')
    return {"user_id": user_id, "profile": profile}
```

### 3. **System Administration**

#### **Log File Management**
```python
# System admin tools with Grapa
g = grapapy.grapa()

# Log file management
g.eval('f = $file(); f.cd("/var/log")')
log_files = g.eval('f.ls()')
for log_file in log_files:
    info = g.eval(f'f.info("{log_file}")')
    if info["$BYTES"] > 1000000:  # 1MB
        # Split large log files
        g.eval(f'f.split(4, "{log_file}", "log_chunks", "\\n", "")')
```

#### **Configuration Management**
```python
# Configuration management
g = grapapy.grapa()
g.eval('f = $file(); f.cd("/etc/configs")')
configs = g.eval('f.ls()')
for config in configs:
    content = g.eval(f'f.get("{config}")')
    if validate_config(content):
        deploy_config(config, content)
```

### 4. **Data Pipeline Integration**

#### **ETL Workflows**
```python
# Extract-Transform-Load with Grapa
g = grapapy.grapa()

# Extract: Read from various sources
g.eval('f = $file(); f.cd("source_data")')
raw_data = g.eval('f.get("input.csv")')

# Transform: Process in Python
processed_data = python_transform_function(raw_data)

# Load: Store in Grapa database
g.eval('f.cd("../processed_data")')
g.eval(f'f.setfield("processed_batch_001", "content", "{processed_data}")')
```

#### **Data Validation**
```python
# Validate data before processing
g = grapapy.grapa()
files = g.eval('f = $file(); f.ls("input_directory")')
for file in files:
    info = g.eval(f'f.info("{file}")')
    if info["$TYPE"] == "FILE":
        content = g.eval(f'f.get("{file}")')
        if validate_data_format(content):
            # Process valid files
            pass
```

## Performance Advantages

### **Memory Efficiency**
- **Column store**: Only loads relevant columns for analysis
- **Fragmented storage**: Efficient for sparse data
- **Block-based I/O**: Memory-efficient file operations

### **Scalability**
- **Large file handling**: Built-in splitting and chunking
- **Database optimization**: Different storage types for different use cases
- **Cross-platform**: Consistent performance across systems

## Bidirectional Communication: Callbacks with `$py().eval()`

**Important**: The `$py().eval()` callback mechanism is **only available in the Python extension** (`grapapy`), not in the standalone CLI.

**Parameter Access Change**: When accessing parameters in Grapa scripts called from Python, use `$local.'param_name'` instead of `param_name`. This is because parameters are now string labels rather than ID labels.

### Overview

GrapaPy provides powerful bidirectional communication through the `$py()` system object, allowing Grapa code to call back into Python functions. This enables seamless integration between Grapa's performance and Python's rich ecosystem.

### Basic Callback Usage

```python
import grapapy

# Define a Python function
def python_function(x):
    return x * 2 + 10

# Create GrapaPy instance
g = grapapy.grapa()

# Register the Python function as a Grapa callback
g.eval("""
    $this.double_and_add = op(n=0) {
        $py().eval('python_function(n)', {'n': $local.'n'});
    };
""")

# Use the callback from Grapa
result = g.eval("double_and_add(5);")
print(result)  # 20 (5 * 2 + 10)
```

### Advanced Callback Patterns

#### Complex Parameter Passing

```python
import grapapy

def process_data(data, multiplier, offset):
    if isinstance(data, list):
        return [x * multiplier + offset for x in data]
    return data * multiplier + offset

g = grapapy.grapa()

# Register with complex parameter handling
g.eval("""
    $this.process = op(data, mult=2, off=0) {
        $local.locals = {'data': $local.'data', 'multiplier': $local.'mult', 'offset': $local.'off'};
        $py().eval('process_data(data, multiplier, offset)', $local.locals);
    };
""")

# Test with different data types
result1 = g.eval("process(5, 3, 10);")        # 25
result2 = g.eval("process([1,2,3], 2, 5);")   # [7, 9, 11]
```

#### Rule-Based Callbacks

```python
import grapapy

def classify_number(n):
    if n >= 10:
        return "high"
    elif n >= 5:
        return "medium"
    else:
        return "low"

def calculate_result(a, b):
    if a >= 10:
        return a / b
    else:
        return a * b

g = grapapy.grapa()

# Create a rule that uses callbacks
g.eval("""
    $this.classify_rule = rule 
          $INT $INT {op(a:$1,b:$2) {$py().eval('calculate_result(v1,v2)', {"v1":a,"v2":b});}}
        | $INT      {op(a:$1)      {$py().eval('classify_number(v)',     {"v":a});}}
        ;
""")

# Use the rule
result1 = g.eval("$sys().eval('4', {}, 'classify_rule');")     # "low"
result2 = g.eval("$sys().eval('4 6', {}, 'classify_rule');")   # 24 (4 * 6)
result3 = g.eval("$sys().eval('12 3', {}, 'classify_rule');")  # 4.0 (12 / 3)
```

### Type Conversion

The callback system automatically handles type conversion between Grapa and Python:

| Grapa Type | Python Type | Notes |
|------------|-------------|-------|
| `$INT` | `int` | Automatic conversion |
| `$FLOAT` | `float` | Automatic conversion |
| `$STR` | `str` | UTF-8 string conversion |
| `$BOOL` | `bool` | True/false conversion |
| `$LIST` | `list` | Element-by-element conversion |
| `$GOBJ` | `dict` | Key-value conversion |
| `$TUPLE` | `tuple` | Element-by-element conversion |
| `$RAW` | `bytes` | Binary data conversion |
| `null` | `None` | Null value conversion |

### Error Handling

```python
import grapapy

def safe_divide(a, b):
    try:
        return a / b
    except ZeroDivisionError:
        return "Error: Division by zero"
    except Exception as e:
        return f"Error: {str(e)}"

g = grapapy.grapa()

g.eval("""
    $this.safe_div = op(a=0, b=1) {
        $py().eval('safe_divide(a, b)', {'a': $local.'a', 'b': $local.'b'});
    };
""")

result1 = g.eval("safe_div(10, 2);")  # 5.0
result2 = g.eval("safe_div(10, 0);")  # "Error: Division by zero"
```

### Compiled Code with Callbacks

```python
import grapapy

def python_processor(x):
    return x ** 2 + 2 * x + 1

g = grapapy.grapa()

# Compile Grapa code that uses callbacks
compiled = g.compile("python_processor(n);", "", "")

# Use the compiled code multiple times
result1 = g.eval(compiled, {'n': 5})  # 36
result2 = g.eval(compiled, {'n': 3})  # 16
```

### Best Practices

1. **Parameter Naming**: Use clear parameter names in the `locals` dictionary for better readability
2. **Error Handling**: Always handle exceptions in Python callback functions
3. **Type Safety**: Be aware of type conversions between Grapa and Python
4. **Performance**: Callbacks have overhead - use for complex operations, not simple calculations
5. **State Management**: Python functions can maintain state between calls if needed

### Limitations

- **Python Extension Only**: `$py().eval()` is not available in the standalone CLI
- **GIL Management**: The system automatically handles Python's Global Interpreter Lock
- **Type Conversion Overhead**: Complex objects may have conversion overhead
- **Error Propagation**: Python exceptions are converted to Grapa error objects

## Integration with Python Ecosystem

### **Text Processing and Search**
GrapaPy provides direct access to Grapa's powerful Unicode-aware grep functionality:

```python
import grapapy

# Basic text search
text = "Hello world\nGoodbye world\nPython programming"
matches = grapapy.grep(text, "world")
print(matches)  # ['Hello world', 'Goodbye world']

# Match-only option (extract just the matched text)
matches = grapapy.grep(text, "world", "o")
print(matches)  # ['world', 'world']

# Case-insensitive search
matches = grapapy.grep(text, "python", "i")
print(matches)  # ['Python programming']

# Advanced regex with Unicode support
matches = grapapy.grep("café naïve résumé", r"\w+é\w+", "o")
print(matches)  # ['café', 'naïve', 'résumé']

# Class-based interface for stateful operations
g = grapapy.grapa()
matches = g.grep(text, "world", "o")
print(matches)  # ['world', 'world']
```

**Advanced Grep Features:**
- **Unicode-aware**: Full Unicode normalization and case folding support:
- **Regex support**: Complete PCRE2 regex engine with advanced features
- **Multiple output formats**: Line-based, match-only, JSON output
- **Performance optimized**: Direct C++ implementation for maximum speed
- **Thread-safe**: Can be used safely in multi-threaded Python applications
- **Direct integration**: Bypasses Python's GIL for maximum performance

### **Data Science Libraries**
```python
import pandas as pd
import numpy as np
import grapapy

# Process Grapa data with pandas
g = grapapy.grapa()
g.eval('f = $file(); f.cd("analytics_data")')

# Extract data for pandas processing
data = []
records = g.eval('f.ls()')
for record in records:
    row = {
        'id': record,
        'value': g.eval(f'f.getfield("{record}", "value")'),
        'category': g.eval(f'f.getfield("{record}", "category")')
    }
    data.append(row)

df = pd.DataFrame(data)
# Perform pandas operations
```

### **Machine Learning Frameworks**
```python
import sklearn
import grapapy

# Feature storage for ML
g = grapapy.grapa()
g.eval('f = $file(); f.mk("ml_features", "COL")')
g.eval('f.cd("ml_features")')

# Store features efficiently
for sample_id, features in feature_data.items():
    g.eval(f'f.setfield("{sample_id}", "features", "{features.tobytes()}")')
    g.eval(f'f.setfield("{sample_id}", "label", "{labels[sample_id]}")')
```

### **Web Frameworks**
```python
# Django with Grapa
from django.http import JsonResponse
import grapapy

def user_profile(request, user_id):
    g = grapapy.grapa()
    g.eval('f = $file(); f.cd("users")')
    
    profile = g.eval(f'f.getfield("{user_id}", "profile")')
    return JsonResponse({"profile": profile})
```

## Best Practices

### **Database Design**
1. **Choose appropriate storage types**:
   - Use **COL** (column store) for analytical workloads
   - Use **ROW** (row store) for transactional data
   - Use **FIX** fields for small, frequently accessed data
   - Use **VAR** fields for variable-length data

2. **Optimize field types**:
   - Match data types to storage needs
   - Use appropriate field sizes
   - Consider growth patterns

### **File Management**
1. **Large file handling**:
   - Use `split()` for files larger than memory
   - Process chunks in parallel
   - Monitor file sizes with `info()`

2. **Path management**:
   - Use `chd()` to set project home directories
   - Use relative paths with `cd()` for navigation
   - Leverage the unified path system

### **Performance Optimization**
1. **Memory management**:
   - Use column store for analytical queries
   - Process data in chunks
   - Monitor memory usage

2. **I/O optimization**:
   - Use appropriate storage types
   - Batch operations when possible
   - Leverage Grapa's efficient I/O

## Conclusion

Grapa's Python integration provides a powerful combination of:
- **Unified data access** across file systems and databases
- **Variable persistence** across `.eval()` calls (unlike CLI `-c` option)
- **Automatic parameter handling** - parameters are automatically available as variables
- **Performance optimization** through specialized storage types
- **Scalability** for large datasets and complex workflows
- **Integration** with Python's rich ecosystem

This makes Grapa particularly valuable for Python applications that need to handle diverse data types, large datasets, or complex data workflows. The unified path system, variable persistence, automatic parameter handling, and efficient storage options provide significant advantages over traditional file system + database combinations and even over using the CLI `-c` option for complex operations. 

---

## See also
- [API Reference](api_reference.md)
- [Language Reference](syntax/basic_syntax.md)
- [Use Cases](use_cases/)
- [Examples](examples/)
- [Advanced Topics](advanced_topics.md) 