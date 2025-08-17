---
tags:
  - maintainer
  - implementation
  - python-integration
  - grapapy
  - pybind11
  - python-extension
---

# GrapaPy Implementation

This document provides comprehensive implementation details for the GrapaPy Python extension module, which allows Python code to execute Grapa scripts and interact with the Grapa system.

## Overview

**Module Name:** `grapapy`
**Purpose:** Python extension module for Grapa integration
**Status:** Complete implementation
**Source:** `source/mainpy.cpp` (PYBIND11_MODULE section)

## Architecture

### **Module Structure**
```
grapapy (Python Extension Module)
├── grapa class (GrapaStruct wrapper)
│   ├── __init__(cmdstr=None)
│   ├── eval(cmdstr, paramstr="", rulestr="", profilestr="")
│   └── compile(cmdstr, rulestr="", profilestr="")
├── eval() function (standalone)
└── __version__ attribute
```

### **Key Components**
- **`PYBIND11_MODULE(grapapy, m)`** - Module definition and initialization
- **`GrapaStruct` class** - Main Python interface for Grapa operations
- **`grapa_eval()` function** - Standalone evaluation function
- **GIL Management** - Thread safety with Global Interpreter Lock

## Implementation Details

### **1. Module Initialization**

#### **PYBIND11_MODULE Definition**
```cpp
PYBIND11_MODULE(grapapy, m)
{
    // Initialize Grapa system
    GrapaCHAR inStr, outStr, runStr;
    bool needExit = false, showConsole = false, showWidget = false;
    GrapaCHAR s = GrapaLink::Start(needExit, showConsole, showWidget, inStr, outStr, runStr);
    
    // Register Python rule event
    GrapaLink::GetGrapaSystem()->mLibraryQueue.PushTail(new GrapaPyRuleEvent(GrapaCHAR("py")));

    // Register cleanup function
    auto atexit = py::module_::import("atexit");
    atexit.attr("register")(py::cpp_function([]() {
        GrapaLink::Stop();
    }));
}
```

#### **Module Documentation**
```cpp
m.doc() = R"pbdoc(
    GrapaPy extension
    -----------------------

    .. currentmodule:: grapapy

    .. autosummary::
       :toctree: _generate

       new - create an instance (state maintained between calls)
       eval - eval a string, return a string

       Pass in 2'nd parameter as string or bytes, available using @$ARG in the script. 
)pbdoc";
```

### **2. GrapaStruct Class Definition**

#### **Class Constructor**
```cpp
py::class_<GrapaStruct>(m, "grapa")
    .def(py::init<py::object>(), py::arg("s") = py::none())
```

**Constructor Parameters:**
- **`s`** (optional): Initial Grapa script to execute during initialization
  - **`None`**: No initial script execution
  - **`str`**: Execute string as Grapa script
  - **`bytes`**: Execute bytes as Grapa script

#### **eval Method**
```cpp
.def("eval", static_cast<py::object(GrapaStruct::*)(py::object, py::object, std::string, std::string)>(&GrapaStruct::eval), 
     "", py::arg("s"), py::arg("a") = "", py::arg("r") = "", py::arg("p") = "", 
     pybind11::call_guard<py::gil_scoped_release>())
```

**Method Parameters:**
- **`s`**: Grapa script to evaluate (string or bytes)
- **`a`** (optional): Arguments passed to script (available as `@$ARG`)
- **`r`** (optional): Rule name for script execution
- **`p`** (optional): Profile name for script execution

**Return Value:**
- **Python object**: Result of Grapa script execution (converted from Grapa type)

#### **compile Method**
```cpp
.def("compile", static_cast<py::object(GrapaStruct::*)(py::object, std::string, std::string)>(&GrapaStruct::compile), 
     "", py::arg("s"), py::arg("r") = "", py::arg("p") = "", 
     pybind11::call_guard<py::gil_scoped_release>())
```

**Method Parameters:**
- **`s`**: Grapa script to compile (string)
- **`r`** (optional): Rule name for compilation
- **`p`** (optional): Profile name for compilation

**Return Value:**
- **Python object**: Compiled Grapa script as compressed bytecode

### **3. Standalone eval Function**

#### **Function Definition**
```cpp
m.def("eval", &grapa_eval, R"pbdoc(
    Evaluate a Grapa script
)pbdoc",
    py::arg("s"), py::arg("a") = "", py::arg("r") = "", py::arg("p") = "", 
    pybind11::call_guard<py::gil_scoped_release>());
```

