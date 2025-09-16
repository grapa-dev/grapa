---
tags:
  - maintainer
  - implementation
  - type-system
  - class-system
  - inheritance
  - dot-notation
  - object-oriented
---

# Class System and Inheritance

This document provides comprehensive implementation details for Grapa's class system, including class structure, inheritance, member storage, and automatic class assignment for dot notation.

## Overview

**Purpose:** Object-oriented programming with inheritance and dot notation
**Implementation:** Class-based system with automatic type-to-class mapping
**Dot Notation:** Every data type automatically gets its class for dot notation
**Status:** Complete class system implementation

## Class System Architecture

### **Core Class Structure**

#### **Class Type Definition**
```cpp
// Class type in GrapaTokenType
GrapaTokenType::CLASS = 24  // Class type identifier
```

#### **Class Storage Structure**
```cpp
class GrapaRuleEvent {
    GrapaValue mValue;           // Token type = GrapaTokenType::CLASS
    GrapaRuleQueue* vQueue;      // Class members and methods
    GrapaRuleEvent* vClass;      // Parent class reference (inheritance)
    GrapaCHAR mName;             // Class name
    bool mClass;                 // Class member flag
    // ... other fields
};
```

### **Class Creation Grammar**

#### **Class Definition Syntax**
```grapa
// Class creation grammar from $grapa.grc
@global["$function"]
    = rule class '(' <$array> ')' '{' <$classlist> '}' {@<class,{null,$6,$3}>}  // Class with inheritance
    | class '(' <$array> ')' {@<class,{null,null,$3}>}                          // Class with inheritance, no body
    | class '{' <$classlist> '}' {@<class,{null,$3}>}                           // Simple class
    ;

@global["$classlist"]
    = rule <$param> ';' <$classlist> {@<prepend,{$3,$1}>}  // Multiple class members
    | <$param> ';' {@<createlist,{$1}>}                     // Single class member with semicolon
    | <$param> {@<createlist,{$1}>}                         // Single class member
    ;
```

#### **Class Usage Examples**
```grapa
/* Simple class definition */
my_class = class {
    name = "default";
    value = 0;
    
    set_name = op(n) { name = n; };
    get_name = op() { name; };
    increment = op() { value = value + 1; };
};

/* Class with inheritance */
derived_class = class (my_class) {
    additional_field = "extra";
    
    new_method = op() { additional_field; };
};

/* Class with parameters */
parameterized_class = class (base_class) (param1, param2) {
    p1 = param1;
    p2 = param2;
    
    process = op() { p1 + p2; };
};
```

## Automatic Class Assignment

### **Type-to-Class Mapping**

Every Grapa data type is automatically assigned its corresponding class for dot notation support:

