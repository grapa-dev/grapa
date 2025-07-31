# BNF Review Notes for $grapa.grc

**Created:** 2024-12-19
**Purpose:** Track ideas and approaches for BNF grammar enhancements

## Key Insight: Custom Language Extension for Prototyping

### For Loop Implementation Strategy
- **Use Custom Language Extension method** to prototype new BNF additions like `for` loops
- This allows testing new language features without modifying core `$grapa.grc`
- Perfect for user experimentation and feature validation
- Could serve as a great educational example of Grapa's extensibility

### Potential For Loop Syntax Options
```grapa
// Option 1: Traditional C-style
for (i = 0; i < 10; i++) {
    print(i)
}

// Option 2: Range-based (Python-like)
for (i in range(10)) {
    print(i)
}

// Option 3: Collection iteration
for (item in collection) {
    print(item)
}

// Option 4: Functional style with map
map(range(10), {i => print(i)})
```

### Implementation Approach
1. **Prototype Phase**: Use Custom Language Extension to add for loop grammar
2. **Test Phase**: Validate syntax and semantics with real examples
3. **Integration Phase**: If successful, integrate into core `$grapa.grc`
4. **Documentation**: Update examples to show both extension and core approaches

### Benefits of Extension Approach
- **Risk Mitigation**: Test features without breaking existing code
- **User Empowerment**: Users can experiment with language design
- **Educational Value**: Demonstrates Grapa's meta-programming capabilities
- **Iterative Development**: Refine syntax before committing to core

### Example Extension Implementation
```grapa
// Extend grammar with for loop
extend_grammar = {
    "statement": "original_statement | for_statement",
    "for_statement": "'for' '(' for_init ';' condition ';' increment ')' '{' statement* '}'",
    "for_init": "assignment | variable_declaration",
    "condition": "expression",
    "increment": "assignment | function_call"
}

// Implement for loop execution
execute_for = {ast =>
    if (ast.type == "for_statement") {
        // Execute initialization
        execute_statement(ast.init)
        
        // Main loop
        while (evaluate_expression(ast.condition)) {
            // Execute loop body
            execute_statements(ast.body)
            
            // Execute increment
            execute_statement(ast.increment)
        }
    }
}
```

### SQL Syntax Extension Example
```grapa
// Extend grammar with SQL syntax
sql_grammar = {
    "statement": "original_statement | sql_statement",
    "sql_statement": "select_statement | insert_statement | update_statement | delete_statement",
    "select_statement": "'SELECT' select_list 'FROM' table_name where_clause? order_clause? limit_clause?",
    "select_list": "'*' | column_list",
    "column_list": "column_name (',' column_name)*",
    "where_clause": "'WHERE' condition",
    "condition": "column_name operator value | condition 'AND' condition | condition 'OR' condition",
    "operator": "'==' | '!=' | '>' | '<' | '>=' | '<=' | 'LIKE'",
    "order_clause": "'ORDER BY' column_name ('ASC' | 'DESC')?",
    "limit_clause": "'LIMIT' number"
}

// Implement SQL execution
execute_sql = {ast =>
    if (ast.type == "select_statement") {
        // Parse the query and execute against data source
        table_data = get_table_data(ast.table_name)
        
        if (ast.where_clause) {
            table_data = filter_data(table_data, ast.where_clause)
        }
        
        if (ast.order_clause) {
            table_data = sort_data(table_data, ast.order_clause)
        }
        
        if (ast.limit_clause) {
            table_data = limit_data(table_data, ast.limit_clause)
        }
        
        return select_columns(table_data, ast.select_list)
    }
}
```

## Other Language Features to Consider
- **SQL Syntax**: High priority - excellent DSL example, practical for data processing
- **Switch/Case statements**
- **Try/Catch exception handling**
- **Class/Object syntax**
- **Import/Module system enhancements**
- **Decorator syntax**

## Review Priorities
1. **For Loop**: High priority - commonly requested, good educational example
2. **SQL Syntax**: High priority - excellent DSL example, practical for data processing
3. **Exception Handling**: Medium priority - important for robust code
4. **Enhanced Import System**: Medium priority - improves modularity
5. **Class Syntax**: Lower priority - may conflict with Grapa's functional approach

## Notes
- Always prototype with Custom Language Extension first
- Consider impact on existing code and parsing performance
- Ensure new features align with Grapa's design philosophy
- Document both extension and core approaches for users 