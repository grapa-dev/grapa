# Extension System Implementation

## Overview

Grapa's extension system allows developers to extend both individual objects and classes with new methods and properties. The system is built on Grapa's fundamental architecture where everything is an object with a class, and classes can have definitions for variables and members separate from objects.

## Core Concepts

### **Object vs Class Inheritance**
- **Objects** can have their own variables and members, but still inherit from their class
- **Classes** define the template for objects, including default variables and methods
- **System Classes** (like `$ARRAY`, `$LIST`) are loaded from `$STATICLIB` and can be extended

### **Extension Operators**
- **`+=`** - Adds a single item to an object or class
- **`++=`** - Adds multiple items (list) to an object or class

## Implementation Patterns

### **Pattern 1: Extending Individual Objects**

Objects can be extended directly with new methods and properties:

```grapa
/* Create a basic object */
test1 = {n:0};

/* Add a method to the object */
test1 += (inc:op(inc:1){n+=inc;});

/* Use the extended object */
test1.inc();      /* n becomes 1 */
test1.inc(2);     /* n becomes 3 */
```

**Key Points:**
- The object maintains its original structure
- New methods are added to the object instance
- Methods can access and modify object properties
- The extension is specific to this object instance

### **Pattern 2: Creating Custom Classes**

Developers can define custom classes that inherit from system classes:

```grapa
/* Define a class that inherits from $LIST */
myclass = class ($LIST) {
    n = 0; 
    inc = op(inc:1){n+=inc;};
};

/* Extend the class with additional methods */
myclass ++= {
    m:0, 
    dec:op(dec:1){m=n; n-=dec;}
};

/* Create instances of the custom class */
test2 = myclass();
test2.inc();      /* n becomes 1 */
test2.inc(1);     /* n becomes 2 */
test2.dec(5);     /* n becomes -3, m becomes 2 */
```

**Key Points:**
- Custom classes inherit from system classes
- Class definitions provide default values and methods
- Classes can be extended incrementally with `++=`
- All instances of the class get the extended functionality

### **Pattern 3: Extending System Classes**

System classes can be extended globally by loading them into the namespace:

```grapa
/* Load the $ARRAY class into the namespace */
$ARRAY();

/* Extend the system class */
@$ARRAY ++= {f:op(){55}};
@$ARRAY += (c:op(){$this.len();});

/* Use the extended system class */
ar = [1,2,4];
ar.f();           /* Returns 55 */
ar.c();           /* Returns 3 (length) */
```

**Key Points:**
- System classes must be loaded into the namespace before modification
- Use `()` to trigger loading: `$ARRAY()`
- Extensions apply to all future instances of the class
- Both `+=` and `++=` work on loaded system classes

## Technical Implementation Details

### **Class Loading Mechanism**
- System classes are stored in `$STATICLIB`
- Calling `$CLASS()` triggers a search and loads the class into the global namespace
- Once loaded, the class can be modified with `+=` and `++=`

### **Inheritance Chain**
```
Object Instance → Custom Class → System Class → Base Class
```

### **Method Resolution**
1. Check object instance for method
2. Check object's class for method
3. Check inherited classes (in order)
4. Return error if method not found

### **Property Access**
- Object properties override class properties
- Class properties provide default values
- Properties can be accessed and modified by methods

## Usage Guidelines

### **When to Use Each Pattern**

**Pattern 1 (Object Extension):**
- Adding functionality to a specific object instance
- Temporary modifications for testing
- Object-specific behavior

**Pattern 2 (Custom Classes):**
- Creating reusable templates
- Defining domain-specific types
- Building application frameworks

**Pattern 3 (System Class Extension):**
- Adding global functionality to system types
- Creating utility methods for all instances
- Extending core language capabilities

### **Best Practices**
- Use `+=` for single additions, `++=` for multiple additions
- Load system classes before extending them
- Consider the impact of global extensions
- Document custom class hierarchies

## Status: ✅ COMPLETE

The Extension System is fully implemented and functional. All three patterns work correctly and provide comprehensive extension capabilities for Grapa developers.

**Phase 2 Extension System Item**: ✅ **RESOLVED**
- `+=` syntax works for objects and loaded classes
- `++=` syntax works for adding multiple items
- System classes can be extended via loading mechanism
- Custom classes provide inheritance and extension capabilities
