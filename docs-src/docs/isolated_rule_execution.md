---
tags:
  - advanced
  - rule-execution
  - isolated-rules
  - etl
  - domain-specific
  - parsing
  - grammar
  - op-pattern
  - sys-eval
---

# Isolated Rule Execution

## Overview

Grapa provides powerful capabilities for executing custom rules in isolation without hooking them into the main language grammar. This is crucial for ETL (Extract, Transform, Load) work, domain-specific processing, and any scenario where you want to leverage Grapa's executable BNF system without affecting the core language.

## Key Benefits

### **1. No Side Effects**
- **Isolated Execution**: Custom rules don't modify the main Grapa language
- **Safe Processing**: No risk of breaking existing scripts or functionality
- **Clean Separation**: Domain-specific logic stays separate from core language

### **2. Better Performance**
- **No Global Rule Modification**: Avoids the overhead of modifying global grammar
- **Optimized Execution**: Direct rule application without grammar injection
- **Cached Rules**: Rules can be compiled once and reused multiple times

### **3. Enhanced Safety**
- **ETL Workflows**: Perfect for data processing pipelines
- **Protocol Parsing**: Safe parsing of network protocols and data formats
- **Validation Systems**: Isolated validation rules for business logic

## Execution Patterns

### **Pattern 1: `op()("input", rule)()`**

Execute a single rule against input data without affecting the main language.

```grapa
/* Define a custom CSV parsing rule */
csv_rule = rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){
    return {
        "name": name,
        "age": age,
        "city": city
    };
}};

/* Execute against the custom rule */
result = op()("John,25,NY", csv_rule)();
/* Result: {name: "John", age: 25, city: "NY"} */
```

**Key Features:**
- **Direct Rule Application**: Apply rule directly to input
- **No Grammar Modification**: Rule exists only for this execution
- **Immediate Results**: Get parsed data immediately
- **Type Safety**: Maintains Grapa's type system

**Advanced Example - Complex Log Parsing:**
```grapa
/* Define helper rules for complex parsing */
catch_sq_bracket_rule = rule ('[') <> (']') {@<lit,{$2}>};
id_list_rule = rule $ID <id_list_rule> {@<prepend,{$2,$1}>} | $ID {@<createarray,{$1}>};

/* Define main log parsing rule */
log_rule = rule <catch_sq_bracket_rule> <id_list_rule> {op(timestamp:$1,message:$2){
    return {
        timestamp: timestamp,
        message: message,
        level: "INFO"
    };
}};

/* Execute log parsing */
result = op()("[2024-01-15 10:30:00] User login successful", log_rule)();
/* Result: {timestamp: [2024,"-",1,"-",15," ",10,":",30,":",0], message: [User,login,successful], level: "INFO"} */
```

### **Pattern 2: Object-Based Rule Organization**

Organize rules in objects with a common parsing interface for better code organization.

#### **2a. $GOBJ Object Organization**

```grapa
/* Define ETL rules in a $GOBJ object */
etl_rules = {
    /* Common parsing interface */
    parse = op(parse_data, parse_rule){op()(parse_data, parse_rule)()},

    /* CSV parsing rule */
    csv_parser = rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){
        {name: name, age: age, city: city};
    }},

    /* Log parsing rules */
    catch_sq_bracket_rule = rule ('[') <> (']') {@<lit,{$2}>},
    id_list_rule = rule $ID <id_list_rule> {@<prepend,{$2,$1}>} | $ID {@<createarray,{$1}>},
    log_parser = rule <catch_sq_bracket_rule> <id_list_rule> {op(timestamp:$1,message:$2){
        {timestamp: timestamp, message: message, level: "INFO"};
    }},
    
    /* Config parsing rule */
    config_parser = rule $ID '=' $ID {op(key:$1,value:$3){
        {key: key, value: value};
    }}
};

/* Execute with custom rule set */
csv_result = etl_rules.parse("John,25,NY", etl_rules.csv_parser);
log_result = etl_rules.parse("[2024-01-15 10:30:00] User login successful", etl_rules.log_parser);
config_result = etl_rules.parse("server_name=myapp", etl_rules.config_parser);
```

#### **2b. $CLASS Object Organization**

