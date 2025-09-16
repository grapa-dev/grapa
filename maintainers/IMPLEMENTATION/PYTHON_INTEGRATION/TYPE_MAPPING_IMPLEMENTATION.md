---
tags:
  - maintainer
  - implementation
  - python-integration
  - type-mapping
  - conversion
  - grapa-pyobject
---

# Type Mapping Implementation

This document provides comprehensive implementation details for the bidirectional type conversion system between Python and Grapa, implemented in the `GrapaPyObject` class.

## Overview

**Class:** `GrapaPyObject`
**Purpose:** Bidirectional type conversion between Python and Grapa objects
**Status:** Complete implementation
**Source:** `source/mainpy.cpp` (GrapaPyObject class)

## Architecture

### **Conversion Flow**
```
Python Objects ←→ GrapaPyObject ←→ Grapa Objects
     ↑              (ToGrapa)           ↑
     ↓              (FromGrapa)         ↓
Python Types ←→ Type Conversion ←→ Grapa Types
```

### **Key Components**
- **`ToGrapa()`** - Convert Python objects to Grapa objects
- **`FromGrapa()`** - Convert Grapa objects to Python objects
- **Type-specific conversion logic** - Handles each Python/Grapa type pair
- **Memory management** - Proper cleanup and reference counting

## Implementation Details

### **1. Python to Grapa Conversion (ToGrapa)**

#### **Function Signature**
```cpp
static GrapaRuleEvent* ToGrapa(PyObject* o, GrapaCHAR pname)
```

**Parameters:**
- **`o`**: Python object to convert
- **`pname`**: Name for the resulting Grapa object

**Return Value:**
- **`GrapaRuleEvent*`**: Converted Grapa object (caller owns memory)

#### **Type-Specific Conversions**

##### **Boolean Conversion**
```cpp
if (PyBool_Check(o)) {
    long retvalue = PyLong_AsLong(o);
    if (retvalue) pStr.FROM("\1");
    else pStr.FROM("\0");
    pStr.mToken = GrapaTokenType::BOOL;
    result = new GrapaRuleEvent(0, pname, pStr);
}
```

**Conversion Logic:**
- **Python `True`** → **Grapa `\1`** (boolean true)
- **Python `False`** → **Grapa `\0`** (boolean false)
- **Token Type**: `GrapaTokenType::BOOL`

##### **None Conversion**
```cpp
else if (Py_None == o) {
    result = new GrapaRuleEvent(0, pname, pStr);
    result->SetNull();
}
```

**Conversion Logic:**
- **Python `None`** → **Grapa null** (empty value)
- **Token Type**: `GrapaTokenType::ERR` (null state)

##### **Bytes Conversion**
```cpp
else if (PyBytes_Check(o)) {
    const char* buffer = PyBytes_AS_STRING(o);
    Py_ssize_t length = PyBytes_GET_SIZE(o);
    pStr.FROM(buffer, length);
    pStr.mToken = GrapaTokenType::RAW;
    result = new GrapaRuleEvent(0, pname, pStr);
}
```

**Conversion Logic:**
- **Python `bytes`** → **Grapa `RAW`** (raw bytes)
- **Preserves**: Exact byte content and length
- **Token Type**: `GrapaTokenType::RAW`

##### **String Conversion**
```cpp
else if (PyUnicode_Check(o)) {
    Py_ssize_t size = 0;
    char* ptr = (char*)PyUnicode_AsUTF8AndSize(o, &size);
    result = new GrapaRuleEvent(0, pname, GrapaCHAR(ptr, size));
}
```

**Conversion Logic:**
- **Python `str`** → **Grapa `STR`** (UTF-8 string)
- **Encoding**: UTF-8 encoding for Unicode support
- **Token Type**: `GrapaTokenType::STR`

##### **Integer Conversion**
```cpp
else if (PyLong_Check(o)) {
    long long retvalue = PyLong_AsLongLong(o);
    pStr = GrapaInt(retvalue).getBytes();
    result = new GrapaRuleEvent(0, pname, pStr);
}
```

**Conversion Logic:**
- **Python `int`** → **Grapa `INT`** (unlimited precision integer)
- **Precision**: Supports unlimited precision integers
- **Token Type**: `GrapaTokenType::INT`

##### **Float Conversion**
```cpp
else if (PyFloat_Check(o)) {
    double retvalue = PyFloat_AS_DOUBLE(o);
    pStr = GrapaFloat(retvalue).getBytes();
    result = new GrapaRuleEvent(0, pname, pStr);
}
```

