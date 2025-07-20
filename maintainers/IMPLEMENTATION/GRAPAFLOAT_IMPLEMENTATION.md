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

#### Float-String Equality Comparison with Adaptive Tolerance
The `Comp()` method includes enhanced handling for float-string equality comparisons with adaptive tolerance to address precision differences between float literals and string-converted floats:

```cpp
// In Comp() method - string comparison branch
if (IsValidNumericString(strVal)) {
    // Convert string to float using *this precision settings
    GrapaFloat b(mFix, mMax, mExtra, 0);
    b.FromString(strVal, 10);
    
    // Use normalized subtraction for comparison
    GrapaFloat result_normalized(mFix, mMax, mExtra, result);
    GrapaFloat b_normalized(mFix, mMax, mExtra, b);
    GrapaFloat diff = result_normalized - b_normalized;
    diff.Truncate(false);
    
    // Adaptive tolerance check that scales with magnitude
    GrapaFloat abs_result = result_normalized.Abs();
    GrapaFloat abs_b = b_normalized.Abs();
    GrapaFloat max_magnitude = (abs_result > abs_b) ? abs_result : abs_b;
    
    // Base tolerance: 10^-10, but scale with magnitude
    GrapaFloat base_tolerance(mFix, mMax, mExtra, 0);
    base_tolerance.FromString("0.0000000001", 10);  // 10^-10
    
    // For very small numbers, use the base tolerance
    // For larger numbers, scale the tolerance proportionally
    GrapaFloat tolerance = base_tolerance;
    if (max_magnitude > base_tolerance) {
        // Scale tolerance to be proportional to the larger number
        // Use a relative tolerance of 10^-10
        tolerance = max_magnitude * base_tolerance;
    }
    
    if (diff.Abs() < tolerance) {
        return GrapaFloat(0.0); // treat as equal
    }
    return diff;
}
```

**Key Features:**
- **Precision normalization**: Both operands use the same precision settings from `*this`
- **Adaptive tolerance**: Scales tolerance proportionally with the magnitude of the numbers being compared
- **Enhanced string validation**: Validates numeric strings before conversion with improved whitespace handling
- **Magnitude-aware comparison**: Uses relative tolerance for large numbers, absolute tolerance for small numbers

**Purpose**: Ensures that `(55.3 == '55.3')` evaluates to `true` while still correctly distinguishing between actually different values like `(55.3 == '55.4')`. The adaptive tolerance provides better handling for both very small and very large numbers.

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

## Precision Handling Patterns

### Core Design Principle
GrapaFloat follows a consistent pattern where **the first operand (`*this`) determines the precision settings** for the operation. This ensures predictable behavior and prevents precision mismatches.

### Operator Precision Patterns

#### Arithmetic Operators (`+`, `-`, `*`, `/`)
```cpp
// Pattern: Use higher precision between operands for result
GrapaFloat result(mFix && bi.mFix ? true : false, 
                  mMax > bi.mMax ? mMax : bi.mMax, 
                  mExtra > bi.mExtra ? mExtra : bi.mExtra, 0);
```
- **Result precision**: Uses the higher precision between `*this` and the other operand
- **Purpose**: Preserves maximum available precision in calculations
- **Example**: `high_precision + low_precision` → result uses `high_precision` settings

#### Comparison Operators (`==`, `!=`, `>`, `<`, `>=`, `<=`)
```cpp
// Pattern: Convert other operand to *this precision, then compare
GrapaFloat bi2(mFix, mMax, mExtra, other_value);
return *this == bi2;  // Uses *this precision for comparison
```
- **Comparison precision**: Uses `*this` precision settings
- **Purpose**: Ensures consistent comparison behavior regardless of operand precision
- **Example**: `high_precision == low_precision` → comparison uses `high_precision` settings

#### String Conversion
```cpp
// Pattern: Convert string using *this precision settings
GrapaFloat b(mFix, mMax, mExtra, 0);  // Use *this precision
b.FromString(strBytes, 10);
```
- **String precision**: Uses `*this` precision settings for conversion
- **Purpose**: Ensures string-to-float conversions match the expected precision context
- **Example**: `high_precision == "55.3"` → string converted to `high_precision` settings

#### Adaptive Tolerance-Based Equality
```cpp
// Pattern: Use adaptive tolerance for float-string equality comparisons
GrapaFloat abs_result = result_normalized.Abs();
GrapaFloat abs_b = b_normalized.Abs();
GrapaFloat max_magnitude = (abs_result > abs_b) ? abs_result : abs_b;

// Base tolerance: 10^-10, but scale with magnitude
GrapaFloat base_tolerance(mFix, mMax, mExtra, 0);
base_tolerance.FromString("0.0000000001", 10);  // 10^-10

// For very small numbers, use the base tolerance
// For larger numbers, scale the tolerance proportionally
GrapaFloat tolerance = base_tolerance;
if (max_magnitude > base_tolerance) {
    // Scale tolerance to be proportional to the larger number
    // Use a relative tolerance of 10^-10
    tolerance = max_magnitude * base_tolerance;
}

if (diff.Abs() < tolerance) {
    return GrapaFloat(0.0); // treat as equal
}
```
- **Adaptive tolerance**: Scales tolerance proportionally with the magnitude of the numbers being compared
- **Purpose**: Distinguishes between precision differences and actual value differences, with better handling for both very small and very large numbers
- **Example**: `(55.3 == '55.3')` → `true`, `(55.3 == '55.4')` → `false`, `(1e100 == '1e100')` → `true`

