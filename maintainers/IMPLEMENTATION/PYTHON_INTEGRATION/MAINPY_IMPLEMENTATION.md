---
tags:
  - maintainer
  - implementation
  - python-integration
  - mainpy
  - c++
---

# mainpy.cpp Implementation

This document provides comprehensive implementation details for `mainpy.cpp`, the main Python integration implementation that provides bidirectional type conversion, Python script execution, and the GrapaPy Python extension module.

## Overview

**File:** `source/mainpy.cpp` (621 lines)
**Purpose:** Main Python integration implementation with pybind11
**Status:** Complete implementation

## Architecture

### **Class Hierarchy**
```
GrapaConsoleResponse
└── GrapaMainResponse (No-op response for Python integration)

GrapaLibraryEvent
├── GrapaLibraryRulePyEvalEvent (Python eval integration)
└── GrapaLibraryRulePyExecEvent (Python exec integration)

GrapaLibraryRuleEvent
└── GrapaPyRuleEvent (Python rule loading)

GrapaStruct (Main Python interface class)
├── GrapaScriptExec mScriptExec
├── GrapaConsoleSend mConsoleSend
├── GrapaMainResponse mConsoleResponse
└── GrapaNames mRuleVariables
```

### **Key Components**
- **`GrapaPyObject`** - Type conversion utilities
- **`GrapaStruct`** - Main Python interface class
- **`GrapaLibraryRulePyEvalEvent`** - Python eval integration
- **`GrapaLibraryRulePyExecEvent`** - Python exec integration
- **pybind11 Module** - Python extension module

## Implementation Details

### **1. Dependencies and Setup**

#### **Static Compilation Definitions**
```cpp
// Static compilation definitions to avoid DLL dependencies
#ifdef _WIN32
#define UTF8PROC_STATIC
#define PCRE2_STATIC
#endif

#include <pybind11/pybind11.h>
#include "pybind11/include/pybind11/pybind11.h"
#include "pybind11/include/pybind11/eval.h"
#include "pybind11/include/pybind11/embed.h"

namespace py = pybind11;
using namespace py::literals;
```

#### **Grapa System Integration**
```cpp
#include "grapa/GrapaLink.h"
#include "grapa/GrapaValue.h"
#include "grapa/GrapaSystem.h"
#include "grapa/GrapaCompress.h"
#include "grapa/GrapaLibRule.h"
#include "grapa/GrapaFloat.h"
```

### **2. Type Conversion System (GrapaPyObject)**

