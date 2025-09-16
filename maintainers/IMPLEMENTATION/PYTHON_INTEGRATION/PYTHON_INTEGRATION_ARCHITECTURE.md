---
tags:
  - maintainer
  - implementation
  - python-integration
  - architecture
  - system-design
  - component-relationships
---

# Python Integration Architecture

This document provides comprehensive architectural overview of Grapa's Python integration system, covering the overall system design, component relationships, and integration patterns.

## Overview

**Purpose:** Comprehensive Python integration for Grapa
**Status:** Complete implementation
**Architecture:** Bidirectional integration with pybind11 framework
**Components:** Multiple interconnected systems for seamless Python/Grapa interaction

## System Architecture

### **High-Level Architecture**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Python Code   │    │   Grapa Code    │    │   C++ Engine    │
│                 │    │                 │    │                 │
│ ┌─────────────┐ │    │ ┌─────────────┐ │    │ ┌─────────────┐ │
│ │ grapapy     │ │    │ │ $py.eval()  │ │    │ │ mainpy.cpp  │ │
│ │ module      │ │◄──►│ │ $py.exec()  │ │◄──►│ │             │ │
│ └─────────────┘ │    │ └─────────────┘ │    │ └─────────────┘ │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Type Conversion Layer                        │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │   ToGrapa()     │  │   FromGrapa()   │  │   GrapaPyObject │  │
│  │  (Python→Grapa) │  │  (Grapa→Python) │  │                 │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
         │                       │                       │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Memory Management Layer                       │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │   GIL Management│  │ Reference Count │  │ Resource Cleanup│  │
│  │                 │  │                 │  │                 │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### **Component Relationships**

#### **1. Python Side Components**
- **`grapapy` module**: Python extension module
- **`grapa` class**: Main Python interface
- **`eval()` function**: Standalone evaluation
- **Type conversion**: Python object handling

#### **2. Grapa Side Components**
- **`$py` class**: Grapa interface to Python
- **`$py.eval()`**: Python expression evaluation
- **`$py.exec()`**: Python statement execution
- **Rule system**: Method resolution and execution

#### **3. C++ Engine Components**
- **`mainpy.cpp`**: Core implementation
- **`GrapaPyObject`**: Type conversion utilities
- **`GrapaStruct`**: Python interface class
- **Event handlers**: Rule execution handlers

## Detailed Component Architecture

### **1. Python Extension Module (grapapy)**

#### **Module Structure**
```
grapapy (Python Extension Module)
├── Module Initialization
│   ├── Grapa System Startup
│   ├── Rule Registration
│   └── Cleanup Registration
├── grapa Class (GrapaStruct wrapper)
│   ├── __init__(cmdstr=None)
│   ├── eval(cmdstr, paramstr="", rulestr="", profilestr="")
│   └── compile(cmdstr, rulestr="", profilestr="")
├── eval() Function (standalone)
└── __version__ Attribute
```

#### **Initialization Flow**
```cpp
PYBIND11_MODULE(grapapy, m) {
    // 1. Initialize Grapa system
    GrapaLink::Start(needExit, showConsole, showWidget, inStr, outStr, runStr);
    
    // 2. Register Python rule event
    GrapaLink::GetGrapaSystem()->mLibraryQueue.PushTail(new GrapaPyRuleEvent(GrapaCHAR("py")));
    
    // 3. Register cleanup function
    auto atexit = py::module_::import("atexit");
    atexit.attr("register")(py::cpp_function([]() { GrapaLink::Stop(); }));
    
    // 4. Define module components
    py::class_<GrapaStruct>(m, "grapa")...
    m.def("eval", &grapa_eval, ...);
}
```

### **2. Grapa Integration System**

#### **$py Class Definition**
```grapa
$global["$py"] = class {
    eval = op(script, locals={}, import="", attr="") {
        @<"py", "eval", {
            @<var, {script}>,
            @<var, {locals}>,
            @<var, {import}>,
            @<var, {attr}>
        }>();
    };
    exec = op(script, locals={}, import="", attr="") {
        @<"py", "exec", {
            @<var, {script}>,
            @<var, {locals}>,
            @<var, {import}>,
            @<var, {attr}>
        }>();
    };
};
```

#### **Rule Resolution Flow**
```
$py.eval("2 + 2")
    ↓
Rule Resolution: @<"py", "eval", {...}>
    ↓
GrapaPyRuleEvent::LoadLib("eval")
    ↓
new GrapaLibraryRulePyEvalEvent("eval")
    ↓
GrapaLibraryRulePyEvalEvent::Run()
    ↓
Python Execution: py::eval("2 + 2")
    ↓
Result Conversion: GrapaPyObject::ToGrapa()
    ↓
Grapa Result: 4
```

### **3. Type Conversion System**

