---
tags:
  - advanced
  - multi-syntax
  - sql
  - json
  - xml
  - language-extension
---

# Multi-Syntax Programming in Grapa

## Overview

Grapa's executable BNF architecture enables support for multiple syntaxes within a single language. This guide explores how to use and extend Grapa with JSON, XML, HTML (native), SQL (example scripts), and other syntaxes, making it a powerful multi-paradigm programming environment.

## Key Concepts

### **Executable BNF Architecture**
Grapa's `$RULE` system goes beyond traditional parsing - it's an executable BNF that can:
- **Define custom syntax** at runtime
- **Execute arbitrary code** during parsing
- **Build execution trees** for later evaluation
- **Support multiple syntaxes** simultaneously

### **Syntax Extension Approaches**

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
Use `custom_command`/`custom_function` as variables that leverage existing grammar rules:

- **`custom_command`**: For domain-specific actions (ETL, DSLs, protocol parsers) - leverages `$comp` and `$command` rules
- **`custom_function`**: For domain-specific expressions - leverages `$comp` rules

### **Lexical Processing and Flags**
For complex formats like XML and HTML, Grapa uses lexical flags to activate special processing:

```grapa
/* XML processing with lexical flags */
xml_data = "<user><name>John</name><age>25</age></user>";
processed = xml_data.encode("XML-GRAPA");
/* Internally uses: $&<user><name>John</name><age>25</age></user>$& */

/* HTML processing with lexical flags */
html_data = "<div><h1>Title</h1><p>Content</p></div>";
processed = html_data.encode("HTML-GRAPA");
/* Internally uses: $&<div><h1>Title</h1><p>Content</p></div>$& */
```

**Key Features:**
- **`$&` Enclosures**: Activate special lexer processing for XML/HTML content
- **Fallback Compatibility**: Works without flags in most cases
- **Complex Syntax Handling**: Enables proper processing of syntax that might conflict with normal Grapa syntax
- **Format-Specific Processing**: Different lexical modes for different data formats

## SQL Syntax Integration (Example Implementation)

### **Basic SQL Usage**

Grapa demonstrates SQL syntax integration through example scripts that show how to add domain-specific SQL syntax using `custom_command` and `custom_function` as variables:

```grapa
/* Define SQL syntax as variables */
custom_command = rule select $STR from $STR {op(fields:$2,table_name:$4){
    ("SQL SELECT: " + fields + " FROM " + table_name).echo();
    /* Database query implementation */
}};

custom_function = rule count '(' $STR ')' from $STR {op(field:$3,table_name:$6){
    ("SQL COUNT: " + field + " FROM " + table_name).echo();
    records = table.ls();
    return records.len();
}};

/* Use SQL syntax directly */
select * from users;
user_count = count(*) from users;
```

### **Advanced SQL Features**

```grapa
/* INSERT statement (example implementation) */
custom_command = rule insert into $STR values $STR {op(table_name:$3,values_str:$5){
    ("SQL INSERT: " + table_name + " VALUES " + values_str).echo();
    /* Insert implementation */
}};

/* UPDATE statement (example implementation) */
custom_command = rule update $STR set $STR where $STR {op(table_name:$2,set_clause:$4,where_clause:$6){
    ("SQL UPDATE: " + table_name + " SET " + set_clause + " WHERE " + where_clause).echo();
    /* Update implementation */
}};

/* Use complex SQL directly (via example scripts) */
insert into users values John,25,New York;
update users set age=26 where name=John;
```

## JSON Syntax Support

### **Native JSON Operations**

```grapa
/* JSON path queries */
custom_function = rule $STR '->' $STR {op(json:$1,path:$3){
    /* JSON path extraction */
    return json.json().get(path);
}};

custom_function = rule $STR '->>' $STR {op(json:$1,path:$3){
    /* JSON path extraction as string */
    return json.json().get(path).str();
}};

/* Execute JSON queries */
data = '{"user": {"name": "John", "age": 30}}';
name = op(parse)('data->"user"->"name"')();
age = op(parse)('data->>"user"->>"age"')();
```

### **JSON Functions**