#### **Python to Grapa Conversion (ToGrapa)**
```cpp
static GrapaRuleEvent* ToGrapa(PyObject* o, GrapaCHAR pname)
{
    GrapaRuleEvent* result = NULL;
    GrapaCHAR pStr;
    
    if (PyBool_Check(o)) {
        // Convert Python bool to Grapa bool
        long retvalue = PyLong_AsLong(o);
        if (retvalue) pStr.FROM("\1");
        else pStr.FROM("\0");
        pStr.mToken = GrapaTokenType::BOOL;
        result = new GrapaRuleEvent(0, pname, pStr);
    }
    else if (Py_None == o) {
        // Convert Python None to Grapa null
        result = new GrapaRuleEvent(0, pname, pStr);
        result->SetNull();
    }
    else if (PyBytes_Check(o)) {
        // Convert Python bytes to Grapa RAW
        const char* buffer = PyBytes_AS_STRING(o);
        Py_ssize_t length = PyBytes_GET_SIZE(o);
        pStr.FROM(buffer, length);
        pStr.mToken = GrapaTokenType::RAW;
        result = new GrapaRuleEvent(0, pname, pStr);
    }
    else if (PyUnicode_Check(o)) {
        // Convert Python string to Grapa STR
        Py_ssize_t size = 0;
        char* ptr = (char*)PyUnicode_AsUTF8AndSize(o, &size);
        result = new GrapaRuleEvent(0, pname, GrapaCHAR(ptr, size));
    }
    else if (PyLong_Check(o)) {
        // Convert Python int to Grapa INT
        long long retvalue = PyLong_AsLongLong(o);
        pStr = GrapaInt(retvalue).getBytes();
        result = new GrapaRuleEvent(0, pname, pStr);
    }
    else if (PyFloat_Check(o)) {
        // Convert Python float to Grapa FLOAT
        double retvalue = PyFloat_AS_DOUBLE(o);
        pStr = GrapaFloat(retvalue).getBytes();
        result = new GrapaRuleEvent(0, pname, pStr);
    }
    else if (PyTuple_Check(o)) {
        // Convert Python tuple to Grapa TUPLE
        result = new GrapaRuleEvent(0, pname, GrapaCHAR());
        result->vQueue = new GrapaRuleQueue();
        result->mValue.mToken = GrapaTokenType::TUPLE;
        for (int i = 0; i < PyTuple_Size(o); ++i) {
            PyObject* item = PyTuple_GetItem(o, i);
            result->vQueue->PushTail(ToGrapa(item, GrapaCHAR()));
        }
    }
    else if (PyList_Check(o)) {
        // Convert Python list to Grapa ARRAY
        result = new GrapaRuleEvent(0, pname, GrapaCHAR());
        result->vQueue = new GrapaRuleQueue();
        result->mValue.mToken = GrapaTokenType::ARRAY;
        for (int i = 0; i < PyList_Size(o); ++i) {
            PyObject* item = PyList_GetItem(o, i);
            result->vQueue->PushTail(ToGrapa(item, GrapaCHAR()));
        }
    }
    else if (PyDict_Check(o)) {
        // Convert Python dict to Grapa LIST
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
    else {
        // Unknown type - return null
        result = new GrapaRuleEvent(0, pname, GrapaCHAR());
        result->SetNull();
    }
    return result;
}
```

#### **Grapa to Python Conversion (FromGrapa)**
```cpp
static void FromGrapa(GrapaScriptExec* vScriptExec, GrapaRuleEvent* e, py::object* o)
{
    if (e == NULL) {
        *o = py::none();
        return;
    }
    
    GrapaInt a;
    GrapaFloat f;
    GrapaCHAR s;
    bool isTrueA = false;
    bool isNegA = false;
    bool isNull = false;
    Py_ssize_t pos = 0;
    
    switch (e->mValue.mToken) {
    case GrapaTokenType::PTR:
        // Handle pointer types
        FromGrapa(vScriptExec, e->vRulePointer, o);
        break;
    case GrapaTokenType::STR:
        // Convert Grapa STR to Python string
        *o = py::str((char*)e->mValue.mBytes, e->mValue.mLength);
        break;
    case GrapaTokenType::INT:
        // Convert Grapa INT to Python int
        a.FromBytes(e->mValue);
        *o = py::int_(a.LongValue());
        break;
    case GrapaTokenType::ID:
    case GrapaTokenType::SYSID:
        // Convert Grapa ID/SYSID to Python string or None
        if (e->IsNull())
            *o = py::none();
        else
            *o = py::str((char*)e->mValue.mBytes, e->mValue.mLength);
        break;
    case GrapaTokenType::RAW:
        // Convert Grapa RAW to Python bytes
        *o = py::bytes((char*)e->mValue.mBytes, e->mValue.mLength);
        break;
    case GrapaTokenType::FLOAT:
        // Convert Grapa FLOAT to Python float
        f.FromBytes(e->mValue);
        s = f.ToString();
        *o = py::float_(std::stod((char*)s.mBytes));
        break;
    case GrapaTokenType::BOOL:
        // Convert Grapa BOOL to Python bool
        if (e) isTrueA = !e->IsNullIsNegIsZero(isNegA, isNull);
        *o = py::bool_(isTrueA);
        break;
    case GrapaTokenType::ARRAY:
        // Convert Grapa ARRAY to Python list
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
    case GrapaTokenType::TUPLE:
        // Convert Grapa TUPLE to Python tuple
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
    case GrapaTokenType::LIST:
        // Convert Grapa LIST to Python dict
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
    default:
        // For complex types, convert to string representation
        if (e) {
            GrapaSystemSend send;
            send.isActive = false;
            if (e->vQueue) 
                vScriptExec->EchoList(&send, e, false, false, false);
            else 
                vScriptExec->EchoValue(&send, e, false, false, false);
            send.GetStr(s);
            *o = py::str((char*)s.mBytes, s.mLength);
        }
        break;
    }
}
```