#### **Conversion Architecture**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Python Types  │    │  Type Mapping   │    │   Grapa Types   │
│                 │    │                 │    │                 │
│ bool            │◄──►│ ToGrapa()       │◄──►│ BOOL            │
│ None            │    │ FromGrapa()     │    │ ERR (null)      │
│ bytes           │    │                 │    │ RAW             │
│ str             │    │                 │    │ STR             │
│ int             │    │                 │    │ INT             │
│ float           │    │                 │    │ FLOAT           │
│ tuple           │    │                 │    │ TUPLE           │
│ list            │    │                 │    │ ARRAY           │
│ dict            │    │                 │    │ LIST            │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

#### **Conversion Flow**
```cpp
// Python to Grapa
PyObject* py_obj = PyLong_FromLong(42);
GrapaRuleEvent* grapa_obj = GrapaPyObject::ToGrapa(py_obj, GrapaCHAR("value"));
// grapa_obj now contains Grapa INT with value 42

// Grapa to Python
GrapaRuleEvent* grapa_str = new GrapaRuleEvent(0, GrapaCHAR("hello"), GrapaCHAR("Hello, World!"));
py::object py_str;
GrapaPyObject::FromGrapa(vScriptExec, grapa_str, &py_str);
// py_str now contains Python string "Hello, World!"
```

### **4. Memory Management System**

#### **GIL Management**
```cpp
// Thread safety with GIL
pybind11::gil_scoped_acquire acquire;  // Acquire GIL
// ... Python operations ...
// GIL automatically released when acquire goes out of scope
```

#### **Resource Management**
```cpp
// Automatic cleanup
class GrapaStruct {
    ~GrapaStruct() { 
        mConsoleSend.Stop();  // Cleanup Grapa resources
    }
};

// Python cleanup registration
atexit.attr("register")(py::cpp_function([]() {
    GrapaLink::Stop();  // Cleanup Grapa system
}));
```

## Integration Patterns

### **1. Python → Grapa Integration**

#### **Direct Function Calls**
```python
import grapapy

# Call Grapa from Python
result = grapapy.eval("2 + 2")
print(result)  # 4

# Stateful Grapa instance
g = grapapy.grapa()
g.eval("x = 10")
result = g.eval("x * 2")
print(result)  # 20

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
- **✅ Serializable** (can be passed between calls): `$INT`, `$FLOAT`, `$STR`, `$LIST`, `$GOBJ`, `$BOOL`
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
```

#### **Data Exchange**
```python
# Pass Python data to Grapa
data = [1, 2, 3, 4, 5]
result = grapapy.eval("sum(@$ARG)", data)
print(result)  # 15

# Get Grapa data in Python
result = grapapy.eval("[i**2 for i in range(5)]")
print(result)  # [0, 1, 4, 9, 16]
```

### **2. Grapa → Python Integration**

#### **Expression Evaluation**
```grapa
/* Evaluate Python expressions */
result = $py.eval("2 + 2");
("Result: " + result).echo();  // Result: 4

/* Use Python libraries */
result = $py.eval("import math; math.sqrt(16)");
("Square root: " + result).echo();  // Square root: 4.0
```

#### **Statement Execution**
```grapa
/* Execute Python statements */
$py.exec("x = 10; y = 20; z = x + y");

/* Define Python functions */
$py.exec("def greet(name): return f'Hello, {name}!'");
result = $py.eval("greet('World')");
("Message: " + result).echo();  // Message: Hello, World!
```

### **3. Bidirectional Integration**

#### **Complex Data Exchange**
```grapa
/* Pass complex Grapa data to Python */
grapa_data = {
    "numbers": [1, 2, 3, 4, 5],
    "text": "Hello from Grapa",
    "nested": {"key": "value"}
};

result = $py.eval("""
import json
data = @$ARG
return {
    'sum': sum(data['numbers']),
    'length': len(data['text']),
    'keys': list(data['nested'].keys())
}
""", grapa_data);

("Python processed: " + result).echo();
```

## Thread Safety Architecture

### **Concurrency Model**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Thread 1      │    │   Thread 2      │    │   Thread 3      │
│                 │    │                 │    │                 │
│ Grapa Script    │    │ Grapa Script    │    │ Python Script   │
│ $py.eval()      │    │ $py.exec()      │    │ grapapy.eval()  │
│                 │    │                 │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                    GIL Management Layer                         │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │   GIL Acquire   │  │   GIL Release   │  │   GIL Contention │  │
│  │                 │  │                 │  │                 │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
         │                       │                       │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Python Interpreter                           │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │   Single Thread │  │   Execution     │  │   Memory Safety │  │
│  │   Execution     │  │   Queue         │  │                 │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### **GIL Management Strategy**
- **Acquisition**: `pybind11::gil_scoped_acquire` for Python operations
- **Release**: Automatic release when scope ends
- **Contention**: GIL ensures only one thread executes Python at a time
- **Safety**: Thread-safe memory access and resource management

## Performance Architecture