#### **Built-in Type Classes**
```cpp
// Automatic class assignment in GrapaLibRule.cpp
case GrapaTokenType::CLASS:    item->vClass = GetClass(pNameSpace, GrapaCHAR("$CLASS")); break;
case GrapaTokenType::SYSID:    item->vClass = GetClass(pNameSpace, GrapaCHAR("$SYSID")); break;
case GrapaTokenType::ARRAY:    item->vClass = GetClass(pNameSpace, GrapaCHAR("$LIST")); break;
case GrapaTokenType::TUPLE:    item->vClass = GetClass(pNameSpace, GrapaCHAR("$TUPLE")); break;
case GrapaTokenType::VECTOR:   item->vClass = GetClass(pNameSpace, GrapaCHAR("$VECTOR")); break;
case GrapaTokenType::WIDGET:   item->vClass = GetClass(pNameSpace, GrapaCHAR("$WIDGET")); break;
case GrapaTokenType::LIST:     item->vClass = GetClass(pNameSpace, GrapaCHAR("$GOBJ")); break;
case GrapaTokenType::XML:      item->vClass = GetClass(pNameSpace, GrapaCHAR("$XML")); break;
case GrapaTokenType::EL:       item->vClass = GetClass(pNameSpace, GrapaCHAR("$EL")); break;
case GrapaTokenType::TAG:      item->vClass = GetClass(pNameSpace, GrapaCHAR("$TAG")); break;
case GrapaTokenType::RULE:     item->vClass = GetClass(pNameSpace, GrapaCHAR("$RULE")); break;
case GrapaTokenType::ERR:      item->vClass = GetClass(pNameSpace, GrapaCHAR("$ERR")); break;
case GrapaTokenType::TABLE:    item->vClass = GetClass(pNameSpace, GrapaCHAR("$TABLE")); break;
case GrapaTokenType::ID:       item->vClass = GetClass(pNameSpace, GrapaCHAR("$ID")); break;
case GrapaTokenType::INT:      item->vClass = GetClass(pNameSpace, GrapaCHAR("$INT")); break;
case GrapaTokenType::SYSINT:   item->vClass = GetClass(pNameSpace, GrapaCHAR("$SYSINT")); break;
case GrapaTokenType::STR:      item->vClass = GetClass(pNameSpace, GrapaCHAR("$STR")); break;
case GrapaTokenType::SYSSTR:   item->vClass = GetClass(pNameSpace, GrapaCHAR("$SYSSTR")); break;
case GrapaTokenType::FLOAT:    item->vClass = GetClass(pNameSpace, GrapaCHAR("$FLOAT")); break;
case GrapaTokenType::RAW:      item->vClass = GetClass(pNameSpace, GrapaCHAR("$RAW")); break;
case GrapaTokenType::BOOL:     item->vClass = GetClass(pNameSpace, GrapaCHAR("$BOOL")); break;
case GrapaTokenType::TIME:     item->vClass = GetClass(pNameSpace, GrapaCHAR("$TIME")); break;
case GrapaTokenType::TOKEN:    item->vClass = GetClass(pNameSpace, GrapaCHAR("$TOKEN")); break;
```

#### **Object and Operation Classes**
```cpp
// Object and operation class assignment
case GrapaTokenType::OBJ:      root->vClass = GetClass(pNameSpace, GrapaCHAR("$OBJ")); break;
case GrapaTokenType::OP:       root->vClass = GetClass(pNameSpace, GrapaCHAR("$OP")); break;
```

### **Class Loading System**

#### **GetClass Implementation**
```cpp
GrapaRuleEvent* GrapaScriptState::GetClass(GrapaNames* pNameSpace, const GrapaCHAR &name)
{
    // Thread-safe class loading with locking
    gSystem->mLibLock.WaitCritical();
    
    // Check if class already exists
    GrapaRuleEvent* result = vScriptExec->vScriptState->SearchVariable(pNameSpace, name);
    if (result && !result->mNull && result->mValue.mToken == GrapaTokenType::CLASS)
    {
        gSystem->mLibLock.LeaveCritical();
        return(result);
    }
    
    // Load class from file system
    GrapaCHAR fNameOp(name);
    fNameOp.Append(".grz");  // Compiled class file
    GrapaCHAR fNameTxt(name);
    fNameTxt.Append(".grc"); // Source class file
    
    // Search in path directories
    GrapaRuleEvent* e = (GrapaRuleEvent*)gSystem->mPath ? gSystem->mPath->Head() : NULL;
    while (e)
    {
        // Load from database or file system
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
    
    // Load from static library if not found
    if (setValue.mLength == 0 && gSystem->mStaticLib && gSystem->mStaticLib->mCount)
    {
        GrapaRuleEvent * ev = gSystem->mStaticLib->Search(fNameOp, idx);
        if (ev)
            setValue.FROM(ev->mValue);
        if (setValue.mLength == 0)
        {
            ev = gSystem->mStaticLib->Search(fNameTxt, idx);
            if (ev)
                setValue.FROM(ev->mValue);
        }
    }
    
    // Process and compile class
    if (setValue.mLength)
    {
        // Decompress and parse class definition
        GrapaBYTE expanded;
        GrapaCompress::Expand(setValue, expanded);
        plan = new GrapaRuleEvent();
        plan->vQueue = new GrapaRuleQueue();
        
        // Set class token type
        plan->mValue.mToken = GrapaTokenType::CLASS;
        
        // Process class content
        plan->vClass = ((GrapaRuleQueue*)plan->vQueue)->FROM(vScriptExec->vScriptState, pNameSpace, expanded);
        
        // Execute class definition
        GrapaRuleEvent *r = vScriptExec->ProcessPlan(pNameSpace, plan);
        result = vScriptExec->vScriptState->SearchVariable(pNameSpace, name);
    }
    
    gSystem->mLibLock.LeaveCritical();
    return(result);
}
```

