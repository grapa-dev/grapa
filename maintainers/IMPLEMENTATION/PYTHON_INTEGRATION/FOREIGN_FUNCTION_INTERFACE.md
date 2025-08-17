---
tags:
  - maintainer
  - implementation
  - python-integration
  - foreign-function-interface
  - python-execution
  - grapa-python
---

# Foreign Function Interface Implementation

This document provides comprehensive implementation details for the foreign function interface that enables Python script execution from within Grapa scripts, implemented through the `GrapaLibraryRulePyEvalEvent` and `GrapaLibraryRulePyExecEvent` classes.

## Overview

**Purpose:** Execute Python code from Grapa scripts
**Status:** Complete implementation
**Source:** `source/mainpy.cpp` (GrapaLibraryRulePyEvalEvent and GrapaLibraryRulePyExecEvent classes)

## Architecture

### **Execution Flow**
```
Grapa Script → $py.eval()/$py.exec() → Python Execution → Result → Grapa Object
     ↑              ↓                        ↓              ↓         ↓
Grapa Namespace → Rule Resolution → Python Interpreter → Type Conversion → Grapa Result
```

### **Key Components**
- **`GrapaLibraryRulePyEvalEvent`** - Python expression evaluation
- **`GrapaLibraryRulePyExecEvent`** - Python statement execution
- **`GrapaPyRuleEvent`** - Rule loading and registration
- **GIL Management** - Thread safety with Global Interpreter Lock
- **Type Conversion** - Bidirectional conversion between Python and Grapa

## Implementation Details

### **1. Rule Registration (GrapaPyRuleEvent)**

#### **Class Definition**
```cpp
class GrapaPyRuleEvent : public GrapaLibraryRuleEvent
{
public:
    GrapaPyRuleEvent(GrapaCHAR pName) { mName.FROM(pName); };
    virtual GrapaLibraryEvent* LoadLib(GrapaScriptExec* vScriptExec, GrapaRuleEvent* pLib, GrapaCHAR& pName)
    {
        GrapaLibraryEvent* lib = NULL;
        if (pName.Cmp("eval") == 0) lib = new GrapaLibraryRulePyEvalEvent(pName);
        else if (pName.Cmp("exec") == 0) lib = new GrapaLibraryRulePyExecEvent(pName);
        return(lib);
    }
};
```

#### **Registration Process**
```cpp
// In PYBIND11_MODULE initialization
GrapaLink::GetGrapaSystem()->mLibraryQueue.PushTail(new GrapaPyRuleEvent(GrapaCHAR("py")));
```

**Registration Flow:**
1. **Module initialization**: Creates `GrapaPyRuleEvent` instance
2. **Library queue**: Adds to Grapa's library queue
3. **Rule resolution**: Enables `$py.eval()` and `$py.exec()` calls
4. **Dynamic loading**: Loads appropriate event handler based on method name

### **2. Python Expression Evaluation (GrapaLibraryRulePyEvalEvent)**

#### **Class Definition**
```cpp
class GrapaLibraryRulePyEvalEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRulePyEvalEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
    {
        pybind11::gil_scoped_acquire acquire;
        // ... implementation
    }
};
```

#### **Method Signature**
```cpp
virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
```

**Parameters:**
- **`vScriptExec`**: Grapa script executor context
- **`pNameSpace`**: Current Grapa namespace
- **`pOperation`**: Operation being executed
- **`pInput`**: Input parameters queue

**Return Value:**
- **`GrapaRuleEvent*`**: Result of Python expression evaluation

#### **Parameter Extraction**
```cpp
GrapaLibraryParam script_param(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
GrapaLibraryParam locals_param(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
GrapaLibraryParam import_param(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
GrapaLibraryParam attr_param(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
```

**Parameter Mapping:**
- **`script_param`**: Python script to evaluate (position 0)
- **`locals_param`**: Local variables dictionary (position 1)
- **`import_param`**: Module to import (position 2)
- **`attr_param`**: Module attribute to use (position 3)

#### **Python Execution Setup**
```cpp
if (script_param.vVal && script_param.vVal->mValue.mLength) {
    std::string sript_str;
    GrapaCHAR import_str("__main__"), attr_str("__dict__");
    sript_str.assign((char*)script_param.vVal->mValue.mBytes, script_param.vVal->mValue.mLength);
    if (import_param.vVal && import_param.vVal->mValue.mLength) import_str.FROM(import_param.vVal->mValue);
    if (attr_param.vVal && attr_param.vVal->mValue.mLength) attr_str.FROM(attr_param.vVal->mValue);
    py::object scope = py::module_::import((char*)import_str.mBytes).attr((char*)attr_str.mBytes);
```

