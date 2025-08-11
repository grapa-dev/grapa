---
tags:
  - use-case
  - multi-syntax
  - data-processing
  - sql
  - json
  - xml
  - etl
---

# Multi-Syntax Data Processing

## Overview

Grapa's multi-syntax capabilities enable powerful data processing workflows that combine SQL, JSON, XML, and custom syntaxes seamlessly. This use case demonstrates how to build sophisticated data pipelines using multiple native syntaxes.

## Scenario: ETL Pipeline with Multi-Syntax Support

### **Problem Statement**
You need to process data from multiple sources:
- **CSV files** with user data
- **JSON APIs** with product information  
- **XML feeds** with inventory data
- **Database tables** with transaction history

The goal is to create a unified data processing pipeline that can handle all these formats natively.

### **Solution: Multi-Syntax ETL Pipeline**

```grapa
/* Multi-Syntax ETL Pipeline Example */

/* Step 1: Define SQL syntax for database operations using custom_command and custom_function as variables */
custom_command = rule select $STR from $STR {op(fields:$2,table_name:$4){
    ("SQL: SELECT " + fields + " FROM " + table_name).echo();
    /* Database query implementation */
}};

custom_function = rule count '(' $STR ')' from $STR {op(field:$3,table_name:$6){
    records = get_table(table_name).ls();
    return records.len();
}};

/* Step 2: Define JSON syntax for API data */
custom_function = rule $STR '->' $STR {op(json:$1,path:$3){
    /* JSON path extraction */
    return json.json().get(path);
}};

custom_function = rule json_extract '(' $STR ',' $STR ')' {op(json:$3,path:$5){
    /* JSON extraction with error handling */
    try {
        return json.json().get(path);
    } catch (error) {
        return $err("JSON extraction failed");
    };
}};

/* Step 3: Define XML syntax for feed processing */
custom_function = rule xpath '(' $STR ',' $STR ')' {op(xml:$3,xpath_expr:$5){
    /* XPath evaluation */
    return xml.xml().xpath(xpath_expr);
}};

custom_function = rule xml_extract '(' $STR ',' $STR ')' {op(xml:$3,path:$5){
    /* XML extraction with validation */
    if (xml.xml().valid()) {
        return xml.xml().extract(path);
    } else {
        return $err("Invalid XML");
    };
}};

/* Step 4: Define custom pipeline syntax */
custom_command = rule pipeline '{' <$pipeline_steps> '}' {op(steps:$3){
    ("Pipeline execution started").echo();
    result = null;
    i = 0;
    while (i < steps.len()) {
        step = steps[i];
        ("Executing step: " + step).echo();
        result = execute_pipeline_step(step, result);
        i += 1;
    };
    ("Pipeline completed").echo();
    return result;
}};

/* Step 5: Execute multi-syntax ETL pipeline */
("=== Multi-Syntax ETL Pipeline ===").echo();

/* Load and process CSV data */
("Loading CSV data...").echo();
csv_data = $file().read("users.csv");
users = parse_csv(csv_data);

/* Query database for transactions */
("Querying database...").echo();
op(parse)("select user_id, amount from transactions")();
transaction_count = op(parse)("count(*) from transactions")();

/* Extract data from JSON API */
("Processing JSON API data...").echo();
api_response = fetch_api("https://api.example.com/products");
product_names = op(parse)('api_response->"products"->"name"')();
product_count = op(parse)('json_extract(api_response, "$.products.length")')();

/* Parse XML inventory feed */
("Processing XML inventory...").echo();
xml_feed = fetch_xml("https://feed.example.com/inventory");
inventory_items = op(parse)('xpath(xml_feed, "//item/name")')();
total_items = op(parse)('xml_extract(xml_feed, "//inventory/@total")')();

/* Execute unified pipeline */
("Executing unified pipeline...").echo();
result = op(parse)('pipeline { 
    load "users.csv" | 
    join "transactions" | 
    enrich "products.json" | 
    validate "inventory.xml" | 
    output "final_report.json" 
}')();

("=== Pipeline Complete ===").echo();
```

