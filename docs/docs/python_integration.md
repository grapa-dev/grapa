# Python Integration with Grapa

## Overview

Grapa provides powerful Python integration capabilities, offering a unified interface for file system operations, database management, and data processing. This integration combines Grapa's efficient storage systems with Python's rich ecosystem for data science, web development, and system administration.

## Key Benefits

### 1. **Unified File System and Database Access**
Grapa provides a single API that seamlessly navigates between file systems and databases:

```python
import grapa

f = grapa.file()
# Navigate from file system into database seamlessly
f.cd("project_data")  # Could be file system directory
f.cd("users")         # Could be database table
f.cd("profile")       # Could be nested database structure
```

**Advantages:**
- **Single API** for both file system and database operations
- **Transparent navigation** between different data sources
- **Consistent interface** regardless of underlying storage

### 2. **Advanced Database Capabilities**

#### **Column Store for Analytics**
Optimized for analytical workloads and data science:

```python
# Python data science workflows with column store
f = grapa.file()
f.mk("analytics_db", "COL")  # Column store for analytical queries
f.cd("analytics_db")

# Define schema optimized for analytics
f.mkfield("timestamp", "TIME", "FIX", 8)
f.mkfield("user_id", "INT", "FIX", 4)
f.mkfield("metric_value", "FLOAT", "FIX", 8)
f.mkfield("category", "STR", "VAR")

# Efficient column scans for aggregations
# Python can process the results for statistical analysis
```

**Benefits:**
- **Native column store** for analytical workloads
- **Better performance** than row stores for aggregations
- **Memory efficient** for large datasets

#### **Row Store for Transactional Data**
Optimized for transactional workloads and frequent updates:

```python
# Transactional data with row store
f.mk("user_sessions", "ROW")
f.cd("user_sessions")

# Fast point queries and updates
f.set("session_123", "user_456", "user_id")
f.set("session_123", "2024-01-15", "login_time")
f.set("session_123", "active", "status")
```

### 3. **File Processing Capabilities**

#### **Large File Management**
Built-in support for handling large files efficiently:

```python
# Python can orchestrate large file operations
f = grapa.file()

# Split large files for parallel processing
result = f.split(8, "large_dataset.csv", "chunks", "", "csv")
# Python can then process each chunk in parallel

# Get file metadata efficiently
file_info = f.info("large_file.txt")
if file_info["$TYPE"] == "FILE" and file_info["$BYTES"] > 1000000:
    # Handle large files appropriately
    pass
```

#### **Unified Path System**
Seamless navigation across complex data structures:

```python
# Navigate complex data structures
f.chd("/project/data")  # Set home directory
f.cd("database")        # Enter database
f.cd("users")           # Navigate to users table
f.cd("..")              # Go back up
f.cd("../logs")         # Navigate to logs directory
```

## Use Cases

### 1. **Data Science and Analytics**

#### **Scientific Computing**
```python
# Scientific computing with Grapa
f = grapa.file()
f.mk("experiment_data", "COL")
f.cd("experiment_data")

# Store experimental results efficiently
for experiment in experiments:
    f.set(f"exp_{experiment.id}", experiment.timestamp, "time")
    f.set(f"exp_{experiment.id}", experiment.results, "data")
    f.set(f"exp_{experiment.id}", experiment.parameters, "config")

# Fast column scans for statistical analysis
```

#### **Machine Learning Workflows**
```python
# ML data management with Grapa
f = grapa.file()
f.mk("ml_dataset", "COL")
f.cd("ml_dataset")

# Store features and labels
for sample in training_data:
    f.set(f"sample_{sample.id}", sample.features, "features")
    f.set(f"sample_{sample.id}", sample.label, "label")
    f.set(f"sample_{sample.id}", sample.metadata, "meta")

# Efficient feature extraction for model training
```

### 2. **Web Application Development**

#### **Backend Data Management**
```python
# Web app with Grapa storage
f = grapa.file()
f.chd("/app/data")

# User management
f.cd("users")
f.set(user_id, user_data, "profile")
f.set(user_id, session_data, "session")

# Content management
f.cd("../content")
f.set(content_id, content_data, "body")
f.set(content_id, metadata, "meta")
```

#### **API Development**
```python
# FastAPI with Grapa backend
from fastapi import FastAPI
import grapa

app = FastAPI()
f = grapa.file()

@app.get("/users/{user_id}")
async def get_user(user_id: str):
    f.cd("users")
    profile = f.get(user_id, "profile")
    return {"user_id": user_id, "profile": profile}
```

