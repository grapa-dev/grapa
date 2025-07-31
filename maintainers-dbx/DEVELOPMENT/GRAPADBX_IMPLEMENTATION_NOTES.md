# GrapaDBX Implementation Notes

## Overview
GrapaDBX is a high-performance database implementation that provides enhanced storage capabilities for the Grapa language. It supports multiple table types (ROW, COL, GROUP) and various field storage formats with proper type conversion and memory management.

## Architecture

### Class Hierarchy
```
GrapaBtree (Base)
    ↓
GrapaDBX
    ↓
GrapaGroup2
```

### Key Components
- **GrapaDBX**: Core database implementation with enhanced storage capabilities
- **GrapaGroup2**: Group-specific operations and table management
- **GrapaDBXCursor**: Internal cursor management for database operations
- **GrapaDBXField**: Field metadata structure for dictionary management

## Table Types

### ROW Tables (`RTABLE_TREE = 4`)
- **Structure**: Key-value pairs with both `$KEY` and `$VALUE` fields
- **Record Type**: `RREC_ITEM` (7)
- **Field Layout**: 
  - `$DICT`: doffset=0, dsize=2
  - `$KEY`: doffset=0, dsize=258
  - `$VALUE`: doffset=258, dsize=8

### COL Tables (`CTABLE_TREE = 5`)
- **Structure**: Column-oriented storage with `$VALUE` field only
- **Record Type**: `CREC_ITEM` (8)
- **Field Layout**:
  - `$DICT`: doffset=1, dsize=8
  - `$VALUE`: doffset=0, dsize=8

### GROUP Tables (`GROUP_TREE = 3`)
- **Structure**: Nested table structure with both `$KEY` and `$VALUE` fields
- **Record Type**: `RREC_ITEM` (7) within nested `RTABLE_TREE`
- **Field Layout**:
  - `$DICT`: doffset=0, dsize=2
  - `$KEY`: doffset=0, dsize=258
  - `$VALUE`: doffset=258, dsize=8

## Field Storage Formats

### STORE_FIX (Fixed Size)
- **Format**: Length bytes + Type byte + Data
- **Example**: `[length][type][data]`
- **Use Case**: Fixed-size fields like integers, floats, booleans

### STORE_VAR (Variable Size)
- **Format**: 8-byte pointer to separate data block
- **Example**: `[pointer_to_data_block]`
- **Use Case**: Variable-size fields like strings, arrays, lists

### STORE_PAR (Parameter Size)
- **Format**: 8-byte pointer to separate data block
- **Example**: `[pointer_to_data_block]`
- **Use Case**: Parameterized fields with dynamic sizing

## Data Type Support

### Supported Grapa Types
- **String**: UTF-8 encoded text data
- **Integer**: 64-bit signed integers
- **Float**: 64-bit floating point numbers
- **Boolean**: True/false values
- **Array**: Ordered collections of values
- **List**: Key-value pairs and complex structures
- **Null**: Null values
- **Empty Values**: Empty strings, zero values

### Type Conversion
- **Storage**: Grapa types are converted to appropriate DBX field types
- **Retrieval**: DBX field types are converted back to Grapa types
- **RAW Fields**: Support for storing any Grapa type with embedded type information

## Memory Management

### Field Dictionary
- **Structure**: `GrapaDBXField` struct with explicit field ordering
- **Storage**: Big-endian format for cross-platform compatibility
- **Field IDs**: Start from 1 for data fields (0 reserved for `$DICT`)

### Data Blocks
- **Creation**: `NewData()` for variable-size data storage
- **Deletion**: `DeleteData()` for proper cleanup
- **Resizing**: `SetDataSize()` for dynamic size adjustment

### Pointer Management
- **Big-endian Conversion**: `BE_S64()` applied to pointers during read operations
- **Null Pointer Handling**: Checks for null pointers in `GetRecordFieldData`
- **Memory Safety**: Proper cleanup and validation

## Field Creation System

### Current Working Directory Management
- **Context Setting**: `CreateTableStructure` sets `mDirId = newTree` and `mDirType = mGrapaDBXTableType` after table creation
- **Field Creation Context**: `GrapaLibraryRuleUnifiedMkfieldEvent` uses current working directory context for all table types
- **Cross-Table Support**: Works correctly for ROW, COL, and GROUP table types

### Field Creation Process
1. **Table Creation**: `db.mk('table_name')` creates table and sets current working directory
2. **Field Creation**: `db.mkfield('field_name')` uses current working directory context
3. **Dictionary Management**: Field names stored in `$DICT` field with proper metadata
4. **Field ID Assignment**: Uses field-based approach matching reference implementation

