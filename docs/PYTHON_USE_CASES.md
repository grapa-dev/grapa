# GrapaPy (Python) Use Cases

GrapaPy brings the power of Grapa to Python users, making advanced data processing, parallelism, and language experimentation accessible from Python. Below are key use cases, each with a quickstart, migration tips, and links to detailed examples.

---

## 1. ETL / Data Engineering
Leverage Grapa's parallelism and unlimited precision for high-performance ETL pipelines in Python.

### Quickstart
```python
import grapa

# Initialize GrapaPy
g = grapa.Grapa()

# Simple ETL pipeline
def process_csv_files(file_paths):
    # Extract: Read files in parallel
    data = g.map(file_paths, lambda file: g.read(file).split("\n"))
    
    # Transform: Clean data
    clean_data = g.map(data, lambda rows: 
        g.filter(rows, lambda row: len(row.split(",")) == 5)
    )
    
    # Load: Aggregate results
    total_records = g.reduce(clean_data, 0, lambda sum, rows: sum + len(rows))
    return total_records

# Usage
files = ["data1.csv", "data2.csv", "data3.csv"]
result = process_csv_files(files)
print(f"Processed {result} records")
```

### Migration from Pandas
```python
import grapa
import pandas as pd

# Before: Pandas approach (memory-intensive)
def pandas_etl():
    dfs = []
    for file in files:
        df = pd.read_csv(file)
        dfs.append(df)
    
    combined = pd.concat(dfs)
    result = combined.groupby('category').sum()
    return result

# After: GrapaPy approach (memory-efficient, parallel)
def grapa_etl():
    g = grapa.Grapa()
    
    # Process files in parallel without loading all into memory
    results = g.map(files, lambda file: {
        'file': file,
        'data': g.read(file),
        'processed': g.process_file(file)
    })
    
    # Aggregate results
    return g.reduce(results, {}, lambda acc, r: g.merge_results(acc, r))
```

### Advanced Example: Data Validation Pipeline
```python
import grapa
import re

def create_validation_pipeline():
    g = grapa.Grapa()
    
    # Define validation rules
    rules = {
        'email': r'^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$',
        'age': {'min': 0, 'max': 150},
        'salary': {'min': 0, 'max': 1000000}
    }
    
    def validate_record(record, rules):
        errors = []
        
        # Email validation
        if not re.match(rules['email'], record['email']):
            errors.append("Invalid email format")
        
        # Age validation
        if record['age'] < rules['age']['min'] or record['age'] > rules['age']['max']:
            errors.append("Age out of range")
        
        # Salary validation
        if record['salary'] < rules['salary']['min'] or record['salary'] > rules['salary']['max']:
            errors.append("Salary out of range")
        
        return {
            'record': record,
            'errors': errors,
            'valid': len(errors) == 0
        }
    
    # Process validation in parallel
    def validate_dataset(data, rules):
        results = g.map(data, lambda record: validate_record(record, rules))
        
        valid_records = g.filter(results, lambda r: r['valid'])
        invalid_records = g.filter(results, lambda r: not r['valid'])
        
        return {
            'valid': valid_records,
            'invalid': invalid_records,
            'valid_count': len(valid_records),
            'invalid_count': len(invalid_records)
        }
    
    return validate_dataset

# Usage
validator = create_validation_pipeline()
validation_results = validator(sample_data, rules)
print(f"Valid: {validation_results['valid_count']}, Invalid: {validation_results['invalid_count']}")
```

### Time Series Processing with Unlimited Precision
```python
import grapa
from datetime import datetime

def time_series_analysis():
    g = grapa.Grapa()
    
    def process_timestamps(data):
        return g.map(data, lambda row: {
            'timestamp': g.parse_time(row['timestamp']),
            'value': g.parse_float(row['value']),
            'hour': g.hour(g.parse_time(row['timestamp'])),
            'day': g.day(g.parse_time(row['timestamp'])),
            'month': g.month(g.parse_time(row['timestamp']))
        })
    
    def aggregate_by_hour(processed_data):
        grouped = g.group_by(processed_data, lambda record: record['hour'])
        
        return g.map(grouped, lambda hour, records: {
            'hour': hour,
            'count': len(records),
            'avg_value': g.reduce(records, 0, lambda sum, r: sum + r['value']) / len(records),
            'min_value': g.min(g.map(records, lambda r: r['value'])),
            'max_value': g.max(g.map(records, lambda r: r['value']))
        })
    
    # Example usage
    time_series_data = [
        {'timestamp': '2024-01-01 10:30:00', 'value': '123.456789'},
        {'timestamp': '2024-01-01 10:35:00', 'value': '124.123456'},
        # ... more data
    ]
    
    processed = process_timestamps(time_series_data)
    hourly_stats = aggregate_by_hour(processed)
    
    return hourly_stats
```

