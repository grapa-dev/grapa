# GrapaFloat Implementation Documentation

**File**: `source/grapa/GrapaFloat.cpp`  
**Header**: `source/grapa/GrapaFloat.h`  
**Purpose**: High-precision floating-point arithmetic with configurable precision and fixed/floating point modes

## Overview

GrapaFloat implements arbitrary-precision floating-point arithmetic using a binary representation with configurable precision limits. It can operate in either fixed-point or floating-point modes, with dynamic precision management to handle unlimited precision scenarios.

## Data Structure

### Core Members

```cpp
class GrapaFloat {
    bool mSigned, mNaN, mTrunc, mFix;
    s64 mExp, mBits, mMax, mExtra;
    GrapaInt mData;
    GrapaBYTE mTail;
};
```

#### Boolean Flags
- **`mSigned`**: Indicates negative value (true = negative)
- **`mNaN`**: Not-a-Number flag for invalid operations
- **`mTrunc`**: Indicates value was truncated due to precision limits
- **`mFix`**: Fixed-point mode flag (true = fixed-point, false = floating-point)

#### Precision Control
- **`mExp`**: Exponent value (position of binary point relative to most significant bit)
- **`mBits`**: Total number of significant bits in `mData`
- **`mMax`**: Maximum allowed bits for the value (precision limit)
- **`mExtra`**: Extra precision bits beyond `mMax` before truncation

#### Data Storage
- **`mData`**: `GrapaInt` containing the actual binary digits
- **`mTail`**: `GrapaBYTE` for future extensibility (maintained in serialization)

## Fixed vs Floating Point Modes

### Fixed-Point Mode (`mFix = true`)
- **Purpose**: Maintains consistent decimal precision regardless of magnitude
- **Behavior**: 
  - Precision is measured from the decimal point
  - `mMax` represents maximum fractional digits
  - Integer part can grow without limit
  - Useful for financial calculations requiring exact decimal precision
- **Example**: `123.456` with `mMax = 3` maintains exactly 3 decimal places

### Floating-Point Mode (`mFix = false`)
- **Purpose**: Maintains consistent total precision regardless of magnitude
- **Behavior**:
  - Precision is measured in total significant bits
  - `mMax` represents total significant bits
  - Both integer and fractional parts share the precision budget
  - Useful for scientific calculations requiring consistent relative precision
- **Example**: `123.456` and `0.00123456` both use same total precision

## Precision Management

### Unlimited Precision Concept
GrapaFloat can handle unlimited precision through:
1. **Dynamic allocation**: `mData` (GrapaInt) can grow as needed
2. **Configurable limits**: `mMax` and `mExtra` control when truncation occurs
3. **Truncation tracking**: `mTrunc` flag indicates when precision was lost

### Precision Calculation
```cpp
// Fixed-point mode
bits = mBits - mExp;  // Only fractional bits count toward limit

// Floating-point mode  
bits = mBits;         // All bits count toward limit
```

### Truncation Logic (`Truncate()`)
1. **Early exit**: Zero values are handled specially
2. **Bit calculation**: Determines total bits based on mode
3. **Shift operation**: Removes excess bits beyond `mMax + mExtra`
4. **Leading zero removal**: Normalizes by removing leading zeros
5. **Truncation flag**: Sets `mTrunc` when precision is lost

## Internal Representation

### Binary Point Positioning
- **`mExp`**: Position of binary point relative to most significant bit
- **Positive `mExp`**: Binary point is to the right (integer part)
- **Negative `mExp`**: Binary point is to the left (fractional part)
- **Example**: `mExp = 2` means binary point is 2 positions right of MSB

### Value Reconstruction
```cpp
// Value = mData * 2^(mExp - mBits + 1)
// If mSigned, value is negative
```

## Key Member Functions

### Construction and Initialization
- **`INIT()`**: Sets default values (32 bits max, 10 extra, floating-point mode)
- **`FROM()`**: Copies all members from another GrapaFloat
- **`FromInt()`**: Converts GrapaInt to GrapaFloat
- **`FromString()`**: Parses string representation with configurable radix