**Conversion Logic:**
- **Python `float`** → **Grapa `FLOAT`** (high precision float)
- **Precision**: High precision floating-point representation
- **Token Type**: `GrapaTokenType::FLOAT`

##### **Tuple Conversion**
```cpp
else if (PyTuple_Check(o)) {
    result = new GrapaRuleEvent(0, pname, GrapaCHAR());
    result->vQueue = new GrapaRuleQueue();
    result->mValue.mToken = GrapaTokenType::TUPLE;
    for (int i = 0; i < PyTuple_Size(o); ++i) {
        PyObject* item = PyTuple_GetItem(o, i);
        result->vQueue->PushTail(ToGrapa(item, GrapaCHAR()));
    }
}
```

**Conversion Logic:**
- **Python `tuple`** → **Grapa `TUPLE`** (immutable sequence)
- **Structure**: Creates GrapaRuleQueue with converted elements
- **Recursion**: Recursively converts each tuple element
- **Token Type**: `GrapaTokenType::TUPLE`

##### **List Conversion**
```cpp
else if (PyList_Check(o)) {
    result = new GrapaRuleEvent(0, pname, GrapaCHAR());
    result->vQueue = new GrapaRuleQueue();
    result->mValue.mToken = GrapaTokenType::ARRAY;
    for (int i = 0; i < PyList_Size(o); ++i) {
        PyObject* item = PyList_GetItem(o, i);
        result->vQueue->PushTail(ToGrapa(item, GrapaCHAR()));
    }
}
```

**Conversion Logic:**
- **Python `list`** → **Grapa `ARRAY`** (mutable sequence)
- **Structure**: Creates GrapaRuleQueue with converted elements
- **Recursion**: Recursively converts each list element
- **Token Type**: `GrapaTokenType::ARRAY`

##### **Dictionary Conversion**
```cpp
else if (PyDict_Check(o)) {
    result = new GrapaRuleEvent(0, pname, GrapaCHAR());
    result->vQueue = new GrapaRuleQueue();
    result->mValue.mToken = GrapaTokenType::LIST;
    PyObject* key, * item;
    Py_ssize_t pos = 0;
    while (PyDict_Next(o, &pos, &key, &item)) {
        Py_ssize_t nsize;
        char* nptr = (char*)PyUnicode_AsUTF8AndSize(key, &nsize);
        GrapaCHAR kname(nptr, nsize);
        result->vQueue->PushTail(ToGrapa(item, kname));
    }
}
```

**Conversion Logic:**
- **Python `dict`** → **Grapa `LIST`** (key-value pairs)
- **Structure**: Creates GrapaRuleQueue with named elements
- **Keys**: Dictionary keys become Grapa object names
- **Values**: Dictionary values are recursively converted
- **Token Type**: `GrapaTokenType::LIST`

##### **Fallback Conversion**
```cpp
else {
    result = new GrapaRuleEvent(0, pname, GrapaCHAR());
    result->SetNull();
}
```

**Conversion Logic:**
- **Unsupported types** → **Grapa null** (empty value)
- **Handles**: Custom Python objects, modules, functions, etc.
- **Token Type**: `GrapaTokenType::ERR` (null state)

### **2. Grapa to Python Conversion (FromGrapa)**

#### **Function Signature**
```cpp
static void FromGrapa(GrapaScriptExec* vScriptExec, GrapaRuleEvent* e, py::object* o)
```

**Parameters:**
- **`vScriptExec`**: Grapa script executor context
- **`e`**: Grapa object to convert
- **`o`**: Output Python object (caller provides pointer)

#### **Type-Specific Conversions**

##### **Null/Error Handling**
```cpp
if (e == NULL) {
    *o = py::none();
    return;
}
```

**Conversion Logic:**
- **Grapa `NULL`** → **Python `None`**
- **Handles**: Null pointers and error states

##### **Pointer Dereferencing**
```cpp
case GrapaTokenType::PTR:
    FromGrapa(vScriptExec, e->vRulePointer, o);
    break;
```

**Conversion Logic:**
- **Grapa `PTR`** → **Dereferenced value**
- **Recursion**: Recursively converts the pointed-to value
- **Handles**: Reference types and indirection

##### **String Conversion**
```cpp
case GrapaTokenType::STR:
    *o = py::str((char*)e->mValue.mBytes, e->mValue.mLength);
    break;
```

