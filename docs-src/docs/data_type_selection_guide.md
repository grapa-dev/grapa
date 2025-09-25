# Data Type Selection Guide

This guide helps you choose the right data type for your specific use case in Grapa.:

## Quick Decision Tree

```
Do you need data to persist after program restart?
├─ YES → Use $file() (persistent storage)
└─ NO → Continue below

Do you need mathematical operations, statistics, or linear algebra?
├─ YES → Use $VECTOR (mathematical operations)
└─ NO → Continue below

Is your dataset large (> 100-500 items) and memory efficiency matters?
├─ YES → Use {}.table() (in-memory BTree)
└─ NO → Use {} (linked list)
```

## Detailed Comparison

| Feature | `{}` (Linked List) | `{}.table()` (BTree) | `$VECTOR` (Mathematical) | `$file()` (Persistent) |
|---------|-------------------|---------------------|-------------------------|----------------------|
| **Storage** | In-memory | In-memory | In-memory | Disk-based |
| **Persistence** | No | No | No | Yes |
| **Performance (small data)** | Very fast | Slower | Very fast | Slowest (disk I/O) |
| **Performance (large data)** | Slower | Fast | Fast | Moderate (disk I/O) |
| **Memory efficiency** | Lower | Higher | High | Unlimited |
| **Mathematical operations** | No | No | Excellent | No |
| **Statistical functions** | No | No | Excellent | No |
| **Linear algebra** | No | No | Excellent | No |
| **Range queries** | No | Yes | No | Yes |
| **Complex queries** | No | Yes | No | Yes |
| **Frequent modifications** | Excellent | Good | Good | Poor |
| **Data size limit** | Memory limit | Memory limit | Memory limit | Disk space limit |

## Use Case Examples

### **Configuration Data**
```grapa
/* Use {} for configuration */
config = {
    "debug_mode": true,
    "max_connections": 100,
    "timeout": 30
};
```
**Why**: Small dataset, frequent access, no persistence needed.

### **Mathematical Data Processing**
```grapa
/* Use $VECTOR for mathematical operations */
data = [1, 2, 3, 4, 5].vector();

/* Statistical analysis */
mean = data.mean();        /* 3 */
std_dev = data.std();      /* 1.58 */
sum = data.sum();          /* 15 */

/* Element-wise operations */
doubled = data * 2;        /* #[2, 4, 6, 8, 10]# */
squared = data ** 2;       /* #[1, 4, 9, 16, 25]# */

/* Matrix operations */
matrix = [[1, 2], [3, 4]].vector();
determinant = matrix.det(); /* -2 */
inverse = matrix.inv();     /* Matrix inverse */
```
**Why**: Need mathematical operations, statistics, or linear algebra.

### **User Session Data**
```grapa
/* Use {} for session data */
session = {
    "user_id": 12345,
    "login_time": $TIME().utc(),
    "permissions": ["read", "write"]
};
```
**Why**: Small dataset, frequent modifications, temporary data.

### **Large Dataset with Range Queries**
```grapa
/* Use {}.table() for large datasets */
data = {}.table();
data.mkfield("timestamp", "TIME", "FIX", 8);
data.mkfield("value", "FLOAT", "FIX", 8);

/* Efficient range queries */
for record in data.range("timestamp", start_time, end_time) {
    /* Process records in time range */
};
```
**Why**: Large dataset, need range queries, memory efficiency important.

### **Persistent Log Data**
```grapa
/* Use $file() for persistent storage */
f = $file();
f.mk("logs", "ROW");
f.cd("logs");
f.mkfield("timestamp", "TIME", "FIX", 8);
f.mkfield("level", "STR", "VAR");
f.mkfield("message", "STR", "VAR");

/* Data persists after program restart */
f.setfield("log_001", "timestamp", $TIME().utc());
f.setfield("log_001", "level", "INFO");
f.setfield("log_001", "message", "Application started");
```
**Why**: Need persistence, large dataset, long-term storage.

### **Cache with Expiration**
```grapa
/* Use {} for cache */
cache = {};

/* Simple cache operations */
cache.set("user_123", {
    "name": "Alice",
    "email": "alice@example.com",
    "cached_at": $TIME().utc()
});

/* Check if cached data is still valid */
if (cache.get("user_123").cached_at.ms() < 300000) { /* 5 minutes */
    /* Use cached data */
} else {
    /* Refresh cache */
};
```
**Why**: Small to medium dataset, frequent access, temporary data.

## Performance Guidelines

### **Dataset Size Thresholds**
- **< 100 items**: Always use `{}` (linked list)
- **100-500 items**: Use `{}` unless you need range queries, memory efficiency, or mathematical operations
- **> 500 items**: Consider `{}.table()` for better memory efficiency, or `$VECTOR` for mathematical operations
- **Very large datasets**: Use `$file()` for persistence and unlimited size

### **Operation Patterns**
- **Frequent insertions/deletions**: Prefer `{}` (linked list)
- **Mathematical operations**: Use `$VECTOR` (statistics, linear algebra, element-wise operations)
- **Range queries**: Use `{}.table()` or `$file()`
- **Complex queries**: Use `{}.table()` or `$file()`
- **Simple key-value access**: Use `{}` (linked list)

### **Memory Considerations**
- **Memory constrained**: Use `{}.table()` for large datasets
- **Memory abundant**: Use `{}` for simplicity
- **Mathematical processing**: Use `$VECTOR` for optimized mathematical operations
- **Unlimited data**: Use `$file()` for persistence

## Common Mistakes

### **❌ Don't Do This**
```grapa
/* Don't use {}.table() for small datasets */
small_config = {}.table();
small_config.set("debug", true);
small_config.set("timeout", 30);
```
**Why**: Overhead of BTree structure makes it slower than `{}` for small datasets.

### **❌ Don't Do This**
```grapa
/* Don't use {} for mathematical operations */
data = [1, 2, 3, 4, 5];
mean = data.mean();  /* This won't work - {} doesn't have .mean() */
doubled = data * 2;  /* This won't work - {} doesn't support element-wise operations */
```
**Why**: `{}` doesn't support mathematical operations, statistics, or linear algebra.

### **❌ Don't Do This**
```grapa
/* Don't use {} for very large datasets */
large_dataset = {};
for i in (10000).range() {
    large_dataset.set("item_" + i.toString(), generate_data(i));
};
```
**Why**: Memory inefficiency and slower access for large datasets.

### **✅ Do This Instead**
```grapa
/* Use {} for small datasets */
small_config = {
    "debug": true,
    "timeout": 30
};

/* Use $VECTOR for mathematical operations */
data = [1, 2, 3, 4, 5].vector();
mean = data.mean();        /* 3 */
doubled = data * 2;        /* #[2, 4, 6, 8, 10]# */

/* Use {}.table() for large datasets */
large_dataset = {}.table();
for i in (10000).range() {
    large_dataset.set("item_" + i.toString(), generate_data(i));
};
```

## Summary

- **`{}` (Linked List)**: Fast, simple, best for small to medium datasets
- **`{}.table()` (BTree)**: Memory efficient, supports complex queries, best for large datasets
- **`$VECTOR` (Mathematical)**: Optimized for mathematical operations, statistics, and linear algebra
- **`$file()` (Persistent)**: Disk-based, unlimited size, best for persistent storage

**Key Insights**: 
- The double-linked list implementation in `{}` is highly optimized. Don't add indexing - it would likely make things slower, not faster.
- Use `$VECTOR` when you need mathematical operations, statistical analysis, or linear algebra - it's specifically optimized for these use cases.
