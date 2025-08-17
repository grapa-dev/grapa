---
tags:
  - maintainer
  - implementation
  - sql-syntax
  - custom-command
  - custom-function
  - rule-system
  - op-pattern
---

# SQL Syntax Implementation Patterns

## Overview

This document captures the implementation patterns discovered for adding native SQL syntax to Grapa using `custom_command` and `custom_function` as variables. These patterns demonstrate how Grapa can support domain-specific syntaxes (SQL, configuration languages, protocol parsers) through variables that leverage existing grammar rules.

## Key Discoveries

### 1. **Critical Distinction: custom_command vs custom_function**

**`custom_command`**: For actions that perform operations (no return value)
- Used for domain-specific statements like `SELECT`, `INSERT`, `UPDATE`, `DELETE`
- Used as variables of type `$RULE` that leverage existing `$comp` and `$command` rules
- Executes actions and displays results

**`custom_function`**: For expressions that return values
- Used for domain-specific functions like `COUNT`, `SUM`, `AVG`
- Used as variables of type `$RULE` that leverage existing `$comp` rules
- Returns values that can be assigned to variables

### 2. **Usage Pattern: Direct Syntax**

The `custom_command` and `custom_function` mechanisms work by:
1. **Variable Assignment**: `custom_command` and `custom_function` are variables of type `$RULE`
2. **Rule Definition**: They define new grammar rules that leverage existing `$comp` and `$command` rules
3. **Session Integration**: They run in the same session as the loaded `$grapa.grc` grammar
4. **Direct Usage**: Once defined, they can be used directly like any other language syntax

## Implementation Examples

### SELECT Statement (custom_command)

```grapa
/* Define SELECT as custom_command (action) */
custom_command = rule select $STR from $STR {op(fields:$2,table_name:$4){
    ("SQL SELECT: " + fields + " FROM " + table_name).echo();
    
    /* Get the appropriate table object */
    table_obj = null;
    if (table_name == "users") {
        table_obj = users_table;
    } else {
        ("Unknown table: " + table_name).echo();
        return;
    };
    
    /* Get all records from the table */
    records = table_obj.ls();
    if (records.len() == 0) {
        ("Table " + table_name + " is empty").echo();
        return;
    };
    
    /* Parse the fields to select */
    if (fields == "*") {
        field_names = ["name", "age", "city"];
    } else {
        field_names = fields.split(",");
        i = 0;
        while (i < field_names.len()) {
            field_names[i] = field_names[i].trim();
            i = i + 1;
        };
    };
    
    /* Display results */
    ("Query results:").echo();
    i = 0;
    while (i < records.len()) {
        record = records[i];
        key = record.getname(1);
        
        row_output = "";
        j = 0;
        while (j < field_names.len()) {
            field_name = field_names[j];
            field_value = table_obj.get(key, field_name);
            if (field_value.type() != $ERR) {
                if (j > 0) {
                    row_output = row_output + " | ";
                };
                row_output = row_output + field_name + ": " + field_value;
            };
            j = j + 1;
        };
        
        if (row_output != "") {
            ("  " + row_output).echo();
        };
        
        i = i + 1;
    };
    
    ("Query returned " + records.len() + " rows").echo();
}};

/* Use SELECT directly */
select * from users;
select name,age from users;
```

### COUNT Function (custom_function)

```grapa
/* Define COUNT as custom_function (expression) */
custom_function = rule count '(' $STR ')' from $STR {op(field:$3,table_name:$6){
    ("SQL COUNT: " + field + " FROM " + table_name).echo();
    
    /* Get the appropriate table object */
    table_obj = null;
    if (table_name == "users") {
        table_obj = users_table;
    } else {
        ("Unknown table: " + table_name).echo();
        return 0;
    };
    
    /* Count records */
    records = table_obj.ls();
    count = records.len();
    
    ("Count returned: " + count).echo();
    return count;  /* Return the count value */
}};

/* Execute COUNT */
user_count = op(parse)("count(*) from users")();
("User count: " + user_count).echo();

/* Use COUNT in expressions */
if (op(parse)("count(*) from users")() > 3) {
    ("There are more than 3 users").echo();
} else {
    ("There are 3 or fewer users").echo();
};
```

### INSERT Statement (custom_command)

