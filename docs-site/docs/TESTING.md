---
tags:
  - user
  - highlevel
  - testing
---

# Testing Grapa Scripts

> **For Users:** Learn how to test your Grapa scripts effectively using Grapa's built-in capabilities.

> **See Also:**
> - [Getting Started](GETTING_STARTED.md)
> - [Examples](EXAMPLES.md)
> - [API Reference](API_REFERENCE.md)

## Table of Contents
- [Basic Testing](#basic-testing)
- [Error Handling Tests](#error-handling-tests)
- [Data Structure Testing](#data-structure-testing)
- [File I/O Testing](#file-io-testing)
- [Parallel Processing Tests](#parallel-processing-tests)
- [Pattern Matching Tests](#pattern-matching-tests)
- [Integration Testing](#integration-testing)
- [Best Practices](#best-practices)

## Syntax Reminders
- Every statement and every block (including after closing braces) must end with a semicolon (`;`).
- Use block comments (`/* ... */`), not line comments (`// ...`).
- To append to arrays, use the `+=` operator (not `.push()` or `.append()`).
- Use `[]` for list/array access, not `.get()` (which is for objects/tables).
- Check for `$ERR` when accessing keys or attributes that may not exist.
- See [Syntax Quick Reference](syntax/basic_syntax.md) for more.

---

## Basic Testing

### Simple Assertion Testing
```grapa
/* Test basic function */
test_function = op(x) { x * 2; };
result = test_function(5);
if (result != 10) {
    ("Test failed: expected 10, got " + result.str()).echo();
} else {
    "Test passed: function correctly doubles input".echo();
};
```

### Testing with Multiple Cases
```grapa
/* Test function with multiple inputs */
test_cases = [0, 1, 5, 10, -3];
expected_results = [0, 2, 10, 20, -6];
i = 0;
while (i < test_cases.len()) {
    result = test_function(test_cases[i]);
    expected = expected_results[i];
    if (result != expected) {
        ("Test failed for input " + test_cases[i].str() + ": expected " + expected.str() + ", got " + result.str()).echo();
    } else {
        ("Test passed for input " + test_cases[i].str()).echo();
    };
    i += 1;
};
```

### Testing String Operations
```grapa
/* Test string operations */
test_string = "  Hello, World!  ";
trimmed = test_string.trim();
upper = trimmed.upper();

if (trimmed != "Hello, World!") {
    "String trim test failed".echo();
} else {
    "String trim test passed".echo();
};

if (upper != "HELLO, WORLD!") {
    "String upper test failed".echo();
} else {
    "String upper test passed".echo();
};
```

## Error Handling Tests

### Testing Error Conditions
```grapa
/* Test division by zero handling */
test_division = op(a, b) {
    if (b == 0) {
        $ERR;
    } else {
        a / b;
    };
};

/* Test error case */
result = test_division(10, 0);
if (result.type() == $ERR) {
    "Division by zero correctly caught".echo();
} else {
    "Division by zero test failed".echo();
};

/* Test success case */
result = test_division(10, 2);
if (result.type() != $ERR && result == 5) {
    "Division test passed".echo();
} else {
    "Division test failed".echo();
};
```

### Testing Object Property Access
```grapa
/* Test safe object property access */
test_obj = {"name": "Alice", "age": 30};

/* Test existing property */
name = test_obj.get("name");
if (name != "Alice") {
    "Object property test failed".echo();
} else {
    "Object property test passed".echo();
};

/* Test missing property */
missing = test_obj.get("nonexistent");
if (missing.type() == $ERR) {
    "Missing property correctly returns $ERR".echo();
} else {
    "Missing property test failed".echo();
};
```

## Data Structure Testing

### Testing Array Operations
```grapa
/* Test array operations */
test_array = [1, 2, 3, 4, 5];
doubled = test_array.map(op(x) { x * 2; });
expected = [2, 4, 6, 8, 10];

/* Compare arrays */
if (doubled.len() != expected.len()) {
    "Array length test failed".echo();
} else {
    "Array length test passed".echo();
};

/* Compare each element */
i = 0;
while (i < doubled.len()) {
    if (doubled[i] != expected[i]) {
        ("Array element test failed at index " + i.str() + ": expected " + expected[i].str() + ", got " + doubled[i].str()).echo();
    } else {
        ("Array element test passed at index " + i.str()).echo();
    };
    i = i + 1;
};
```

### Testing Array Filtering
```grapa
/* Test array filtering */
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
evens = numbers.filter(op(x) { x % 2 == 0; });
expected_evens = [2, 4, 6, 8, 10];

if (evens.len() != expected_evens.len()) {
    "Filter length test failed".echo();
} else {
    "Filter length test passed".echo();
};

/* Sort for comparison (order may vary with parallel processing) */
evens_sorted = evens.sort();
expected_sorted = expected_evens.sort();

if (evens_sorted.str() == expected_sorted.str()) {
    "Filter test passed".echo();
} else {
    "Filter test failed".echo();
};
```

### Testing Array Reduction
```grapa
/* Test array reduction */
numbers = [1, 2, 3, 4, 5];
sum = numbers.reduce(op(acc, x) { acc + x; }, 0);
expected_sum = 15;

if (sum == expected_sum) {
    "Reduce test passed".echo();
} else {
    ("Reduce test failed: expected " + expected_sum.str() + ", got " + sum.str()).echo();
};
```

## File I/O Testing

### Testing File Operations
```grapa
/* Test file operations */
test_file = "test_data.txt";
test_content = "Hello, Grapa!";

/* Write test data */
file_set(test_file, test_content);

/* Read and verify */
read_content = file_get(test_file);
if (read_content != test_content) {
    "File content test failed".echo();
} else {
    "File content test passed".echo();
};

/* Test file listing */
files = file_ls(".");
test_file_found = false;
i = 0;
while (i < files.len()) {
    if (files[i].name == test_file) {
        test_file_found = true;
    };
    i = i + 1;
};

if (test_file_found) {
    "File listing test passed".echo();
} else {
    "File listing test failed".echo();
};

/* Cleanup */
file_set(test_file, "");  /* Clear file */
```

### Testing JSON Processing
```grapa
/* Test JSON parsing and processing */
json_text = '{"name": "Alice", "age": 30, "active": true}';
parsed = json_text.json();

/* Test object access */
name = parsed.get("name");
age = parsed.get("age");
active = parsed.get("active");

if (name == "Alice" && age == 30 && active == true) {
    "JSON parsing test passed".echo();
} else {
    "JSON parsing test failed".echo();
};

/* Test missing key */
missing = parsed.get("nonexistent");
if (missing.type() == $ERR) {
    "JSON missing key test passed".echo();
} else {
    "JSON missing key test failed".echo();
};
```

## Parallel Processing Tests

### Testing Parallel Operations
```grapa
/* Test parallel operations */
test_data = (1000).range(0, 1);  /* Generate 0-999 */
expected_sum = test_data.reduce(op(acc, x) { acc + x; }, 0);

/* Test parallel sum with limited threads */
/* 4 threads */
parallel_sum = test_data.reduce(op(acc, x) { acc + x; }, 0, 4);

if (parallel_sum == expected_sum) {
    "Parallel sum test passed".echo();
} else {
    ("Parallel sum test failed: expected " + expected_sum.str() + ", got " + parallel_sum.str()).echo();
};

/* Test parallel mapping */
/* 4 threads */
doubled = test_data.map(op(x) { x * 2; }, 4);
/* Sequential for comparison */
expected_doubled = test_data.map(op(x) { x * 2; });

/* Sort for comparison (parallel order may vary) */
doubled_sorted = doubled.sort();
expected_sorted = expected_doubled.sort();

if (doubled_sorted.str() == expected_sorted.str()) {
    "Parallel map test passed".echo();
} else {
    "Parallel map test failed".echo();
};
```

### Testing Thread Safety
```grapa
/* Test thread safety with shared data */
shared_counter = 0;
lock_obj = $thread();

/* Create multiple threads that increment counter */
increment_worker = op() {
    lock_obj.lock();
    shared_counter = shared_counter + 1;
    lock_obj.unlock();
};

/* Run multiple increments */
i = 0;
while (i < 100) {
    increment_worker();
    i = i + 1;
};

if (shared_counter == 100) {
    "Thread safety test passed".echo();
} else {
    ("Thread safety test failed: expected 100, got " + shared_counter.str()).echo();
};
```

## Pattern Matching Tests

### Testing Grep Patterns
```grapa
/* Test basic pattern matching */
test_text = "Hello world\nGoodbye world\nHello again";
matches = test_text.grep("Hello", "o");

expected_matches = ["Hello", "Hello"];
if (matches.len() != expected_matches.len()) {
    "Grep match count test failed".echo();
} else {
    "Grep match count test passed".echo();
};

/* Test case-insensitive matching */
case_matches = test_text.grep("hello", "oi");
if (case_matches.len() == 2) {
    "Case-insensitive grep test passed".echo();
} else {
    "Case-insensitive grep test failed".echo();
};
```

### Testing Complex Patterns
```grapa
/* Test complex regex patterns */
test_text = "abc123\ndef456\nghi789";
number_matches = test_text.grep("[0-9]+", "o");

if (number_matches.len() == 3) {
    "Number pattern test passed".echo();
} else {
    "Number pattern test failed".echo();
};

/* Test word boundaries */
word_text = "hello world hello123 hello_test";
word_matches = word_text.grep("hello", "wo");

if (word_matches.len() == 2) {
    "Word boundary test passed".echo();
} else {
    "Word boundary test failed".echo();
};
```

### Testing Unicode Patterns
```grapa
/* Test Unicode pattern matching */
unicode_text = "café résumé naïve";
unicode_matches = unicode_text.grep("é", "o");

if (unicode_matches.len() == 3) {
    "Unicode pattern test passed".echo();
} else {
    "Unicode pattern test failed".echo();
};

/* Test grapheme clusters */
emoji_text = "Hello 👨‍👩‍👧‍👦 World";
grapheme_matches = emoji_text.grep("\\X", "o");

if (grapheme_matches.len() > 0) {
    "Grapheme cluster test passed".echo();
} else {
    "Grapheme cluster test failed".echo();
};
```

## Integration Testing

### Testing Complete Workflows
```grapa
/* Test complete ETL workflow */
test_etl_workflow = op() {
    /* Setup test data */
    test_data = [{"name": "Alice", "age": 30}, {"name": "Bob", "age": 25}];

    /* Transform data */
    processed = test_data.map(op(record) {
        {"name": record.get("name").upper(), "age": record.get("age")};
    });

    /* Verify results */
    if (processed.len() != 2) {
        "ETL workflow test failed: expected 2 records".echo();
        return false;
    };

    if (processed[0].get("name") != "ALICE") {
        "ETL workflow test failed: name not uppercased".echo();
        return false;
    };

    if (processed[1].get("name") != "BOB") {
        "ETL workflow test failed: second name not uppercased".echo();
        return false;
    };

    "ETL workflow test passed".echo();
    return true;
};

test_etl_workflow();
```

### Testing Data Validation Pipeline
```grapa
/* Test data validation pipeline */
test_validation_pipeline = op() {
    /* Define validation rules */
    rules = {
        "email": {"pattern": "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"},
        "age": {"min": 0, "max": 150},
        "salary": {"min": 0, "max": 1000000}
    };

    /* Test data */
    test_records = [
        {"email": "alice@example.com", "age": 30, "salary": 50000},
        {"email": "invalid-email", "age": -5, "salary": 2000000},
        {"email": "bob@test.org", "age": 25, "salary": 75000}
    ];

    /* Validate function */
    validate_record = op(record, rules) {
        errors = [];

        /* Check email pattern */
        if (!record.get("email").grep(rules.get("email").get("pattern"), "o").len() > 0) {
            errors += "Invalid email format";
        };

        /* Check age range */
        age = record.get("age");
        if (age < rules.get("age").get("min") || age > rules.get("age").get("max")) {
            errors += "Age out of range";
        };

        /* Check salary range */
        salary = record.get("salary");
        if (salary < rules.get("salary").get("min") || salary > rules.get("salary").get("max")) {
            errors += "Salary out of range";
        };

        {"record": record, "errors": errors, "valid": errors.len() == 0};
    };

    /* Process validation results */
    results = test_records.map(op(record) { validate_record(record, rules); });
    valid_records = results.filter(op(result) { result.get("valid"); });
    invalid_records = results.filter(op(result) { !result.get("valid"); });

    /* Verify expected results */
    if (valid_records.len() == 2 && invalid_records.len() == 1) {
        "Validation pipeline test passed".echo();
        return true;
    } else {
        "Validation pipeline test failed".echo();
        return false;
    };
};

test_validation_pipeline();
```

## Best Practices

### 1. Use Descriptive Test Names
```grapa
/* Good: Descriptive test name */
test_array_doubling_function = op() {
    /* Test that array doubling function works correctly */
    test_data = [1, 2, 3];
    result = test_data.map(op(x) { x * 2; });
    expected = [2, 4, 6];

    if (result.str() == expected.str()) {
        "Array doubling test passed".echo();
    } else {
        "Array doubling test failed".echo();
    };
};
```

### 2. Test Edge Cases
```grapa
/* Test edge cases */
test_edge_cases = op() {
    /* Test empty array */
    empty_result = [].map(op(x) { x * 2; });
    if (empty_result.len() == 0) {
        "Empty array test passed".echo();
    } else {
        "Empty array test failed".echo();
    };

    /* Test single element */
    single_result = [5].map(op(x) { x * 2; });
    if (single_result[0] == 10) {
        "Single element test passed".echo();
    } else {
        "Single element test failed".echo();
    };

    /* Test negative numbers */
    negative_result = [-1, -2, -3].map(op(x) { x * 2; });
    if (negative_result.str() == "[-2,-4,-6]") {
        "Negative numbers test passed".echo();
    } else {
        "Negative numbers test failed".echo();
    };
};
```

### 3. Clean Up Test Data
```grapa
/* Always clean up test files */
test_with_cleanup = op() {
    test_file = "temp_test.txt";

    /* Run test */
    file_set(test_file, "test data");
    content = file_get(test_file);

    if (content == "test data") {
        "File test passed".echo();
    } else {
        "File test failed".echo();
    };

    /* Clean up */
    file_set(test_file, "");
    "Test cleanup completed".echo();
};
```

### 4. Use Consistent Assertion Patterns
```grapa
/* Create reusable assertion functions */
assert_equal = op(actual, expected, message) {
    if (actual == expected) {
        ("✓ " + message + " passed").echo();
        return true;
    } else {
        ("✗ " + message + " failed: expected " + expected.str() + ", got " + actual.str()).echo();
        return false;
    };
};

assert_array_equal = op(actual, expected, message) {
    if (actual.len() != expected.len()) {
        ("✗ " + message + " failed: length mismatch").echo();
        return false;
    };

    i = 0;
    while (i < actual.len()) {
        if (actual[i] != expected[i]) {
            ("✗ " + message + " failed at index " + i.str()).echo();
            return false;
        };
        i = i + 1;
    };

    ("✓ " + message + " passed").echo();
    return true;
};

/* Use assertions in tests */
test_with_assertions = op() {
    result = 5 * 2;
    assert_equal(result, 10, "Multiplication test");

    array_result = [1, 2, 3].map(op(x) { x * 2; });
    assert_array_equal(array_result, [2, 4, 6], "Array mapping test");
};
```

### 5. Test Both Success and Failure Paths
```grapa
/* Test both success and failure scenarios */
test_comprehensive = op() {
    /* Test success case */
    success_result = test_function(5);
    if (success_result == 10) {
        "Success case test passed".echo();
    } else {
        "Success case test failed".echo();
    };

    /* Test failure case */
    error_result = test_function(null);
    if (error_result.type() == $ERR) {
        "Error case test passed".echo();
    } else {
        "Error case test failed".echo();
    };
};
```

### 6. Use Parallel Processing Carefully
```grapa
/* Test parallel operations with thread limits */
test_parallel_safely = op() {
    large_data = (10000).range(0, 1);

    /* Use limited threads for large datasets */
    /* Limit to 4 threads */
result = large_data.map(op(x) { x * 2; }, 4);

    if (result.len() == 10000) {
        "Parallel processing test passed".echo();
    } else {
        "Parallel processing test failed".echo();
    };
};
```

---

## Next Steps
- Learn about [Testing](TESTING.md) your Grapa code
- Check out the [Grep functionality](grep.md) for advanced pattern matching
- Review the [Syntax Quick Reference](syntax/basic_syntax.md) for more syntax rules and tips

## Thread Safety and Parallelism
Grapa is fully thread safe by design. All variable and data structure updates are internally synchronized at the C++ level, so you will never encounter crashes or corruption from concurrent access. However, if your program logic allows multiple threads to read and write the same variable or data structure, you may see *logical* race conditions (unexpected values, overwrites, etc.). This is a design consideration, not a stability issue. Minimize shared mutable state between threads unless intentional.

**Only `$thread()` objects provide explicit locking and unlocking via `lock()`, `unlock()`, and `trylock()`.** To protect access to a shared resource, create a `$thread()` lock object and use it to guard access. Calling `.lock()` or `.unlock()` on a regular variable (like an array or scalar) will return an error.

**Canonical Example:**
```grapa
lock_obj = $thread();
lock_obj.lock();
/* ... perform thread-safe operations on shared data ... */
lock_obj.unlock();
```

See [Threading and Locking](sys/thread.md) and [Function Operators: static and const](operators/function.md) for details and best practices. 