## Inheritance System

### **Inheritance Structure**

#### **Class Hierarchy**
```cpp
// Inheritance is stored in vClass pointer
class GrapaRuleEvent {
    GrapaRuleEvent* vClass;  // Points to parent class
    GrapaRuleQueue* vQueue;  // Current class members
    // ... other fields
};
```

#### **Inheritance Resolution**
```cpp
GrapaRuleEvent* GrapaScriptState::FindClassVar(GrapaNames* pNameSpace, GrapaRuleEvent *r, const GrapaCHAR &s)
{
    if (r == NULL)
        return(NULL);
    
    // Handle ID/SYSID types by loading their class
    switch (r->mValue.mToken)
    {
    case GrapaTokenType::ID:
    case GrapaTokenType::SYSID:
        if (r->vClass == NULL)
        {
            GrapaCHAR nm;
            if (r->mValue.mToken == GrapaTokenType::SYSID || r->mValue.mToken == GrapaTokenType::SYSSTR)
                nm.Append("$");
            nm.Append(r->mValue);
            r->vClass = GetClass(pNameSpace, r->mValue);
        }
        if (r->vClass)
            r = r->vClass;
        break;
    }
    
    // Search in class hierarchy
    switch (r->mValue.mToken)
    {
    case GrapaTokenType::CLASS:
        if (r->vQueue)
        {
            // Search current class members
            GrapaRuleEvent *r2 = (GrapaRuleEvent*)r->vQueue->Head();
            while (r2)
            {
                GrapaRuleEvent *r4 = FindClassVar(pNameSpace, r2, s);
                if (r4)
                {
                    r4->mClass = true;
                    return(r4);
                }
                r2 = r2->Next();
            }
        }
        break;
    default:
        // Search in current object's members
        if (r->vQueue)
            return ((GrapaRuleQueue*)r->vQueue)->Search(s, idx);
        break;
    }
    return(NULL);
}
```

### **Inheritance Examples**

#### **Simple Inheritance**
```grapa
/* Base class */
base_class = class {
    name = "base";
    value = 0;
    
    get_name = op() { name; };
    set_name = op(n) { name = n; };
};

/* Derived class */
derived_class = class (base_class) {
    additional = "extra";
    
    get_full_info = op() { name + " " + additional; };
};

/* Usage */
obj = derived_class();
obj.get_name();        // Inherited from base_class
obj.get_full_info();   // Defined in derived_class
```

#### **Multiple Inheritance**
```grapa
/* Multiple base classes */
mixin1 = class {
    method1 = op() { "mixin1"; };
};

mixin2 = class {
    method2 = op() { "mixin2"; };
};

/* Combined class */
combined = class (mixin1, mixin2) {
    combined_method = op() { method1() + " " + method2(); };
};
```

## Dot Notation System

### **Dot Notation Implementation**