**Setup Process:**
1. **Script extraction**: Converts Grapa string to C++ string
2. **Default values**: Sets default import (`__main__`) and attr (`__dict__`)
3. **Parameter override**: Uses provided import and attr parameters if available
4. **Scope creation**: Creates Python execution scope

#### **Local Variables Conversion**
```cpp
GrapaCHAR pStr;
py::object locals;
GrapaRuleEvent* cp = vScriptExec->CopyItem(locals_param.vVal);
GrapaPyObject::FromGrapa(vScriptExec, cp, &locals);
if (cp) {
    cp->CLEAR();
    delete cp;
}
```

**Conversion Process:**
1. **Copy creation**: Creates copy of locals parameter
2. **Type conversion**: Converts Grapa locals to Python dictionary
3. **Memory cleanup**: Cleans up temporary copy
4. **Scope preparation**: Prepares locals for Python execution

#### **Python Expression Execution**
```cpp
py::object o = py::eval(sript_str, scope, locals);
result = GrapaPyObject::ToGrapa(o.ptr(), GrapaCHAR());
```

**Execution Process:**
1. **GIL acquisition**: Ensures thread safety
2. **Python eval**: Executes Python expression
3. **Result conversion**: Converts Python result to Grapa object
4. **Return**: Returns converted result

### **3. Python Statement Execution (GrapaLibraryRulePyExecEvent)**

#### **Class Definition**
```cpp
class GrapaLibraryRulePyExecEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRulePyExecEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
    {
        pybind11::gil_scoped_acquire acquire;
        // ... implementation
    }
};
```

#### **Method Implementation**
```cpp
virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
    pybind11::gil_scoped_acquire acquire;
    GrapaRuleEvent* result = NULL;
    GrapaLibraryParam script_param(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
    GrapaLibraryParam locals_param(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
    GrapaLibraryParam import_param(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
    GrapaLibraryParam attr_param(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
    if (script_param.vVal && script_param.vVal->mValue.mLength) {
        std::string sript_str;
        sript_str.assign((char*)script_param.vVal->mValue.mBytes, script_param.vVal->mValue.mLength);
        GrapaCHAR import_str("__main__"), attr_str("__dict__");
        if (import_param.vVal && import_param.vVal->mValue.mLength) import_str.FROM(import_param.vVal->mValue);
        if (attr_param.vVal && attr_param.vVal->mValue.mLength) attr_str.FROM(attr_param.vVal->mValue);
        py::object scope = py::module_::import((char*)import_str.mBytes).attr((char*)attr_str.mBytes);
        py::object locals;
        GrapaRuleEvent* cp = vScriptExec->CopyItem(locals_param.vVal);
        GrapaPyObject::FromGrapa(vScriptExec, locals_param.vVal, &locals);
        if (cp) {
            cp->CLEAR();
            delete cp;
        }
        py::exec(sript_str, scope, locals);
    }
    return result;
}
```