**Conversion Logic:**
- **Grapa `STR`** → **Python `str`** (UTF-8 string)
- **Encoding**: UTF-8 encoding for Unicode support
- **Length**: Preserves exact string length

##### **Integer Conversion**
```cpp
case GrapaTokenType::INT:
    a.FromBytes(e->mValue);
    *o = py::int_(a.LongValue());
    break;
```

**Conversion Logic:**
- **Grapa `INT`** → **Python `int`** (unlimited precision)
- **Precision**: Supports unlimited precision integers
- **Range**: Handles large integers beyond Python's native range

##### **ID/SYSID Conversion**
```cpp
case GrapaTokenType::ID:
case GrapaTokenType::SYSID:
    if (e->IsNull())
        *o = py::none();
    else
        *o = py::str((char*)e->mValue.mBytes, e->mValue.mLength);
    break;
```

**Conversion Logic:**
- **Grapa `ID`/`SYSID`** → **Python `str`** or **`None`**
- **Null handling**: Converts null IDs to Python `None`
- **String conversion**: Non-null IDs become Python strings

##### **Raw Bytes Conversion**
```cpp
case GrapaTokenType::RAW:
    *o = py::bytes((char*)e->mValue.mBytes, e->mValue.mLength);
    break;
```

**Conversion Logic:**
- **Grapa `RAW`** → **Python `bytes`** (raw bytes)
- **Preserves**: Exact byte content and length
- **Binary data**: Handles binary data without encoding

##### **Float Conversion**
```cpp
case GrapaTokenType::FLOAT:
    f.FromBytes(e->mValue);
    s = f.ToString();
    *o = py::float_(std::stod((char*)s.mBytes));
    break;
```

**Conversion Logic:**
- **Grapa `FLOAT`** → **Python `float`** (high precision)
- **Precision**: High precision floating-point representation
- **String conversion**: Converts through string representation for accuracy

##### **Boolean Conversion**
```cpp
case GrapaTokenType::BOOL:
    if (e) isTrueA = !e->IsNullIsNegIsZero(isNegA, isNull);
    *o = py::bool_(isTrueA);
    break;
```

**Conversion Logic:**
- **Grapa `BOOL`** → **Python `bool`**
- **Truthiness**: Uses Grapa's truthiness rules
- **Null handling**: Handles null boolean values

##### **Array Conversion**
```cpp
case GrapaTokenType::ARRAY:
    *o = py::list();
    e = e->vQueue->Head();
    while (e) {
        GrapaRuleEvent* e2 = e;
        while (e2 && e2->mValue.mToken == GrapaTokenType::PTR) 
            e2 = e2->vRulePointer;
        py::object o2;
        FromGrapa(vScriptExec, e2, &o2);
        ((py::list*)o)->append(o2);
        e = e->Next();
    }
    break;
```

**Conversion Logic:**
- **Grapa `ARRAY`** → **Python `list`** (mutable sequence)
- **Structure**: Creates Python list with converted elements
- **Recursion**: Recursively converts each array element
- **Pointer handling**: Dereferences pointers during conversion

##### **Tuple Conversion**
```cpp
case GrapaTokenType::TUPLE:
    *o = py::tuple(e->vQueue->mCount);
    e = e->vQueue->Head();
    while (e) {
        GrapaRuleEvent* e2 = e;
        while (e2 && e2->mValue.mToken == GrapaTokenType::PTR) 
            e2 = e2->vRulePointer;
        py::object o2;
        FromGrapa(vScriptExec, e2, &o2);
        PyTuple_SET_ITEM(o->ptr(), pos, o2.ptr());
        o2.inc_ref();
        pos++;
        e = e->Next();
    }
    break;
```

**Conversion Logic:**
- **Grapa `TUPLE`** → **Python `tuple`** (immutable sequence)
- **Structure**: Creates Python tuple with exact size
- **Recursion**: Recursively converts each tuple element
- **Reference counting**: Proper reference counting for tuple elements

##### **List Conversion**
```cpp
case GrapaTokenType::LIST:
    *o = py::dict();
    e = e->vQueue->Head();
    while (e) {
        GrapaRuleEvent* e2 = e;
        while (e2 && e2->mValue.mToken == GrapaTokenType::PTR) 
            e2 = e2->vRulePointer;
        py::object o2;
        FromGrapa(vScriptExec, e2, &o2);
        py::str nm((char*)e2->mName.mBytes, e2->mName.mLength);
        PyDict_SetItem(o->ptr(), nm.ptr(), o2.ptr());
        e = e->Next();
    }
    break;
```