## Advanced Multi-Syntax Patterns

### **1. SQL with JSON Fields**

```grapa
/* Query database with JSON field filtering */
custom_command = rule select $STR from $STR where $STR '->' $STR '=' $STR {
    op(fields:$2,table:$4,json_field:$6,path:$8,value:$10){
        ("SQL with JSON: " + fields + " FROM " + table + " WHERE " + json_field + "->" + path + " = " + value).echo();
        
        records = get_table(table).ls();
        filtered = records.filter(op(record) {
            json_data = record.get(json_field);
            return json_data.json().get(path) == value;
        });
        
        display_results(fields, filtered);
    }
};

/* Execute SQL with JSON filtering */
op(parse)('select * from users where metadata->"city" = "New York"')();
op(parse)('select name, age from users where preferences->"theme" = "dark"')();
```

### **2. XML with SQL Integration**

```grapa
/* Process XML and store in database */
custom_command = rule process_xml $STR into $STR {op(xml_file:$2,table_name:$4){
    ("Processing XML: " + xml_file + " into " + table_name).echo();
    
    xml_data = $file().read(xml_file);
    items = op(parse)('xpath(xml_data, "//item")')();
    
    i = 0;
    while (i < items.len()) {
        item = items[i];
        name = op(parse)('xml_extract(item, "name")')();
        price = op(parse)('xml_extract(item, "price")')();
        
        /* Insert into database using SQL */
        op(parse)('insert into ' + table_name + ' values ' + name + ',' + price)();
        i += 1;
    };
}};

/* Execute XML to SQL pipeline */
op(parse)('process_xml inventory.xml into products')();
```

### **3. Custom DSL for Data Validation**

```grapa
/* Define validation DSL */
custom_command = rule validate $STR '{' <$validation_rules> '}' {op(data:$2,rules:$4){
    ("Validating data: " + data).echo();
    
    validation_result = true;
    i = 0;
    while (i < rules.len()) {
        rule = rules[i];
        if (!apply_validation_rule(data, rule)) {
            validation_result = false;
        };
        i += 1;
    };
    
    if (validation_result) {
        ("Validation passed").echo();
    } else {
        ("Validation failed").echo();
    };
    
    return validation_result;
}};

custom_command = rule field $STR required {op(field_name:$2){
    return {"type": "required", "field": field_name};
}};

custom_command = rule field $STR type $STR {op(field_name:$2,type_name:$4){
    return {"type": "type_check", "field": field_name, "expected_type": type_name};
}};

/* Execute validation DSL */
is_valid = op(parse)('validate user_data { 
    field "name" required | 
    field "age" type "integer" | 
    field "email" type "email" 
}')();
```

## Performance Optimization

### **1. Syntax Compilation Caching**

```grapa
/* Cache compiled syntax for performance */
syntax_cache = {};

compile_syntax = op(syntax_name, syntax_def) {
    if (syntax_cache[syntax_name]) {
        return syntax_cache[syntax_name];
    };
    
    compiled = op(parse)(syntax_def);
    syntax_cache[syntax_name] = compiled;
    return compiled;
};

/* Use cached syntax */
sql_syntax = compile_syntax("sql", "custom_command = rule select...");
json_syntax = compile_syntax("json", "custom_function = rule $STR->$STR...");
xml_syntax = compile_syntax("xml", "custom_function = rule xpath...");
```

### **2. Parallel Multi-Syntax Processing**

```grapa
/* Process multiple data sources in parallel */
process_sources = op(sources) {
    results = sources.map(op(source) {
        if (source.type == "csv") {
            return process_csv(source.file);
        } else if (source.type == "json") {
            return op(parse)('json_extract(' + source.data + ', "$")')();
        } else if (source.type == "xml") {
            return op(parse)('xpath(' + source.data + ', "//root")')();
        } else if (source.type == "sql") {
            return op(parse)('select * from ' + source.table)();
        };
    }, 4);  /* 4 threads */
    
    return results;
};

/* Execute parallel processing */
sources = [
    {"type": "csv", "file": "users.csv"},
    {"type": "json", "data": api_response},
    {"type": "xml", "data": xml_feed},
    {"type": "sql", "table": "transactions"}
];

results = process_sources(sources);
```

