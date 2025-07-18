# Database Troubleshooting Guide

## Common Issues and Solutions

### Column Store Performance Issues

#### Issue: Slow Performance on Small Datasets
**Symptoms**: Column store operations are slower than expected for small datasets
**Cause**: Column store overhead is not justified for small datasets
**Solution**: 
- Use row store (`"ROW"`) for small, transactional datasets
- Reserve column store for analytical workloads with large datasets

#### Issue: Infinite Loops During Data Setting
**Symptoms**: Database operations hang or crash during field creation or data setting
**Cause**: Growth parameters not properly set, causing fragmented data handling issues
**Solution**:
- Ensure `storeGrow` parameter is specified for fixed fields
- The system automatically sets `storeGrow = storeSize` if not specified
- Check field creation parameters

#### Issue: Fragmented Data Storage Errors
**Symptoms**: Crashes or errors when working with column store fixed fields
**Cause**: FREC_DATA storage issues due to improper initialization
**Solution**:
- Verify field creation parameters are correct
- Use appropriate storage types for your data
- Check debug output for specific error messages

### Field Creation Issues

#### Issue: Fields Not Created Properly
**Symptoms**: Fields appear to be created but don't work as expected
**Cause**: Incorrect field type or storage parameters
**Solution**:
- Verify field type is supported (`BOOL`, `INT`, `FLOAT`, `STR`, `TIME`, `TABLE`, `RAW`)
- Check storage type is appropriate (`FIX`, `VAR`, `PAR`)
- Ensure size parameters are reasonable

#### Issue: Storage Type Mismatch
**Symptoms**: Data not stored or retrieved correctly
**Cause**: Storage type not suitable for the data being stored
**Solution**:
- Use `FIX` for small, fixed-size data
- Use `VAR` for variable-length data
- Use `PAR` for large data requiring partial updates

### Database Type Selection

#### Issue: Wrong Database Type for Use Case
**Symptoms**: Poor performance for intended workload
**Cause**: Database type not optimized for the access pattern
**Solution**:

**Choose Row Store When**:
- Frequent record updates
- Point queries on specific records
- Small to medium datasets
- Transactional workloads

**Choose Column Store When**:
- Analytical queries and aggregations
- Column scans across many records
- Large datasets with many columns
- Sparse data (many NULL values)

### Storage Efficiency Issues

#### Issue: High Storage Overhead
**Symptoms**: Database uses more storage than expected
**Cause**: Inefficient storage type or growth parameters
**Solution**:
- Use appropriate storage types for your data
- Monitor growth parameters
- Consider data compression for similar data types

#### Issue: Poor Query Performance
**Symptoms**: Queries are slower than expected
**Cause**: Access pattern not optimized for database type
**Solution**:
- Design queries for the database type (row vs column oriented)
- Use appropriate field types and storage options
- Consider indexing strategies

## Debug Information

### Using the Debug Function
The `debug()` function provides detailed information about database structure:

```grapa
f.debug()
```

**Information Provided**:
- Database type and structure
- Field definitions and storage types
- Data distribution and storage efficiency
- Tree structure details

### Common Debug Output Analysis

#### Column Store Debug Output
Look for:
- `CTABLE_TREE` - Confirms column store database
- `FREC_DATA` - Fragmented data storage for fixed fields
- `SDATA_TREE` - Tree storage for variable fields
- Growth parameters and storage efficiency

#### Row Store Debug Output
Look for:
- `RTABLE_TREE` - Confirms row store database
- `BYTE_DATA` - Contiguous storage for fixed fields
- Field offsets and sizes
- Record structure information

## Performance Optimization

### Column Store Optimization

#### Schema Design
- Use fixed fields for small, frequently accessed data
- Consider data sparsity when designing schemas
- Choose appropriate storage types

#### Query Design
- Design for column-oriented access patterns
- Use aggregations and column scans when possible
- Avoid frequent point queries

#### Storage Management
- Monitor growth parameters
- Consider data compression
- Plan for data growth patterns

### Row Store Optimization

#### Schema Design
- Group related fields together
- Use fixed fields for primary keys
- Consider record size for optimal performance

#### Query Design
- Design for record-based access patterns
- Use point queries efficiently
- Minimize column scans

## Error Messages and Solutions

### Common Error Codes

#### Error -1: General Failure
**Possible Causes**:
- Invalid parameters
- Insufficient resources
- File system issues

**Solutions**:
- Check parameter validity
- Verify file system permissions
- Ensure sufficient disk space

#### Error -2: Not Found
**Possible Causes**:
- File or field doesn't exist
- Incorrect path or name

**Solutions**:
- Verify file/field exists
- Check path and naming
- Use `ls()` to verify contents

### Specific Error Scenarios

#### Field Creation Errors
```grapa
# Error: Field not created
f.mkfield("test", "INVALID_TYPE")
# Solution: Use valid field type
f.mkfield("test", "STR", "VAR")
```

#### Database Type Errors
```grapa
# Error: Wrong database type for workload
f.mk("small_db", "COL")  # Column store for small dataset
# Solution: Use row store for small datasets
f.mk("small_db", "ROW")
```

#### Storage Parameter Errors
```grapa
# Error: Missing size parameter for fixed field
f.mkfield("age", "INT", "FIX")  # Missing size
# Solution: Specify size for fixed fields
f.mkfield("age", "INT", "FIX", 4)
```

## Best Practices Summary

### Database Selection
1. **Row Store**: Transactional workloads, small datasets, frequent updates
2. **Column Store**: Analytical workloads, large datasets, sparse data

### Field Design
1. **Fixed Fields**: Small, frequently accessed data
2. **Variable Fields**: Medium-sized variable data
3. **Partitioned Fields**: Large data requiring partial updates

### Performance Monitoring
1. Use `debug()` function regularly
2. Monitor storage efficiency
3. Track query performance patterns
4. Adjust parameters based on usage

### Maintenance
1. Regular performance analysis
2. Storage optimization
3. Schema evolution planning
4. Backup and recovery procedures 