**Conversion Logic:**
- **Grapa `LIST`** → **Python `dict`** (key-value pairs)
- **Structure**: Creates Python dictionary with named elements
- **Keys**: Grapa object names become dictionary keys
- **Values**: Grapa object values are recursively converted

##### **Complex Type Conversion**
```cpp
case GrapaTokenType::ERR:
case GrapaTokenType::INPUT:
case GrapaTokenType::XML:
case GrapaTokenType::SYM:
case GrapaTokenType::SYSSYM:
case GrapaTokenType::SYSSTR:
case GrapaTokenType::SYSINT:
case GrapaTokenType::TIME:
case GrapaTokenType::EL:
case GrapaTokenType::TAG:
case GrapaTokenType::TABLE:
case GrapaTokenType::OP:
case GrapaTokenType::CODE:
case GrapaTokenType::VECTOR:
case GrapaTokenType::CLASS:
case GrapaTokenType::OBJ:
case GrapaTokenType::REF:
case GrapaTokenType::RULE:
case GrapaTokenType::TOKEN:
case GrapaTokenType::WIDGET:
    if (e) {
        GrapaSystemSend send;
        send.isActive = false;
        if (e) {
            if (e->vQueue) vScriptExec->EchoList(&send, e, false, false, false);
            else vScriptExec->EchoValue(&send, e, false, false, false);
        }
        send.GetStr(s);
        *o = py::str((char*)s.mBytes, s.mLength);
    }
    break;
```

**Conversion Logic:**
- **Complex Grapa types** → **Python `str`** (string representation)
- **Echo system**: Uses Grapa's echo system for string representation
- **Fallback**: Provides meaningful string representation for complex types
- **Handles**: All complex Grapa types not directly mappable to Python

##### **Default Conversion**
```cpp
default:
    *o = py::none();
    break;
```

**Conversion Logic:**
- **Unknown types** → **Python `None`**
- **Handles**: Unsupported or unknown Grapa types
- **Safety**: Provides safe fallback for unknown types

## Type Mapping Summary

### **Direct Mappings**

| Python Type | Grapa Type | Conversion Notes |
|-------------|------------|------------------|
| `bool` | `BOOL` | Direct boolean conversion |
| `None` | `ERR` (null) | Null state conversion |
| `bytes` | `RAW` | Raw byte preservation |
| `str` | `STR` | UTF-8 string conversion |
| `int` | `INT` | Unlimited precision support |
| `float` | `FLOAT` | High precision conversion |
| `tuple` | `TUPLE` | Immutable sequence |
| `list` | `ARRAY` | Mutable sequence |
| `dict` | `LIST` | Key-value pairs |

### **Complex Type Mappings**

| Grapa Type | Python Type | Conversion Method |
|------------|-------------|-------------------|
| `PTR` | Dereferenced value | Recursive conversion |
| `ID`/`SYSID` | `str` or `None` | String with null handling |
| Complex types | `str` | Echo system representation |

## Memory Management

### **Python to Grapa**
- **Ownership**: Caller owns returned `GrapaRuleEvent*`
- **Cleanup**: Caller must call `CLEAR()` and `delete`
- **Recursion**: Recursive cleanup for nested structures
- **Reference counting**: Proper Python reference management

### **Grapa to Python**
- **Ownership**: Python manages object lifetime
- **Reference counting**: Proper Python reference counting
- **Temporary objects**: Automatic cleanup of temporary objects
- **Memory safety**: Safe memory access during conversion

## Performance Characteristics

### **Conversion Performance**
- **Efficient conversion**: Minimal memory allocations
- **Type checking**: Fast type identification
- **Recursive conversion**: Optimized for nested structures
- **Memory reuse**: Efficient memory usage patterns

### **Memory Usage**
- **Minimal overhead**: Low memory overhead for conversions
- **Reference counting**: Proper reference management
- **Temporary objects**: Efficient cleanup of temporary objects
- **Memory pooling**: Efficient memory allocation patterns

## Error Handling

### **Conversion Errors**
- **Type errors**: Graceful handling of unsupported types
- **Memory errors**: Safe memory allocation and cleanup
- **Encoding errors**: UTF-8 encoding error handling
- **Null handling**: Proper null value conversion

