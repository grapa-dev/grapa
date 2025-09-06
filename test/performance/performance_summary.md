# Grapa Performance Summary

## Key Performance Insights

### **`{}` (Linked List) and `[]` (Array)**
- **Very fast** for most use cases due to double-linked list implementation
- **Don't add indexing** - the overhead would likely make them slower, not faster
- **Use for**: Configuration data, user data, cache data, most general-purpose collections

### **`{}.table()` (BTree) and `$file`**
- **Use when** persistence, order, range queries, or memory efficiency at scale are important
- **BTree advantages**: Efficient range queries, maintains sort order, better memory usage for large datasets
- **Persistence**: `$file` provides disk-based storage with BTree indexing

## Decision Matrix

| Use Case | Recommended Type | Reason |
|----------|------------------|---------|
| Configuration data | `{}` | Fast, simple, no persistence needed |
| User data (in-memory) | `{}` | Fast access, moderate size |
| User data (persistent) | `{}.table()` or `$file` | Persistence required |
| Cache data | `{}` | Frequent access, moderate size |
| Log data | `{}.table()` or `$file` | Large datasets, persistence needed |
| Range queries | `{}.table()` | BTree supports efficient range operations |
| Ordered data | `{}.table()` | Maintains sort order efficiently |
| Memory efficiency at scale | `{}.table()` | Better memory usage for large datasets |
| Temporary collections | `{}` or `[]` | Fast, no persistence needed |

## Performance Characteristics

### **Linked List (`{}`)**
- **Insertion**: O(1) - very fast
- **Access**: O(n) - but optimized double-linked list makes it fast in practice
- **Memory**: Higher overhead per item
- **Best for**: Small to medium datasets, frequent modifications

### **BTree (`{}.table()`)**
- **Insertion**: O(log n) - slower than linked list for small datasets
- **Access**: O(log n) - consistent performance
- **Memory**: More efficient for large datasets
- **Best for**: Large datasets, range queries, persistence needs

## Key Takeaway

**Don't overthink it**: `{}` and `[]` are fast enough for most use cases. Only use `{}.table()` or `$file` when you have specific requirements for persistence, ordering, range queries, or memory efficiency at scale.

The double-linked list implementation in Grapa is highly optimized and adding indexing would likely introduce overhead without meaningful performance benefits for typical use cases.