### 3. **System Administration**

#### **Log File Management**
```python
# System admin tools with Grapa
f = grapa.file()

# Log file management
f.cd("/var/log")
log_files = f.ls()
for log_file in log_files:
    info = f.info(log_file)
    if info["$BYTES"] > 1000000:  # 1MB
        # Split large log files
        f.split(4, log_file, "log_chunks", "\n", "")
```

#### **Configuration Management**
```python
# Configuration management
f.cd("/etc/configs")
configs = f.ls()
for config in configs:
    content = f.get(config)
    if validate_config(content):
        deploy_config(config, content)
```

### 4. **Data Pipeline Integration**

#### **ETL Workflows**
```python
# Extract-Transform-Load with Grapa
f = grapa.file()

# Extract: Read from various sources
f.cd("source_data")
raw_data = f.get("input.csv")

# Transform: Process in Python
processed_data = python_transform_function(raw_data)

# Load: Store in Grapa database
f.cd("../processed_data")
f.set("processed_batch_001", processed_data, "content")
```

#### **Data Validation**
```python
# Validate data before processing
files = f.ls("input_directory")
for file in files:
    info = f.info(file)
    if info["$TYPE"] == "FILE":
        content = f.get(file)
        if validate_data_format(content):
            # Process valid files
            pass
```

## Performance Advantages

### **Memory Efficiency**
- **Column store**: Only loads relevant columns for analysis
- **Fragmented storage**: Efficient for sparse data
- **Block-based I/O**: Memory-efficient file operations

### **Scalability**
- **Large file handling**: Built-in splitting and chunking
- **Database optimization**: Different storage types for different use cases
- **Cross-platform**: Consistent performance across systems

## Integration with Python Ecosystem

### **Data Science Libraries**
```python
import pandas as pd
import numpy as np
import grapa

# Process Grapa data with pandas
f = grapa.file()
f.cd("analytics_data")

# Extract data for pandas processing
data = []
for record in f.ls():
    row = {
        'id': record,
        'value': f.get(record, "value"),
        'category': f.get(record, "category")
    }
    data.append(row)

df = pd.DataFrame(data)
# Perform pandas operations
```

### **Machine Learning Frameworks**
```python
import sklearn
import grapa

# Feature storage for ML
f = grapa.file()
f.mk("ml_features", "COL")
f.cd("ml_features")

# Store features efficiently
for sample_id, features in feature_data.items():
    f.set(sample_id, features.tobytes(), "features")
    f.set(sample_id, labels[sample_id], "label")
```

### **Web Frameworks**
```python
# Django with Grapa
from django.http import JsonResponse
import grapa

def user_profile(request, user_id):
    f = grapa.file()
    f.cd("users")
    
    profile = f.get(user_id, "profile")
    return JsonResponse({"profile": profile})
```

## Best Practices

### **Database Design**
1. **Choose appropriate storage types**:
   - Use **COL** (column store) for analytical workloads
   - Use **ROW** (row store) for transactional data
   - Use **FIX** fields for small, frequently accessed data
   - Use **VAR** fields for variable-length data

2. **Optimize field types**:
   - Match data types to storage needs
   - Use appropriate field sizes
   - Consider growth patterns

### **File Management**
1. **Large file handling**:
   - Use `split()` for files larger than memory
   - Process chunks in parallel
   - Monitor file sizes with `info()`

2. **Path management**:
   - Use `chd()` to set project home directories
   - Use relative paths with `cd()` for navigation
   - Leverage the unified path system

### **Performance Optimization**
1. **Memory management**:
   - Use column store for analytical queries
   - Process data in chunks
   - Monitor memory usage

2. **I/O optimization**:
   - Use appropriate storage types
   - Batch operations when possible
   - Leverage Grapa's efficient I/O

## Conclusion

Grapa's Python integration provides a powerful combination of:
- **Unified data access** across file systems and databases
- **Performance optimization** through specialized storage types
- **Scalability** for large datasets and complex workflows
- **Integration** with Python's rich ecosystem

This makes Grapa particularly valuable for Python applications that need to handle diverse data types, large datasets, or complex data workflows. The unified path system and efficient storage options provide significant advantages over traditional file system + database combinations. 

> **See Also:**
> - [GrapaPy Quickstart](GRAPAPY_INTEGRATION.md#quickstart-for-python-users)
> - [Python-to-Grapa Migration Guide](PYTHON_TO_GRAPA_MIGRATION.md)
> - [Python Use Cases](PYTHON_USE_CASES.md) 