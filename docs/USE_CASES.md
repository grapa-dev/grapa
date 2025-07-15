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
// Extract: Read multiple CSV files in parallel
files = ["data1.csv", "data2.csv", "data3.csv"]
data = map(files, {file => read(file).split("\n")})

// Transform: Clean and validate data in parallel
clean_data = map(data, {rows => 
    filter(rows, {row => 
        fields = row.split(",")
        length(fields) == 5 && fields[0] != ""  // Validate row structure
    })
})

// Load: Aggregate results and save to database
total_records = reduce(clean_data, 0, {sum, rows => sum + length(rows)})
print("Processed", total_records, "records across", length(files), "files")
```

### Example: Data Validation Pipeline
```grapa
// Define validation rules
rules = {
    "email": {pattern => "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"},
    "age": {min => 0, max => 150},
    "salary": {min => 0, max => 1000000}
}

// Validate data in parallel
validate_record = {record, rules =>
    errors = []
    if (!match(record.email, rules.email.pattern)) {
        errors.push("Invalid email format")
    }
    if (record.age < rules.age.min || record.age > rules.age.max) {
        errors.push("Age out of range")
    }
    if (record.salary < rules.salary.min || record.salary > rules.salary.max) {
        errors.push("Salary out of range")
    }
    return {record, errors, valid => length(errors) == 0}
}

// Process validation results
results = map(data, {record => validate_record(record, rules)})
valid_records = filter(results, {result => result.valid})
invalid_records = filter(results, {result => !result.valid})

print("Valid records:", length(valid_records))
print("Invalid records:", length(invalid_records))
```

### Example: Time Series Data Processing
```grapa
// Process time series data with unlimited precision
process_timestamps = {data =>
    map(data, {row =>
        timestamp = parse_time(row.timestamp)
        value = parse_float(row.value)
        return {
            timestamp: timestamp,
            value: value,
            hour: hour(timestamp),
            day: day(timestamp),
            month: month(timestamp)
        }
    })
}

// Aggregate by time periods
aggregate_by_hour = {processed_data =>
    grouped = group_by(processed_data, {record => record.hour})
    map(grouped, {hour, records =>
        return {
            hour: hour,
            count: length(records),
            avg_value: reduce(records, 0, {sum, r => sum + r.value}) / length(records),
            min_value: min(map(records, {r => r.value})),
            max_value: max(map(records, {r => r.value}))
        }
    })
}
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
// Define a simple arithmetic expression grammar
expression_grammar = {
    "expression": "term (('+' | '-') term)*",
    "term": "factor (('*' | '/') factor)*", 
    "factor": "number | '(' expression ')'",
    "number": "[0-9]+"
}

// Parse and evaluate expressions
parse_expression = {expr =>
    ast = parse(expr, expression_grammar)
    return evaluate_ast(ast)
}

// Test the parser
test_expressions = ["2+3*4", "(1+2)*3", "10/2+5"]
results = map(test_expressions, {expr => parse_expression(expr)})
```

### Example: Custom Language Extension
```grapa
// Extend Grapa with a custom "repeat" construct
extend_grammar = {
    "statement": "original_statement | repeat_statement",
    "repeat_statement": "'repeat' number 'times' '{' statement* '}'"
}

// Implement the repeat functionality
execute_repeat = {ast =>
    if (ast.type == "repeat_statement") {
        count = ast.count
        body = ast.body
        for (i = 0; i < count; i++) {
            execute_statements(body)
        }
    }
}
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
// Generate large prime numbers
generate_prime = {bits =>
    while (true) {
        candidate = random_int(2^(bits-1), 2^bits)
        if (is_prime(candidate)) {
            return candidate
        }
    }
}

// RSA key generation
generate_rsa_keys = {bits =>
    p = generate_prime(bits/2)
    q = generate_prime(bits/2)
    n = p * q
    phi = (p-1) * (q-1)
    e = 65537  // Common public exponent
    d = mod_inverse(e, phi)
    
    return {
        public_key: {n: n, e: e},
        private_key: {n: n, d: d}
    }
}

// Test with 1024-bit keys
keys = generate_rsa_keys(1024)
print("Generated RSA keys with", length(keys.public_key.n.str()), "bits")
```

### Example: Financial Calculations
```grapa
// Calculate compound interest with unlimited precision
compound_interest = {principal, rate, time, periods =>
    rate_per_period = rate / periods
    total_periods = time * periods
    amount = principal * (1 + rate_per_period)^total_periods
    return amount
}

// Calculate mortgage payments
mortgage_payment = {principal, annual_rate, years =>
    monthly_rate = annual_rate / 12 / 100
    total_payments = years * 12
    payment = principal * (monthly_rate * (1 + monthly_rate)^total_payments) / 
              ((1 + monthly_rate)^total_payments - 1)
    return payment
}

// Example calculations
loan_amount = 300000
annual_rate = 3.5
loan_years = 30

monthly_payment = mortgage_payment(loan_amount, annual_rate, loan_years)
total_paid = monthly_payment * loan_years * 12
total_interest = total_paid - loan_amount

