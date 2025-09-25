---
tags:
  - advanced
  - language-design
  - executable-bnf
  - rule-system
  - grammar
  - dsl
  - meta-programming
---

# Language Design with Executable BNF

## Overview

Grapa's `$RULE` system represents a fundamental innovation in language design - an **executable BNF (Backus-Naur Form)** that goes far beyond traditional grammar definitions. This guide explores how to leverage this powerful system for creating custom languages, domain-specific languages (DSLs), and extending Grapa itself.

## Key Concepts

### **Executable BNF: Beyond Traditional Parsing**

Unlike traditional BNF systems that only define syntax, Grapa's `$RULE` system:

- **Executes arbitrary code** during parsing via action codes
- **Builds execution trees** that can be evaluated later
- **Supports dynamic grammar mutation** at runtime
- **Handles complex token types** with special behaviors
- **Manages execution context** and parameter binding

### **Three-Phase Processing Model**

```
Input → Compilation → Execution Tree → Runtime Evaluation
```

1. **Compilation Phase**: Raw input is parsed against BNF rules, building execution trees
2. **Tree Construction**: Action codes create `$OP` and `$CODE` nodes with parameter binding
3. **Runtime Evaluation**: Trees are evaluated with lazy parameter resolution

### **Advanced Rule Token Capabilities**

The rule token `< >` provides sophisticated reference and lookup capabilities:

#### **Rule References**
```grapa
/* Reference another rule */
$global["$expression"] = rule <$term> '+' <$expression> {@<add,{$1,$3}>}
```

#### **Variable References with Complex Lookups**
```grapa
/* Simple variable reference */
$global["$lookup"] = rule @variable {@<var,{$1}>}

/* Array/List indexing */
$global["$array_lookup"] = rule @tb["d"] {@<var,{$1}>}
$global["$list_lookup"] = rule @tb.d {@<var,{$1}>}
$global["$index_lookup"] = rule @tb[8] {@<var,{$1}>}
```

#### **Post-Processing with Optional $OP**
```grapa
/* Rule with compile-time data transformation */
$global["$filtered_data"] = rule <$raw_data,op(a:$1){a.grep("pattern")}> {@<var,{$1}>}
```

#### **Predefined Functions in Rule Tokens**

**Working Solution - Wrapper Function Pattern:**
```grapa
/* Define function using op(){} syntax */
my_func = op(p){p.len()};

/* Use wrapper function in rule token */
$global["$processed_data"] = rule <$raw_data,op(b:$1){my_func(b)}> {@<var,{$1}>}
```

**Key Benefits:**
- **Explicit Parameter Passing**: `op(b:$1){my_func(b)}` clearly shows parameter flow
- **Works with Any Function**: Can wrap any function definition
- **No Grammar Changes**: Uses existing `$op` syntax
- **ETL-Friendly**: Perfect for data transformation pipelines
- **Reusable**: Functions can be defined once and used in multiple rules

**ETL Processing Example:**
```grapa
/* Define ETL processing functions */
validate_data = op(p){p.grep("valid")};
transform_data = op(p){p.upper()};
filter_data = op(p){p.len() > 10 ? p : null};

/* Use in ETL pipeline rules */
$global["$etl_pipeline"] = rule 
    <$raw_data,op(b:$1){validate_data(b)}>
    <$validated,op(b:$1){transform_data(b)}>
    <$transformed,op(b:$1){filter_data(b)}>
    {@<var,{$1}>}
```

**Alternative Syntax (Future Enhancement):**
```grapa
/* Define predefined function using @<op,{parameters}> syntax */
filter_function = @<grep,{@<this>,@<lit,{"pattern"}>}>;

/* Use predefined function in rule token */
$global["$filtered_data"] = rule <$raw_data,filter_function> {@<var,{$1}>}
```

