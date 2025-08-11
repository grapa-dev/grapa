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
csv_rule = rule $STR ',' $STR ',' $STR {op(name:$1,age:$3,city:$5){
    return {
        name: name,
        age: age.int(),
        city: city
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

### **Pattern 2: `$sys().eval()` with `srule` Parameter**

Execute input against a custom rule set using the system evaluation function.

```grapa
/* Define rules in a namespace */
@global["etl_rules"] = {
    csv_parser: rule $STR ',' $STR ',' $STR {op(name:$1,age:$3,city:$5){
        return {name: name, age: age.int(), city: city};
    }},
    
    log_parser: rule '[' $STR ']' $STR {op(timestamp:$2,message:$4){
        return {timestamp: timestamp, message: message, level: "INFO"};
    }},
    
    config_parser: rule $STR '=' $STR {op(key:$1,value:$3){
        return {key: key, value: value};
    }}
};

/* Execute using custom rule set */
csv_result = $sys().eval("John,25,NY", {}, "etl_rules.csv_parser");
log_result = $sys().eval("[2024-01-15 10:30:00] User login", {}, "etl_rules.log_parser");
config_result = $sys().eval("server_name=myapp", {}, "etl_rules.config_parser");
```

**Key Features:**
- **Rule Set Organization**: Group related rules in namespaces
- **System Integration**: Use Grapa's evaluation system
- **Parameter Support**: Pass additional parameters if needed
- **Error Handling**: Built-in error handling and recovery

### **Pattern 3: Dynamic Rule Creation and Execution**

Create rules dynamically and execute them immediately.

```grapa
/* Rule factory function */
create_parser = op(rule_name, rule_definition) {
    /* Define the rule dynamically */
    @global[rule_name] = rule_definition;
    
    /* Return a function that executes against this rule */
    return op(input) {
        return op()(input, @global[rule_name])();
    };
};

/* Create different parsers */
json_parser = create_parser("json_rule", rule '{' $STR ':' $STR '}' {op(key:$2,value:$4){
    return {key: key, value: value};
}});

xml_parser = create_parser("xml_rule", rule '<' $STR '>' $STR '</' $STR '>' {op(tag:$2,content:$4,end_tag:$6){
    if (tag == end_tag) {
        return {tag: tag, content: content};
    } else {
        return $err("Mismatched tags");
    };
}});

/* Execute the dynamically created parsers */
json_result = json_parser('{"name":"John"}');
xml_result = xml_parser('<user>John Doe</user>');
```

**Key Features:**
- **Dynamic Creation**: Create rules based on runtime conditions
- **Factory Pattern**: Reusable rule creation functions
- **Immediate Execution**: Execute rules as soon as they're created
- **Flexible Definitions**: Support complex rule definitions

## Use Case Scenarios

### **1. ETL Data Processing**

Isolated rules are perfect for ETL workflows where you need to parse various data formats.

```grapa
/* ETL Pipeline with isolated rules */
@global["etl_pipeline_rules"] = {
    /* Extract rules */
    extract_csv: rule $STR ',' $STR ',' $STR {op(name:$1,age:$3,city:$5){
        return {name: name, age: age.int(), city: city, source: "csv"};
    }},
    
    extract_json: rule '{' $STR ':' $STR '}' {op(key:$2,value:$4){
        return {key: key, value: value, source: "json"};
    }},
    
    /* Transform rules */
    transform_uppercase: rule $STR {op(text:$1){
        return text.upper();
    }},
    
    transform_add_prefix: rule $STR $STR {op(prefix:$1,text:$2){
        return prefix + ": " + text;
    }},
    
    /* Load rules */
    load_database: rule $STR {op(data:$1){
        return {action: "insert", table: "users", data: data};
    }}
};

/* ETL Pipeline function */
etl_pipeline = op(input_data, input_type, transformations, output_type) {
    /* Extract */
    if (input_type == "csv") {
        extracted = op()(input_data, @global["etl_pipeline_rules"].extract_csv)();
    } else if (input_type == "json") {
        extracted = op()(input_data, @global["etl_pipeline_rules"].extract_json)();
    } else {
        return $err("Unsupported input type");
    };
    
    /* Transform */
    transformed = extracted;
    i = 0;
    while (i < transformations.len()) {
        transform = transformations[i];
        if (transform == "uppercase") {
            transformed.name = op()(transformed.name, @global["etl_pipeline_rules"].transform_uppercase)();
        } else if (transform == "add_prefix") {
            transformed.name = op()("USER", transformed.name, @global["etl_pipeline_rules"].transform_add_prefix)();
        };
        i += 1;
    };
    
    /* Load */
    if (output_type == "database") {
        result = op()(transformed, @global["etl_pipeline_rules"].load_database)();
    } else {
        result = transformed;
    };
    
    return result;
};

/* Execute ETL pipeline */
result = etl_pipeline("John,25,NY", "csv", ["uppercase", "add_prefix"], "database");
```

### **2. Protocol Parsing**

Parse network protocols and communication formats safely.

**Advanced Pattern - Wrapper Functions in Rule Tokens:**
```grapa
/* Define processing functions */
validate_http = op(p){p.grep(/^[A-Z]+ /)?p:null};
parse_headers = op(p){p.split("\n").filter(op(line){line.grep(/^[A-Za-z-]+: /)})};
extract_body = op(p){p.split("\r\n\r\n")[1] || ""};

/* Use wrapper functions in isolated rules */
@global["http_parser_rules"] = {
    http_request: rule 
        <$raw_request,op(b:$1){validate_http(b)}>
        <$validated,op(b:$1){parse_headers(b)}>
        <$headers,op(b:$1){extract_body(b)}>
        {op(request:$1,headers:$3,body:$5){
            return {request: request, headers: headers, body: body};
        }}
};
```

```grapa
/* Protocol-specific rules */
@global["protocol_rules"] = {
    /* HTTP Request parsing */
    http_request: rule $STR ' ' $STR ' ' $STR {op(method:$1,path:$3,version:$5){
        return {method: method, path: path, version: version, protocol: "HTTP"};
    }},
    
    /* SMTP command parsing */
    smtp_command: rule $STR ' ' $STR {op(command:$1,parameter:$3){
        return {command: command, parameter: parameter, protocol: "SMTP"};
    }},
    
    /* Custom binary protocol */
    binary_protocol: rule $INT $INT $STR {op(type:$1,length:$3,data:$5){
        return {type: type, length: length, data: data, protocol: "BINARY"};
    }}
};

/* Protocol parser function */
parse_protocol = op(input, protocol_type) {
    if (protocol_type == "http") {
        return op()(input, @global["protocol_rules"].http_request)();
    } else if (protocol_type == "smtp") {
        return op()(input, @global["protocol_rules"].smtp_command)();
    } else if (protocol_type == "binary") {
        return op()(input, @global["protocol_rules"].binary_protocol)();
    } else {
        return $err("Unknown protocol");
    };
};

/* Execute protocol parsing */
http_result = parse_protocol("GET /api/users HTTP/1.1", "http");
smtp_result = parse_protocol("MAIL FROM: user@example.com", "smtp");
```

### **3. Data Validation**

Create isolated validation rules for business logic and data integrity.

```grapa
/* Validation rules */
@global["validation_rules"] = {
    /* Email validation */
    email_validation: rule $STR '@' $STR '.' $STR {op(local:$1,domain:$3,tld:$5){
        if (local.len() > 0 && domain.len() > 0 && tld.len() > 0) {
            return {valid: true, email: local + "@" + domain + "." + tld};
        } else {
            return {valid: false, error: "Invalid email format"};
        };
    }},
    
    /* Phone number validation */
    phone_validation: rule '(' $INT ')' ' ' $INT '-' $INT {op(area:$2,prefix:$5,number:$7){
        if (area >= 100 && area <= 999 && prefix >= 100 && prefix <= 999 && 
            number >= 1000 && number <= 9999) {
            return {valid: true, phone: "(" + area + ") " + prefix + "-" + number};
        } else {
            return {valid: false, error: "Invalid phone format"};
        };
    }},
    
    /* Credit card validation */
    credit_card_validation: rule $INT $INT $INT $INT {op(part1:$1,part2:$3,part3:$5,part4:$7){
        if (part1 >= 1000 && part1 <= 9999 && part2 >= 1000 && part2 <= 9999 && 
            part3 >= 1000 && part3 <= 9999 && part4 >= 1000 && part4 <= 9999) {
            return {valid: true, card: part1 + "-" + part2 + "-" + part3 + "-" + part4};
        } else {
            return {valid: false, error: "Invalid card format"};
        };
    }}
};

/* Validation function */
validate_data = op(input, validation_type) {
    if (validation_type == "email") {
        return op()(input, @global["validation_rules"].email_validation)();
    } else if (validation_type == "phone") {
        return op()(input, @global["validation_rules"].phone_validation)();
    } else if (validation_type == "credit_card") {
        return op()(input, @global["validation_rules"].credit_card_validation)();
    } else {
        return $err("Unknown validation type");
    };
};

/* Execute validation */
email_result = validate_data("user@example.com", "email");
phone_result = validate_data("(555) 123-4567", "phone");
card_result = validate_data("1234 5678 9012 3456", "credit_card");
```

### **4. Configuration Parsing**

Parse configuration files and settings safely.

```grapa
/* Configuration parsing rules */
@global["config_rules"] = {
    /* Key-value pairs */
    key_value: rule $STR '=' $STR {op(key:$1,value:$3){
        return {key: key, value: value, type: "setting"};
    }},
    
    /* Include directive */
    include: rule 'include' $STR {op(file:$2){
        return {file: file, type: "include"};
    }},
    
    /* Section headers */
    section: rule '[' $STR ']' {op(name:$2){
        return {name: name, type: "section"};
    }},
    
    /* Comments */
    comment: rule '#' $STR {op(text:$2){
        return {text: text, type: "comment"};
    }}
};

/* Configuration parser */
parse_config = op(input) {
    /* Try each rule until one matches */
    result = op()(input, @global["config_rules"].key_value)();
    if (result.type() != $ERR) {
        return result;
    };
    
    result = op()(input, @global["config_rules"].include)();
    if (result.type() != $ERR) {
        return result;
    };
    
    result = op()(input, @global["config_rules"].section)();
    if (result.type() != $ERR) {
        return result;
    };
    
    result = op()(input, @global["config_rules"].comment)();
    if (result.type() != $ERR) {
        return result;
    };
    
    return $err("Unknown config format");
};

/* Parse configuration entries */
setting = parse_config("server_name=myapp");
include = parse_config("include database.conf");
section = parse_config("[database]");
comment = parse_config("# This is a comment");
```

## Advanced Patterns

### **1. Error Handling and Recovery**

Create robust parsing rules with fallback mechanisms.

```grapa
/* Robust parsing rules with error handling */
@global["robust_rules"] = {
    /* Primary rule with fallback */
    robust_csv: rule $STR ',' $STR ',' $STR {op(name:$1,age:$3,city:$5){
        return {name: name, age: age.int(), city: city, status: "parsed"};
    }} | $STR {op(data:$1){
        return {raw_data: data, status: "fallback", error: "Invalid CSV format"};
    }},
    
    /* Rule with validation */
    validated_json: rule '{' $STR ':' $STR '}' {op(key:$2,value:$4){
        if (key.len() > 0 && value.len() > 0) {
            return {key: key, value: value, valid: true};
        } else {
            return {valid: false, error: "Empty key or value"};
        };
    }}
};

/* Robust parsing function */
robust_parse = op(input, rule_name) {
    try {
        result = op()(input, @global["robust_rules"][rule_name])();
        return result;
    } catch (error) {
        return {error: "Parsing failed", details: error, input: input};
    };
};

/* Test robust parsing */
valid_result = robust_parse("John,25,NY", "robust_csv");
invalid_result = robust_parse("Invalid data", "robust_csv");
```

### **2. Performance Optimization**

Cache compiled rules for better performance.

```grapa
/* Rule compilation cache */
rule_cache = {};

/* Compile and cache rule */
compile_rule = op(rule_name, rule_definition) {
    if (rule_cache[rule_name]) {
        return rule_cache[rule_name];
    };
    
    /* Create the rule */
    @global[rule_name] = rule_definition;
    
    /* Cache the compiled rule */
    rule_cache[rule_name] = @global[rule_name];
    
    return rule_cache[rule_name];
};

/* Use cached rules */
csv_rule = compile_rule("csv_parser", rule $STR ',' $STR ',' $STR {op(name:$1,age:$3,city:$5){
    return {name: name, age: age.int(), city: city};
}});

/* Execute with cached rule */
result = op()("John,25,NY", csv_rule)();
```

### **3. Rule Composition**

Combine multiple rules for complex parsing scenarios.

```grapa
/* Composite rule system */
@global["composite_rules"] = {
    /* Base rules */
    field: rule $STR {op(value:$1){return value}},
    separator: rule ',' {op(){return ","}},
    
    /* Composite rules */
    csv_row: rule <field> (<separator> <csv_row> | ) {op(first:$1,rest:$3){
        if (rest) {
            return [first] + rest;
        } else {
            return [first];
        };
    }},
    
    /* Complex parsing */
    structured_data: rule <csv_row> '|' <csv_row> {op(row1:$1,row2:$3){
        return {
            primary: row1,
            secondary: row2,
            combined: row1 + row2
        };
    }}
};

/* Execute composite parsing */
simple_result = op()("a,b,c", @global["composite_rules"].csv_row)();
complex_result = op()("a,b,c|d,e,f", @global["composite_rules"].structured_data)();
```

## Best Practices

### **1. Rule Organization**
- **Namespace Management**: Group related rules in namespaces
- **Clear Naming**: Use descriptive names for rules and functions
- **Documentation**: Document rule behavior and expected input/output
- **Versioning**: Consider versioning for complex rule sets

### **2. Performance Considerations**
- **Rule Caching**: Cache compiled rules for repeated use
- **Lazy Loading**: Load rules only when needed
- **Memory Management**: Clean up unused rules to free memory
- **Optimization**: Profile rule execution for bottlenecks

### **3. Error Handling**
- **Graceful Degradation**: Provide fallback mechanisms
- **Error Recovery**: Handle parsing errors without crashing
- **Validation**: Validate input before rule application
- **Logging**: Log parsing failures for debugging

### **4. Testing and Debugging**
- **Unit Testing**: Test individual rules in isolation
- **Integration Testing**: Test rule combinations
- **Edge Cases**: Test with malformed or unexpected input
- **Performance Testing**: Measure execution time for optimization

## Real-World Applications

### **1. Data Processing Pipelines**
- **ETL Workflows**: Extract, transform, and load data
- **Data Validation**: Validate data formats and business rules
- **Data Transformation**: Convert between different formats
- **Data Enrichment**: Add computed fields and metadata

### **2. Network Protocol Processing**
- **HTTP Parsing**: Parse HTTP requests and responses
- **SMTP Processing**: Handle email protocol parsing
- **Custom Protocols**: Parse proprietary communication formats
- **API Integration**: Parse API responses and requests

### **3. Configuration Management**
- **File Parsing**: Parse configuration files
- **Environment Variables**: Process environment settings
- **Command Line Arguments**: Parse CLI parameters
- **Settings Validation**: Validate configuration values

### **4. Log Processing**
- **Log Parsing**: Extract structured data from log files
- **Event Processing**: Process system events and alerts
- **Audit Trails**: Parse audit log entries
- **Performance Monitoring**: Extract metrics from logs

## Conclusion

Isolated rule execution provides the power of Grapa's executable BNF system without the risks of modifying the main language grammar. This approach is essential for:

- **ETL and data processing** workflows
- **Domain-specific language** creation
- **Protocol parsing** and network communication
- **Data validation** and business rule enforcement
- **Configuration management** and file parsing

By using isolated rules, you can leverage Grapa's powerful parsing capabilities while maintaining clean separation between domain-specific logic and the core language, ensuring better performance, safety, and maintainability.

## See Also
- [Language Design with Executable BNF](language_design_with_executable_bnf.md)
- [Multi-Syntax Programming](multi_syntax_programming.md)
- [Advanced Topics](advanced_topics.md)
- [Examples](examples.md)
- [Use Cases](use_cases/index.md) 