### **3. Python Script Execution Integration**

#### **GrapaLibraryRulePyEvalEvent (Python eval)**
```cpp
class GrapaLibraryRulePyEvalEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRulePyEvalEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, 
                               GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
    {
        pybind11::gil_scoped_acquire acquire;  // Acquire GIL for thread safety
        
        GrapaRuleEvent* result = NULL;
        GrapaLibraryParam script_param(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
        GrapaLibraryParam locals_param(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
        GrapaLibraryParam import_param(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
        GrapaLibraryParam attr_param(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
        
        if (script_param.vVal && script_param.vVal->mValue.mLength) {
            std::string script_str;
            GrapaCHAR import_str("__main__"), attr_str("__dict__");
            
            script_str.assign((char*)script_param.vVal->mValue.mBytes, 
                             script_param.vVal->mValue.mLength);
            
            if (import_param.vVal && import_param.vVal->mValue.mLength) 
                import_str.FROM(import_param.vVal->mValue);
            if (attr_param.vVal && attr_param.vVal->mValue.mLength) 
                attr_str.FROM(attr_param.vVal->mValue);
            
            // Import Python module and get scope
            py::object scope = py::module_::import((char*)import_str.mBytes)
                              .attr((char*)attr_str.mBytes);
            
            // Convert Grapa locals to Python locals
            py::object locals;
            GrapaRuleEvent* cp = vScriptExec->CopyItem(locals_param.vVal);
            GrapaPyObject::FromGrapa(vScriptExec, cp, &locals);
            if (cp) {
                cp->CLEAR();
                delete cp;
            }
            
            // Execute Python code
            py::object o = py::eval(script_str, scope, locals);
            result = GrapaPyObject::ToGrapa(o.ptr(), GrapaCHAR());
        }
        return result;
    }
};
```

#### **GrapaLibraryRulePyExecEvent (Python exec)**
```cpp
class GrapaLibraryRulePyExecEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRulePyExecEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, 
                               GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
    {
        pybind11::gil_scoped_acquire acquire;  // Acquire GIL for thread safety
        
        GrapaRuleEvent* result = NULL;
        GrapaLibraryParam script_param(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
        GrapaLibraryParam locals_param(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
        GrapaLibraryParam import_param(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
        GrapaLibraryParam attr_param(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
        
        if (script_param.vVal && script_param.vVal->mValue.mLength) {
            std::string script_str;
            script_str.assign((char*)script_param.vVal->mValue.mBytes, 
                             script_param.vVal->mValue.mLength);
            
            GrapaCHAR import_str("__main__"), attr_str("__dict__");
            if (import_param.vVal && import_param.vVal->mValue.mLength) 
                import_str.FROM(import_param.vVal->mValue);
            if (attr_param.vVal && attr_param.vVal->mValue.mLength) 
                attr_str.FROM(attr_param.vVal->mValue);
            
            // Import Python module and get scope
            py::object scope = py::module_::import((char*)import_str.mBytes)
                              .attr((char*)attr_str.mBytes);
            
            // Convert Grapa locals to Python locals
            py::object locals;
            GrapaRuleEvent* cp = vScriptExec->CopyItem(locals_param.vVal);
            GrapaPyObject::FromGrapa(vScriptExec, locals_param.vVal, &locals);
            if (cp) {
                cp->CLEAR();
                delete cp;
            }
            
            // Execute Python code (no return value)
            py::exec(script_str, scope, locals);
        }
        return result;
    }
};
```

### **4. GrapaStruct (Main Python Interface)**