### Three-Way Comparison (`Comp()`)
The `Comp()` method follows the same pattern:
```cpp
// For string comparisons: use *this precision
GrapaFloat b(mFix, mMax, mExtra, 0);
b.FromString(strBytes, 10);

// For float comparisons: use higher precision
GrapaFloat result_normalized(mFix, mMax, mExtra, result);
GrapaFloat b_normalized(mFix, mMax, mExtra, b);
```

### Precision Inheritance Strategy

#### When `*this` Has Higher Precision
- **Arithmetic**: Result uses `*this` precision (higher)
- **Comparison**: Other operand converted to `*this` precision
- **String conversion**: String converted to `*this` precision

#### When `*this` Has Lower Precision  
- **Arithmetic**: Result uses other operand precision (higher)
- **Comparison**: Other operand converted to `*this` precision
- **String conversion**: String converted to `*this` precision

#### Benefits of This Approach
1. **Predictable behavior**: `*this` always determines the precision context
2. **Precision preservation**: Arithmetic operations don't lose precision unnecessarily
3. **Consistent comparisons**: Same values always compare equal regardless of original precision
4. **String compatibility**: String literals adapt to the expected precision context

### Implementation Examples

#### Addition with Different Precisions
```cpp
GrapaFloat high_prec(false, 128, 10, 0);   // 128-bit precision
GrapaFloat low_prec(false, 64, 10, 0);     // 64-bit precision

// high_prec + low_prec → result uses 128-bit precision
GrapaFloat result = high_prec + low_prec;  // result.mMax = 128
```

#### String Comparison
```cpp
GrapaFloat precise(false, 180, 10, 55.3);  // High precision float
// precise == "55.3" → string converted to 180-bit precision for comparison
bool equal = (precise == "55.3");  // Uses precise's precision settings
```

#### Mixed-Type Operations
```cpp
GrapaFloat base(false, 96, 10, 0);         // 96-bit precision
// All operations use base's precision context
GrapaFloat result1 = base + 42;            // 42 converted to 96-bit
GrapaFloat result2 = base * "3.14";        // "3.14" converted to 96-bit
bool check = (base == "55.3");             // "55.3" converted to 96-bit
```

## Integration Notes

### Grapa Language Integration
- **Operator overloading**: All standard operators are available
- **Type conversion**: Automatic conversion from strings, integers, doubles
- **Precision inheritance**: New values inherit precision settings from operands

### Comparison with Standard Types
- **vs `double`**: Unlimited precision vs 53-bit precision
- **vs `float`**: Configurable precision vs 24-bit precision
- **vs `decimal`**: Binary vs decimal representation

## Known Limitations

### Scientific Notation Support
**Status**: Not currently supported  
**Impact**: Scientific notation like `1e-10`, `1.5e+3`, `2.3E-5` cannot be parsed  
**Workaround**: Use decimal notation (e.g., `0.0000000001` instead of `1e-10`)  
**Future Enhancement**: Scientific notation parsing should be implemented at the tokenization/parsing level, not in GrapaFloat itself

**Technical Details**:
- `FromString()` method only handles decimal point parsing (splits on `.`)
- No `e` or `E` exponent parsing in string conversion
- `GrapaFloat(double)` constructor limited by double precision (53 bits)
- Would require changes to base-level byte parsing and tokenization step or higher-level rules in `$grapa.grc`

## Encode/Decode Methods

### Internal Structure Access
GrapaFloat provides bidirectional access to its internal structure through `.encode("FLOAT")` and `.decode("FLOAT")` methods.

#### Decode Method (Extract Components)
```grapa
> "30.75".float(300,6).decode("FLOAT")
{"sign":false,"trunc":false,"fix":false,"exp":4,"max":300,"extra":6,"data":123}
```

**Implementation**: Located in `GrapaLibraryRuleDecodeEvent::Run()` in `GrapaLibRule.cpp`
- Creates a new GrapaFloat with script state precision settings
- Calls `FromBytes()` to populate the float from binary data
- Returns a list object with all internal components

#### Encode Method (Create from Components)
```grapa
components = {
    "sign": false,
    "trunc": false, 
    "fix": false,
    "exp": 4,
    "max": 300,
    "extra": 6,
    "data": 123
};
float_value = components.encode("FLOAT");
```

**Implementation**: Located in `GrapaLibraryRuleEncodeEvent::Run()` in `GrapaLibRule.cpp`
- Extracts components from the input list object
- Creates a new GrapaFloat with specified precision settings
- Sets internal flags and values from component data
- Returns the float as binary bytes

### Component Structure
Both methods work with the same component structure:
- **`sign`**: Boolean for negative numbers
- **`trunc`**: Boolean for truncation flag
- **`fix`**: Boolean for fixed-point mode
- **`exp`**: Integer exponent value
- **`max`**: Integer maximum bit precision
- **`extra`**: Integer extra bits for calculation
- **`data`**: Integer/raw data representing the numeric value

### Use Cases
- **Debugging**: Inspect internal float state during development
- **Serialization**: Convert floats to/from structured data
- **Precision Analysis**: Examine precision settings and truncation
- **Binary Analysis**: Access raw binary representation via `.data.raw()`

---

**Reference**: This documentation should be consulted when working with any float-related operations in Grapa, including operator implementations, string parsing, and precision management. 