### Field Dictionary Structure
- **Field Names**: Stored as UTF-8 strings in separate data blocks
- **Field Metadata**: `GrapaDBXField` struct with type, store, size, and grow information
- **Dictionary Layout**: Sequential storage with proper offset calculations
- **Field ID Assignment**: Dynamic assignment based on existing field count

## Key Implementation Details

### Field Offset Calculation
```cpp
static u64 runningDataOffset = 0;  // Track data field offsets independently

// Reset for dictionary field
if (mId == 0) {
    runningDataOffset = 0;
}

// Set offset for data fields
pField.mDictOffset = runningDataOffset;
runningDataOffset += pField.mDictSize;
```

### Record Data Storage
```cpp
// STORE_FIX: Complex format with length/type bytes
SetDataValue(itemPtr, offset, lengthBytes, buffer);
SetDataValue(itemPtr, offset + lengthBytes, typeByte, buffer);
SetDataValue(itemPtr, offset + lengthBytes + typeByte, dataSize, dataBuffer);

// STORE_VAR/STORE_PAR: Pointer to separate data block
u64 dataPtr = NewData(dataSize);
SetDataValue(itemPtr, offset, 8, &dataPtr);
```

### Field Retrieval
```cpp
// Convert pointer from big-endian
dataPtr = BE_S64(dataPtr);

// Handle null pointers
if (!dataPtr) {
    // Return empty buffer
    return;
}

// Read data from separate block
GetDataValue(dataPtr, 0, dataSize, buffer);
```

## Performance Characteristics

### Storage Efficiency
- **Fixed Fields**: Direct storage in record data block
- **Variable Fields**: Pointer-based storage with separate data blocks
- **Dictionary**: Compact field metadata storage

### Memory Usage
- **Field Metadata**: Minimal overhead with efficient struct layout
- **Data Storage**: Optimized for common data types
- **Pointer Management**: Efficient big-endian conversion

### Cross-platform Compatibility
- **Big-endian Storage**: Ensures compatibility across different architectures
- **Struct Layout**: Explicit field ordering prevents alignment issues
- **Memory Safety**: Proper null pointer handling and validation

## Testing and Validation

### Test Coverage
- **Basic Operations**: Set/get operations across all table types
- **Data Types**: All supported Grapa data types
- **Complex Structures**: Nested arrays, lists, and objects
- **Multiple Records**: Bulk operations and record management
- **Reference Compatibility**: Matching behavior with reference implementation

### Validation Results
- ✅ **Basic Functionality**: All table types working correctly
- ✅ **Data Type Support**: All Grapa types properly handled
- ✅ **Field Management**: Dictionary and field operations working
- ✅ **Field Creation**: `mkfield` functionality working across all table types
- ✅ **Current Working Directory**: Proper context management after table creation
- ✅ **Record Operations**: Create, read, update, delete working
- ✅ **Memory Safety**: No corruption or leaks detected
- ✅ **Reference Compatibility**: Matches reference implementation behavior

## Future Enhancements

### Planned Features
- **Field Deletion**: `rmfield` functionality for dynamic schema changes
- **Custom Field Types**: User-defined field types beyond defaults
- **Performance Optimization**: Large dataset optimizations
- **Enhanced Error Handling**: Better error reporting and recovery

### Technical Improvements
- **Concurrent Access**: Multi-threaded database operations
- **Recovery Mechanisms**: Database corruption detection and repair
- **Compression**: Data compression for large datasets
- **Indexing**: Advanced indexing for complex queries

## Debugging and Troubleshooting

### Common Issues
1. **Field Corruption**: Check struct layout and endian conversion
2. **Memory Leaks**: Verify proper cleanup in `DeleteData`
3. **Null Pointers**: Ensure null checks in `GetRecordFieldData`
4. **Offset Errors**: Verify `runningDataOffset` calculation

### Debug Output
- **Field Metadata**: Shows field offsets, sizes, and types
- **Record Data**: Displays stored field values
- **Memory Usage**: Tracks data block allocation and deallocation
- **Error Reporting**: Detailed error messages for troubleshooting

## Integration with Grapa Language

### Unified Storage Interface
- **`$unified()`**: Provides unified access to different storage types
- **URL-based Configuration**: `grapadbx://filename.dbx?type=TABLE_TYPE`
- **Automatic Type Detection**: Detects table type from URL parameters

### API Compatibility
- **Standard Operations**: `create`, `mk`, `set`, `get`, `rm`
- **Field Operations**: `mkfield`, `rmfield` (planned)
- **Debug Operations**: `debug()` for database inspection
- **List Operations**: `ls()` for directory listing

This implementation provides a robust, high-performance database solution that integrates seamlessly with the Grapa language while maintaining compatibility with existing code patterns. 