#### **Constructor and Initialization**
```cpp
class GrapaStruct 
{
public:
    GrapaScriptExec mScriptExec;
    GrapaConsoleSend mConsoleSend;
    GrapaMainResponse mConsoleResponse;
    GrapaNames mRuleVariables;
    
    GrapaStruct(py::object cmdstr)
    {
        // Initialize Grapa system components
        mConsoleSend.mScriptState.vScriptExec = &mScriptExec;
        mScriptExec.vScriptState = &mConsoleSend.mScriptState;
        mConsoleSend.mScriptState.SetNameSpace(&mRuleVariables);
        mRuleVariables.SetResponse(&mConsoleResponse);
        mConsoleSend.Start();
        
        GrapaCHAR runStr;
        if (cmdstr.ptr() == Py_None) {
            // Initialize with default grammar
            GrapaSystem* gSystem = GrapaLink::GetGrapaSystem();
            GrapaCHAR grresult;
            if (gSystem->mGrammar.mLength)
                grresult = mConsoleSend.SendSync(gSystem->mGrammar, NULL, 0, GrapaCHAR());
            
            // Define $py class for Python integration
            GrapaCHAR runStr("$global[\"$py\"] = class {eval = op(script,locals={},import=\"\",attr=\"\"){@<\"py\",\"eval\",{@<var,{script}>,@<var,{locals}>,@<var,{import}>,@<var,{attr}>}>();};exec = op(script,locals={},import=\"\",attr=\"\"){@<\"py\",\"exec\",{@<var,{script}>,@<var,{locals}>,@<var,{import}>,@<var,{attr}>}>();};};");
            grresult = mConsoleSend.SendSync(runStr, NULL, 0, GrapaCHAR());
        }
        else if (PyUnicode_Check(cmdstr.ptr())) {
            // Initialize with Unicode string
            Py_ssize_t size = 0;
            const char* buffer = (char*)PyUnicode_AsUTF8AndSize(cmdstr.ptr(), &size);
            size_t length = size;
            if (length > 0) {
                runStr.FROM(buffer, length);
                GrapaCHAR grresult;
                grresult = mConsoleSend.SendSync(runStr, NULL, 0, GrapaCHAR());
            }
        }
        else if (PyBytes_Check(cmdstr.ptr())) {
            // Initialize with bytes
            const char* buffer = PyBytes_AS_STRING(cmdstr.ptr());
            size_t length = PyBytes_GET_SIZE(cmdstr.ptr());
            if (length > 0) {
                runStr.FROM(buffer, length);
                runStr.mToken = GrapaTokenType::RAW;
                GrapaCHAR grresult;
                grresult = mConsoleSend.SendSync(runStr, NULL, 0, GrapaCHAR());
            }
        }
    }
    
    ~GrapaStruct() 
    { 
        mConsoleSend.Stop();
    }
```

