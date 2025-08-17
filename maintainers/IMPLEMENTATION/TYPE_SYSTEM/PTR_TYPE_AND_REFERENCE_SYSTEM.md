---
tags:
  - maintainer
  - implementation
  - type-system
  - ptr-type
  - reference-system
  - parameter-passing
  - memory-management
---

# PTR Type and Reference System

This document provides comprehensive implementation details for Grapa's PTR type, including its purpose for parameter passing, avoiding data copying, and returning through multiple calls with special memory management considerations.

## Overview

**Purpose:** Reference-based parameter passing and data sharing
**Implementation:** Pointer-based reference system with automatic dereferencing
**Memory Management:** Special care required for multi-call scenarios
**Status:** Complete PTR type implementation

## PTR Type Architecture

### **Core PTR Structure**

#### **PTR Type Definition**
```cpp
// PTR type in GrapaTokenType
GrapaTokenType::PTR = 35  // PTR type identifier (from GrapaValue.h)
```

#### **PTR Storage Structure**
```cpp
class GrapaObjectEvent {
    GrapaValue mValue;           // Token type = GrapaTokenType::PTR
    GrapaRuleEvent* vRulePointer; // Points to the actual data
    GrapaCHAR mName;             // PTR name
    bool mVar;                   // Variable flag for PTR
    bool mLocal;                 // Local scope flag
    // ... other fields
};
```

#### **PTR Creation and Usage**
```cpp
// PTR creation in GrapaLibraryRuleVarEvent::Run
result = new GrapaRuleEvent();
result->mValue.mToken = GrapaTokenType::PTR;
result->mLocal = parameter->mLocal;
result->vRulePointer = parameter;  // Point to actual data
result->mVar = true;               // Mark as variable reference
```

## PTR Type Purposes

### **1. Parameter Passing with Modification**

#### **Purpose: Allow Destination to Modify Source**
PTR types enable functions to modify the original data passed as parameters:

```cpp
// From GrapaLibRule.cpp - PTR parameter handling
GrapaRuleEvent* GrapaLibraryRuleVarEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    // Create PTR to allow modification of original variable
    result = new GrapaRuleEvent();
    result->mValue.mToken = GrapaTokenType::PTR;
    result->mLocal = parameter->mLocal;
    result->vRulePointer = parameter;  // Point to original variable
    result->mVar = true;               // Mark as modifiable reference
}
```

#### **Assignment Operations with PTR**
```cpp
// From ItemAssignRun - PTR assignment handling
if (rDel && rDel->mValue.mToken == GrapaTokenType::PTR)
{
    // Special handling for PTR assignments to avoid copying
    if (GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignappend") != 0 && 
        GrapaMem::StrCmp((char*)pName.mBytes, pName.mLength, "assignextend") != 0)
    {
        // Copy the PTR target instead of the PTR itself
        GrapaRuleEvent* rold = rDel;
        r = rDel = vScriptExec->CopyItem(r);
        rold->CLEAR();
        delete rold;
    }
}
```

### **2. Avoiding Data Copying**

#### **Purpose: Efficient Data Sharing**
PTR types avoid expensive data copying by sharing references:

```cpp
// From GrapaLibRule.cpp - PTR creation for complex types
switch (p1.vVal->mValue.mToken)
{
case GrapaTokenType::ARRAY:
case GrapaTokenType::TUPLE:
case GrapaTokenType::LIST:
case GrapaTokenType::XML:
case GrapaTokenType::EL:
case GrapaTokenType::TAG:
case GrapaTokenType::OP:
case GrapaTokenType::CODE:
case GrapaTokenType::OBJ:
    // Create PTR instead of copying large data structures
    result = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
    result->mName.FROM(p1.vVal->mName);
    result->vRulePointer = p1.vVal;  // Share reference
    break;
default:
    // Copy simple types
    result = vScriptExec->CopyItem(p1.vVal);
    break;
}
```

#### **Performance Benefits**
```cpp
// PTR vs Copy performance comparison
// PTR: O(1) - just pointer assignment
result = new GrapaRuleEvent(GrapaTokenType::PTR, 0, "", "");
result->vRulePointer = largeDataStructure;

// Copy: O(n) - full data structure copy
result = vScriptExec->CopyItem(largeDataStructure);
```

### **3. Returning Through Multiple Calls**

#### **Purpose: Reference Chain Management**
PTR types can be returned through multiple function calls, but require special care:

```cpp
// From GrapaLibRule.cpp - Multi-call PTR handling
if (result && result->mValue.mToken == GrapaTokenType::PTR || 
    (result->mValue.mToken != GrapaTokenType::STR && 
     result->mValue.mToken != GrapaTokenType::RAW && 
     result->mValue.mToken != GrapaTokenType::TABLE))
{
    // Special care for PTR types in multi-call scenarios
    if (!(result->mValue.mToken == GrapaTokenType::PTR && 
          result->mVar && 
          result->vRulePointer->mValue.mToken == GrapaTokenType::WIDGET))
    {
        // Copy at the place where original data resides
        GrapaRuleEvent* v = vScriptExec->CopyItem(result);
        result->CLEAR();
        delete result;
        result = v;
    }
}
```

