---
tags:
  - user
  - highlevel
  - sql
  - database
  - syntax
---
# SQL Integration in Grapa

Grapa's executable BNF architecture allows you to add **true native SQL syntax** to the language using `custom_command` and `custom_function` rules. This is **not** manual parsing - it's **native syntax** that integrates seamlessly with Grapa's grammar system.

## True Native Syntax Implementation

The SQL integration uses Grapa's **rule-based grammar system** exclusively. All parsing is handled by Grapa's grammar engine - **no manual string manipulation** is required.

### Working SQL Syntax Implementation

```grapa
/* SELECT with individual field parsing - TRUE NATIVE SYNTAX */
custom_command = custom_command | rule select $STR ',' $STR from $STR {op(field1:$2,field2:$4,table_name:$6){
    /* Implementation receives already-parsed tokens from Grapa's grammar */
    /* NO manual parsing - Grapa grammar handled everything */
}};

/* INSERT with individual value parsing - TRUE NATIVE SYNTAX */
custom_command = custom_command | rule insert into $STR values $STR ',' $INT ',' $STR {op(table_name:$3,name:$6,age:$8,city:$10){
    /* Implementation receives already-parsed, structured data */
    /* NO manual parsing - Grapa grammar captured each component */
}};

/* UPDATE with native grammar */
custom_command = custom_command | rule update $STR set $STR '=' $STR where $STR '=' $STR {op(table_name:$2,field:$4,value:$6,where_field:$8,where_value:$10){
    /* All components parsed by Grapa's grammar system */
}};
```

**Key Point**: The `|` operator **adds** to existing rules rather than replacing them, allowing multiple syntax extensions to coexist.

### Using SQL Syntax

Once defined, SQL syntax works **natively** in Grapa:

```grapa
/* Execute SQL using op(parse)() - TRUE NATIVE SYNTAX */
op(parse)("select name,age from users")();
op(parse)("insert into users values David,40,Seattle")();
op(parse)("update users set age=45 where name=David")();

/* Use in expressions */
user_count = op(parse)("count(*) from users")();
if (user_count > 3) {
    ("More than 3 users found!").echo();
};
```

## Power of Syntax Extension

This demonstrates **true native syntax** - not string storage or manual parsing:

- **Grammar Integration**: SQL syntax is processed by the same parser that handles all Grapa syntax
- **Token Capture**: Individual components (field names, values, operators) are captured as separate tokens
- **Structured Data**: Implementation code receives already-parsed, structured data
- **No Manual Parsing**: Zero string manipulation or manual parsing required
- **Seamless Integration**: SQL expressions work in Grapa expressions, conditionals, and functions
- **Rule Addition**: Uses `|` operator to add to existing grammar rules without replacement

## Two Implementation Approaches

### 1. Syntax Extension (Recommended)
Uses `custom_command = custom_command | rule` and `custom_function = custom_function | rule` to define **true native syntax**:

```grapa
/* TRUE NATIVE SYNTAX - No manual parsing */
custom_command = custom_command | rule select $STR from $STR {op(fields:$2,table:$4){
    /* Grapa grammar provides already-parsed tokens */
}};
```

### 2. Function-based (Not Recommended)
Uses manual string parsing and manipulation:

```grapa
/* MANUAL PARSING - Not true native syntax */
sql_select = op(fields, table) {
    /* Manual parsing required */
    field_list = fields.split(",");
    /* ... manual string manipulation ... */
};
```

**Recommendation**: Always use **syntax extension** for true native language integration.

## SQL Syntax Extension Pattern

The key features for true native syntax are:

- **`custom_command = custom_command | rule`**: Defines action-based syntax (SELECT, INSERT, UPDATE)
- **`custom_function = custom_function | rule`**: Defines expression-based syntax (COUNT, SUM, AVG)
- **`op(parse)()`**: Executes the custom syntax
- **Token Capture**: Use `$STR`, `$INT`, etc. to capture individual components
- **No Manual Parsing**: Let Grapa's grammar handle all parsing
- **Rule Addition**: Use `|` operator to add to existing rules

## Comparison with Other Languages

Unlike other programming languages that require:
- Manual parser implementation
- String manipulation
- Token extraction
- Custom lexers

Grapa provides **true native syntax** through its executable BNF architecture, making SQL (and other languages) **first-class citizens** in the language.

## Use Cases

- **Database Operations**: Native SQL syntax for database queries
- **Data Analysis**: SQL expressions in data processing pipelines
- **Report Generation**: SQL queries embedded in Grapa applications
- **Multi-Syntax Applications**: Mix SQL, JSON, XML, and custom DSLs seamlessly

This demonstrates Grapa's unique capability to add **true native syntax** for other programming languages and domain-specific languages through its rule-based grammar system.