#### **Grammar Rules**
```grapa
// Dot notation grammar from $grapa.grc
@global["$comppart"]
    = rule '[' <$comp> ']' <$comppart> {@<prepend,{$4,$2}>}  // Array indexing
    | '[' <$comp> ']' {@<createlist,{$2}>}                    // Array indexing
    | '[' ']' <$comppart> {@<prepend,{$3,null}>}             // Array indexing
    | '[' ']' {@<createlist,{null}>}                          // Array indexing
    | '.' <$compName> <$comppart> {@<prepend,{$3,$2}>}       // Dot notation
    | '.' <$compName> {@<createlist,{$2}>}                    // Dot notation
    ;

@global["$compL3"]
    = rule <$lit> <$comppart> {@<search,{$1,$2}>}            // Object member access
    | <$function> <$comppart> {@<search,{$1,$2}>}            // Function member access
    | <$function>                                             // Direct function call
    ;
```

#### **Dot Notation Resolution**
```cpp
// Dot notation resolution in SearchVariable
GrapaRuleEvent* GrapaScriptState::SearchVariable(GrapaNames* pNameSpace, GrapaRuleEvent *pValue)
{
    GrapaRuleEvent* parameter = NULL;
    
    while (!parameter && names)
    {
        GrapaRuleEvent* operation = names->GetNameQueue()->Tail();
        while (operation)
        {
            GrapaRuleEvent* op = operation;
            while (op->mValue.mToken == GrapaTokenType::PTR && op->vRulePointer) 
                op = op->vRulePointer;
            
            switch (pValue->mValue.mToken)
            {
            case GrapaTokenType::ID:
            case GrapaTokenType::REF:
            case GrapaTokenType::STR:
                // Search in current namespace
                parameter = (GrapaRuleEvent*)(op->vQueue ? op->vQueue->Search(pValue->mValue, idx) : NULL);
                
                // If not found and object has class, search in class
                if (parameter == NULL && op->mValue.mToken == GrapaTokenType::OBJ && op->vClass)
                    parameter = FindClassVar(pNameSpace, op->vClass, pValue->mValue);
                break;
            }
            operation = operation->Prev();
        }
        names = names->GetSkipThis();
    }
    return parameter;
}
```

### **Dot Notation Examples**

#### **Built-in Type Methods**
```grapa
/* String methods */
text = "Hello World";
length = text.len();           // String length
upper = text.upper();          // Convert to uppercase
parts = text.split(" ");       // Split into array

/* Array methods */
numbers = [1, 2, 3, 4, 5];
count = numbers.len();         // Array length
first = numbers.get(0);        // Get first element
numbers += 6;             /* Add element */

/* Integer methods */
large_num = 123456789;
hex_val = large_num.hex();     // Convert to hex
binary = large_num.bin();      // Convert to binary
```

#### **Custom Class Methods**
```grapa
/* Custom class with methods */
calculator = class {
    result = 0;
    
    add = op(x) { result = result + x; };
    subtract = op(x) { result = result - x; };
    multiply = op(x) { result = result * x; };
    divide = op(x) { result = result / x; };
    get_result = op() { result; };
    clear = op() { result = 0; };
};

/* Usage with dot notation */
calc = calculator();
calc.add(5);
calc.multiply(3);
final_result = calc.get_result();  // 15
```

## Member Storage and Access

### **Class Member Structure**

#### **Member Storage**
```cpp
// Class members are stored in GrapaRuleQueue
class GrapaRuleEvent {
    GrapaRuleQueue* vQueue;  // Contains class members
    // Each member is a GrapaRuleEvent with:
    // - mName: Member name
    // - mValue: Member value
    // - vClass: Member's class (for methods)
    // - mClass: true if it's a class member
};
```

#### **Member Types**
```grapa
/* Class with different member types */
example_class = class {
    // Data members
    name = "example";
    value = 42;
    list = [1, 2, 3];
    
    // Method members
    get_name = op() { name; };
    set_value = op(v) { value = v; };
    process_list = op() { list.map(op(x) { x * 2; }); };
    
    // Computed properties
    computed = op() { name + "_" + value; };
};
```

### **Member Access Patterns**