**Key Features:**
- **Execution Tree Return**: Always returns execution trees that must be evaluated
- **Namespace Resolution**: Follows Grapa's namespace hierarchy (local → function → global)
- **Dynamic Lookup**: Supports arrays, lists, objects, database objects
- **Compile-Time Processing**: Post-processing `$OP` runs during compilation phase
- **Token Lookback**: Access previous tokens via `$1`, `$2`, etc.
- **Error Handling**: Can return `$ERR` to cause rule failure
- **Performance Note**: Post-processing runs every evaluation (avoid heavy operations)
- **Predefined Functions**: Functions can be defined once and reused across multiple rules
- **Cleaner Syntax**: Avoids embedding raw code in grammar rules

## Language Extensibility Patterns

### **1. Syntax Extension Approaches**

#### **Primary Approach: Direct BNF Integration**
Add syntax directly to existing BNF rules using `@<function_name,{parameters}>` pattern:

```grapa
/* Add to $command rule for control structures */
| for '(' <$comp> ';' <$comp> ';' <$comp> ')' <$command> {@<for,{$3,$5,$7,$9}>}
| for $ID in <$comp> <$command> {@<forin,{$2,$4,$6}>}

/* Add to $comp rule for expressions */
| '$' '{' <$comp> '}' {@<interpolate,{$3}>}
| range '(' <$comp> ')' {@<range,{$3}>}
```

#### **Secondary Approach: Custom Command/Function Variables**
Use `$custom_command`/`$custom_function` as variables that leverage existing grammar rules:

```grapa
/* $custom_command - leverages existing $comp and $command rules */
$custom_command = rule for $ID from <$comp> to <$comp> <$command> {
    op(var:$2, start:$4, end:$6, body:$8){
        /* Uses existing $comp and $command rules */
        op()(var + " = " + start)();
        while (op()(var + " <= " + end)()) {
            op()(body)();
            op()(var + " = " + var + " + 1")();
        };
    }
};

/* $custom_function - leverages existing $comp rules */
$custom_function = rule <$comp> '*=' <$comp> {
    op(left:$1, right:$3){
        result = op()(left)() * op()(right)();
        op()(left + " = " + result)();
        result;
    }
};

/* Use directly like any other syntax */
for i from 1 to 5 { ("Count: " + i).echo(); };
x = 10; x *= 3; ("x = " + x).echo();
```

### **2. Dynamic Grammar Modification**

```grapa
/* Add new language constructs at runtime */
$global["$custom_loop"] = rule 'repeat' $INT 'times' '{' <$command_list> '}' {
    op(count:$2, body:$5) {
        i = 0;
        while (i < count) {
            body();
            i += 1;
        };
    }
};

/* Use the new syntax immediately */
op(parse)('repeat 5 times { "Hello".echo(); }')();
```

### **3. Protocol Parsing**

```grapa
/* Define HTTP request grammar */
$global["$http_request"] = rule <$http_method> ' ' <$http_path> ' ' <$http_version> '\r\n' <$http_headers> {
    op(method:$1, path:$3, version:$5, headers:$7) {
        return create_request(method, path, version, headers);
    }
};

/* Parse HTTP requests */
request = op(parse)('GET /api/users HTTP/1.1\r\nHost: example.com\r\n')();
```

## Domain-Specific Language (DSL) Creation

### **1. Configuration DSL**

```grapa
/* Configuration language grammar */
$global["$config_entry"] = rule $ID '=' <$config_value> ';' {
    op(key:$1, value:$3) {
        set_config(key, value);
    }
};

$global["$config_value"] = rule $STR | $INT | $BOOL | '[' <$config_list> ']' | '{' <$config_object> '}';

/* Use the configuration DSL */
op(parse)('server_name = "myapp";')();
op(parse)('port = 8080;')();
op(parse)('debug = true;')();
op(parse)('allowed_hosts = ["localhost", "127.0.0.1"];')();
```

### **2. Data Processing DSL**

```grapa
/* Pipeline processing language */
$global["$pipeline"] = rule 'pipeline' '{' <$pipeline_steps> '}' {
    op(steps:$3) {
        result = null;
        i = 0;
        while (i < steps.len()) {
            step = steps[i];
            result = execute_pipeline_step(step, result);
            i += 1;
        };
        return result;
    }
};

$global["$pipeline_steps"] = rule <$pipeline_step> ('|' <$pipeline_steps> | );

/* Execute data processing pipeline */
result = op(parse)('pipeline { 
    load "data.csv" | 
    filter "age > 25" | 
    sort "name" | 
    output "results.json" 
}')();
```