#### **Memory Management Strategy**
```cpp
// Critical: Copy PTR data when original may be deleted
if (result->mValue.mToken == GrapaTokenType::PTR && result->mVar)
{
    // Check if original data is in current scope
    if (operation->vQueue->HasEvent(result->vRulePointer) || 
        result->vRulePointer == operation)
    {
        // Copy to prevent invalid PTR after scope exit
        GrapaRuleEvent* v = vScriptExec->CopyItem(result);
        result->CLEAR();
        delete result;
        result = v;
    }
}
```

## PTR Dereferencing System

### **Automatic PTR Dereferencing**

#### **Dereferencing Implementation**
```cpp
// From GrapaState.cpp - PTR dereferencing
while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) 
    op = op->vRulePointer;

// From GrapaLibRule.cpp - PTR dereferencing
while (parameter && parameter->mValue.mToken == GrapaTokenType::PTR) 
    parameter = parameter->vRulePointer;
```

#### **Dereferencing in Search Operations**
```cpp
// From SearchVariable - PTR handling in variable search
GrapaRuleEvent* op = operation;
while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) 
    op = op->vRulePointer;

// Search in dereferenced object
parameter = (GrapaRuleEvent*)(op->vQueue ? op->vQueue->Search(pValue->mValue, idx) : NULL);
```

### **PTR Chain Resolution**

#### **Multi-level PTR Resolution**
```cpp
// From GrapaEncode.cpp - Multi-level PTR resolution
while (e2 && e2->mValue.mToken == GrapaTokenType::PTR && e2->vRulePointer) 
    e2 = e2->vRulePointer;

// From GrapaLibRule.cpp - PTR chain handling
while (r && r->mValue.mToken == GrapaTokenType::PTR && r->vRulePointer) 
    r = r->vRulePointer;
```

## PTR Serialization and Deserialization

### **PTR Serialization**
```cpp
// From GrapaState.cpp - PTR serialization
case GrapaTokenType::PTR:
    size = 0;
    if (vRulePointer) vRulePointer->TOSize(size);
    pSize += (sizeof(u64) + 1 + sizeof(u64) + (sizeof(u64) + mName.mLength) + (sizeof(u64) + size));
    break;
```

### **PTR Deserialization**
```cpp
// From GrapaState.cpp - PTR deserialization
case GrapaTokenType::PTR:
    if (vRulePointer) vRulePointer->TO(pValue);
    break;
```

## PTR Usage Patterns

### **1. Variable References**

#### **Creating Variable References**
```grapa
/* Variable reference creation */
x = 42;
ref_x = @x;  /* Creates PTR to x */

/* Modifying through reference */
ref_x = 100;  /* Modifies original x */
x;  /* Returns 100 */
```

#### **Function Parameter References**
```grapa
/* Function with reference parameter */
modify_value = op(ref_param) {
    ref_param = ref_param * 2;  /* Modifies original */
};

/* Usage */
original = 5;
modify_value(@original);  /* Pass reference */
original;  /* Returns 10 */
```

### **2. Complex Data Structure References**

#### **Array References**
```grapa
/* Array reference */
data = [1, 2, 3, 4, 5];
ref_data = @data;  /* PTR to array */

/* Modifying through reference */
ref_data += 6;  /* Modifies original array */
data;  /* Returns [1, 2, 3, 4, 5, 6] */
```

#### **Object References**
```grapa
/* Object reference */
my_obj = class {
    value = 42;
    name = "test";
};

ref_obj = @my_obj;  /* PTR to object */
ref_obj.value = 100;  /* Modifies original object */
my_obj.value;  /* Returns 100 */
```

### **3. Multi-call Reference Chains**

#### **Returning References Through Functions**
```grapa
/* Function returning reference */
get_reference = op() {
    data = [1, 2, 3];
    @data;  /* Return PTR to data */
};

/* Multi-call usage */
ref1 = get_reference();
ref2 = ref1;  /* PTR to same data */
ref1 += 4;
ref2;  /* Returns [1, 2, 3, 4] */
```

## Memory Management Considerations

### **Critical Memory Management Rules**

#### **1. Copy at Original Location**
```cpp
// Critical rule: Copy PTR data where original resides
if (result->mValue.mToken == GrapaTokenType::PTR && result->mVar)
{
    // Check if original data is in current scope
    if (operation->vQueue->HasEvent(result->vRulePointer))
    {
        // Copy to prevent invalid PTR after scope exit
        GrapaRuleEvent* v = vScriptExec->CopyItem(result);
        result->CLEAR();
        delete result;
        result = v;
    }
}
```

#### **2. Scope-Aware PTR Handling**
```cpp
// From GrapaLibRule.cpp - Scope-aware PTR management
if (!(result->mValue.mToken == GrapaTokenType::PTR && 
      result->mVar && 
      result->vRulePointer->mValue.mToken == GrapaTokenType::WIDGET))
{
    // Copy PTR data for non-widget types
    GrapaRuleEvent* v = vScriptExec->CopyItem(result);
    result->CLEAR();
    delete result;
    result = v;
}
```