**Function Parameters:**
- **`s`**: Grapa script to evaluate (string or bytes)
- **`a`** (optional): Arguments passed to script
- **`r`** (optional): Rule name for script execution
- **`p`** (optional): Profile name for script execution

**Return Value:**
- **Python object**: Result of Grapa script execution

**Implementation:**
```cpp
py::object grapa_eval(py::object cmdstr, py::object paramstr, std::string rulestr, std::string profilestr)
{
    GrapaStruct* gs = new GrapaStruct(py::none());
    py::object o = gs->eval(cmdstr, paramstr, rulestr, profilestr);
    delete gs;
    return o;
}
```

### **4. Module Attributes**

#### **Version Information**
```cpp
m.attr("__version__") = "0.1.47";
```

## Usage Patterns

### **1. Standalone Evaluation**
```python
import grapapy

# Evaluate simple expression
result = grapapy.eval("2 + 2")
print(result)  # 4

# Evaluate with arguments
result = grapapy.eval("$ARG[0] + $ARG[1]", [10, 20])
print(result)  # 30
```

### **2. Stateful Grapa Instance**
```python
import grapapy

# Create Grapa instance with state
g = grapapy.grapa()

# Execute multiple scripts with shared state
g.eval("x = 10")
g.eval("y = 20")
result = g.eval("x + y")
print(result)  # 30

# Use arguments
result = g.eval("$ARG[0] * $ARG[1]", [5, 6])
print(result)  # 30
```

**⚠️ Important Note on Variable Persistence:**
Each `grapapy` call creates a temporary namespace for parameter passing. Local variables created during script execution are lost when the call returns. For data that needs to persist across calls, use `$global` variables:

```python
# ❌ Local variables are lost between calls
g.eval("x = 10")  # x is created in temporary namespace
result = g.eval("x + 5")  # Error: x not found

# ✅ Use $global for persistent data
g.eval("$global['x'] = 10")  # Store in global namespace
result = g.eval("x + 5")  # Success: x found in global namespace
print(result)  # 15

# ✅ Direct global assignment also works
g.eval("$global.x = 20")  # Alternative syntax
result = g.eval("x * 2")  # Success: x found in global namespace
print(result)  # 40
```

**🗄️ Database and Network Object Persistence:**
Some Grapa objects like `$file`, `$net`, and database connections **cannot be serialized** and must be stored in `$global` to persist across calls:

```python
# ❌ Database connection will be lost
g.eval("db = $file('database.db')")  # db created in temporary namespace
result = g.eval("db.table('users').select()")  # Error: db not found

# ✅ Store database connection in global namespace
g.eval("$global['db'] = $file('database.db')")  # Persistent connection
result = g.eval("db.table('users').select()")  # Success: db found in global
print(result)  # Database query results

# ✅ Network connections also need global persistence
g.eval("$global['net'] = $net()")  # Persistent network object
g.eval("net.connect('example.com', 80)")  # Connection persists across calls
```

**📊 Serializable vs Non-Serializable Types:**
- **✅ Serializable** (can be passed between calls): `$INT`, `$FLOAT`, `$STR`, `$ARRAY`, `$LIST`, `$BOOL`
- **❌ Non-Serializable** (must use `$global`): `$file`, `$net`, `$TABLE`, database connections, network connections

**⚡ Performance Optimization for Large Values:**
For very large `$INT` or `$FLOAT` values (e.g., 5000+ bits), consider these strategies:

```python
import grapapy
g = grapapy.grapa()

# ❌ Inefficient: Large value converted to Python native type
large_int = g.eval("2**5000")  # 5000-bit integer converted to Python
result = g.eval("large_int * 2")  # Converted back to Grapa, then back to Python

# ✅ Efficient: Keep large values in Grapa $global
g.eval("$global['large_int'] = 2**5000")  # Stays in Grapa memory
result = g.eval("large_int * 2")  # No conversion overhead
print(result)  # Only convert final result to Python

# ✅ Alternative: Use $RAW for efficient transfer
g.eval("$global['large_int_raw'] = (2**5000).raw()")  # Store as $RAW
raw_data = g.eval("large_int_raw")  # Transfer as bytes (efficient)
# Reconstruct in Python if needed: int.from_bytes(raw_data, 'big')
```

### **3. Script Compilation**
```python
import grapapy

# Compile script to bytecode
g = grapapy.grapa()
compiled = g.compile("2 + 2")
print(type(compiled))  # <class 'bytes'>

# Execute compiled script
result = g.eval(compiled)
print(result)  # 4
```