```grapa
/* Define rules in a class */
etl_rules_class = class {
    /* Common parsing interface */
    parse = op(parse_data, parse_rule){op()(parse_data, parse_rule)()};

    /* CSV parsing rule */
    csv_parser = rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){
        {name: name, age: age, city: city};
    }};

    /* Log parsing rules */
    catch_sq_bracket_rule = rule ('[') <> (']') {@<lit,{$2}>};
    id_list_rule = rule $ID <id_list_rule> {@<prepend,{$2,$1}>} | $ID {@<createarray,{$1}>};
    log_parser = rule <catch_sq_bracket_rule> <id_list_rule> {op(timestamp:$1,message:$2){
        {timestamp: timestamp, message: message, level: "INFO"};
    }};
    
    /* Config parsing rule */
    config_parser = rule $ID '=' $ID {op(key:$1,value:$3){
        {key: key, value: value};
    }};
};

/* Create instance and execute */
etl_rules = etl_rules_class();
csv_result = etl_rules.parse("John,25,NY", etl_rules.csv_parser);
log_result = etl_rules.parse("[2024-01-15 10:30:00] User login successful", etl_rules.log_parser);
config_result = etl_rules.parse("server_name=myapp", etl_rules.config_parser);
```

**Key Features:**
- **Organized Structure**: Rules grouped logically in objects or classes
- **Common Interface**: Shared parsing method across all rules
- **Easy Extension**: Add new rules to the object or class
- **Clean Syntax**: Simple method calls for rule execution
- **Object-Oriented**: Classes provide better encapsulation and reusability

### **Pattern 3: Dynamic Rule Creation and Execution**

Create rules dynamically and execute them immediately.

#### **3a. Dynamic Rule Addition to $GOBJ Objects**

```grapa
/* Start with base ETL rules object */
etl_rules = {
    parse = op(parse_data, parse_rule){op()(parse_data, parse_rule)()},
};

/* Dynamically add new rules using += operator */
etl_rules += (csv_parser : rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){{name: name, age: age, city: city};}});

/* Execute with dynamically added rule */
csv_result = etl_rules.parse("John,25,NY", etl_rules.csv_parser);
/* Result: {name: "John", age: 25, city: "NY"} */
```

#### **3b. Dynamic Rule Addition to $CLASS Objects**

```grapa
/* Define base ETL rules class with add_rule method */
etl_rules_class = class {
    parse = op(parse_data, parse_rule){op()(parse_data, parse_rule)()};
    add_rule = op(pName,pRule) {$this.@pName=pRule;};
};

/* Create instance and add rules dynamically */
etl_rules = etl_rules_class();
etl_rules.add_rule("csv_parser", rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){{name: name, age: age, city: city};}});

/* Execute with dynamically added rule */
csv_result = etl_rules.parse("John,25,NY", etl_rules.csv_parser);
/* Result: {name: "John", age: 25, city: "NY"} */
```

**Key Features:**
- **Dynamic Creation**: Create rules based on runtime conditions
- **Object Extension**: Add rules to existing `$GOBJ` objects using `+=` operator
- **Class Methods**: Use class methods to add rules dynamically to `$CLASS` instances
- **Immediate Execution**: Execute rules as soon as they're created
- **Flexible Definitions**: Support complex rule definitions

## Use Case Scenarios

The patterns demonstrated in the isolated rule execution demo are perfect for:

### **1. ETL Data Processing**
- **CSV Parsing**: Parse structured data with custom field mappings
- **Log Processing**: Extract structured information from log entries
- **Configuration Parsing**: Parse key-value configuration files
- **Data Validation**: Validate data formats and business rules

### **2. Domain-Specific Processing**
- **Protocol Parsing**: Parse network protocols and communication formats
- **File Format Processing**: Handle custom file formats and data structures
- **Business Rule Validation**: Implement domain-specific validation logic
- **Data Transformation**: Convert between different data representations

### **3. Dynamic Rule Systems**
- **Plugin Systems**: Add new parsing rules at runtime
- **Configuration-Driven Processing**: Load rules from configuration files
- **Adaptive Parsing**: Modify parsing behavior based on runtime conditions
- **Extensible Frameworks**: Build systems that can be extended with new rules

## Conclusion

Isolated rule execution provides the power of Grapa's executable BNF system without the risks of modifying the main language grammar. The three key patterns demonstrated are:

- **Pattern 1**: `op()("input", rule)()` - Direct rule execution for simple parsing tasks
- **Pattern 2**: Object-based organization with `$GOBJ` and `$CLASS` for structured rule management
- **Pattern 3**: Dynamic rule creation and execution for extensible parsing systems

These patterns are essential for ETL workflows, domain-specific processing, protocol parsing, and any scenario where you need isolated, safe rule execution without affecting the main Grapa language.

## See Also
- [Language Design with Executable BNF](language_design_with_executable_bnf.md)
- [Multi-Syntax Programming](multi_syntax_programming.md)
- [Advanced Topics](advanced_topics.md)
- [Examples](examples.md)
- [Use Cases](use_cases/index.md) 
