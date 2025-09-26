---
tags:
  - user
  - highlevel
  - api
  - types
---

# $CLASS Type Reference

> **Class System:** Grapa provides a sophisticated object-oriented programming system with classes, inheritance, and copy-on-write semantics for instance variables.

## Table of Contents
- [Class Definition](#class-definition)
- [Class Inheritance](#class-inheritance)
- [Instance Creation](#instance-creation)
- [Variable Scoping and Copy-on-Write](#variable-scoping-and-copy-on-write)
- [Method Inheritance](#method-inheritance)
- [Inheritance Precedence](#inheritance-precedence)
- [System Classes vs User Classes](#system-classes-vs-user-classes)
- [Dynamic Class Creation with `obj`](#dynamic-class-creation-with-obj)
- [Examples](#examples)

---

## Class Definition

Classes in Grapa are defined using the `class` keyword with optional inheritance:

```grapa
/* Basic class definition */
MyClass = class ($OBJ) {
    /* Class-level variables */
    i = 1;
    name = "default";
    
    /* Class-level methods */
    f = op() { i; };
    greet = op() { "Hello, " + name; };
};

/* Class without inheritance */
SimpleClass = class {
    value = 42;
    getValue = op() { value; };
};
```

**Key Features:**
- **Class-level variables**: Shared by all instances until modified
- **Class-level methods**: Defined once and inherited by all instances
- **Optional inheritance**: Can inherit from other classes (including system classes)
- **Dynamic modification**: Classes can be modified at runtime

---

## Class Inheritance

Grapa supports single and multiple inheritance:

```grapa
/* Single inheritance */
BaseClass = class ($OBJ) {
    baseValue = 10;
    baseMethod = op() { "Base: " + baseValue; };
};

DerivedClass = class (BaseClass) {
    derivedValue = 20;
    derivedMethod = op() { "Derived: " + derivedValue; };
};

/* Multiple inheritance */
Mixin1 = class {
    mixin1Method = op() { "Mixin1"; };
};

Mixin2 = class {
    mixin2Method = op() { "Mixin2"; };
};

MultiClass = class (BaseClass, Mixin1, Mixin2) {
    multiValue = 30;
};
```

**Inheritance Rules:**
- **Method Resolution Order**: Closer classes in inheritance chain override more distant ones
- **Variable Inheritance**: All class-level variables are inherited
- **Method Inheritance**: All methods are inherited and can be overridden
- **System Class Inheritance**: Can inherit from system classes like `$OBJ`, `$LIST`, etc.

---

## Instance Creation

Instances are created by calling the class as a function:

```grapa
/* Create instances */
obj1 = MyClass();
obj2 = MyClass();
derived = DerivedClass();
multi = MultiClass();

/* Check instance types */
obj1.type();        /* Returns: "MyClass" */
derived.type();     /* Returns: "DerivedClass" */
multi.type();       /* Returns: "MultiClass" */
```

**Instance Properties:**
- **Type**: Instance type matches the class name
- **Initial State**: Initially empty `{}` - uses class variables
- **Independent**: Each instance is independent of others
- **Class Reference**: Maintains reference to class definition

---

## Variable Scoping and Copy-on-Write

Grapa implements sophisticated copy-on-write semantics for instance variables:

```grapa
/* Class definition */
MyClass = class ($OBJ) {
    i = 1;                    /* Class-level variable */
    f = op() { i; };          /* Class-level method */
};

/* Create instance */
x = MyClass();

/* Initially uses class variable */
x.f();                        /* Returns: 1 (from class) */
x;                            /* Returns: {} (no instance variables yet) */

/* Modifying creates instance copy */
x.i += 1;                     /* Copies i to instance, then increments */

/* Now uses instance variable */
x.f();                        /* Returns: 2 (from instance) */
x;                            /* Returns: {"i": 2} (instance variable created) */

/* Class definition unchanged */
MyClass;                      /* Still shows: class ($OBJ) {i = 1; f = @<[op,@<var,{i}>],{}>; }; */
```

**Copy-on-Write Behavior:**
- **Shared by Default**: All instances share class-level variables
- **Copy on Modification**: Instance variables created only when modified
- **Method Context**: Methods automatically use appropriate variable context
- **Memory Efficiency**: Unmodified variables remain shared at class level
- **Class Isolation**: Class definition remains unchanged

---

## Method Inheritance

Methods inherit from the class definition but access the appropriate variable context:

```grapa
/* Class with methods */
Counter = class ($OBJ) {
    count = 0;
    increment = op() { count += 1; };
    getCount = op() { count; };
    reset = op() { count = 0; };
};

/* Create instances */
counter1 = Counter();
counter2 = Counter();

/* Both use class method but different instance variables */
counter1.increment();         /* Uses class method, creates instance count */
counter1.increment();         /* Uses class method, modifies instance count */
counter1.getCount();          /* Returns: 2 (from instance) */

counter2.increment();         /* Uses class method, creates separate instance count */
counter2.getCount();          /* Returns: 1 (from separate instance) */

/* Class method definition unchanged */
Counter;                      /* Method definitions remain in class */
```

**Method Inheritance Features:**
- **Shared Definition**: All instances use the same method definition
- **Instance Context**: Methods access instance variables when they exist
- **Class Context**: Methods fall back to class variables when instance variables don't exist
- **Dynamic Resolution**: Variable resolution happens at method execution time

---

## Inheritance Precedence

When multiple classes are inherited, closer classes in the inheritance chain take precedence:

```grapa
/* Base class */
Base = class ($OBJ) {
    value = "base";
    method = op() { "Base method: " + value; };
};

/* Intermediate class */
Intermediate = class (Base) {
    value = "intermediate";
    method = op() { "Intermediate method: " + value; };
};

/* Derived class */
Derived = class (Intermediate) {
    value = "derived";
    method = op() { "Derived method: " + value; };
};

/* Create instance */
obj = Derived();

/* Method resolution follows inheritance chain */
obj.method();                 /* Returns: "Derived method: derived" */
obj.value;                    /* Returns: "derived" (from closest class) */
```

**Precedence Rules:**
- **Closest First**: Classes closer to the instance override more distant ones
- **Left to Right**: In multiple inheritance, leftmost classes take precedence
- **Method Override**: Methods can be overridden at any level
- **Variable Override**: Variables can be overridden at any level

---

## System Classes vs User Classes

Grapa distinguishes between system classes (names starting with "$") and user classes:

```grapa
/* User class - inherits full $OBJ functionality */
MyClass = class ($OBJ) {
    data = "user data";
    getData = op() { data; };
};

obj = MyClass();
obj.set("key", "value");      /* Works - has universal .set()/.get() methods */
obj.getfield("key", "value"); /* Works - has flexible .getfield()/.setfield() methods */

/* System classes have specialized implementations */
file = $file();
file.setfield("key", "value"); /* Works - specialized file system operations */
/* file.setfield(0, "value");  /* Would NOT work - no index support */

table = {}.table("ROW");
table.setfield("key", "field", "value"); /* Works - specialized database operations */
/* table.setfield(0, "field", "value");  /* Would NOT work - no index support */
```

**System Class Characteristics:**
- **Names start with "$"**: `$file`, `$TABLE`, `$OBJ`, etc.
- **Specialized implementations**: Override universal methods for specific data structures
- **C++ routing**: System classes bypass universal `$OBJ` processing
- **Limited flexibility**: May not support all universal method features

**User Class Characteristics:**
- **Names don't start with "$"**: `MyClass`, `Counter`, etc.
- **Universal functionality**: Inherit full `$OBJ` method capabilities
- **Flexible methods**: Support mix/match of names and indices
- **Standard inheritance**: Use normal class inheritance mechanisms

---

## Examples

### Complete Class System Example

```grapa
/* Base class for all vehicles */
Vehicle = class ($OBJ) {
    wheels = 4;
    speed = 0;
    maxSpeed = 100;
    
    accelerate = op(amount) {
        speed += amount;
        if (speed > maxSpeed) {
            speed = maxSpeed;
        };
        speed;
    };
    
    brake = op(amount) {
        speed -= amount;
        if (speed < 0) {
            speed = 0;
        };
        speed;
    };
    
    getInfo = op() {
        "Vehicle: " + wheels + " wheels, speed: " + speed + "/" + maxSpeed;
    };
};

/* Car inherits from Vehicle */
Car = class (Vehicle) {
    doors = 4;
    fuel = 100;
    
    refuel = op(amount) {
        fuel += amount;
        if (fuel > 100) {
            fuel = 100;
        };
        fuel;
    };
    
    getInfo = op() {
        Vehicle.getInfo() + ", doors: " + doors + ", fuel: " + fuel + "%";
    };
};

/* Motorcycle inherits from Vehicle */
Motorcycle = class (Vehicle) {
    wheels = 2;
    doors = 0;
    maxSpeed = 150;
    
    wheelie = op() {
        if (speed > 20) {
            "Doing a wheelie at " + speed + " mph!";
        } else {
            "Need more speed for a wheelie!";
        };
    };
};

/* Create instances */
myCar = Car();
myBike = Motorcycle();

/* Demonstrate inheritance and copy-on-write */
myCar.accelerate(50);         /* Uses inherited method, creates instance speed */
myCar.refuel(25);             /* Uses Car method, creates instance fuel */
myCar.getInfo();              /* Returns: "Vehicle: 4 wheels, speed: 50/100, doors: 4, fuel: 125%" */

myBike.accelerate(80);        /* Uses inherited method, creates separate instance speed */
myBike.wheelie();             /* Uses Motorcycle method */
myBike.getInfo();             /* Returns: "Vehicle: 2 wheels, speed: 80/150" */

/* Class definitions unchanged */
Vehicle;                      /* Original definition preserved */
Car;                          /* Original definition preserved */
```

### Mixin Pattern Example

```grapa
/* Mixin for logging functionality */
Loggable = class {
    log = op(message) {
        $time() + ": " + message;
    };
    
    logError = op(error) {
        log("ERROR: " + error);
    };
};

/* Mixin for validation functionality */
Validatable = class {
    validate = op(data) {
        if (data == null || data == "") {
            false;
        } else {
            true;
        };
    };
    
    validateAndLog = op(data) {
        if (validate(data)) {
            log("Validation passed: " + data);
            true;
        } else {
            logError("Validation failed: " + data);
            false;
        };
    };
};

/* Class using multiple mixins */
DataProcessor = class ($OBJ, Loggable, Validatable) {
    process = op(data) {
        if (validateAndLog(data)) {
            "Processed: " + data;
        } else {
            "Failed to process: " + data;
        };
    };
};

/* Usage */
processor = DataProcessor();
processor.process("valid data");    /* Returns: "Processed: valid data" */
processor.process("");              /* Returns: "Failed to process: " */
```

---

## Dynamic Class Creation with `obj`

Grapa provides the `obj` function for creating objects with custom class inheritance dynamically. This is particularly useful when you want to create objects that inherit from system classes like `$GOBJ` while adding your own methods and properties.

### Automatic vs Explicit Class Assignment

When you create an object with `{}`, Grapa automatically assigns the `$GOBJ` class:

```grapa
/* Automatic class assignment */
x2 = {x: 1, timesx: op(y){x*y}, setx: op(pX){x = pX;}};
x2.type();  /* Returns: $GOBJ */
```

However, if you want to create an object that inherits from `$GOBJ` but has your own class definition, you can use the `obj` function:

```grapa
/* Explicit class inheritance */
x1 = obj class($GOBJ) {
    x = 1;
    timesx = op(y) { x * y; };
    setx = op(pX) { x = pX; };
};
x1.type();  /* Returns: $OBJ (with $GOBJ inheritance) */
```

### Key Differences

| Aspect | `{}` (Automatic) | `obj class($GOBJ){...}` (Explicit) |
|--------|------------------|-------------------------------------|
| **Type** | `$GOBJ` | `$OBJ` |
| **Inheritance** | Built-in `$GOBJ` methods only | `$GOBJ` methods + your custom methods |
| **Extensibility** | Limited to `$GOBJ` functionality | Can add more classes to inheritance chain |
| **Method Resolution** | `$GOBJ` methods | Your methods + `$GOBJ` methods |
| **Use Case** | Simple data structures | Complex objects with custom behavior |

### Practical Example

```grapa
/* Both approaches work similarly for basic operations */
x1 = obj class($GOBJ) {
    x = 1;
    timesx = op(y) { x * y; };
    setx = op(pX) { x = pX; };
};

x2 = {x: 1, timesx: op(y){x*y}, setx: op(pX){x = pX;}};

/* Both support the same operations */
x1.x;           /* 1 */
x1.timesx(5);   /* 5 */
x1.setx(2);     /* Sets x to 2 */
x1.timesx(5);   /* 10 */

x2.x;           /* 1 */
x2.timesx(5);   /* 5 */
x2.setx(2);     /* Sets x to 2 */
x2.timesx(5);   /* 10 */
```

### When to Use Each Approach

**Use `{}` (Automatic) when:**
- Creating simple data structures
- You only need basic `$GOBJ` functionality
- Quick prototyping or simple objects
- You don't need custom inheritance chains

**Use `obj class($GOBJ){...}` (Explicit) when:**
- You need custom methods beyond `$GOBJ` functionality
- You want to create reusable class definitions
- You need to add multiple classes to the inheritance chain
- You want to create objects that behave like lists but with additional capabilities

### Advanced Inheritance Example

```grapa
/* Create a class that inherits from multiple system classes */
AdvancedObject = obj class($GOBJ, $OBJ) {
    /* Custom properties */
    name = "default";
    data = [];
    
    /* Custom methods */
    addData = op(item) { 
        data.push(item); 
        this;  /* Return self for chaining */
    };
    
    getName = op() { name; };
    setName = op(newName) { name = newName; };
    
    /* Override or extend inherited methods */
    len = op() { data.len(); };
};

/* Usage */
obj = AdvancedObject();
obj.setName("MyObject");
obj.addData("item1").addData("item2");
obj.getName();  /* "MyObject" */
obj.len();      /* 2 */
obj.type();     /* $OBJ (with $GOBJ and $OBJ inheritance) */
```

---

## Best Practices

### Class Design
- **Single Responsibility**: Each class should have a clear, single purpose
- **Composition over Inheritance**: Prefer composition when inheritance isn't necessary
- **Clear Naming**: Use descriptive names that don't start with "$"
- **Documentation**: Document class purpose and method behavior

### Inheritance Usage
- **Inherit from `$OBJ`**: For user classes that need universal method support
- **Avoid Deep Hierarchies**: Keep inheritance chains shallow and clear
- **Use Mixins**: For cross-cutting concerns like logging, validation, etc.
- **Override Appropriately**: Only override methods when you need different behavior

### Variable Management
- **Initialize in Class**: Set default values in class definition:
- **Modify in Instances**: Let copy-on-write handle instance-specific values
- **Use Methods**: Access variables through methods for encapsulation
- **Avoid Direct Access**: Prefer method access over direct variable access

### Performance Considerations
- **Shared Variables**: Leverage copy-on-write for memory efficiency
- **Method Inheritance**: Reuse method definitions across instances
- **System Classes**: Use system classes for specialized functionality
- **User Classes**: Use user classes for application-specific logic

---

## Notes

- **Runtime Modification**: Classes can be modified after definition
- **Dynamic Typing**: All type checking happens at runtime
- **Memory Efficiency**: Copy-on-write minimizes memory usage
- **Method Resolution**: Automatic method lookup through inheritance chain
- **System Integration**: Seamless integration with Grapa's type system

[Back to Top](#class-type-reference)
