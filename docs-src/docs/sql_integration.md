---
tags:
  - user
  - highlevel
  - sql
  - database
  - syntax
---
# SQL Integration in Grapa

> **Status:** Example Implementation
> 
> This document describes how SQL syntax can be integrated into Grapa using the executable BNF system. This is currently demonstrated through example scripts showing the pattern for extending Grapa's grammar with domain-specific syntax.

## Overview

Grapa's executable BNF architecture allows for seamless integration of SQL syntax alongside native Grapa code. While SQL is not natively supported, the language provides the building blocks to implement SQL-like functionality through custom syntax rules and the `$file().table()` system.

## Key Features

### **Native Database Integration**
- **`$file().table()`** - Built-in relational database with B-tree indexing
- **Automatic field management** - No need to pre-define schemas
- **Complex object support** - Store structured data as JSON objects
- **Efficient queries** - Leverage built-in indexing for performance

### **SQL Syntax Extension Pattern**
- **Custom commands** - Define SQL-like syntax using `custom_command`
- **Custom functions** - Implement SQL operations as Grapa functions
- **Executable BNF** - Parse SQL statements using Grapa's grammar system
- **Seamless integration** - Mix SQL and Grapa syntax in the same script

## Implementation Examples

### **Basic SQL Pattern**

```grapa
/* Define SQL syntax as variables */
custom_command = op(fields, table_name) {
    ("SQL SELECT: " + fields + " FROM " + table_name).echo();
    /* Implementation using $file().table() */
};

custom_function = op(field, table_name) {
    ("SQL COUNT: " + field + " FROM " + table_name).echo();
    /* Implementation using $file().table() */
};

/* Use SQL syntax directly */
custom_command("name, age", "users");
custom_function("age", "users");
```

### **Advanced SQL Features**

```grapa
/* INSERT pattern */
insert_command = op(table_name, values) {
    values_str = values.str();
    ("SQL INSERT: " + table_name + " VALUES " + values_str).echo();
    /* Implementation using kb.set() */
};

/* UPDATE pattern */
update_command = op(table_name, set_clause, where_clause) {
    ("SQL UPDATE: " + table_name + " SET " + set_clause + " WHERE " + where_clause).echo();
    /* Implementation using kb.set() with conditions */
};

/* Use complex SQL directly */
insert_command("users", {"name": "Alice", "age": 25});
update_command("users", "age = 26", "name = 'Alice'");
```

### **Complete SQL Implementation Example**

See [SQL Syntax Example](../examples/sql_syntax_example.grc) for a full implementation demonstrating:

- **SELECT statements** with field specification
- **INSERT operations** with value mapping
- **WHERE clauses** with condition parsing
- **JOIN operations** using multiple tables
- **Aggregate functions** like COUNT, SUM, AVG

## Database Operations

### **Table Creation and Management**

```grapa
/* Create a table */
kb = $file().table("ROW");

/* Define fields (optional - Grapa handles this automatically) */
kb.mkfield("name", "STR", "VAR");
kb.mkfield("age", "INT", "FIX", 4);
kb.mkfield("city", "STR", "VAR");

/* Store data */
kb.set("user1", "Alice", "name");
kb.set("user1", 25, "age");
kb.set("user1", "New York", "city");
```

### **Query Operations**

```grapa
/* Retrieve data */
user_data = kb.get("user1", "name");  /* "Alice" */
user_age = kb.get("user1", "age");    /* 25 */

/* Complex queries using functional methods */
all_users = kb.keys().range().map(op(i) {
    key = kb.keys()[i];
    {"name": kb.get(key, "name"), "age": kb.get(key, "age")};
});
```

## Integration with Other Syntaxes

### **SQL + JSON Processing**

```grapa
/* SQL with JSON fields */
sql_result = custom_command("name, preferences", "users");
json_data = sql_result.parse();  /* Parse JSON preferences */

/* SQL query with JSON path filtering */
filtered_result = custom_command("name", "users", "preferences.city = 'New York'");
```

### **SQL + XML Processing**

```grapa
/* Export SQL results to XML */
sql_data = custom_command("name, age, city", "users");
xml_output = sql_data.to_xml();  /* Convert to XML format */
```

## Use Cases

### **Data Analysis Pipelines**

```grapa
/* Multi-format data processing */
json_data = load_json("customers.json");
xml_data = load_xml("orders.xml");
sql_data = custom_command("customer_id, total", "sales");

/* Unified processing */
result = process_all_formats(json_data, xml_data, sql_data);
```

### **Web Applications**

```grapa
/* API with SQL-like queries */
api_handler = op(request) {
    if (request.type == "sql") {
        result = custom_command(request.fields, request.table, request.where);
        return result.to_json();
    };
};
```

### **ETL Processes**

```grapa
/* Extract from SQL-like source */
source_data = custom_command("*", "source_table");

/* Transform using Grapa */
transformed = source_data.range().map(op(record) {
    /* Apply transformations */
    record.age = record.age + 1;
    record.status = record.age > 18 ? "adult" : "minor";
    return record;
});

/* Load to target */
transformed.range().map(op(record) {
    kb.set(record.id, record.name, "name");
    kb.set(record.id, record.age, "age");
});
```

## Best Practices

### **Performance Optimization**

1. **Use indexing** - Leverage `$file().table()` built-in B-tree indexing
2. **Batch operations** - Group multiple operations together
3. **Selective queries** - Only retrieve needed fields
4. **Connection pooling** - Reuse table connections

### **Error Handling**

```grapa
/* SQL with error handling */
try {
    result = custom_command("name, age", "users", "age > 25");
} catch (error) {
    ("SQL Error: " + error).echo();
    /* Fallback to native Grapa query */
    result = kb.keys().range().map(op(i) {
        key = kb.keys()[i];
        age = kb.get(key, "age");
        if (age > 25) {
            return {"name": kb.get(key, "name"), "age": age};
        };
    });
}
```

### **Security Considerations**

1. **Input validation** - Sanitize SQL-like input
2. **Parameter binding** - Use parameterized queries
3. **Access control** - Implement proper permissions
4. **SQL injection prevention** - Validate and escape input

## Future Enhancements

### **Planned Features**

- **Native SQL parser** - Built-in SQL syntax support
- **Advanced joins** - Multi-table relationship support
- **Stored procedures** - SQL-like function definitions
- **Transaction support** - ACID compliance for complex operations

### **Integration Roadmap**

- **SQLite compatibility** - Direct SQLite query support
- **PostgreSQL integration** - Native PostgreSQL driver
- **MySQL support** - MySQL query compatibility
- **NoSQL bridges** - MongoDB, Redis integration

## Related Documentation

- [Multi-Syntax Programming](multi_syntax_programming.md) - Overview of syntax integration
- [Database Quick Reference](../database/quick_reference.md) - Native database operations
- [Executable BNF](language_design_with_executable_bnf.md) - Grammar extension system
- [Examples](../examples.md) - Complete SQL implementation examples

## Example Files

- [SQL Syntax Example](../examples/sql_syntax_example.grc) - Complete SQL implementation
- [Database Test Suite](../../test/database/) - Database operation tests
- [Multi-Format Examples](../examples/) - SQL + JSON + XML integration examples