### **3. Validation DSL**

```grapa
/* Data validation language */
$global["$validation"] = rule 'validate' $STR '{' <$validation_rules> '}' {
    op(data:$2, rules:$4) {
        validation_result = true;
        i = 0;
        while (i < rules.len()) {
            rule = rules[i];
            if (!apply_validation_rule(data, rule)) {
                validation_result = false;
            };
            i += 1;
        };
        return validation_result;
    }
};

$global["$validation_rules"] = rule <$validation_rule> ('|' <$validation_rules> | );

$global["$validation_rule"] = rule 'field' $STR 'required' | 'field' $STR 'type' $STR;

/* Execute validation */
is_valid = op(parse)('validate user_data { 
    field "name" required | 
    field "age" type "integer" | 
    field "email" type "email" 
}')();
```

## Advanced Language Design Patterns

### **1. Recursive Grammar Rules**

```grapa
/* CSV parser with recursive grammar */
$global["$csv_parser"] = rule <$csv_row> ('\n' <$csv_parser> | '\n' | ) {
    op(row:$1, rest:$3) {
        if (rest) {
            return [row] + rest;
        } else {
            return [row];
        };
    }
};

$global["$csv_row"] = rule <$csv_field> (',' <$csv_row> | );

/* Parse CSV data */
csv_data = "name,age,city\nJohn,25,NY\nJane,30,LA";
parsed = op(parse)(csv_data)();
```

### **2. Context-Sensitive Parsing**

```grapa
/* Context-aware parsing with state */
$global["$context_parser"] = rule <$context_state> <$context_rule> {
    op(state:$1, rule:$2) {
        /* Apply context-specific parsing rules */
        return parse_with_context(state, rule);
    }
};

$global["$context_state"] = rule 'in' $STR '{' | 'out' $STR '}';

/* Use context-sensitive parsing */
result = op(parse)('in "sql" { SELECT * FROM users }')();
```

### **3. Dynamic Token Types**

```grapa
/* Define custom token types */
$global["$custom_token"] = rule $SYM("SQL_KEYWORD") | $SYM("JSON_PATH") | $SYM("XPATH_EXPR");

/* Use custom tokens in grammar */
$global["$sql_statement"] = rule $SYM("SQL_KEYWORD") <$sql_expression> {
    op(keyword:$1, expr:$2) {
        return execute_sql_statement(keyword, expr);
    }
};
```

## Performance Optimization

### **1. Grammar Compilation Caching**

```grapa
/* Cache compiled grammar rules for performance */
grammar_cache = {};

compile_grammar = op(rule_name, rule_def) {
    if (grammar_cache[rule_name]) {
        return grammar_cache[rule_name];
    };
    
    compiled = op(parse)(rule_def);
    grammar_cache[rule_name] = compiled;
    return compiled;
};

/* Use cached grammar */
sql_grammar = compile_grammar("sql", "$custom_command = rule select...");
json_grammar = compile_grammar("json", "$custom_function = rule $STR->$STR...");
```

### **2. Lazy Evaluation**

```grapa
/* Lazy evaluation of complex expressions */
$global["$lazy_expression"] = rule 'lazy' '{' <$expression> '}' {
    op(expr:$3) {
        /* Create lazy evaluation wrapper */
        return op() {
            return expr();
        };
    }
};

/* Execute lazy expression only when needed */
lazy_result = op(parse)('lazy { expensive_calculation() }')();
/* Expression not evaluated until lazy_result() is called */
```

## Advanced Variable Manipulation

### **1. Indirect Variable Assignment (`@@`)**

A key discovery in Grapa's rule system is the `@@` syntax for indirect variable access and assignment:

```grapa
/* Basic variable reference */
x = 1;
y = "x";  /* y contains the string "x" */

/* Variable reference */
@y;       /* Returns "x" (the variable name) */

/* Indirect variable access */
@@y;      /* Returns 1 (the value of variable x) */

/* Indirect variable assignment */
@@y = 10; /* Assigns 10 to variable x */
@@y += 1; /* Increments variable x */

/* In rule implementations */
$custom_command = rule for $ID from <$comp> to <$comp> <$command> {
    op(var:$2, start:$4, end:$6, body:$8){
        /* Instead of complex string concatenation */
        /* op()(var + " = " + start())(); */
        
        /* Use indirect assignment */
        @@var = start();
        
        while (@@var <= end()) {
            body();
            @@var += 1;
        };
    }
};
```

**Benefits:**
- **Cleaner Syntax**: Eliminates complex string concatenation
- **Better Performance**: No string operations for variable assignment
- **More Readable**: Intent is much clearer
- **Type Safety**: Direct variable access without string manipulation

### **Historical Context: `@` Symbol Evolution**

The `@` symbol in Grapa has evolved from the original design:

**Original Design (Early Grapa):**
```grapa
/* Variables were literal by default */:
x = 5;        /* x was the literal string "x" */
@x;           /* @x was needed to get the value 5 */
```

**Current Design:**
```grapa
/* Automatic dereferencing for cleaner syntax */
x = 5;        /* x automatically gets the value 5 */
@x;           /* @x gets the variable name "x" (for rule tokens) */
@@x;          /* @@x gets the value of the variable named by x */
```

**Why This Matters:**
- **Cleaner Code**: No need for `@` symbols everywhere
- **Rule Tokens**: `@` still needed in `< >` tokens for variable dereferencing
- **Indirect Access**: `@@` provides one more level of indirection for dynamic variable names

**Consistent `@` Pattern:**
The `@` symbol serves the same purpose throughout Grapa - **dereferencing**:

```grapa
/* In rule definitions */
{@<assignappend,{$1,$4}>}  /* @ dereferences the rule token */
{@<if,{$3,$5}>}            /* @ dereferences the rule token */

/* In variable references */
<@variable>                 /* @ dereferences the variable name */
<@tb["d"]>                 /* @ dereferences the array access */

/* In direct usage */
@x;                        /* @ dereferences the variable name */
@@x;                       /* @ dereferences twice */
```

**Unified Concept:** `@` always means "get the actual value/operation, not the literal token"

### **System Namespace Protection: `$` Prefix**

Grapa uses the `$` prefix as a **system namespace protection mechanism**:

```grapa
/* User namespace (recommended) */
x = 5;                    /* User variable */
name = "hello";           /* User string */

/* System namespace (reserved) */
$x = 5;                   /* System variable */
$name = "hello";          /* System string */
```

**Why This Matters:**
- **Everything in Grapa is variables** (language, classes, functions, etc.)
- **Protection**: Prevents accidental override of core system features
- **Guideline**: Use `$` prefix only when accessing system namespace is necessary
- **Interchangeable**: Both work, but system namespace should be avoided in user code

## Error Handling and Debugging

### **1. Grammar Error Recovery**

```grapa
/* Robust grammar with error recovery */
$global["$robust_rule"] = rule <$primary_rule> | <$fallback_rule> {
    op(primary:$1, fallback:$2) {
        if (primary.type() != $ERR) {
            return primary;
        } else {
            ("Recovering from error with fallback").echo();
            return fallback;
        };
    }
};
```

### **2. Debug Grammar Execution**

```grapa
/* Debug grammar execution */
$global["$debug_rule"] = rule 'debug' <$rule> {
    op(rule:$2) {
        ("Debug: Executing rule").echo();
        result = rule();
        ("Debug: Rule result: " + result).echo();
        return result;
    }
};

/* Use debug wrapper */
debug_result = op(parse)('debug { custom_rule() }')();
```

## Real-World Applications

### **1. Configuration Management System**