### Precision Operations
- **`TO()`**: Extracts integer and fractional parts
- **`Truncate()`**: Enforces precision limits and normalizes
- **`IsInt()`**: Checks if value represents an integer
- **`IsZero()`**: Checks if value is zero

### Comparison Operations
- **`Comp()`**: Normalized three-way comparison (returns difference)
- **`Equals()`**: Exact equality check
- **`operator==`, `operator!=`, etc.**: Standard comparison operators

### Arithmetic Operations
- **`Add()`**: Core addition algorithm with alignment
- **`operator+`, `operator-`, `operator*`, `operator/`**: Standard arithmetic
- **`Pow()`, `Root()`**: Power and root functions
- **Trigonometric functions**: `Sin()`, `Cos()`, `Tan()`, etc.

## String Parsing (`FromString()`)

### Decimal Parsing Algorithm
1. **Split on decimal point**: Separates integer and fractional parts
2. **Integer conversion**: Uses GrapaInt::FromString()
3. **Fractional conversion**: 
   - Converts decimal digits to binary using repeated multiplication
   - Handles precision limits and truncation
   - Supports different radixes (2, 4, 8, 10, 16)

### Precision Control
- **`max` parameter**: Overrides default precision limits
- **Dynamic adjustment**: Adjusts precision based on integer part length
- **Truncation detection**: Sets `mTrunc` when fractional precision is limited

## Serialization (`FromBytes()`/`getBytes()`)

### Binary Format
- **Header byte**: Contains flags (signed, extended, trunc, fix, exponent bits)
- **Exponent**: Variable-length signed integer
- **Precision info**: `mMax` and `mExtra` values
- **Data**: Actual binary digits
- **Tail**: Future extensibility data

## Usage Patterns

### Fixed-Point for Financial Calculations
```cpp
GrapaFloat price(true, 32, 10, 0);  // Fixed-point, 32 fractional bits
price.FromString("123.456", 10);    // Maintains exact decimal precision
```

### Floating-Point for Scientific Calculations
```cpp
GrapaFloat pi(false, 64, 10, 0);    // Floating-point, 64 total bits
pi = GrapaFloat::Pi(pi);            // High-precision π calculation
```

### Unlimited Precision Operations
```cpp
GrapaFloat result(false, 0, 0, 0);  // No limits (unlimited precision)
// Perform calculations that may exceed normal precision limits
```

## Error Handling

### NaN Conditions
- **Invalid operations**: Division by zero, invalid string parsing
- **`mNaN` flag**: Indicates invalid state
- **Propagation**: NaN values propagate through arithmetic operations

### Truncation Detection
- **`mTrunc` flag**: Indicates precision was lost
- **User notification**: Applications can check truncation status
- **Precision adjustment**: Can increase `mMax` or `mExtra` to reduce truncation

## Performance Considerations

### Memory Usage
- **Dynamic allocation**: Memory grows with precision requirements
- **Truncation**: Prevents unbounded memory growth
- **Efficient storage**: Uses GrapaInt for optimal bit storage

### Computational Complexity
- **Arithmetic operations**: Complexity depends on bit length
- **String parsing**: More complex for high-precision decimal values
- **Truncation**: O(n) where n is number of excess bits

## Integration Notes

### Grapa Language Integration
- **Operator overloading**: All standard operators are available
- **Type conversion**: Automatic conversion from strings, integers, doubles
- **Precision inheritance**: New values inherit precision settings from operands

### Comparison with Standard Types
- **vs `double`**: Unlimited precision vs 53-bit precision
- **vs `float`**: Configurable precision vs 24-bit precision
- **vs `decimal`**: Binary vs decimal representation

---

**Reference**: This documentation should be consulted when working with any float-related operations in Grapa, including operator implementations, string parsing, and precision management. 