#### **eval Method (Grapa Script Execution)**
```cpp
py::object eval(py::object cmdstr, py::object paramstr, std::string rulestr, std::string profilestr)
{
    size_t length = 0;
    GrapaCHAR runStr;
    GrapaRuleEvent* rulexx = NULL;
    GrapaCHAR profStr;
    py::object o;
    
    if (true) {
        pybind11::gil_scoped_acquire acquire;
        o = py::none();
        
        // Convert Python command string to Grapa string
        if (PyUnicode_Check(cmdstr.ptr())) {
            Py_ssize_t size = 0;
            const char* buffer = (char*)PyUnicode_AsUTF8AndSize(cmdstr.ptr(), &size);
            length = size;
            if (length > 0) {
                runStr.FROM(buffer, length);
            }
        }
        else if (PyBytes_Check(cmdstr.ptr())) {
            const char* buffer = PyBytes_AS_STRING(cmdstr.ptr());
            length = PyBytes_GET_SIZE(cmdstr.ptr());
            if (length > 0) {
                runStr.FROM(buffer, length);
                runStr.mToken = GrapaTokenType::RAW;
            }
        }
        
        // Handle rule string and profile string
        if (rulestr.length() > 0) {
            GrapaCHAR rStr(rulestr.c_str(), rulestr.length());
            rulexx = mConsoleSend.mScriptState.SearchVariable(
                mConsoleSend.mScriptState.GetNameSpace(), rStr);
        }
        profStr.FROM(profilestr.c_str(), profilestr.length());
    }
    
    // Execute Grapa script
    if (GrapaRuleEvent* operation = mConsoleSend.mScriptState.AddRuleOperation(
        mConsoleSend.mScriptState.GetNameSpace()->GetNameQueue(), "", "")) {
        
        GrapaRuleEvent* grresult = NULL;
        if (length > 0) {
            pybind11::gil_scoped_acquire acquire;
            
            // Convert Python parameters to Grapa parameters
            GrapaRuleEvent* e = GrapaPyObject::ToGrapa(paramstr.ptr(), GrapaCHAR());
            GrapaRuleEvent* q = mConsoleSend.mScriptState.GetNameSpace()->GetNameQueue()->Tail();
            GrapaRuleEvent* op = q;
            while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) 
                op = op->vRulePointer;
            
            if (e && e->mValue.mToken == GrapaTokenType::LIST && e->vQueue)
                while (e->vQueue->Head())
                    op->vQueue->PushTail(e->vQueue->PopHead());
            
            if (e) {
                e->CLEAR();
                delete e;
            }
        }
        
        // Setup locals for script execution
        GrapaRuleEvent* vLocals = new GrapaRuleEvent();
        vLocals->mValue.mToken = GrapaTokenType::LIST;
        vLocals->vQueue = new GrapaRuleQueue();
        mConsoleSend.mScriptState.GetNameSpace()->GetNameQueue()->PushTail(vLocals);
        
        if (length > 0) {
            grresult = mConsoleSend.SendSyncResult(runStr, rulexx, 0, profStr);
        }
        
        // Convert result back to Python
        if (grresult) {
            pybind11::gil_scoped_acquire acquire;
            GrapaRuleEvent* echo = grresult;
            while (echo && echo->mValue.mToken == GrapaTokenType::PTR)
                echo = echo->vRulePointer;
            GrapaPyObject::FromGrapa(&mScriptExec, echo, &o);
            grresult->CLEAR();
            delete grresult;
        }
        
        // Cleanup
        mConsoleSend.mScriptState.GetNameSpace()->GetNameQueue()->PopEvent(vLocals);
        if (vLocals) {
            vLocals->CLEAR();
            delete vLocals;
        }
        
        if (mConsoleSend.mScriptState.GetNameSpace()->GetNameQueue()->PopEvent(operation)) {
            operation->CLEAR();
            delete operation;
            operation = NULL;
        }
    }
    return o;
}
```

**⚠️ Critical Implementation Detail - Temporary Namespace Behavior:**

Each `grapapy` call creates a temporary namespace (`vLocals`) for parameter passing and local variables. This namespace is automatically created, used for script execution, and then completely destroyed during cleanup. This has important implications:

1. **Local variables created during script execution are lost** when the call returns
2. **Data that needs to persist across calls must be stored in `$global`**
3. **The global namespace is preserved** between calls and can be accessed normally
4. **Namespace search order**: Local → Global (so `$global` variables are accessible without prefix unless shadowed)

**🗄️ Database and Network Object Persistence:**
Some Grapa objects like `$file`, `$net`, and database connections **cannot be serialized** and must be stored in `$global` to persist across calls:

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