```grapa
/* Define INSERT as custom_command (action) */
custom_command = rule insert into $STR values $STR {op(table_name:$3,values_str:$5){
    ("SQL INSERT: " + table_name + " VALUES " + values_str).echo();
    
    /* Get the appropriate table object */
    table_obj = null;
    if (table_name == "users") {
        table_obj = users_table;
    } else {
        ("Unknown table: " + table_name).echo();
        return;
    };
    
    /* Parse values (comma-separated) */
    values = values_str.split(",");
    if (values.len() < 3) {
        ("Invalid values format. Need: name,age,city").echo();
        return;
    };
    
    /* Generate a new key */
    existing_records = table_obj.ls();
    new_key = "${table_name}_${existing_records.len() + 1}".interpolate();
    
    /* Insert values */
    table_obj.set(new_key, values[1].trim(), "name");
    table_obj.set(new_key, values[2].trim().int(), "age");
    table_obj.set(new_key, values[3].trim(), "city");
    
    ("Successfully inserted user: " + values[1].trim()).echo();
}};

/* Execute INSERT */
op(parse)("insert into users values David,40,Seattle")();
```

## Integration Points in Grammar

### Command Integration

```grapa
@global["$command"]
    = rule '{' <$command_list> ';' '}' {@<scope,{$2}>}
    | $SYSID("INCLUDE") <$comp> {@<include,{$2}>}
    | include <$comp> {@<include,{$2}>}
    | $SYSID("RESET") {@<reset,{}>}
    | reset {@<reset,{}>}
    | if '(' <$comp> ')' <$command> ';' <$else> {@<if,{$3,$5,$7}>}
    | if '(' <$comp> ')' <$command> <$else> {@<if,{$3,$5,$6}>}
    | if '(' <$comp> ')' <$command> {@<if,{$3,$5}>}
    | while '(' <$comp> ')' <$command> {@<while,{$3,$5}>}
    | switch '(' <$comp> ')' '{' <$case_list> '}' {@<switch,{$3,$6}>}
    | break {@<break,{}>}
    | return '(' <$comp> ')' {@<return,{$3}>}
    | exit {@<exit,{}>}
    | <custom_command>  /* ← Domain-specific processing (SQL, ETL, DSLs) */
    | <$litname> '=' <$comp> {@<assign,{$1,$3}>}
    | <$comp> '=' <$comp> {@<assign,{$1,$3}>}
    | <$comp>
    ;
```

### Function Integration

```grapa
@global["$function"]
    = rule '[' ']' {@<createarray,{}>}
    | '[' <$comp_list> ']' {@<createarray,{$2}>}
    | '{' '}' {@<createlist,{}>}
    | '{' <$comp_list> '}' {@<createlist,{$2}>}
    | '(' <$comp_list> ')' {@<createtuple,{$2}>}
    | <$litname> '(' <$comp_list> ')' {@<call,{$1,$3}>}
    | <$litname> '(' ')' {@<call,{$1,{}}>}
    | <custom_function>  /* ← SQL functions (COUNT, SUM, AVG) */
    | <$litname> '.' <$litname> '(' <$comp_list> ')' {@<search,{$1,$3,$5}>}
    | <$litname> '.' <$litname> '(' ')' {@<search,{$1,$3,{}}>}
    | <$litname> '.' <$litname> {@<search,{$1,$3,{}}>}
    | <$litname> '[' <$comp> ']' {@<search,{$1,$3,{}}>}
    | <$litname> '[' <$comp> ':' <$comp> ']' {@<search,{$1,$3,$5,{}}>}
    | <$litname> '[' <$comp> ':' ']' {@<search,{$1,$3,{},{}}>}
    | <$litname> '[' ':' <$comp> ']' {@<search,{$1,{},{},$3}>}
    | <$litname> '[' ':' ']' {@<search,{$1,{},{},{}}>}
    | <$litname> '[' <$comp> ']' '=' <$comp> {@<assign,{$1,$3,$6}>}
    | <$litname> '[' <$comp> ':' <$comp> ']' '=' <$comp> {@<assign,{$1,$3,$5,$8}>}
    | <$litname> '[' <$comp> ':' ']' '=' <$comp> {@<assign,{$1,$3,{},{},$6}>}
    | <$litname> '[' ':' <$comp> ']' '=' <$comp> {@<assign,{$1,{},{},$4,$7}>}
    | <$litname> '[' ':' ']' '=' <$comp> {@<assign,{$1,{},{},{},$5}>}
    | <$litname> '.' <$litname> '=' <$comp> {@<assign,{$1,$3,$5}>}
    | <$litname> '=' <$comp> {@<assign,{$1,$3}>}
    | <$litname>
    | <$comp> '.' <$litname> '(' <$comp_list> ')' {@<search,{$1,$3,$5}>}
    | <$comp> '.' <$litname> '(' ')' {@<search,{$1,$3,{}}>}
    | <$comp> '.' <$litname> {@<search,{$1,$3,{}}>}
    | <$comp> '[' <$comp> ']' {@<search,{$1,$3,{}}>}
    | <$comp> '[' <$comp> ':' <$comp> ']' {@<search,{$1,$3,$5,{}}>}
    | <$comp> '[' <$comp> ':' ']' {@<search,{$1,$3,{},{}}>}
    | <$comp> '[' ':' <$comp> ']' {@<search,{$1,{},{},$3}>}
    | <$comp> '[' ':' ']' {@<search,{$1,{},{},{}}>}
    | <$comp> '[' <$comp> ']' '=' <$comp> {@<assign,{$1,$3,$6}>}
    | <$comp> '[' <$comp> ':' <$comp> ']' '=' <$comp> {@<assign,{$1,$3,$5,$8}>}
    | <$comp> '[' <$comp> ':' ']' '=' <$comp> {@<assign,{$1,$3,{},{},$6}>}
    | <$comp> '[' ':' <$comp> ']' '=' <$comp> {@<assign,{$1,{},{},$4,$7}>}
    | <$comp> '[' ':' ']' '=' <$comp> {@<assign,{$1,{},{},{},$5}>}
    | <$comp> '.' <$litname> '=' <$comp> {@<assign,{$1,$3,$5}>}
    | <$comp> '=' <$comp> {@<assign,{$1,$3}>}
    | <$comp>
    ;
```

