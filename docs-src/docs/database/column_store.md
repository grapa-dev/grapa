# Database Storage Types

## Overview

Grapa supports three distinct database storage types, each optimized for different workloads and use cases:

- **COL (Column Store)**: Optimized for analytical workloads and large datasets
- **ROW (Row Store)**: Optimized for transactional workloads and frequent updates
- **GROUP**: Optimized for grouped data operations and hierarchical structures

## Storage Type Comparison

| Aspect | Column Store (COL) | Row Store (ROW) | Group Store (GROUP) |
|--------|-------------------|-----------------|-------------------|
| **Storage Pattern** | Column-oriented | Row-oriented | Group-oriented |
| **Best For** | Analytical queries | Transactional workloads | Hierarchical data |
| **Sparse Data** | Very efficient | Less efficient | Moderate efficiency |
| **Point Queries** | Slower | Faster | Moderate |
| **Column Scans** | Very fast | Slower | Moderate |
| **Updates** | Block-level | Record-level | Group-level |
| **Compression** | Excellent | Good | Moderate |

## Column Store (COL) - Innovative Implementation

### Architecture

Column store databases in Grapa are optimized for analytical workloads and large datasets. Unlike row store databases that store data record-by-record, column stores organize data by columns, making them ideal for queries that access specific fields across many records.

### Storage Model
- **Fragmented Data Storage**: Uses FREC_DATA for efficient handling of sparse data
- **Column-Oriented**: Each field is stored as a separate column
- **Dynamic Growth**: Columns can grow incrementally as new records are added
- **Sparse Data Support**: Only stores data where it actually exists

### Key Components
- **CTABLE_TREE**: The main tree structure for column store databases
- **FREC_DATA**: Fragmented data storage for fixed fields
- **SDATA_TREE**: Tree structure for variable fields
- **Growth Parameters**: Configurable growth sizes for efficient storage management

### Use Cases

#### Ideal For
- **Analytical Queries**: Aggregations, sums, averages across columns
- **Data Warehousing**: Large datasets with many columns
- **Time-Series Data**: Historical data analysis
- **Sparse Data**: Tables with many optional fields
- **Column Scans**: Queries that access specific fields across records

#### Not Ideal For
- **Frequent Record Updates**: Row store is better for transactional workloads
- **Point Queries**: Individual record lookups
- **Small Datasets**: Overhead may not be justified

### Performance Optimizations

#### Storage Efficiency
- **Sparse Data Handling**: Only allocates space for actual data
- **Compression**: Similar data types can be compressed efficiently
- **Block-Level Access**: Loads only relevant data blocks into memory

#### Query Performance
- **Column Scans**: Fast access to all values in a column
- **Aggregations**: Efficient computation of sums, averages, etc.
- **Cache Locality**: Better memory cache utilization for column-oriented access

### Field Types and Storage

#### Fixed Fields (STORE_FIX)
- Use FREC_DATA for fragmented storage
- Automatically set growth size to field size if not specified
- Efficient for small, frequently accessed fields

#### Variable Fields (STORE_VAR)
- Use SDATA_TREE for tree-based storage
- Good for medium-sized variable data
- Supports efficient updates and deletions

#### Partitioned Fields (STORE_PAR)
- Use FREC_DATA for large data requiring partial updates
- Ideal for very large data that needs incremental updates
- Used for COL store $TABLE types

### Implementation Details

#### Fragmented Data Storage (FREC_DATA)
Column store fixed fields use fragmented data storage for several key reasons:

1. **Sparse Data Efficiency**: Only creates data blocks when needed, avoiding wasted space for NULL values
2. **Dynamic Growth**: Columns can grow incrementally without reallocating entire data structures
3. **Update Efficiency**: Individual blocks can be updated without affecting the entire column
4. **Memory Efficiency**: Only relevant blocks need to be loaded into memory for queries

#### Growth Parameter Management
The system automatically sets growth parameters for optimal performance:
- **Fixed Fields**: `storeGrow` defaults to `storeSize` if not specified
- **Variable Fields**: Uses configurable growth sizes for tree-based storage
- **Dynamic Adjustment**: Growth parameters can be tuned based on usage patterns

## Row Store (ROW)

### Overview
Row store databases store data record-by-record, making them ideal for transactional workloads where you frequently access complete records.

### Use Cases
- **Transactional Applications**: Banking, e-commerce, user management
- **Frequent Updates**: Applications with many INSERT/UPDATE operations
- **Point Queries**: Individual record lookups
- **Small to Medium Datasets**: Where analytical performance isn't critical

### Performance Characteristics
- **Fast Point Queries**: Quick access to individual records
- **Efficient Updates**: Record-level updates without affecting other data
- **Good for OLTP**: Online transaction processing workloads

## Group Store (GROUP)

### Overview
Group store databases are optimized for hierarchical data structures and grouped operations.

### Use Cases
- **Hierarchical Data**: Organizational charts, file systems, nested structures
- **Grouped Operations**: Data that naturally forms groups or categories
- **Tree-like Structures**: Data with parent-child relationships

### Performance Characteristics
- **Efficient Group Operations**: Fast access to grouped data
- **Hierarchical Queries**: Good performance for tree-like data structures
- **Moderate Flexibility**: Balanced between row and column stores

## Best Practices

### Choosing the Right Storage Type

#### Use Column Store (COL) When:
- You have analytical workloads with aggregations
- Your data is sparse (many NULL values)
- You perform column scans frequently
- You have large datasets (>1M records)
- You need excellent compression

#### Use Row Store (ROW) When:
- You have transactional workloads
- You frequently update individual records
- You perform many point queries
- You have small to medium datasets
- You need fast individual record access

#### Use Group Store (GROUP) When:
- Your data has natural hierarchical structure
- You perform grouped operations frequently
- You have tree-like data relationships
- You need balanced performance characteristics

### Schema Design
- Choose appropriate field types and storage options
- Consider data sparsity when designing schemas
- Use fixed fields for small, frequently accessed data

### Query Optimization
- Design queries to match your storage type's strengths
- Use aggregations and column scans for column stores
- Use point queries for row stores
- Use grouped operations for group stores

## Monitoring and Maintenance

### Performance Monitoring
- Track access patterns for each storage type
- Monitor storage efficiency
- Analyze query performance

### Storage Optimization
- Regular compression analysis for column stores
- Growth parameter tuning
- Data distribution optimization

### Maintenance Tasks
- Block-level defragmentation for column stores
- Index maintenance
- Statistics updates

## Future Enhancements

### Planned Features
- Advanced compression algorithms for all storage types
- Cross-storage-type query optimization
- Materialized views
- Query result caching

### Performance Improvements
- Vectorized query execution
- SIMD optimizations
- Parallel query processing
- Adaptive query optimization 