#### **3. PTR Validation**
```cpp
// Validate PTR before use
if (ptr && ptr->mValue.mToken == GrapaTokenType::PTR && ptr->vRulePointer)
{
    // Valid PTR - dereference and use
    actual_data = ptr->vRulePointer;
} else {
    // Invalid PTR - handle error
    // Error handling code
}
```

### **Memory Leak Prevention**

#### **PTR Cleanup**
```cpp
// Proper PTR cleanup
void GrapaRuleEvent::CLEAR()
{
    // Clear PTR-specific data
    if (mValue.mToken == GrapaTokenType::PTR)
    {
        vRulePointer = NULL;  // Don't delete - just clear reference
    }
    // ... other cleanup
}
```

#### **Circular Reference Prevention**
```cpp
// Check for circular references
static bool ItemAssignCheck(GrapaRuleEvent* parameter, GrapaRuleEvent* r)
{
    if (!r) return(false);
    if (parameter == r) return(true);
    if (parameter == r->vRulePointer) return(true);
    // ... recursive check
    return(true);
}
```

## Performance Characteristics

### **PTR Performance**
| Operation | Complexity | Memory | Notes |
|-----------|------------|--------|-------|
| PTR Creation | O(1) | Fixed | Just pointer assignment |
| PTR Dereferencing | O(depth) | Fixed | Depth of PTR chain |
| PTR Assignment | O(1) | Fixed | Pointer copy |
| PTR Validation | O(1) | Fixed | Null check |
| PTR Copy | O(n) | Dynamic | Full data copy when needed |

### **Memory Usage**
- **PTR Overhead**: Minimal (just pointer + type info)
- **Dereferencing**: No additional memory
- **Copy Operations**: Full data size when copying required
- **Chain Resolution**: Stack space for recursion

## Error Handling

### **PTR Error Scenarios**
```grapa
/* Invalid PTR access */
try {
    invalid_ptr = @non_existent_variable;
    value = invalid_ptr;  // Error: Invalid PTR
} catch (error) {
    // Handle PTR error
    error_message = error.message();
}

/* Null PTR handling */
try {
    null_ptr = null;
    ref_null = @null_ptr;  // Error: Cannot reference null
} catch (error) {
    // Handle null PTR error
    error_message = error.message();
}
```

### **PTR Validation**
```cpp
// PTR validation in code
if (ptr && ptr->mValue.mToken == GrapaTokenType::PTR)
{
    if (ptr->vRulePointer)
    {
        // Valid PTR - proceed
        actual_data = ptr->vRulePointer;
    } else {
        // Null PTR - handle error
        // Error handling
    }
} else {
    // Not a PTR - handle error
    // Error handling
}
```

## Integration with Other Systems

### **Database Integration**
```grapa
/* PTR in database operations */
table = $TABLE().create("references", {
    "id": $INT,
    "data": $OBJ,
    "ref_data": $PTR  // PTR to data
});

/* Store PTR reference */
my_data = [1, 2, 3];
table.insert({
    "id": 1,
    "data": my_data,
    "ref_data": @my_data  // Store PTR reference
});
```

### **Network Integration**
```grapa
/* PTR serialization for network */
data = {
    "original": [1, 2, 3],
    "reference": @data.original  // PTR reference
};

/* Serialize with PTR handling */
json_data = data.json();  // PTR gets serialized as reference
response = $net().post("https://api.example.com/data", json_data);
```

### **Python Integration**
```python
# Python side - PTR handling
import grapapy

# Create Grapa PTR
grapa_ptr = grapa.eval("""
x = [1, 2, 3];
ref_x = @x;
ref_x
""")

# Access PTR from Python
original_data = grapa_ptr.vRulePointer  # Access pointed data
```

## Future Enhancements

### **Planned PTR Improvements**
- **Smart PTR**: Automatic memory management
- **Weak PTR**: Non-owning references
- **Shared PTR**: Reference counting
- **PTR Validation**: Compile-time PTR validation
- **PTR Debugging**: Enhanced PTR debugging tools

### **Performance Optimizations**
- **PTR Caching**: Cache frequently accessed PTR targets
- **Lazy Dereferencing**: Defer PTR resolution until needed
- **PTR Pooling**: Reuse PTR objects
- **Compile-time PTR Analysis**: Optimize PTR usage patterns

## Related Documentation

- **[TYPE_DEFINITIONS_AND_CLASSIFICATIONS.md](TYPE_DEFINITIONS_AND_CLASSIFICATIONS.md)** - Complete type definitions
- **[TYPE_SYSTEM_ARCHITECTURE.md](TYPE_SYSTEM_ARCHITECTURE.md)** - Type system architecture
- **[CLASS_SYSTEM_AND_INHERITANCE.md](CLASS_SYSTEM_AND_INHERITANCE.md)** - Class system with PTR support
- **[../PYTHON_INTEGRATION/MAINPY_IMPLEMENTATION.md](../PYTHON_INTEGRATION/MAINPY_IMPLEMENTATION.md)** - Python PTR integration