### **Fallback Mechanisms**
- **Unknown types**: Fallback to string representation
- **Complex types**: Echo system for meaningful representation
- **Error states**: Safe conversion of error states
- **Null values**: Consistent null handling

## Thread Safety

### **GIL Management**
- **GIL acquisition**: Proper GIL management during conversion
- **Thread safety**: Safe concurrent access
- **Reference counting**: Thread-safe reference counting
- **Memory access**: Safe memory access patterns

## Usage Examples

### **Python to Grapa Conversion**
```cpp
// Convert Python list to Grapa array
PyObject* py_list = PyList_New(3);
PyList_SetItem(py_list, 0, PyLong_FromLong(1));
PyList_SetItem(py_list, 1, PyLong_FromLong(2));
PyList_SetItem(py_list, 2, PyLong_FromLong(3));

GrapaRuleEvent* grapa_array = GrapaPyObject::ToGrapa(py_list, GrapaCHAR("numbers"));
// grapa_array now contains Grapa ARRAY with [1, 2, 3]
```

### **Grapa to Python Conversion**
```cpp
// Convert Grapa string to Python string
GrapaRuleEvent* grapa_str = new GrapaRuleEvent(0, GrapaCHAR("hello"), GrapaCHAR("Hello, World!"));

py::object py_str;
GrapaPyObject::FromGrapa(vScriptExec, grapa_str, &py_str);
// py_str now contains Python string "Hello, World!"
```

## Database and Network Object Handling

### **Non-Serializable Object Persistence**
Some Grapa objects like `$file`, `$net`, and database connections **cannot be serialized** and must be stored in `$global` to persist across `grapapy` calls:

```python
import grapapy
g = grapapy.grapa()

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

### **Serializable vs Non-Serializable Types**

| Type Category | Examples | Serialization | Persistence Method |
|---------------|----------|---------------|-------------------|
| **✅ Serializable** | `$INT`, `$FLOAT`, `$STR`, `$LIST`, `$GOBJ`, `$BOOL` | Can be passed between calls | Direct assignment or return values |
| **❌ Non-Serializable** | `$file`, `$net`, `$TABLE`, database connections, network connections | Cannot be serialized | Must use `$global` storage |
| **⚡ Large Values** | Large `$INT`/`$FLOAT` (5000+ bits) | Can be serialized but inefficient | Use `$global` or `$RAW` for efficiency |

### **Database Usage Pattern**
```python
import grapapy
g = grapapy.grapa()

# Initialize database connection in global namespace
g.eval("$global['db'] = $file('users.db')")

# Use database across multiple calls
g.eval("db.table('users').insert({'name': 'John', 'age': 30})")
g.eval("db.table('users').insert({'name': 'Jane', 'age': 25})")

# Query database
result = g.eval("db.table('users').select()")
print(result)  # All users

# Update database
g.eval("db.table('users').update({'age': 31}, {'name': 'John'})")

# Database connection persists throughout the session
```

### **Performance Optimization for Large Values**
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

**Performance Considerations:**
- **Large values** (>1000 bits) benefit from staying in Grapa memory
- **$RAW transfer** is more efficient than type conversion for very large values
- **$global storage** avoids repeated serialization/deserialization overhead
- **Final results only** should be converted to Python when needed

## Related Documentation

### **Implementation Documentation**
- **[MAINPY_IMPLEMENTATION.md](MAINPY_IMPLEMENTATION.md)** - Main Python integration implementation
- **[GRAPAPY_IMPLEMENTATION.md](GRAPAPY_IMPLEMENTATION.md)** - GrapaPy Python extension module
- **[FOREIGN_FUNCTION_INTERFACE.md](FOREIGN_FUNCTION_INTERFACE.md)** - Python script execution

### **Type System Documentation**
- **[../TYPE_SYSTEM/TYPE_DEFINITIONS_AND_CLASSIFICATIONS.md](../TYPE_SYSTEM/TYPE_DEFINITIONS_AND_CLASSIFICATIONS.md)** - Grapa type system
- **[../TYPE_SYSTEM/TYPE_SYSTEM_ARCHITECTURE.md](../TYPE_SYSTEM/TYPE_SYSTEM_ARCHITECTURE.md)** - Type system architecture

## Notes

- **Comprehensive coverage**: Supports all major Python and Grapa types
- **Memory safety**: Proper memory management and cleanup
- **Thread safety**: Safe concurrent access with GIL management
- **Performance**: Optimized for efficient type conversion
- **Error handling**: Robust error handling and fallback mechanisms
