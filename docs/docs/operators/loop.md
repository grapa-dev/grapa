# Loop and Functional Array Methods

/*
# Thread Safety
Grapa is fully thread safe in all supported environments (command line, Grapa shell, and Python/GrapaPy). All built-in operations—including map, filter, reduce, $thread, and $net—are safe to use concurrently. Users do not need to take any special precautions for thread safety in these environments.

*Note: Only if Grapa is integrated directly into a non-thread-safe C++ host (not anticipated for normal users) would additional thread safety considerations arise.*
*/

/*
# Quick Reference: map, filter, reduce

| Method  | Signature                                      | Parallel? | Purpose                        |
|---------|------------------------------------------------|-----------|--------------------------------|
| map     | arr.map(op(x) { ... }, [params...])            | Yes       | Transform each item            |
| filter  | arr.filter(op(x) { ... }, [params...])         | Yes       | Select items matching a test   |
| reduce  | arr.reduce(op(acc, x) { ... }, init, [params]) | No        | Accumulate to a single value   |

- All methods accept an operation (op) as the first argument.
- For map/filter, the op is called in parallel for each item.
- For reduce, the op is called sequentially, passing the accumulator.
- See also: [EXAMPLES.md](../EXAMPLES.md), [use_cases/index.md](../use_cases/index.md)
*/

## while
Syntax:
* while(bool) statement;

Example:
```
i = 0;
while (i < 5) {
    i += 1;
    (i.str() + ":").echo();
};
"\n".echo();
/* Output: 1:2:3:4:5: */
```

## map
Syntax:
* arr.map(op(x) { ... }, [params...]);

- Calls the operation for each item in the array, in parallel.
- Returns a new array with the results.

Example:
```
arr = [1,2,3];
doubled = arr.map(op(n) { n * 2; });
doubled.echo();
/* Output: [2,4,6] */
```

## filter
Syntax:
* arr.filter(op(x) { ... }, [params...]);

- Calls the operation for each item in the array, in parallel.
- Returns a new array containing only items for which the op returns a non-null, non-zero, non-empty value.

Minimal Example:
```
arr = [1,2,3,4];
filtered = arr.filter(op(x) { x > 2; });
filtered.echo();
/* Output: [3,4] */
```

Edge Cases:
```
arr = [1,2,3,4];
filtered_none = arr.filter(op(x) { 0; });
filtered_none.echo();
/* Output: [] */
```

## reduce
Syntax:
* arr.reduce(op(acc, x) { ... }, init, [params...]);

- Calls the operation sequentially for each item, passing the accumulator (acc) and the current item (x).
- The accumulator must be mutated inside the op (e.g., acc += x;), not just returned as a new value.
- Returns the final value of the accumulator.
- If init is omitted, the first item is used as the initial value.
- Reduce is always sequential (not parallel).

Minimal Example (Sum):
```
arr = [1,2,3,4];
sum = arr.reduce(op(acc, x) { acc += x; }, 0);
sum.echo();
/* Output: 10 */
```

Canonical Example (All Together):
```
arr = [1,2,3,4];
filtered_gt2 = arr.filter(op(x) { x > 2; });
filtered_eq2 = arr.filter(op(x) { x == 2; });
filtered_none = arr.filter(op(x) { 0; });
reduced_sum = arr.reduce(op(acc, x) { acc += x; }, 0);
("filtered_gt2: " + filtered_gt2.str() + "\n").echo();
("filtered_eq2: " + filtered_eq2.str() + "\n").echo();
("filtered_none: " + filtered_none.str() + "\n").echo();
("reduced_sum: " + reduced_sum.str() + "\n").echo();
/*
Output:
filtered_gt2: [3,4]
filtered_eq2: [2]
filtered_none: []
reduced_sum: 10
*/
```

### Important: Accumulator Mutation in reduce
- The accumulator variable (acc) must be mutated inside the op for reduce to work as expected.
- This means you must use `acc += x;` or similar, not just `acc + x;`.
- Returning a new value does NOT update the accumulator.
- Common mistake:
```
arr = [1,2,3,4];
sum = arr.reduce(op(acc, x) { acc + x; }, 0);
sum.echo();
/* Output: 0 (WRONG!) */
```
- Correct:
```
arr = [1,2,3,4];
sum = arr.reduce(op(acc, x) { acc += x; }, 0);
sum.echo();
/* Output: 10 */
```

### Troubleshooting reduce
- If your reduce result is always the initial value, check that you are mutating the accumulator.
- Use `acc += x;` or `acc = acc * x;` as needed.
- If you want to build an array, use `acc += x;` with an array accumulator.

### Best Practices for map/filter/reduce
- Use map for transformations, filter for selection, reduce for aggregation.
- For side effects, prefer reduce (sequential) over map/filter (parallel).
- Always use block comments (`/* ... */`) for documentation.
- Test your op in the REPL with small arrays before using in scripts.
- For complex reductions, consider using an object or array as the accumulator.

---

## See Also
- [EXAMPLES.md](../EXAMPLES.md)
- [use_cases/index.md](../use_cases/index.md)
- [Basic Syntax Guide](../syntax/basic_syntax.md)
- [Array Type](../type/ARRAY.md)

---

/* End of canonical map/filter/reduce documentation. */