#### **Direct Access**
```grapa
/* Direct member access */
obj = example_class();
obj.name;           // Access data member
obj.get_name();     // Call method
obj.computed();     // Call computed property
```

#### **Dynamic Access**
```grapa
/* Dynamic member access */
member_name = "get_name";
result = obj.(member_name)();  // Dynamic method call

property = "name";
value = obj.(property);        // Dynamic property access
```

## Class System Integration

### **Database Integration**

#### **Class Storage in Database**
```grapa
/* Store objects with classes in database */
table = $TABLE().create("objects", {
    "id": $INT,
    "name": $STR,
    "data": $OBJ,      // Objects with classes
    "class_name": $STR
});

/* Insert object with class */
my_obj = my_class();
table.insert({
    "id": 1,
    "name": "test",
    "data": my_obj,
    "class_name": "my_class"
});
```

### **Network Integration**

#### **Class Serialization**
```grapa
/* Serialize objects with classes */
data = {
    "type": "my_class",
    "object": my_obj
};

json_data = data.json();
response = $net().post("https://api.example.com/objects", json_data);
```

### **Python Integration**

#### **Class Mapping**
```python
# Python side - Grapa objects with classes
import grapapy

# Create Grapa object with class
grapa_obj = grapa.eval("""
my_class = class {
    name = "test";
    get_name = op() { name; };
};
my_class()
""")

# Access class methods from Python
name = grapa_obj.get_name()
```

## Performance Characteristics

### **Class System Performance**
| Operation | Complexity | Memory | Notes |
|-----------|------------|--------|-------|
| Class Loading | O(1) | Dynamic | Cached after first load |
| Member Access | O(n) | Fixed | Linear search in class |
| Inheritance | O(depth) | Fixed | Depth of inheritance chain |
| Dot Notation | O(n) | Fixed | Member lookup in class |
| Method Call | O(1) | Fixed | Direct method execution |

### **Memory Usage**
- **Class Definition**: Stored once per class
- **Object Instances**: Share class definition
- **Member Storage**: Each object has its own member values
- **Inheritance**: Parent classes cached and shared

## Error Handling

### **Class System Errors**
```grapa
/* Handle missing class errors */
try {
    result = obj.non_existent_method();
} catch (error) {
    // Handle method not found
    error_message = error.message();
}

/* Handle inheritance errors */
try {
    derived = class (non_existent_base) {
        // Class definition
    };
} catch (error) {
    // Handle base class not found
    error_message = error.message();
}
```

### **Dot Notation Errors**
```grapa
/* Handle dot notation errors */
try {
    value = obj.missing_property;
} catch (error) {
    // Handle property not found
    error_message = error.message();
}
```

## Future Enhancements

### **Planned Features**
- **Method Overriding**: Enhanced inheritance with method overriding
- **Private Members**: Access control for class members
- **Static Members**: Class-level static members and methods
- **Interfaces**: Interface definitions and implementation
- **Generics**: Generic class support with type parameters

### **Performance Improvements**
- **Member Caching**: Cache frequently accessed members
- **Method Inlining**: Inline simple method calls
- **Class Optimization**: Optimize class loading and caching
- **Inheritance Optimization**: Optimize inheritance chain resolution

## Related Documentation

- **[TYPE_DEFINITIONS_AND_CLASSIFICATIONS.md](TYPE_DEFINITIONS_AND_CLASSIFICATIONS.md)** - Complete type definitions
- **[TYPE_SYSTEM_ARCHITECTURE.md](TYPE_SYSTEM_ARCHITECTURE.md)** - Type system architecture
- **[../LANGUAGE_FEATURES/UNIFIED_DOT_NOTATION_SYSTEM.md](../LANGUAGE_FEATURES/UNIFIED_DOT_NOTATION_SYSTEM.md)** - Dot notation system
- **[../PYTHON_INTEGRATION/MAINPY_IMPLEMENTATION.md](../PYTHON_INTEGRATION/MAINPY_IMPLEMENTATION.md)** - Python class integration
