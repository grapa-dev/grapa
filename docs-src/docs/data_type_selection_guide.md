# Data Type Selection Guide

This guide helps you choose the right data type for your specific use case in Grapa.

## Quick Decision Tree

```
Do you need data to persist after program restart?
├─ YES → Use $file() (persistent storage)
└─ NO → Continue below

Is your dataset large (> 100-500 items) and memory efficiency matters?
├─ YES → Use {}.table() (in-memory BTree)
└─ NO → Use {} (linked list)
```

## Detailed Comparison

| Feature | `{}` (Linked List) | `{}.table()` (BTree) | `$file()` (Persistent) |
|---------|-------------------|---------------------|----------------------|
| **Storage** | In-memory | In-memory | Disk-based |
| **Persistence** | No | No | Yes |
| **Performance (small data)** | Very fast | Slower | Slowest (disk I/O) |
| **Performance (large data)** | Slower | Fast | Moderate (disk I/O) |
| **Memory efficiency** | Lower | Higher | Unlimited |
| **Range queries** | No | Yes | Yes |
| **Complex queries** | No | Yes | Yes |
| **Frequent modifications** | Excellent | Good | Poor |
| **Data size limit** | Memory limit | Memory limit | Disk space limit |

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
- **100-500 items**: Use `{}` unless you need range queries or memory efficiency
- **> 500 items**: Consider `{}.table()` for better memory efficiency
- **Very large datasets**: Use `$file()` for persistence and unlimited size

### **Operation Patterns**
- **Frequent insertions/deletions**: Prefer `{}` (linked list)
- **Range queries**: Use `{}.table()` or `$file()`
- **Complex queries**: Use `{}.table()` or `$file()`
- **Simple key-value access**: Use `{}` (linked list)

### **Memory Considerations**
- **Memory constrained**: Use `{}.table()` for large datasets
- **Memory abundant**: Use `{}` for simplicity
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

/* Use {}.table() for large datasets */
large_dataset = {}.table();
for i in (10000).range() {
    large_dataset.set("item_" + i.toString(), generate_data(i));
};
```

## Summary

- **`{}` (Linked List)**: Fast, simple, best for small to medium datasets
- **`{}.table()` (BTree)**: Memory efficient, supports complex queries, best for large datasets
- **`$file()` (Persistent)**: Disk-based, unlimited size, best for persistent storage

**Key Insight**: The double-linked list implementation in `{}` is highly optimized. Don't add indexing - it would likely make things slower, not faster.