## Real-World Applications

### **1. E-commerce Data Integration**

```grapa
/* E-commerce data pipeline */
custom_command = rule ecommerce_pipeline '{' <$pipeline_steps> '}' {op(steps:$3){
    ("E-commerce pipeline started").echo();
    
    /* Process customer data from CSV */
    customers = process_csv("customers.csv");
    
    /* Query order database */
    orders = op(parse)("select customer_id, total from orders")();
    
    /* Extract product data from JSON API */
    products = op(parse)('api_response->"products"')();
    
    /* Parse inventory from XML feed */
    inventory = op(parse)('xpath(inventory_feed, "//item")')();
    
    /* Generate unified report */
    report = generate_unified_report(customers, orders, products, inventory);
    
    ("E-commerce pipeline completed").echo();
    return report;
}};

/* Execute e-commerce pipeline */
report = op(parse)('ecommerce_pipeline { 
    load_customers | 
    query_orders | 
    fetch_products | 
    sync_inventory | 
    generate_report 
}')();
```

### **2. Financial Data Processing**

```grapa
/* Financial data processing with validation */
custom_command = rule financial_pipeline '{' <$pipeline_steps> '}' {op(steps:$3){
    ("Financial pipeline started").echo();
    
    /* Load transaction data */
    transactions = op(parse)("select * from transactions")();
    
    /* Validate transaction data */
    validation_result = op(parse)('validate transactions { 
        field "amount" required | 
        field "amount" type "decimal" | 
        field "date" type "date" 
    }')();
    
    if (!validation_result) {
        ("Validation failed - stopping pipeline").echo();
        return $err("Validation failed");
    };
    
    /* Process with JSON enrichment */
    enriched = transactions.map(op(tx) {
        customer_data = op(parse)('json_extract(customers, "$.customers[?(@.id==' + tx.customer_id + ')]")')();
        return merge_transaction_customer(tx, customer_data);
    });
    
    /* Generate financial report */
    report = generate_financial_report(enriched);
    
    ("Financial pipeline completed").echo();
    return report;
}};
```

## Benefits of Multi-Syntax Approach

### **1. Unified Processing**
- **Single Language**: All data processing in one language
- **Consistent Syntax**: Familiar patterns across different data types
- **Integrated Workflows**: Seamless combination of different syntaxes

### **2. Performance**
- **Native Implementation**: Leverages existing C++ libraries
- **Compilation Caching**: Syntax compiled once and reused
- **Parallel Processing**: Multi-threaded execution across syntaxes

### **3. Maintainability**
- **Clear Separation**: Each syntax handles its domain
- **Modular Design**: Easy to add new syntaxes
- **Error Handling**: Consistent error patterns across syntaxes

### **4. Extensibility**
- **Custom DSLs**: Create domain-specific languages
- **Plugin Architecture**: Add new syntaxes without core changes
- **Future-Proof**: Easy to adapt to new data formats

## Conclusion

Multi-syntax data processing in Grapa provides a powerful approach to handling complex data integration scenarios. By combining SQL, JSON, XML, and custom syntaxes, you can build sophisticated ETL pipelines that are:

- **Unified**: All processing in one language
- **Performant**: Leverages native implementations
- **Maintainable**: Clear separation of concerns
- **Extensible**: Easy to add new capabilities

This approach makes Grapa uniquely suited for modern data processing challenges where multiple data formats and sources need to be integrated seamlessly.

## See Also
- [Multi-Syntax Programming](multi_syntax_programming.md)
- [ETL Data Engineering](etl_data_engineering.md)
- [Database File System](database_file_system.md)
- [Examples](examples.md)
- [Advanced Topics](advanced_topics.md) 