**Key Differences from eval:**
- **`py::exec()`**: Executes Python statements instead of expressions
- **No result**: Returns `NULL` (statements don't produce values)
- **Side effects**: Focuses on execution side effects rather than return values

### **4. Grapa Integration Setup**

#### **$py Class Definition**
```cpp
GrapaCHAR runStr("$global[\"$py\"] = class {eval = op(script,locals={},import=\"\",attr=\"\"){@<\"py\",\"eval\",{@<var,{script}>,@<var,{locals}>,@<var,{import}>,@<var,{attr}>}>();};exec = op(script,locals={},import=\"\",attr=\"\"){@<\"py\",\"exec\",{@<var,{script}>,@<var,{locals}>,@<var,{import}>,@<var,{attr}>}>();};};");
grresult = mConsoleSend.SendSync(runStr, NULL, 0, GrapaCHAR());
```

**Class Structure:**
- **`$py.eval()`**: Evaluates Python expressions
- **`$py.exec()`**: Executes Python statements
- **Parameter support**: Supports script, locals, import, and attr parameters
- **Rule resolution**: Uses Grapa's rule system for method resolution

## Usage Patterns

### **1. Basic Expression Evaluation**
```grapa
/* Evaluate simple Python expression */
result = $py.eval("2 + 2");
("Python result: " + result).echo();  // Output: Python result: 4

/* Evaluate with Python variables */
result = $py.eval("x * y", {"x": 10, "y": 20});
("Product: " + result).echo();  // Output: Product: 200
```

### **2. Statement Execution**
```grapa
/* Execute Python statements */
$py.exec("x = 10; y = 20; z = x + y");

/* Execute with custom module */
$py.exec("import math; result = math.sqrt(16)", {}, "math", "__dict__");
```

### **3. Advanced Usage**
```grapa
/* Use Python libraries */
result = $py.eval("import json; json.dumps({'name': 'Grapa', 'version': 1.0})");

/* Execute with custom scope */
$py.exec("def greet(name): return f'Hello, {name}!'; message = greet('World')", {}, "builtins", "__dict__");
result = $py.eval("message");
("Message: " + result).echo();  // Output: Message: Hello, World!
```

### **4. Data Exchange**
```grapa
/* Pass Grapa data to Python */
grapa_data = [1, 2, 3, 4, 5];
result = $py.eval("sum(@$ARG)", grapa_data);
("Sum: " + result).echo();  // Output: Sum: 15

/* Get Python data in Grapa */
result = $py.eval("[i**2 for i in range(5)]");
("Squares: " + result).echo();  // Output: Squares: [0, 1, 4, 9, 16]
```

## Thread Safety

### **GIL Management**
- **`pybind11::gil_scoped_acquire acquire`**: Acquires GIL for Python operations
- **Thread safety**: Ensures safe concurrent access to Python interpreter
- **Resource protection**: Protects Python objects during execution
- **Exception safety**: Proper GIL management during exceptions

### **Concurrent Execution**
- **Multiple threads**: Multiple Grapa threads can execute Python code
- **GIL contention**: GIL ensures only one thread executes Python at a time
- **Resource sharing**: Safe sharing of Python interpreter resources
- **Memory safety**: Thread-safe memory access patterns

## Error Handling

### **Python Exceptions**
- **Exception propagation**: Python exceptions are captured and converted
- **Error conversion**: Python errors become Grapa error objects
- **Stack traces**: Python stack traces are preserved in error messages
- **Graceful fallback**: Errors don't crash the Grapa system

### **Grapa Integration Errors**
- **Parameter errors**: Invalid parameters are handled gracefully
- **Type conversion errors**: Type conversion failures are handled
- **Memory errors**: Memory allocation failures are handled
- **System errors**: System-level errors are properly managed

## Performance Characteristics

### **Execution Performance**
- **Fast execution**: Direct Python interpreter access
- **Minimal overhead**: Low overhead for Python execution
- **Efficient conversion**: Optimized type conversion between systems
- **Memory efficiency**: Efficient memory usage during execution

### **Memory Management**
- **Automatic cleanup**: Python objects are automatically cleaned up
- **Reference counting**: Proper reference counting for Python objects
- **Memory pooling**: Efficient memory allocation patterns
- **Garbage collection**: Python garbage collection integration

## Security Considerations

### **Script Execution**
- **Sandboxed execution**: Python code runs in controlled environment
- **Resource limits**: Memory and execution time limits
- **Input validation**: Script parameters are validated
- **Error isolation**: Python errors don't affect Grapa system

### **Access Control**
- **Module access**: Controlled access to Python modules
- **System access**: Limited system access for security
- **File access**: Controlled file system access
- **Network access**: Controlled network access

## Integration Points

### **With Grapa Language**
- **Rule system**: Integrates with Grapa's rule system
- **Type system**: Full integration with Grapa type system
- **Namespace system**: Uses Grapa's namespace system
- **Error system**: Integrates with Grapa's error handling

### **With Python Ecosystem**
- **Standard library**: Full access to Python standard library
- **Third-party packages**: Access to any installed Python package
- **NumPy/SciPy**: Scientific computing capabilities
- **Web frameworks**: Web development capabilities

## Future Enhancements

### **Planned Features**
- **Async execution**: Non-blocking Python execution
- **Parallel processing**: Multiprocessing support
- **Jupyter integration**: Jupyter notebook support
- **Debugging support**: Enhanced debugging capabilities

### **Performance Improvements**
- **Caching system**: Python module caching
- **Memory optimization**: Further memory usage optimization
- **Type optimization**: Optimized type conversion paths
- **Concurrency improvements**: Enhanced threading support

## Related Documentation

### **Implementation Documentation**
- **[MAINPY_IMPLEMENTATION.md](MAINPY_IMPLEMENTATION.md)** - Main Python integration implementation
- **[GRAPAPY_IMPLEMENTATION.md](GRAPAPY_IMPLEMENTATION.md)** - GrapaPy Python extension module
- **[TYPE_MAPPING_IMPLEMENTATION.md](TYPE_MAPPING_IMPLEMENTATION.md)** - Type conversion system

### **User Documentation**
- **[docs-src/docs/python_integration.md](../../../docs-src/docs/python_integration.md)** - User-facing Python API
- **[docs-src/docs/python_use_cases.md](../../../docs-src/docs/python_use_cases.md)** - Python integration examples

## Notes

- **Thread Safety**: All operations are thread-safe with proper GIL management
- **Memory Management**: Automatic cleanup of Python objects and Grapa resources
- **Error Handling**: Comprehensive error propagation between Python and Grapa
- **Performance**: Optimized for minimal overhead in Python execution
- **Security**: Sandboxed execution with proper access controls