**Example of the behavior:**
```python
import grapapy
g = grapapy.grapa()

# ❌ Local variables are lost
g.eval("x = 10")  # x created in temporary namespace
result = g.eval("x + 5")  # Error: x not found

# ✅ Global variables persist
g.eval("$global['x'] = 10")  # Store in global namespace
result = g.eval("x + 5")  # Success: x found in global namespace
print(result)  # 15
```
```

#### **compile Method (Grapa Script Compilation)**
```cpp
py::object compile(py::object cmdstr, std::string rulestr, std::string profilestr)
{
    size_t length = 0;
    GrapaCHAR runStr;
    GrapaRuleEvent* rulexx = NULL;
    GrapaCHAR profStr;
    py::object o;
    
    if (true) {
        pybind11::gil_scoped_acquire acquire;
        o = py::none();
        
        // Convert Python command string to Grapa string
        Py_ssize_t size = 0;
        const char* buffer = (char*)PyUnicode_AsUTF8AndSize(cmdstr.ptr(), &size);
        length = size;
        if (length > 0) {
            runStr.FROM(buffer, length);
        }
        
        // Handle rule string and profile string
        if (rulestr.length() > 0) {
            GrapaCHAR rStr(rulestr.c_str(), rulestr.length());
            rulexx = mConsoleSend.mScriptState.SearchVariable(
                mConsoleSend.mScriptState.GetNameSpace(), rStr);
        }
        profStr.FROM(profilestr.c_str(), profilestr.length());
    }
    
    // Compile Grapa script
    GrapaRuleEvent* grresult = NULL;
    if (length > 0) {
        GrapaRuleEvent* opEvent = mScriptExec.Plan(
            mConsoleSend.mScriptState.GetNameSpace(), runStr, rulexx, 0, profStr);
        opEvent->mName.FROM("op");
        
        GrapaRuleEvent* verEvent = new GrapaRuleEvent(0, GrapaCHAR("version"), 
            GrapaLink::GetGrapaSystem()->mVersion);
        
        // Create compilation result
        GrapaRuleEvent* e = new GrapaRuleEvent();
        e->mValue.mToken = GrapaTokenType::LIST;
        e->vQueue = new GrapaRuleQueue();
        e->vQueue->PushTail(opEvent);
        e->vQueue->PushTail(verEvent);
        
        grresult = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
        GrapaCHAR s;
        e->TO(s);
        GrapaCompress::Compress(s, grresult->mValue);
        grresult->mValue.mToken = GrapaTokenType::RAW;
        
        e->CLEAR();
        delete e;
    }
    
    // Convert result to Python
    if (grresult) {
        pybind11::gil_scoped_acquire acquire;
        GrapaRuleEvent* echo = grresult;
        while (echo && echo->mValue.mToken == GrapaTokenType::PTR)
            echo = echo->vRulePointer;
        GrapaPyObject::FromGrapa(&mScriptExec, echo, &o);
        grresult->CLEAR();
        delete grresult;
    }
    
    return o;
}
```

### **5. pybind11 Module Definition**

#### **Module Initialization**
```cpp
PYBIND11_MODULE(grapapy, m)
{
    // Initialize Grapa system
    GrapaCHAR inStr, outStr, runStr;
    bool needExit = false, showConsole = false, showWidget = false;
    GrapaCHAR s = GrapaLink::Start(needExit, showConsole, showWidget, inStr, outStr, runStr);
    
    // Register Python rule event
    GrapaLink::GetGrapaSystem()->mLibraryQueue.PushTail(
        new GrapaPyRuleEvent(GrapaCHAR("py")));
    
    // Register cleanup function
    auto atexit = py::module_::import("atexit");
    atexit.attr("register")(py::cpp_function([]() {
        // perform cleanup here -- this function is called with the GIL held
        GrapaLink::Stop();
    }));
    
    // Module documentation
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
    
    // Define GrapaStruct class
    py::class_<GrapaStruct>(m, "grapa")
        .def(py::init<py::object>(), py::arg("s") = py::none())
        .def("eval", static_cast<py::object(GrapaStruct::*)(py::object, py::object, std::string, std::string)>(&GrapaStruct::eval), 
             "", py::arg("s"), py::arg("a") = "", py::arg("r") = "", py::arg("p") = "", 
             pybind11::call_guard<py::gil_scoped_release>())
        .def("compile", static_cast<py::object(GrapaStruct::*)(py::object, std::string, std::string)>(&GrapaStruct::compile), 
             "", py::arg("s"), py::arg("r") = "", py::arg("p") = "", 
             pybind11::call_guard<py::gil_scoped_release>());
    
    // Define standalone eval function
    m.def("eval", &grapa_eval, R"pbdoc(
        Evaluate a Grapa script
    )pbdoc",
        py::arg("s"), py::arg("a") = "", py::arg("r") = "", py::arg("p") = "", 
        pybind11::call_guard<py::gil_scoped_release>());
    
    // Module version
    m.attr("__version__") = "0.1.47";
}
```

#### **Standalone eval Function**
```cpp
py::object grapa_eval(py::object cmdstr, py::object paramstr, std::string rulestr, std::string profilestr)
{
    GrapaStruct* gs = new GrapaStruct(py::none());
    py::object o = gs->eval(cmdstr, paramstr, rulestr, profilestr);
    delete gs;
    return o;
}
```

## Integration with Grapa Language

### **System Function Registration**
```cpp
// Python integration functions
py_eval, py_exec
```

### **$py Class Definition**
```grapa
@global["$py"] = class {
    eval = op(script, locals={}, import="", attr="") {
        @<"py", "eval", {@<var,{script}>, @<var,{locals}>, @<var,{import}>, @<var,{attr}>}>();
    };
    exec = op(script, locals={}, import="", attr="") {
        @<"py", "exec", {@<var,{script}>, @<var,{locals}>, @<var,{import}>, @<var,{attr}>}>();
    };
};
```

## Performance Characteristics

### **Type Conversion Performance**
- **Efficient conversion** with minimal allocations
- **Reference counting** for memory management
- **Direct memory access** for primitive types
- **Lazy evaluation** for complex types

### **Script Execution Performance**
- **Fast Python execution** via pybind11
- **Minimal overhead** for Grapa→Python calls
- **GIL management** for thread safety
- **Memory pooling** for efficient allocation

### **Memory Management**
- **Automatic cleanup** of Python objects
- **Reference counting** for proper memory management
- **Exception safety** for resource cleanup
- **Garbage collection** integration
- **Temporary namespace cleanup** - Each call creates and destroys temporary namespace for local variables
- **Variable persistence** - Local variables are lost between calls; use `$global` for persistent data

## Error Handling

### **Python Exceptions**
- **Exception propagation** from Python to Grapa
- **Error type conversion** for meaningful error messages
- **Stack trace preservation** for debugging
- **Graceful fallback** for conversion errors

### **Grapa Errors**
- **Error propagation** from Grapa to Python
- **Type conversion** for error objects
- **Context preservation** for debugging
- **Exception handling** in Python code

## Security Considerations

### **Python Code Execution**
- **Sandboxed execution** for untrusted code
- **Resource limits** to prevent abuse
- **Input validation** for script parameters
- **Error isolation** to prevent crashes

### **Memory Safety**
- **Reference counting** for proper cleanup
- **Memory bounds checking** for conversions
- **Exception safety** for resource management
- **Thread safety** with GIL management

## Future Enhancements

### **Planned Features**
- **NumPy integration** for array operations
- **Pandas integration** for data analysis
- **Matplotlib integration** for plotting
- **Jupyter notebook** integration

### **Performance Improvements**
- **Async execution** for non-blocking operations
- **Parallel processing** with multiprocessing
- **Memory optimization** for large datasets
- **Caching system** for repeated operations

## Related Documentation

- **[GRAPAPY_IMPLEMENTATION.md](GRAPAPY_IMPLEMENTATION.md)** - GrapaPy Python extension details
- **[TYPE_MAPPING_IMPLEMENTATION.md](TYPE_MAPPING_IMPLEMENTATION.md)** - Type conversion system
- **[FOREIGN_FUNCTION_INTERFACE.md](FOREIGN_FUNCTION_INTERFACE.md)** - Python script execution
- **[PYTHON_INTEGRATION_ARCHITECTURE.md](PYTHON_INTEGRATION_ARCHITECTURE.md)** - Overall architecture
- **[docs-src/docs/python_use_cases.md](../../../docs-src/docs/python_use_cases.md)** - Python integration examples