### **4. Initialization with Script**
```python
import grapapy

# Initialize with initial script
g = grapapy.grapa("x = 100; y = 200")

# Use variables from initialization
result = g.eval("x + y")
print(result)  # 300
```

## Thread Safety

### **GIL Management**
- **`pybind11::call_guard<py::gil_scoped_release>()`**: Releases GIL during Grapa execution
- **`pybind11::gil_scoped_acquire acquire`**: Acquires GIL for Python operations
- **Thread-safe execution**: Multiple Python threads can use GrapaPy concurrently

### **Resource Management**
- **Automatic cleanup**: `atexit` registration ensures proper Grapa system shutdown
- **Memory management**: Proper cleanup of GrapaStruct instances
- **Exception safety**: GIL management ensures proper exception handling

## Error Handling

### **Python Exceptions**
- **Type conversion errors**: Handled gracefully with fallback to string representation
- **Grapa errors**: Converted to Python exceptions with meaningful messages
- **Memory errors**: Proper cleanup on exceptions

### **Grapa Errors**
- **Script errors**: Returned as Python objects with error information
- **System errors**: Handled with proper error propagation
- **Resource errors**: Cleanup performed even on errors

## Performance Characteristics

### **Memory Management**
- **Efficient type conversion**: Minimal memory allocations during conversion
- **Reference counting**: Proper Python reference counting for objects
- **Memory pooling**: Efficient memory usage for repeated operations
- **Temporary namespaces**: Each call creates and destroys temporary namespace for parameter passing
- **Variable persistence**: Local variables are lost between calls; use `$global` for persistent data

### **Execution Performance**
- **Fast execution**: Direct Grapa script execution without Python overhead
- **Compiled scripts**: Bytecode compilation for repeated execution
- **Stateful instances**: Efficient state management between calls

### **Threading Performance**
- **Concurrent execution**: Multiple threads can execute Grapa scripts simultaneously
- **GIL optimization**: GIL released during Grapa execution for better concurrency
- **Resource sharing**: Efficient sharing of Grapa system resources

## Integration Points

### **With Python Ecosystem**
- **NumPy compatibility**: Array operations with NumPy arrays
- **Standard library**: Full access to Python standard library
- **Third-party packages**: Integration with any Python package

### **With Grapa System**
- **Type system**: Full integration with Grapa type system
- **Rule system**: Access to Grapa rule system and BNF grammar
- **Database system**: Full access to GrapaDB capabilities
- **Networking**: Access to Grapa networking features

## Security Considerations

### **Script Execution**
- **Sandboxed execution**: Grapa scripts run in controlled environment
- **Resource limits**: Memory and execution time limits
- **Input validation**: Proper validation of script inputs
- **Error isolation**: Script errors don't affect Python environment

### **Memory Safety**
- **Reference counting**: Proper Python reference counting
- **Memory bounds**: Safe memory access during type conversion
- **Exception safety**: Proper cleanup on exceptions
- **Thread safety**: Safe concurrent access

## Future Enhancements

### **Planned Features**
- **Async execution**: Non-blocking script execution
- **Parallel processing**: Multiprocessing support
- **Jupyter integration**: Jupyter notebook support
- **Debugging support**: Enhanced debugging capabilities

### **Performance Improvements**
- **Caching system**: Script compilation caching
- **Memory optimization**: Further memory usage optimization
- **Type optimization**: Optimized type conversion paths
- **Concurrency improvements**: Enhanced threading support

## Related Documentation

### **Implementation Documentation**
- **[MAINPY_IMPLEMENTATION.md](MAINPY_IMPLEMENTATION.md)** - Main Python integration implementation
- **[TYPE_MAPPING_IMPLEMENTATION.md](TYPE_MAPPING_IMPLEMENTATION.md)** - Type conversion system
- **[FOREIGN_FUNCTION_INTERFACE.md](FOREIGN_FUNCTION_INTERFACE.md)** - Python script execution

### **User Documentation**
- **[docs-src/docs/python_integration.md](../../../docs-src/docs/python_integration.md)** - User-facing Python API
- **[docs-src/docs/python_use_cases.md](../../../docs-src/docs/python_use_cases.md)** - Python integration examples

## Notes

- **Version**: Current version is 0.1.47
- **Thread Safety**: All operations are thread-safe with proper GIL management
- **Memory Management**: Automatic cleanup of Grapa resources
- **Error Handling**: Comprehensive error propagation between Python and Grapa
- **Performance**: Optimized for minimal overhead in script execution
- **Namespace Behavior**: Each call creates temporary namespace; local variables don't persist between calls
- **Data Persistence**: Use `$global` variables for data that needs to persist across multiple calls