print("Monthly payment: $", monthly_payment.str())
print("Total interest: $", total_interest.str())
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
// Process large dataset in parallel
process_chunk = {chunk =>
    return map(chunk, {item =>
        // Expensive computation
        result = complex_calculation(item)
        return {input: item, result: result}
    })
}

// Split data and process in parallel
parallel_process = {data, num_threads =>
    chunk_size = length(data) / num_threads
    chunks = []
    
    for (i = 0; i < num_threads; i++) {
        start = i * chunk_size
        end = (i == num_threads-1) ? length(data) : (i+1) * chunk_size
        chunks.push(slice(data, start, end))
    }
    
    // Process chunks in parallel
    results = map(chunks, {chunk => process_chunk(chunk)})
    
    // Combine results
    return flatten(results)
}

// Example usage
large_dataset = generate_test_data(1000000)
processed_data = parallel_process(large_dataset, 8)
print("Processed", length(processed_data), "items in parallel")
```

### Example: Concurrent Network Operations
```grapa
// Fetch multiple URLs concurrently
fetch_urls = {urls =>
    responses = map(urls, {url =>
        try {
            response = $net.get(url)
            return {url: url, success: true, data: response.body}
        } catch (error) {
            return {url: url, success: false, error: error.message}
        }
    })
    return responses
}

// Process API endpoints in parallel
api_endpoints = [
    "https://api.example.com/users",
    "https://api.example.com/products", 
    "https://api.example.com/orders"
]

results = fetch_urls(api_endpoints)
successful = filter(results, {r => r.success})
failed = filter(results, {r => !r.success})

print("Successful requests:", length(successful))
print("Failed requests:", length(failed))
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
// Scrape multiple pages with rate limiting
scrape_pages = {urls, delay_ms =>
    results = map(urls, {url, index =>
        // Add delay between requests
        if (index > 0) {
            sleep(delay_ms)
        }
        
        try {
            response = $net.get(url)
            if (response.status == 200) {
                return {
                    url: url,
                    success: true,
                    content: response.body,
                    size: length(response.body)
                }
            } else {
                return {url: url, success: false, error: "HTTP " + response.status}
            }
        } catch (error) {
            return {url: url, success: false, error: error.message}
        }
    })
    return results
}

// Extract data from HTML
extract_data = {html_content =>
    // Parse HTML and extract specific elements
    doc = parse_html(html_content)
    titles = doc.select("h1, h2, h3")
    links = doc.select("a[href]")
    
    return {
        titles: map(titles, {t => t.text}),
        links: map(links, {l => l.attr("href")})
    }
}

// Example usage
target_urls = [
    "https://example.com/page1",
    "https://example.com/page2",
    "https://example.com/page3"
]

scraped_data = scrape_pages(target_urls, 1000)  // 1 second delay
extracted_data = map(scraped_data, {page => 
    if (page.success) {
        return extract_data(page.content)
    } else {
        return {error: page.error}
    }
})
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
// Recursive file processing
process_files = {directory, file_pattern =>
    files = find_files(directory, file_pattern)
    
    results = map(files, {file =>
        try {
            content = read(file)
            stats = file_stats(file)
            
            return {
                file: file,
                size: stats.size,
                modified: stats.modified,
                lines: length(content.split("\n")),
                words: length(content.split(" "))
            }
        } catch (error) {
            return {file: file, error: error.message}
        }
    })
    
    return results
}

// Database-like file queries
query_files = {directory =>
    // Find all Python files larger than 1KB
    large_python_files = select * from directory 
                         where name like "*.py" and size > 1024
    
    // Group by directory
    grouped = group_by(large_python_files, {file => dirname(file)})
    
    return map(grouped, {dir, files =>
        return {
            directory: dir,
            file_count: length(files),
            total_size: reduce(files, 0, {sum, f => sum + f.size})
        }
    })
}
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
// Visualize recursive function execution
factorial_with_trace = {n, depth =>
    indent = repeat("  ", depth)
    print(indent + "factorial(" + n + ")")
    
    if (n <= 1) {
        print(indent + "return 1")
        return 1
    } else {
        result = n * factorial_with_trace(n-1, depth+1)
        print(indent + "return " + n + " * " + (n-1) + "! = " + result)
        return result
    }
}

// Example: Calculate 4! with trace
print("Calculating 4! with execution trace:")
result = factorial_with_trace(4, 0)
print("Final result:", result)
```

### Example: Custom DSL for Data Processing
```grapa
// Define a simple data processing language
data_dsl_grammar = {
    "pipeline": "command*",
    "command": "filter_command | map_command | reduce_command",
    "filter_command": "'filter' condition",
    "map_command": "'map' expression", 
    "reduce_command": "'reduce' expression 'with' initial_value",
    "condition": "field operator value",
    "expression": "field | function_call",
    "operator": "'==' | '!=' | '>' | '<' | '>=' | '<='"
}

// Example DSL usage
dsl_script = "
filter age > 18
map salary * 1.1
reduce sum with 0
"

// Execute the DSL
execute_dsl = {script, data =>
    ast = parse(script, data_dsl_grammar)
    return execute_pipeline(ast, data)
}
```

- **See also:** [Python Education Examples](PYTHON_USE_CASES.md#7-education--prototyping)

---

For each use case, see the corresponding section for detailed examples in both Grapa and Python (GrapaPy). 