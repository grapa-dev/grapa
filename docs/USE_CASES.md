# Grapa Use Cases

Grapa is designed to be inviting and usable for the broadest audience, from data engineers and scientists to educators and language researchers. Below are key use cases, each with a high-level overview and links to examples in both Grapa and Python (GrapaPy).

---

## 1. ETL / Data Engineering
Grapa excels at Extract, Transform, Load (ETL) workflows, offering parallelism, unlimited precision, and unified file/database APIs. Ideal for data cleaning, transformation, and large-scale processing.

### Key Features for ETL:
- **Parallel Processing**: Use `$thread` and `map/reduce/filter` for high-performance data transformation
- **Unlimited Precision**: Handle large numbers and precise calculations with `$INT` and `$FLOAT`
- **Unified APIs**: Seamlessly work with files, databases, and network resources
- **Memory Efficiency**: Process large datasets without loading everything into memory

### Example: Parallel CSV Processing
```grapa
/* Extract: Read multiple CSV files in parallel */
files = ["data1.csv", "data2.csv", "data3.csv"];
data = files.map(op(file) { $file().read(file).split("\n"); });

/* Transform: Clean and validate data in parallel */
clean_data = data.map(op(rows) { 
    rows.filter(op(row) { 
        fields = row.split(",");
        fields.len() == 5 && fields.get(0) != "";  /* Validate row structure */
    });
});

/* Load: Aggregate results and save to database */
total_records = clean_data.reduce(op(sum, rows) { sum + rows.len(); }, 0);
("Processed " + total_records.str() + " records across " + files.len().str() + " files").echo();
```

### Example: Data Validation Pipeline
```grapa
/* Define validation rules */
rules = {
    "email": {"pattern": "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"},
    "age": {"min": 0, "max": 150},
    "salary": {"min": 0, "max": 1000000}
};

/* Validate data in parallel */
validate_record = op(record, rules) {
    errors = [];
    if (!record.get("email").match(rules.get("email").get("pattern"))) {
        errors += "Invalid email format";
    };
    if (record.get("age") < rules.get("age").get("min") || record.get("age") > rules.get("age").get("max")) {
        errors += "Age out of range";
    };
    if (record.get("salary") < rules.get("salary").get("min") || record.get("salary") > rules.get("salary").get("max")) {
        errors += "Salary out of range";
    };
    {"record": record, "errors": errors, "valid": errors.len() == 0};
};

/* Process validation results */
results = data.map(op(record) { validate_record(record, rules); });
valid_records = results.filter(op(result) { result.get("valid"); });
invalid_records = results.filter(op(result) { !result.get("valid"); });

("Valid records: " + valid_records.len().str()).echo();
("Invalid records: " + invalid_records.len().str()).echo();
```

### Example: Time Series Data Processing
```grapa
/* Process time series data with unlimited precision */
process_timestamps = op(data) {
    data.map(op(row) {
        timestamp = $TIME().parse(row.get("timestamp"));
        value = row.get("value").float();
        {
            "timestamp": timestamp,
            "value": value,
            "hour": timestamp.hour(),
            "day": timestamp.day(),
            "month": timestamp.month()
        };
    });
};

/* Aggregate by time periods */
aggregate_by_hour = op(processed_data) {
    grouped = processed_data.group(op(record) { record.get("hour"); });
    grouped.map(op(hour, records) {
        {
            "hour": hour,
            "count": records.len(),
            "avg_value": records.reduce(op(sum, r) { sum + r.get("value"); }, 0) / records.len(),
            "min_value": records.map(op(r) { r.get("value"); }).min(),
            "max_value": records.map(op(r) { r.get("value"); }).max()
        };
    });
};
```

