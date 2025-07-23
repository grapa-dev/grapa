
# Examples

This directory contains practical examples demonstrating Grapa's capabilities.

## Available Examples

- [basic_example.grc](basic_example.grc)
- [advanced_example.grc](advanced_example.grc)
- [performance_example.grc](performance_example.grc)
- [database_examples.grc](database_examples.grc)
- [python_integration_example.py](python_integration_example.py)

---

## Example Code Viewer

???+ example "basic_example.grc"
    ```grapa
    /* Basic Grapa Example */
    /* This file demonstrates basic Grapa syntax and operations */

    /* Basic variable assignment */
    x = 42;

    /* Basic array operations */
    arr = [1, 2, 3, 4, 5];

    /* Basic string operations */
    str = "Grapa is awesome!";

    /* Basic math operations */
    result = 10 + 5 * 2;

    /* Return the result for testing */
    result 
    ```

???+ example "advanced_example.grc"
    ```grapa
    /* Advanced Grapa Example */
    /* This file demonstrates more complex Grapa operations */

    /* String processing */
    text = "Hello World Grapa Programming";
    words = text.split(" ");

    /* Array operations */
    numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];

    /* Return results for testing */
    [words, numbers.len()] 
    ```

???+ example "performance_example.grc"
    ```grapa
    /* Performance Example */
    /* This file demonstrates performance-related operations */

    /* Simple loop using while */
    sum = 0;
    i = 0;
    while (i < 100) {
        sum = sum + i;
        i = i + 1;
    }

    /* Array creation */
    large_array = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];

    /* Return results for testing */
    [sum, large_array.len()] 
    ```

???+ example "database_examples.grc"
    ```grapa
    /* Database Examples - Comprehensive Guide */
    /* This file demonstrates all three database types: ROW, COL, and GROUP */
    // ... (file is long, so consider truncating or linking to the full file if needed) ...
    ```
    [View full database_examples.grc](database_examples.grc)

---

## Database Examples

The `database_examples.grc` file demonstrates:
- **ROW databases**: Transactional workloads, user management
- **COL databases**: Analytical workloads, metrics and analytics
- **GROUP databases**: Hierarchical structures, nested databases
- **Cross-database operations**: Data integrity validation
- **Database type selection guidelines**: When to use each type

---

## Related topics
- [API Reference](../api_reference.md)
- [Use Cases](../use_cases/)
- [Language Reference](../syntax/basic_syntax.md)
- [Database Quick Reference](../database/quick_reference.md)
- [Database Storage Types](../database/column_store.md) 