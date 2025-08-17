---
tags:
  - maintainer
  - lowlevel
  - implementation
  - reflection
  - describe
  - enhanced-reflection
---

# Enhanced .describe() Implementation

## Overview

The enhanced `.describe()` function provides comprehensive object inspection and reflection capabilities in Grapa. It replaces the need for multiple separate reflection methods by offering a single, configurable method that can provide as much or as little information as needed.

## Implementation Details

### C++ Implementation

**File:** `source/grapa/GrapaLibRule.cpp`
**Class:** `GrapaLibraryRuleDescribeEvent`

The implementation follows the same pattern as other Grapa library functions:

1. **Parameter Parsing**: Uses `GrapaLibraryParam` to extract the object and options
2. **Options Handling**: Parses options object with PTR handling (like string distance functions)
3. **Type-Specific Logic**: Generates descriptions based on object type
4. **Output Formatting**: Supports both text and JSON output formats

### Options Parameter Structure

```cpp
// Parse options with defaults
bool include_properties = true;
bool include_methods = true;
bool include_structure = false;
bool include_values = false;
std::string format = "text";

// Parse options if provided
if (r2.vVal && (r2.vVal->mValue.mToken == GrapaTokenType::OBJ || r2.vVal->mValue.mToken == GrapaTokenType::LIST))
{
    // Parse each option with PTR handling
    s64 properties_index = 0;
    GrapaRuleEvent* properties_opt = r2.vVal->vQueue->Search(GrapaCHAR("properties"), properties_index);
    // Handle PTR types like other functions do
    while (properties_opt && properties_opt->mValue.mToken == GrapaTokenType::PTR && properties_opt->vRulePointer) 
        properties_opt = properties_opt->vRulePointer;
    // ... parse option value
}
```

### Type-Specific Descriptions

#### String/ID Types
```cpp
case GrapaTokenType::STR:
case GrapaTokenType::ID:
{
    std::string str_val(reinterpret_cast<const char*>(r1.vVal->mValue.mBytes), r1.vVal->mValue.mLength);
    if (format == "json")
    {
        description = "{\"type\":\"string\",\"length\":" + std::to_string(str_val.length());
        if (include_values)
        {
            description += ",\"value\":\"" + str_val + "\"";
        }
        description += "}";
    }
    else
    {
        description = "String with length " + std::to_string(str_val.length());
        if (include_values)
        {
            description += ": \"" + str_val + "\"";
        }
    }
}
```

#### Float Types with Structure
```cpp
case GrapaTokenType::FLOAT:
{
    GrapaFloat float_val(vScriptExec->vScriptState->mItemState.mFloatFix, vScriptExec->vScriptState->mItemState.mFloatMax, vScriptExec->vScriptState->mItemState.mFloatExtra, 0);
    float_val.FromBytes(r1.vVal->mValue);
    GrapaCHAR float_str = float_val.ToString();
    std::string str_val(reinterpret_cast<const char*>(float_str.mBytes), float_str.mLength);
    
    if (format == "json")
    {
        description = "{\"type\":\"float\",\"value\":\"" + str_val + "\"";
        if (include_structure)
        {
            description += ",\"structure\":{\"sign\":" + std::string(float_val.mSigned ? "true" : "false") + 
                            ",\"trunc\":" + std::string(float_val.mTrunc ? "true" : "false") + 
                            ",\"fix\":" + std::string(float_val.mFix ? "true" : "false") + 
                            ",\"exp\":" + std::to_string(float_val.mExp) + 
                            ",\"max\":" + std::to_string(float_val.mMax) + 
                            ",\"extra\":" + std::to_string(float_val.mExtra) + 
                            ",\"data\":\"" + std::to_string(float_val.mData.LongValue()) + "\"}";
        }
        description += "}";
    }
    else
    {
        description = "Float: " + str_val;
        if (include_structure)
        {
            description += " (sign:" + std::string(float_val.mSigned ? "true" : "false") + 
                            ", trunc:" + std::string(float_val.mTrunc ? "true" : "false") + 
                            ", fix:" + std::string(float_val.mFix ? "true" : "false") + 
                            ", exp:" + std::to_string(float_val.mExp) + 
                            ", max:" + std::to_string(float_val.mMax) + 
                            ", extra:" + std::to_string(float_val.mExtra) + 
                            ", data:" + std::to_string(float_val.mData.LongValue()) + ")";
        }
    }
}
```