- **See also:** [Python ETL Examples](PYTHON_USE_CASES.md#1-etl--data-engineering)

## 2. Compiler/BNF Learning
With executable BNF and mutable grammar, Grapa is perfect for teaching and experimenting with language design, parsing, and compiler construction.

### Key Features for BNF/Compiler Work:
- **Executable BNF**: Define and test grammars directly in Grapa
- **Mutable Grammar**: Modify language syntax at runtime
- **AST Manipulation**: Build and transform abstract syntax trees
- **Meta-programming**: Generate code and modify language behavior

### Example: Simple Expression Parser
```grapa
/* Define a simple arithmetic expression grammar */
expression_grammar = {
    "expression": "term (('+' | '-') term)*",
    "term": "factor (('*' | '/') factor)*", 
    "factor": "number | '(' expression ')'",
    "number": "[0-9]+"
};

/* Parse and evaluate expressions */
parse_expression = op(expr) {
    ast = $rule().parse(expr, expression_grammar);
    $rule().evaluate(ast);
};

/* Test the parser */
test_expressions = ["2+3*4", "(1+2)*3", "10/2+5"];
results = test_expressions.map(op(expr) { parse_expression(expr); });
```

### Example: Custom Language Extension
```grapa
/* Extend Grapa with a custom "repeat" construct */
extend_grammar = {
    "statement": "original_statement | repeat_statement",
    "repeat_statement": "'repeat' number 'times' '{' statement* '}'"
};

/* Implement the repeat functionality */
execute_repeat = op(ast) {
    if (ast.get("type") == "repeat_statement") {
        count = ast.get("count");
        body = ast.get("body");
        i = 0;
        while (i < count) {
            $rule().execute(body);
            i = i + 1;
        };
    };
};
```

- **See also:** [Python BNF/Compiler Examples](PYTHON_USE_CASES.md#2-compilerbnf-learning)

## 3. High-Precision Math & Scientific Computing
Grapa's $INT, $FLOAT, and $TIME types support unlimited precision, making it valuable for scientific, cryptographic, and financial applications.

### Key Features for Math/Scientific Work:
- **Unlimited Precision**: Handle arbitrarily large numbers and precise calculations
- **Time Series**: Built-in time manipulation and analysis
- **Parallel Computation**: Distribute mathematical workloads across threads
- **Memory Efficiency**: Process large datasets without precision loss

### Example: Cryptographic Calculations
```grapa
/* Generate large prime numbers */
generate_prime = op(bits) {
    while (true) {
        candidate = $random().genbits(bits);
        if ($crypt().isprime(candidate)) {
            candidate;
        };
    };
};

/* RSA key generation */
generate_rsa_keys = op(bits) {
    p = generate_prime(bits / 2);
    q = generate_prime(bits / 2);
    n = p * q;
    phi = (p - 1) * (q - 1);
    e = 65537;  /* Common public exponent */
    d = $crypt().modinv(e, phi);
    
    {
        "public_key": {"n": n, "e": e},
        "private_key": {"n": n, "d": d}
    };
};

/* Test with 1024-bit keys */
keys = generate_rsa_keys(1024);
("Generated RSA keys with " + keys.get("public_key").get("n").str().len().str() + " bits").echo();
```

### Example: Financial Calculations
```grapa
/* Calculate compound interest with unlimited precision */
compound_interest = op(principal, rate, time, periods) {
    rate_per_period = rate / periods;
    total_periods = time * periods;
    principal * (1 + rate_per_period).pow(total_periods);
};

/* Calculate mortgage payments */
mortgage_payment = op(principal, annual_rate, years) {
    monthly_rate = annual_rate / 12 / 100;
    total_payments = years * 12;
    principal * (monthly_rate * (1 + monthly_rate).pow(total_payments)) / 
              ((1 + monthly_rate).pow(total_payments) - 1);
};

/* Example calculations */
loan_amount = 300000;
annual_rate = 3.5;
loan_years = 30;

monthly_payment = mortgage_payment(loan_amount, annual_rate, loan_years);
total_paid = monthly_payment * loan_years * 12;
total_interest = total_paid - loan_amount;

("Monthly payment: $" + monthly_payment.str()).echo();
("Total interest: $" + total_interest.str()).echo();
```

- **See also:** [Python Math Examples](PYTHON_USE_CASES.md#3-high-precision-math--scientific-computing)

## 4. Parallel/Concurrent Programming
Grapa's $thread, $net, and map/reduce/filter features enable true parallelism, overcoming Python's GIL and enabling high-performance data processing.

### Key Features for Parallel Programming:
- **True Parallelism**: Bypass GIL limitations with native threads
- **Network Parallelism**: Concurrent network operations
- **Functional Programming**: Map/reduce/filter for data parallelism
- **Thread Safety**: Built-in synchronization primitives

### Example: Parallel Data Processing
```grapa
/* Process large dataset in parallel */
process_chunk = op(chunk) {
    chunk.map(op(item) {
        /* Expensive computation */
        result = complex_calculation(item);
        {"input": item, "result": result};
    });
};

/* Split data and process in parallel */
parallel_process = op(data, num_threads) {
    chunk_size = data.len() / num_threads;
    chunks = [];
    
    i = 0;
    while (i < num_threads) {
        start = i * chunk_size;
        end = (i == num_threads - 1) ? data.len() : (i + 1) * chunk_size;
        chunks += data.slice(start, end);
        i = i + 1;
    };
    
    /* Process chunks in parallel */
    results = chunks.map(op(chunk) { process_chunk(chunk); });
    
    /* Combine results */
    results.flatten();
};

/* Example usage */
large_dataset = generate_test_data(1000000);
processed_data = parallel_process(large_dataset, 8);
("Processed " + processed_data.len().str() + " items in parallel").echo();
```

### Example: Concurrent Network Operations
```grapa
/* Fetch multiple URLs concurrently */
fetch_urls = op(urls) {
    responses = urls.map(op(url) {
        try {
            response = $net().get(url);
            {"url": url, "success": true, "data": response.get("body")};
        } catch (error) {
            {"url": url, "success": false, "error": error.get("message")};
        };
    });
    responses;
};

/* Process API endpoints in parallel */
api_endpoints = [
    "https://api.example.com/users",
    "https://api.example.com/products", 
    "https://api.example.com/orders"
];

results = fetch_urls(api_endpoints);
successful = results.filter(op(r) { r.get("success"); });
failed = results.filter(op(r) { !r.get("success"); });

("Successful requests: " + successful.len().str()).echo();
("Failed requests: " + failed.len().str()).echo();
```

- **See also:** [Python Parallelism Examples](PYTHON_USE_CASES.md#4-parallelconcurrent-programming)

## 5. Web/Data Scraping & Automation
Grapa's $net and parallelism features are powerful for web scraping, automation, and API integration.

### Key Features for Web Scraping:
- **Concurrent Requests**: Multiple simultaneous network operations
- **HTML/XML Parsing**: Built-in parsing capabilities
- **Rate Limiting**: Control request frequency
- **Error Handling**: Robust error recovery

### Example: Web Scraper with Rate Limiting
```grapa
/* Scrape multiple pages with rate limiting */
scrape_pages = op(urls, delay_ms) {
    results = urls.map(op(url, index) {
        /* Add delay between requests */
        if (index > 0) {
            $sys().sleep(delay_ms);
        };
        
        try {
            response = $net().get(url);
            if (response.get("status") == 200) {
                {
                    "url": url,
                    "success": true,
                    "content": response.get("body"),
                    "size": response.get("body").len()
                };
            } else {
                {"url": url, "success": false, "error": "HTTP " + response.get("status").str()};
            };
        } catch (error) {
            {"url": url, "success": false, "error": error.get("message")};
        };
    });
    results;
};

/* Extract data from HTML */
extract_data = op(html_content) {
    /* Parse HTML and extract specific elements */
    doc = $XML().parse(html_content);
    titles = doc.select("h1, h2, h3");
    links = doc.select("a[href]");
    
    {
        "titles": titles.map(op(t) { t.get("text"); }),
        "links": links.map(op(l) { l.get("href"); })
    };
};

/* Example usage */
target_urls = [
    "https://example.com/page1",
    "https://example.com/page2",
    "https://example.com/page3"
];

scraped_data = scrape_pages(target_urls, 1000);  /* 1 second delay */
extracted_data = scraped_data.map(op(page) { 
    if (page.get("success")) {
        extract_data(page.get("content"));
    } else {
        {"error": page.get("error")};
    };
});
```

- **See also:** [Python Web Scraping Examples](PYTHON_USE_CASES.md#5-webdata-scraping--automation)

## 6. Database & File System Integration
Unified APIs for files and databases boost productivity for backend developers, sysadmins, and data engineers.

### Key Features for File/DB Work:
- **Unified Interface**: Same APIs for files and databases
- **Query Language**: SQL-like syntax for file operations
- **Transaction Support**: ACID compliance for data integrity
- **Parallel Access**: Concurrent read/write operations

### Example: File System Operations
```grapa
/* Recursive file processing */
process_files = op(directory, file_pattern) {
    files = $file().find(directory, file_pattern);
    
    results = files.map(op(file) {
        try {
            content = $file().read(file);
            stats = $file().stat(file);
            
            {
                "file": file,
                "size": stats.get("size"),
                "modified": stats.get("modified"),
                "lines": content.split("\n").len(),
                "words": content.split(" ").len()
            };
        } catch (error) {
            {"file": file, "error": error.get("message")};
        };
    });
    
    results;
};

/* Database-like file queries */
query_files = op(directory) {
    /* Find all Python files larger than 1KB */
    large_python_files = $file().select("* from " + directory + " where name like '*.py' and size > 1024");
    
    /* Group by directory */
    grouped = large_python_files.group(op(file) { $file().dirname(file); });
    
    grouped.map(op(dir, files) {
        {
            "directory": dir,
            "file_count": files.len(),
            "total_size": files.reduce(op(sum, f) { sum + f.get("size"); }, 0)
        };
    });
};
```

- **See also:** [Python File/DB Examples](PYTHON_USE_CASES.md#6-database--file-system-integration)

## 7. Education & Prototyping
Grapa's dynamic grammar and meta-programming are ideal for rapid prototyping and teaching programming concepts.

### Key Features for Education:
- **Interactive Learning**: Immediate feedback and experimentation
- **Visual Debugging**: See AST and execution flow
- **Language Design**: Create custom languages and DSLs
- **Algorithm Visualization**: Step-through execution

### Example: Teaching Recursion
```grapa
/* Visualize recursive function execution */
factorial_with_trace = op(n, depth) {
    indent = "  ".repeat(depth);
    (indent + "factorial(" + n.str() + ")").echo();
    
    if (n <= 1) {
        (indent + "return 1").echo();
        1;
    } else {
        result = n * factorial_with_trace(n - 1, depth + 1);
        (indent + "return " + n.str() + " * " + (n - 1).str() + "! = " + result.str()).echo();
        result;
    };
};

/* Example: Calculate 4! with trace */
"Calculating 4! with execution trace:".echo();
result = factorial_with_trace(4, 0);
("Final result: " + result.str()).echo();
```

### Example: Custom DSL for Data Processing
```grapa
/* Define a simple data processing language */
data_dsl_grammar = {
    "pipeline": "command*",
    "command": "filter_command | map_command | reduce_command",
    "filter_command": "'filter' condition",
    "map_command": "'map' expression", 
    "reduce_command": "'reduce' expression 'with' initial_value",
    "condition": "field operator value",
    "expression": "field | function_call",
    "operator": "'==' | '!=' | '>' | '<' | '>=' | '<='"
};

/* Example DSL usage */
dsl_script = "
filter age > 18
map salary * 1.1
reduce sum with 0
";

/* Execute the DSL */
execute_dsl = op(script, data) {
    ast = $rule().parse(script, data_dsl_grammar);
    $rule().execute(ast, data);
};
```

- **See also:** [Python Education Examples](PYTHON_USE_CASES.md#7-education--prototyping)

---

For each use case, see the corresponding section for detailed examples in both Grapa and Python (GrapaPy). 