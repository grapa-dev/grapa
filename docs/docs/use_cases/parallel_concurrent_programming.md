# Parallel/Concurrent Programming

Grapa's $thread, $net, and map/reduce/filter features enable true parallelism, overcoming Python's GIL and enabling high-performance data processing.

**Note:** Grapa is fully thread safe, but if you share mutable state between threads, you are responsible for the logic. Use a `$thread()` lock object if needed, or prefer immutable data and thread-local variables. See [Threading and Locking](../sys/thread.md) for examples and best practices.

## Key Features for Parallel Programming:
- **True Parallelism**: Bypass GIL limitations with native threads
- **Network Parallelism**: Concurrent network operations
- **Functional Programming**: Map/reduce/filter for data parallelism
- **Thread Safety**: Built-in synchronization primitives

## Example: Parallel Data Processing
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
        i += 1;
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

## Example: Concurrent Network Operations
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

## Example: Thread-Safe Data Processing
```grapa
/* Thread-safe counter with locks */
thread_safe_counter = op() {
    counter = 0;
    lock = $thread();
    
    {
        "increment": op() {
            lock.lock();
            counter += 1;
            result = counter;
            lock.unlock();
            result;
        },
        "get": op() {
            lock.lock();
            result = counter;
            lock.unlock();
            result;
        }
    };
};

/* Use the thread-safe counter */
counter = thread_safe_counter();
threads = [];

/* Create multiple threads that increment the counter */
i = 0;
while (i < 10) {
    thread = $thread();
    thread.run(op() {
        j = 0;
        while (j < 100) {
            counter.increment();
            j += 1;
        };
    });
    threads += thread;
    i += 1;
};

/* Wait for all threads to complete */
threads.map(op(t) { t.join(); });
("Final counter value: " + counter.get().str()).echo();
```

- **See also:** [Python Parallelism Examples](../PYTHON_USE_CASES.md#4-parallelconcurrent-programming) 