### **Performance Characteristics**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Fast Path     │    │   Type Conv.    │    │   Memory Mgmt.  │
│                 │    │                 │    │                 │
│ Direct Calls    │    │ Optimized       │    │ Efficient       │
│ Minimal Overhead│    │ Conversions     │    │ Allocation      │
│ C++ Execution   │    │ Caching         │    │ Pooling         │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Performance Optimization                      │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │   Zero-Copy     │  │   Reference     │  │   Memory        │  │
│  │   Operations    │  │   Counting      │  │   Reuse         │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### **Optimization Strategies**
- **Zero-copy operations**: Where possible, avoid data copying
- **Reference counting**: Efficient Python object management
- **Memory pooling**: Reuse memory allocations
- **Type caching**: Cache frequently used type conversions
- **GIL optimization**: Minimize GIL contention
- **Temporary namespace cleanup**: Each call creates and destroys temporary namespace for local variables
- **Variable persistence**: Local variables are lost between calls; use `$global` for persistent data

## Security Architecture

### **Security Model**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Input         │    │   Execution     │    │   Output        │
│   Validation    │    │   Sandboxing    │    │   Sanitization  │
│                 │    │                 │    │                 │
│ Script Params   │    │ Python Code     │    │ Result Data     │
│ Type Checking   │    │ Resource Limits │    │ Type Validation │
│ Size Limits     │    │ Access Control  │    │ Content Filter  │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Security Enforcement                         │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │   Parameter     │  │   Execution     │  │   Result        │  │
│  │   Validation    │  │   Isolation     │  │   Validation    │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### **Security Features**
- **Input validation**: Validate all script parameters
- **Resource limits**: Memory and execution time limits
- **Sandboxed execution**: Controlled Python execution environment
- **Access control**: Limited system and file access
- **Error isolation**: Python errors don't affect Grapa system

## Error Handling Architecture

### **Error Propagation**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Python        │    │   Conversion    │    │   Grapa         │
│   Exceptions    │    │   Layer         │    │   Errors        │
│                 │    │                 │    │                 │
│ SyntaxError     │    │ Type Errors     │    │ $ERR Objects    │
│ ValueError      │    │ Memory Errors   │    │ Error Messages  │
│ RuntimeError    │    │ Conversion      │    │ Stack Traces    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Error Handling Layer                         │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │   Exception     │  │   Error         │  │   Graceful      │  │
│  │   Capture       │  │   Conversion    │  │   Fallback      │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### **Error Handling Strategy**
- **Exception capture**: Capture Python exceptions
- **Error conversion**: Convert Python errors to Grapa errors
- **Stack trace preservation**: Preserve error context
- **Graceful fallback**: Provide meaningful error messages
- **System stability**: Ensure errors don't crash the system

## Future Architecture Enhancements

### **Planned Architectural Improvements**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Async         │    │   Parallel      │    │   Jupyter       │
│   Execution     │    │   Processing    │    │   Integration   │
│                 │    │                 │    │                 │
│ Non-blocking    │    │ Multiprocessing │    │ Notebook        │
│ Operations      │    │ Support         │    │ Support         │
│ Event Loop      │    │ Shared Memory   │    │ Magic Commands  │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Enhanced Integration                          │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │   Performance   │  │   Scalability   │  │   Usability     │  │
│  │   Improvements  │  │   Enhancements  │  │   Enhancements  │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### **Architectural Goals**
- **Async execution**: Non-blocking Python execution
- **Parallel processing**: Multiprocessing support
- **Jupyter integration**: Jupyter notebook support
- **Performance optimization**: Further performance improvements
- **Enhanced debugging**: Better debugging capabilities

## Related Documentation

### **Implementation Documentation**
- **[MAINPY_IMPLEMENTATION.md](MAINPY_IMPLEMENTATION.md)** - Main Python integration implementation
- **[GRAPAPY_IMPLEMENTATION.md](GRAPAPY_IMPLEMENTATION.md)** - GrapaPy Python extension module
- **[TYPE_MAPPING_IMPLEMENTATION.md](TYPE_MAPPING_IMPLEMENTATION.md)** - Type conversion system
- **[FOREIGN_FUNCTION_INTERFACE.md](FOREIGN_FUNCTION_INTERFACE.md)** - Python script execution

### **System Documentation**
- **[../CORE/GRAPA_COMPILATION_AND_EXECUTION.md](../CORE/GRAPA_COMPILATION_AND_EXECUTION.md)** - Grapa compilation and execution
- **[../LANGUAGE_FEATURES/RULE_SYSTEM_ARCHITECTURE.md](../LANGUAGE_FEATURES/RULE_SYSTEM_ARCHITECTURE.md)** - Rule system architecture
- **[../TYPE_SYSTEM/TYPE_SYSTEM_ARCHITECTURE.md](../TYPE_SYSTEM/TYPE_SYSTEM_ARCHITECTURE.md)** - Type system architecture

## Notes

- **Comprehensive integration**: Full bidirectional integration between Python and Grapa
- **Thread safety**: Robust thread safety with proper GIL management
- **Memory safety**: Comprehensive memory management and cleanup
- **Performance**: Optimized for minimal overhead and maximum efficiency
- **Security**: Secure execution with proper access controls
- **Extensibility**: Designed for future enhancements and improvements
- **Namespace behavior**: Each call creates temporary namespace; local variables don't persist between calls
- **Data persistence**: Use `$global` variables for data that needs to persist across multiple calls