#### List Types (Key-Value Pairs)
```cpp
case GrapaTokenType::LIST:
{
    if (r1.vVal->vQueue)
    {
        u64 count = r1.vVal->vQueue->mCount;
        // For LIST, count represents the number of key-value pairs
        u64 property_count = count;
        if (format == "json")
        {
            description = "{\"type\":\"list\",\"length\":" + std::to_string(count) + ",\"properties\":" + std::to_string(property_count);
            if (include_properties && count > 0)
            {
                description += ",\"keys\":[";
                // Extract keys by searching for them (like we do with options)
                for (u64 i = 0; i < count && i < 5; i++)
                {
                    GrapaRuleEvent* item = r1.vVal->vQueue->Head(i);
                    if (i > 0) description += ",";
                    // The item should have a name (key) and value
                    if (item && item->mName.mLength > 0)
                    {
                        std::string key_str(reinterpret_cast<const char*>(item->mName.mBytes), item->mName.mLength);
                        description += "\"" + key_str + "\"";
                    }
                }
                if (count > 5) description += ",\"...\"";
                description += "]";
            }
            description += "}";
        }
        else
        {
            description = "List with " + std::to_string(property_count) + " properties";
            if (include_properties && count > 0)
            {
                description += " (keys: ";
                for (u64 i = 0; i < count && i < 3; i++)
                {
                    GrapaRuleEvent* item = r1.vVal->vQueue->Head(i);
                    if (i > 0) description += ", ";
                    // The item should have a name (key) and value
                    if (item && item->mName.mLength > 0)
                    {
                        std::string key_str(reinterpret_cast<const char*>(item->mName.mBytes), item->mName.mLength);
                        description += key_str;
                    }
                }
                if (count > 3) description += ", ...";
                description += ")";
            }
        }
    }
    else
    {
        description = "Empty list";
    }
}
```

## Grapa Class Definition

**File:** `lib/grapa/$OBJ.grc`

```grapa
describe = @<[op,@<describe,{@<this>,@<var,{options}>}>],{options}>;
```

## Float Structure Components

When `structure: true` is used with float values, the method reveals the 7 internal components of Grapa's `$FLOAT` type:

1. **sign** - Boolean indicating if the number is signed
2. **trunc** - Boolean indicating if truncation occurred
3. **fix** - Boolean indicating if it's a fixed-point number
4. **exp** - Exponent value
5. **max** - Maximum bits
6. **extra** - Extra precision bits
7. **data** - The actual numeric data

This provides complete introspection into Grapa's high-precision floating-point implementation.

## PTR Handling

The implementation properly handles PTR types in options parsing, following the same pattern used in string distance functions:

```cpp
// Handle PTR types like other functions do
while (properties_opt && properties_opt->mValue.mToken == GrapaTokenType::PTR && properties_opt->vRulePointer) 
    properties_opt = properties_opt->vRulePointer;
```

This ensures that options can be accessed correctly regardless of how they're stored in the Grapa execution tree.

## Testing

The implementation has been tested with:

- Basic type descriptions (strings, integers, arrays, lists)
- Float structure inspection with all 7 components
- Options parsing with various data types
- JSON and text output formats
- PTR handling in options
- Edge cases (empty objects, large values)

## Future Enhancements

Potential future enhancements could include:

- XML output format support
- More detailed method signatures for functions
- Class inheritance information
- Table structure details for database objects
- Performance metrics for large objects
- Custom formatter plugins

## Integration

This implementation satisfies the "Enhanced Reflection" roadmap item by providing:

- Single method for all reflection needs
- Configurable options for different use cases
- Multiple output formats
- Type-specific detailed information
- Complete float structure inspection
- Efficient implementation with minimal overhead