```grapa
/* JSON validation */
custom_function = rule json_valid '(' $STR ')' {op(json_str:$3){
    return json_str.json().type() != $ERR;
}};

/* JSON aggregation */
custom_function = rule json_agg '(' $STR ')' {op(expr:$3){
    /* JSON array aggregation */
    return expr.json();
}};
```

## XML Syntax Support

### **XPath Queries**

```grapa
/* XPath expression */
custom_function = rule xpath '(' $STR ',' $STR ')' {op(xml:$3,xpath_expr:$5){
    /* XPath evaluation */
    return xml.xml().xpath(xpath_expr);
}};

/* XML validation */
custom_function = rule xml_valid '(' $STR ')' {op(xml_str:$3){
    return xml_str.xml().type() != $ERR;
}};

/* Execute XPath */
xml_data = '<user><name>John</name><age>30</age></user>';
name = op(parse)('xpath(xml_data, "/user/name")')();
```

## HTML Syntax Support

### **HTML Parsing and Manipulation**

```grapa
/* HTML element selection */
custom_function = rule $STR '>>' $STR {op(html:$1,selector:$3){
    /* CSS selector implementation */
    return html.html().select(selector);
}};

/* HTML attribute extraction */
custom_function = rule $STR '>>' $STR '@' $STR {op(html:$1,selector:$3,attr:$5){
    /* Attribute extraction */
    element = html.html().select(selector);
    return element.attr(attr);
}};

/* Execute HTML queries */
page = '<html><body><div class="user">John</div></body></html>';
user = op(parse)('page>>".user"')();
```

## Creating Custom Syntax

### **Defining New Syntax**

```grapa
/* Custom configuration syntax */
custom_command = rule config $STR '=' $STR {op(key:$2,value:$4){
    /* Configuration setting */
    set_config(key, value);
}};

/* Custom loop syntax */
custom_command = rule repeat $INT times '{' <$command_list> '}' {op(count:$2,body:$5){
    /* Repeat loop implementation */
    i = 0;
    while (i < count) {
        body();
        i += 1;
    };
}};

/* Execute custom syntax */
op(parse)('config theme = dark')();
op(parse)('repeat 5 times { "Hello".echo(); }')();
```

### **Advanced Syntax Patterns**

```grapa
/* Template string interpolation */
custom_function = rule '`' <> '`' {op(template:$2){
    /* Template processing */
    result = template;
    /* Replace variables */
    result = result.replace(/\$\{(\w+)\}/g, op(match) {
        return get_variable(match[1]);
    });
    return result;
}};

/* Range syntax */
custom_function = rule $INT '..' $INT {op(start:$1,end:$3){
    /* Range generation */
    return (end - start + 1).range(start, 1);
}};

/* Execute advanced syntax */
message = op(parse)('`Hello ${name}!`')();
numbers = op(parse)('1..10')();
```

## Integration Patterns

### **Combining Multiple Syntaxes**

```grapa
/* SQL with JSON fields */
custom_command = rule select $STR from $STR where $STR '->' $STR '=' $STR {
    op(fields:$2,table:$4,json_field:$6,path:$8,value:$10){
        /* SQL query with JSON path filtering */
        records = table.ls();
        filtered = records.filter(op(record) {
            json_data = record.get(json_field);
            return json_data.json().get(path) == value;
        });
        /* Display results */
    }
};

/* Execute multi-syntax query */
op(parse)('select * from users where metadata->"city" = "New York"')();
```

### **Dynamic Syntax Loading**

```grapa
/* Load syntax from file */
load_syntax = op(filename) {
    syntax_def = $file().read(filename);
    /* Parse and register syntax */
    op(parse)(syntax_def)();
};

/* Load custom syntax */
load_syntax("my_syntax.grc");
op(parse)("my_custom_command arg1 arg2")();
```

## Performance Considerations

### **Syntax Compilation**
- Custom syntax is compiled once and cached
- Execution trees are optimized for performance
- Multiple syntaxes can coexist without conflicts

### **Memory Management**
- Syntax definitions are stored efficiently
- Execution trees are garbage collected
- Large syntax sets don't impact performance

## Advanced Rule Token Patterns

### **Wrapper Function Pattern for ETL**

The wrapper function pattern is a powerful technique for using custom functions in rule tokens:

```grapa
/* Define ETL processing functions */
validate_data = op(p){p.grep("valid")};
transform_data = op(p){p.upper()};
filter_data = op(p){p.len() > 10 ? p : null};

/* Use wrapper functions in rule tokens */
@global["$etl_pipeline"] = rule 
    <$raw_data,op(b:$1){validate_data(b)}>
    <$validated,op(b:$1){transform_data(b)}>
    <$transformed,op(b:$1){filter_data(b)}>
    {@<var,{$1}>}
```

**Key Benefits:**
- **Explicit Parameter Passing**: `op(b:$1){my_func(b)}` clearly shows parameter flow
- **Works with Any Function**: Can wrap any function definition
- **ETL-Friendly**: Perfect for data transformation pipelines
- **Reusable**: Functions can be defined once and used in multiple rules

**Real-World ETL Example:**
```grapa
/* Data processing pipeline */
clean_data = op(p){p.trim().lower()};
validate_email = op(p){p.grep(/^[^@]+@[^@]+\.[^@]+$/)?p:null};
format_output = op(p){p.upper() + " - PROCESSED"};

/* ETL grammar */
@global["$data_pipeline"] = rule 
    'process' $STR {
        op(input:$2) {
            /* Apply ETL pipeline */
            cleaned = op(parse)('clean ' + input)();
            validated = op(parse)('validate ' + cleaned)();
            formatted = op(parse)('format ' + validated)();
            return formatted;
        }
    }
;

@global["$clean_step"] = rule <$raw_data,op(b:$1){clean_data(b)}> {@<var,{$1}>};
@global["$validate_step"] = rule <$cleaned_data,op(b:$1){validate_email(b)}> {@<var,{$1}>};
@global["$format_step"] = rule <$validated_data,op(b:$1){format_output(b)}> {@<var,{$1}>};
```

## Best Practices

### **Syntax Design**
1. **Use `custom_command` for actions** (statements, commands)
2. **Use `custom_function` for expressions** (operators, functions)
3. **Follow existing patterns** for consistency
4. **Document syntax thoroughly** for users
5. **Use wrapper functions** for complex ETL processing

### **Integration**
1. **Leverage existing C++ libraries** when possible
2. **Use `op(parse)()` for execution**
3. **Test syntax thoroughly** before deployment
4. **Consider performance implications**

### **Error Handling**
```grapa
/* Robust syntax definition */
custom_function = rule safe_query '(' $STR ')' {op(query:$3){
    try {
        return execute_query(query);
    } catch (error) {
        ("Query error: " + error).echo();
        return $err("Query failed");
    };
}};
```

## Advanced Examples

### **Domain-Specific Language (DSL)**

```grapa
/* Configuration DSL */
custom_command = rule server '{' <$server_config> '}' {op(config:$3){
    /* Server configuration */
    create_server(config);
}};

custom_command = rule port $INT {op(port_num:$2){
    /* Port setting */
    set_port(port_num);
}};

/* Execute DSL */
op(parse)('server { port 8080 }')();
```

### **Data Processing Pipeline**

```grapa
/* Pipeline syntax */
custom_command = rule pipeline '{' <$pipeline_steps> '}' {op(steps:$3){
    /* Execute pipeline */
    result = null;
    i = 0;
    while (i < steps.len()) {
        step = steps[i];
        result = execute_step(step, result);
        i += 1;
    };
    return result;
}};

/* Execute pipeline */
result = op(parse)('pipeline { load "data.csv" | filter "age > 25" | sort "name" }')();
```

## Conclusion

Grapa's multi-syntax capabilities make it uniquely powerful for:
- **Data processing** with JSON, XML (native), and SQL (example scripts)
- **Web development** with HTML and CSS selectors
- **Configuration management** with custom DSLs
- **Domain-specific applications** with tailored syntax

The executable BNF architecture provides unprecedented flexibility while maintaining performance and consistency across all syntaxes.

## See Also
- [Advanced Topics](advanced_topics.md)
- [Language Syntax Extension](type/rule.md#language-syntax-extension)
- [Grammar Design](syntax/grammar_design.md)
- [Examples](examples.md)
- [Use Cases](use_cases/index.md) 