```grapa
/* Complete configuration DSL */
$global["$config_system"] = rule 'config' '{' <$config_entries> '}' {
    op(entries:$3) {
        ("Loading configuration...").echo();
        i = 0;
        while (i < entries.len()) {
            entry = entries[i];
            apply_config_entry(entry);
            i += 1;
        };
        ("Configuration loaded successfully").echo();
    }
};

$global["$config_entries"] = rule <$config_entry> (';' <$config_entries> | );

$global["$config_entry"] = rule $ID '=' <$config_value> | 'include' $STR;

/* Load configuration */
op(parse)('config { 
    server_name = "myapp"; 
    port = 8080; 
    debug = true; 
    include "local.conf"; 
}')();
```

### **2. Data Transformation Pipeline**

```grapa
/* ETL pipeline DSL */
$global["$etl_pipeline"] = rule 'etl' '{' <$etl_steps> '}' {
    op(steps:$3) {
        ("Starting ETL pipeline...").echo();
        result = null;
        i = 0;
        while (i < steps.len()) {
            step = steps[i];
            ("Executing step: " + step).echo();
            result = execute_etl_step(step, result);
            i += 1;
        };
        ("ETL pipeline completed").echo();
        return result;
    }
};

$global["$etl_steps"] = rule <$etl_step> ('|' <$etl_steps> | );

$global["$etl_step"] = rule 'extract' $STR | 'transform' $STR | 'load' $STR;

/* Execute ETL pipeline */
result = op(parse)('etl { 
    extract "source.csv" | 
    transform "clean_data" | 
    load "target.db" 
}')();
```

### **3. API Definition Language**

```grapa
/* API definition DSL */
$global["$api_definition"] = rule 'api' $STR '{' <$api_endpoints> '}' {
    op(name:$2, endpoints:$4) {
        ("Defining API: " + name).echo();
        return create_api(name, endpoints);
    }
};

$global["$api_endpoints"] = rule <$api_endpoint> (';' <$api_endpoints> | );

$global["$api_endpoint"] = rule $STR $STR '{' <$endpoint_body> '}' {
    op(method:$1, path:$2, body:$4) {
        return define_endpoint(method, path, body);
    }
};

/* Define API */
api = op(parse)('api "user_api" { 
    GET "/users" { return get_users(); } | 
    POST "/users" { return create_user(request.body); } | 
    PUT "/users/{id}" { return update_user(id, request.body); } 
}')();
```

## Best Practices

### **1. Grammar Design**
1. **Start Simple**: Begin with basic rules and gradually add complexity
2. **Use Clear Names**: Name rules and tokens descriptively
3. **Handle Errors**: Include error recovery and fallback mechanisms
4. **Document Grammar**: Provide clear documentation for custom syntax

### **2. Performance**
1. **Cache Compiled Rules**: Reuse compiled grammar rules when possible
2. **Optimize Token Types**: Use appropriate token types for efficiency
3. **Lazy Evaluation**: Defer expensive operations until needed
4. **Profile Execution**: Monitor performance of custom grammar rules

### **3. Integration**
1. **Leverage Existing Libraries**: Use Grapa's C++ libraries when possible
2. **Follow Patterns**: Use established patterns like direct BNF integration for native features, `$custom_command`/`$custom_function` as variables that leverage existing grammar rules
3. **Test Thoroughly**: Validate grammar rules with comprehensive testing
4. **Version Control**: Track grammar changes and maintain compatibility

## Conclusion

Grapa's executable BNF system provides unprecedented power for language design and extension. By understanding and leveraging this system, you can:

- **Create custom languages** tailored to specific domains
- **Build sophisticated DSLs** for complex workflows
- **Extend Grapa itself** with new syntax and capabilities
- **Implement protocol parsers** for various data formats
- **Design configuration languages** for dynamic systems

The key is to start with simple patterns and gradually build complexity, always keeping in mind the three-phase processing model and the distinction between commands and functions.

## See Also
- [Advanced Topics](advanced_topics.md)
- [Multi-Syntax Programming](multi_syntax_programming.md)
- [Grammar Design](syntax/grammar_design.md)
- [Examples](examples.md)
- [Use Cases](use_cases/index.md) 