## Complete Working Example

```grapa
/* SQL Syntax Demo - Complete Working Example */

/* Step 1: Create sample data */
users_table = $file().table("ROW");
users_table.mkfield("name", "STR", "VAR");
users_table.mkfield("age", "INT");
users_table.mkfield("city", "STR", "VAR");

users_table.set("user1", "John", "name");
users_table.set("user1", 25, "age");
users_table.set("user1", "New York", "city");

users_table.set("user2", "Jane", "name");
users_table.set("user2", 30, "age");
users_table.set("user2", "Los Angeles", "city");

/* Step 2: Define SQL syntax */
custom_command = rule select $STR from $STR {op(fields:$2,table_name:$4){
    ("SQL SELECT: " + fields + " FROM " + table_name).echo();
    if (table_name == "users") {
        records = users_table.ls();
        ("Found " + records.len() + " records:").echo();
        i = 0;
        while (i < records.len()) {
            record = records[i];
            key = record.getname(1);
            name = users_table.get(key, "name");
            age = users_table.get(key, "age");
            ("  " + name + " (age " + age + ")").echo();
            i = i + 1;
        };
    } else {
        ("Unknown table: " + table_name).echo();
    };
}};

custom_function = rule count '(' $STR ')' from $STR {op(field:$3,table_name:$6){
    ("SQL COUNT: " + field + " FROM " + table_name).echo();
    if (table_name == "users") {
        records = users_table.ls();
        count = records.len();
        ("Count: " + count).echo();
        return count;
    } else {
        ("Unknown table: " + table_name).echo();
        return 0;
    };
}};

/* Step 3: Execute SQL syntax */
("=== Testing SQL Syntax ===").echo();

("1. Testing SELECT (action):").echo();
op(parse)("select * from users")();

("\n2. Testing COUNT (expression):").echo();
user_count = op(parse)("count(*) from users")();
("User count: " + user_count).echo();

("\n3. Using COUNT in expressions:").echo();
if (op(parse)("count(*) from users")() > 1) {
    ("There are multiple users").echo();
} else {
    ("There is only one user").echo();
};

("\n=== SQL Syntax Demo Complete ===").echo();
```

## Key Insights

### 1. **Architectural Power**
The `custom_command` and `custom_function` mechanisms enable Grapa to support domain-specific syntaxes through isolated rule execution, without modifying core language infrastructure.

### 2. **Execution Pattern**
The `op(parse)()` pattern provides a clean way to execute custom syntax after it's defined, enabling dynamic language extension.

### 3. **Multi-Syntax Potential**
This pattern can be extended to support:
- **JSON**: Native JSON operators and path queries
- **XML**: Native XPath and XML processing
- **HTML**: Native HTML parsing and manipulation
- **Regex**: Native regex syntax and operations

### 4. **Implementation Strategy**
1. **Define syntax** using `custom_command` (domain-specific actions) and `custom_function` (domain-specific expressions)
2. **Implement logic** using existing C++ libraries
3. **Execute** using `op(parse)()` pattern
4. **Integrate** into grammar at the appropriate points

## Conclusion

The SQL syntax implementation patterns demonstrate Grapa's unique capability to support multiple native syntaxes through its executable BNF architecture. This approach provides:

- **Immediate Implementation**: No core language changes required
- **Seamless Integration**: Native syntax feels like part of the language
- **Extensibility**: Easy to add new syntaxes following the same pattern
- **Performance**: Leverages existing optimized C++ libraries

This makes Grapa uniquely positioned to become a multi-syntax language that can handle SQL, JSON, XML, and HTML natively while maintaining the power and flexibility of its core architecture. 