- **Migration Tip:** Replace Python multiprocessing or pandas for large ETL jobs with GrapaPy for better parallelism.
- **See also:** [Main Use Cases](USE_CASES.md#1-etl--data-engineering)

## 2. Compiler/BNF Learning
Experiment with executable BNF and grammar design from Python, ideal for teaching and research.

### Quickstart
```python
import grapa

def create_expression_parser():
    g = grapa.Grapa()
    
    # Define a simple arithmetic expression grammar
    expression_grammar = {
        "expression": "term (('+' | '-') term)*",
        "term": "factor (('*' | '/') factor)*", 
        "factor": "number | '(' expression ')'",
        "number": "[0-9]+"
    }
    
    def parse_expression(expr):
        ast = g.parse(expr, expression_grammar)
        return g.evaluate_ast(ast)
    
    return parse_expression

# Usage
parser = create_expression_parser()
test_expressions = ["2+3*4", "(1+2)*3", "10/2+5"]
results = [parser(expr) for expr in test_expressions]
print(f"Parse results: {results}")
```

### Custom Language Extension
```python
import grapa

def extend_grapa_with_repeat():
    g = grapa.Grapa()
    
    # Extend Grapa with a custom "repeat" construct
    extend_grammar = {
        "statement": "original_statement | repeat_statement",
        "repeat_statement": "'repeat' number 'times' '{' statement* '}'"
    }
    
    def execute_repeat(ast):
        if ast['type'] == 'repeat_statement':
            count = ast['count']
            body = ast['body']
            
            for i in range(count):
                g.execute_statements(body)
    
    # Register the new grammar and executor
    g.extend_grammar(extend_grammar)
    g.register_executor('repeat_statement', execute_repeat)
    
    return g

# Usage
extended_grapa = extend_grapa_with_repeat()

# Now you can use the repeat construct
repeat_code = """
repeat 3 times {
    print("Hello, World!")
}
"""

extended_grapa.execute(repeat_code)
```

### AST Visualization for Education
```python
import grapa
import json

def visualize_ast():
    g = grapa.Grapa()
    
    def create_ast_visualizer():
        def visualize_node(node, depth=0):
            indent = "  " * depth
            result = f"{indent}{node['type']}"
            
            if 'value' in node:
                result += f": {node['value']}"
            
            if 'children' in node:
                for child in node['children']:
                    result += "\n" + visualize_node(child, depth + 1)
            
            return result
        
        return visualize_node
    
    # Example usage
    expression = "2 + 3 * 4"
    ast = g.parse(expression)
    
    visualizer = create_ast_visualizer()
    ast_diagram = visualizer(ast)
    print("AST Visualization:")
    print(ast_diagram)
    
    return ast_diagram
```

- **Migration Tip:** Use GrapaPy instead of ANTLR or PLY for rapid prototyping.
- **See also:** [Main Use Cases](USE_CASES.md#2-compilerbnf-learning)

## 3. High-Precision Math & Scientific Computing
Perform arbitrary-precision calculations and time series analysis from Python.

### Quickstart
```python
import grapa

def unlimited_precision_math():
    g = grapa.Grapa()
    
    # Handle arbitrarily large numbers
    large_number = g.int("1234567890123456789012345678901234567890")
    precise_float = g.float("3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679")
    
    # Perform calculations with unlimited precision
    result = large_number * precise_float
    print(f"Result: {result}")
    
    return result
```

### Cryptographic Calculations
```python
import grapa

def rsa_key_generation():
    g = grapa.Grapa()
    
    def generate_prime(bits):
        while True:
            candidate = g.random_int(2**(bits-1), 2**bits)
            if g.is_prime(candidate):
                return candidate
    
    def generate_rsa_keys(bits):
        p = generate_prime(bits // 2)
        q = generate_prime(bits // 2)
        n = p * q
        phi = (p - 1) * (q - 1)
        e = 65537  # Common public exponent
        d = g.mod_inverse(e, phi)
        
        return {
            'public_key': {'n': n, 'e': e},
            'private_key': {'n': n, 'd': d}
        }
    
    # Generate 1024-bit RSA keys
    keys = generate_rsa_keys(1024)
    print(f"Generated RSA keys with {len(str(keys['public_key']['n']))} bits")
    
    return keys
```

### Financial Calculations
```python
import grapa

def financial_calculations():
    g = grapa.Grapa()
    
    def compound_interest(principal, rate, time, periods):
        rate_per_period = rate / periods
        total_periods = time * periods
        amount = principal * (1 + rate_per_period) ** total_periods
        return amount
    
    def mortgage_payment(principal, annual_rate, years):
        monthly_rate = annual_rate / 12 / 100
        total_payments = years * 12
        payment = principal * (monthly_rate * (1 + monthly_rate) ** total_payments) / \
                  ((1 + monthly_rate) ** total_payments - 1)
        return payment
    
    # Example calculations
    loan_amount = g.float("300000")
    annual_rate = g.float("3.5")
    loan_years = 30
    
    monthly_payment = mortgage_payment(loan_amount, annual_rate, loan_years)
    total_paid = monthly_payment * loan_years * 12
    total_interest = total_paid - loan_amount
    
    print(f"Monthly payment: ${monthly_payment}")
    print(f"Total interest: ${total_interest}")
    
    return {
        'monthly_payment': monthly_payment,
        'total_interest': total_interest
    }
```

- **Migration Tip:** Replace Python decimal or numpy for high-precision needs.
- **See also:** [Main Use Cases](USE_CASES.md#3-high-precision-math--scientific-computing)

## 4. Parallel/Concurrent Programming
Overcome Python's GIL with GrapaPy's true parallelism features.

### Quickstart
```python
import grapa
import time

def parallel_processing():
    g = grapa.Grapa()
    
    def expensive_calculation(item):
        # Simulate expensive computation
        time.sleep(0.1)
        return item * 2
    
    # Process data in parallel
    data = list(range(1000))
    results = g.map(data, expensive_calculation)
    
    print(f"Processed {len(results)} items in parallel")
    return results
```

### Concurrent Network Operations
```python
import grapa
import requests

def concurrent_web_requests():
    g = grapa.Grapa()
    
    def fetch_url(url):
        try:
            response = g.net.get(url)
            return {
                'url': url,
                'success': True,
                'data': response.body,
                'status': response.status
            }
        except Exception as error:
            return {
                'url': url,
                'success': False,
                'error': str(error)
            }
    
    # Fetch multiple URLs concurrently
    urls = [
        "https://api.example.com/users",
        "https://api.example.com/products",
        "https://api.example.com/orders"
    ]
    
    results = g.map(urls, fetch_url)
    
    successful = g.filter(results, lambda r: r['success'])
    failed = g.filter(results, lambda r: not r['success'])
    
    print(f"Successful requests: {len(successful)}")
    print(f"Failed requests: {len(failed)}")
    
    return results
```

### Parallel Data Processing Pipeline
```python
import grapa

def parallel_data_pipeline():
    g = grapa.Grapa()
    
    def process_chunk(chunk):
        return g.map(chunk, lambda item: {
            'input': item,
            'processed': item * 2,
            'timestamp': g.now()
        })
    
    def parallel_process(data, num_threads):
        chunk_size = len(data) // num_threads
        chunks = []
        
        for i in range(num_threads):
            start = i * chunk_size
            end = len(data) if i == num_threads - 1 else (i + 1) * chunk_size
            chunks.append(data[start:end])
        
        # Process chunks in parallel
        results = g.map(chunks, process_chunk)
        
        # Combine results
        return g.flatten(results)
    
    # Example usage
    large_dataset = list(range(100000))
    processed_data = parallel_process(large_dataset, 8)
    
    print(f"Processed {len(processed_data)} items using 8 threads")
    return processed_data
```

- **Migration Tip:** Use GrapaPy instead of Python threading/multiprocessing for CPU-bound tasks.
- **See also:** [Main Use Cases](USE_CASES.md#4-parallelconcurrent-programming)

## 5. Web/Data Scraping & Automation
Build scalable web scrapers and automation pipelines in Python using GrapaPy's $net and parallelism.

### Quickstart
```python
import grapa

def web_scraping_pipeline():
    g = grapa.Grapa()
    
    def scrape_pages(urls, delay_ms=1000):
        def scrape_single_page(url, index):
            # Add delay between requests
            if index > 0:
                g.sleep(delay_ms)
            
            try:
                response = g.net.get(url)
                if response.status == 200:
                    return {
                        'url': url,
                        'success': True,
                        'content': response.body,
                        'size': len(response.body)
                    }
                else:
                    return {'url': url, 'success': False, 'error': f"HTTP {response.status}"}
            except Exception as error:
                return {'url': url, 'success': False, 'error': str(error)}
        
        return g.map(urls, scrape_single_page)
    
    # Example usage
    target_urls = [
        "https://example.com/page1",
        "https://example.com/page2",
        "https://example.com/page3"
    ]
    
    scraped_data = scrape_pages(target_urls, 1000)  # 1 second delay
    return scraped_data
```

### HTML Data Extraction
```python
import grapa

def extract_web_data():
    g = grapa.Grapa()
    
    def extract_data(html_content):
        # Parse HTML and extract specific elements
        doc = g.parse_html(html_content)
        titles = doc.select("h1, h2, h3")
        links = doc.select("a[href]")
        
        return {
            'titles': g.map(titles, lambda t: t.text),
            'links': g.map(links, lambda l: l.attr("href"))
        }
    
    def process_scraped_data(scraped_data):
        return g.map(scraped_data, lambda page: 
            extract_data(page['content']) if page['success'] else {'error': page['error']}
        )
    
    # Example usage
    scraped_pages = scrape_pages(target_urls)
    extracted_data = process_scraped_data(scraped_pages)
    
    return extracted_data
```

### API Automation
```python
import grapa

def api_automation():
    g = grapa.Grapa()
    
    def api_request(endpoint, method="GET", data=None):
        try:
            if method == "GET":
                response = g.net.get(endpoint)
            elif method == "POST":
                response = g.net.post(endpoint, data)
            
            return {
                'endpoint': endpoint,
                'method': method,
                'success': True,
                'data': response.body,
                'status': response.status
            }
        except Exception as error:
            return {
                'endpoint': endpoint,
                'method': method,
                'success': False,
                'error': str(error)
            }
    
    def batch_api_calls(endpoints):
        return g.map(endpoints, lambda ep: api_request(ep))
    
    # Example usage
    api_endpoints = [
        "https://api.example.com/users",
        "https://api.example.com/products",
        "https://api.example.com/orders"
    ]
    
    results = batch_api_calls(api_endpoints)
    return results
```

- **Migration Tip:** Replace requests/asyncio with GrapaPy for parallel scraping.
- **See also:** [Main Use Cases](USE_CASES.md#5-webdata-scraping--automation)

## 6. Database & File System Integration
Unify file and database operations in Python with GrapaPy's APIs.

### Quickstart
```python
import grapa

def file_system_operations():
    g = grapa.Grapa()
    
    def process_files(directory, file_pattern):
        files = g.find_files(directory, file_pattern)
        
        def process_single_file(file):
            try:
                content = g.read(file)
                stats = g.file_stats(file)
                
                return {
                    'file': file,
                    'size': stats['size'],
                    'modified': stats['modified'],
                    'lines': len(content.split('\n')),
                    'words': len(content.split())
                }
            except Exception as error:
                return {'file': file, 'error': str(error)}
        
        return g.map(files, process_single_file)
    
    # Example usage
    python_files = process_files(".", "*.py")
    return python_files
```

### Database-like File Queries
```python
import grapa

def file_database_queries():
    g = grapa.Grapa()
    
    def query_files(directory):
        # Find all Python files larger than 1KB
        large_python_files = g.select("* from directory where name like '*.py' and size > 1024")
        
        # Group by directory
        grouped = g.group_by(large_python_files, lambda file: g.dirname(file))
        
        return g.map(grouped, lambda dir, files: {
            'directory': dir,
            'file_count': len(files),
            'total_size': g.reduce(files, 0, lambda sum, f: sum + f['size'])
        })
    
    # Example usage
    file_stats = query_files(".")
    return file_stats
```

### Unified File/Database Operations
```python
import grapa

def unified_data_operations():
    g = grapa.Grapa()
    
    def process_data_sources(sources):
        def process_source(source):
            if source['type'] == 'file':
                return g.read(source['path'])
            elif source['type'] == 'database':
                return g.query(source['connection'], source['query'])
            elif source['type'] == 'api':
                return g.net.get(source['url']).body
        
        return g.map(sources, process_source)
    
    # Example usage
    data_sources = [
        {'type': 'file', 'path': 'data.csv'},
        {'type': 'database', 'connection': 'db://localhost', 'query': 'SELECT * FROM users'},
        {'type': 'api', 'url': 'https://api.example.com/data'}
    ]
    
    all_data = process_data_sources(data_sources)
    return all_data
```

- **Migration Tip:** Use GrapaPy instead of os, pathlib, or SQLAlchemy for hybrid workflows.
- **See also:** [Main Use Cases](USE_CASES.md#6-database--file-system-integration)

## 7. Education & Prototyping
Prototype new languages and teach programming concepts from Python.

### Quickstart
```python
import grapa

def educational_examples():
    g = grapa.Grapa()
    
    def factorial_with_trace(n, depth=0):
        indent = "  " * depth
        print(f"{indent}factorial({n})")
        
        if n <= 1:
            print(f"{indent}return 1")
            return 1
        else:
            result = n * factorial_with_trace(n - 1, depth + 1)
            print(f"{indent}return {n} * {n-1}! = {result}")
            return result
    
    # Example: Calculate 4! with trace
    print("Calculating 4! with execution trace:")
    result = factorial_with_trace(4, 0)
    print(f"Final result: {result}")
    
    return result
```

### Custom DSL Creation
```python
import grapa

def create_data_processing_dsl():
    g = grapa.Grapa()
    
    # Define a simple data processing language
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
    
    def execute_dsl(script, data):
        ast = g.parse(script, data_dsl_grammar)
        return g.execute_pipeline(ast, data)
    
    # Example DSL usage
    dsl_script = """
    filter age > 18
    map salary * 1.1
    reduce sum with 0
    """
    
    sample_data = [
        {'name': 'Alice', 'age': 25, 'salary': 50000},
        {'name': 'Bob', 'age': 17, 'salary': 30000},
        {'name': 'Charlie', 'age': 30, 'salary': 70000}
    ]
    
    result = execute_dsl(dsl_script, sample_data)
    return result
```

### Algorithm Visualization
```python
import grapa

def algorithm_visualization():
    g = grapa.Grapa()
    
    def visualize_sort(data, algorithm_name):
        print(f"Visualizing {algorithm_name} sort:")
        print(f"Initial data: {data}")
        
        def bubble_sort_with_trace(arr):
            n = len(arr)
            for i in range(n):
                for j in range(0, n - i - 1):
                    if arr[j] > arr[j + 1]:
                        arr[j], arr[j + 1] = arr[j + 1], arr[j]
                        print(f"  Swap: {arr[j]} <-> {arr[j + 1]}, Array: {arr}")
            return arr
        
        sorted_data = bubble_sort_with_trace(data.copy())
        print(f"Final result: {sorted_data}")
        return sorted_data
    
    # Example usage
    test_data = [64, 34, 25, 12, 22, 11, 90]
    result = visualize_sort(test_data, "Bubble")
    return result
```

- **Migration Tip:** Use GrapaPy for rapid prototyping instead of custom Python scripts.
- **See also:** [Main Use Cases](USE_CASES.md#7-education--prototyping)

---

For each use case, see the corresponding section for detailed examples in both Grapa and Python (GrapaPy). 