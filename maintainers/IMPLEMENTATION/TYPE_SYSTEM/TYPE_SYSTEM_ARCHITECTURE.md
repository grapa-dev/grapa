---
tags:
  - maintainer
  - implementation
  - type-system
  - architecture
  - design
---

# Type System Architecture

This document provides comprehensive implementation details for Grapa's type system architecture, including the type hierarchy, type checking mechanisms, and integration with other system components.

## Overview

**Purpose:** Sophisticated type system with unlimited precision arithmetic and comprehensive type safety
**Design:** Object-oriented type hierarchy with runtime type checking
**Status:** Complete implementation

## Architecture Overview

### **Type System Layers**
```
┌─────────────────────────────────────────────────────────────┐
│                    Grapa Language Layer                     │
│  $INT, $FLOAT, $STR, $LIST, $GOBJ, $TIME, etc.            │
├─────────────────────────────────────────────────────────────┤
│                   Type Checking Layer                       │
│  Runtime type validation, conversion, safety               │
├─────────────────────────────────────────────────────────────┤
│                   Serialization Layer                       │
│  Binary serialization, database storage, memory management │
├─────────────────────────────────────────────────────────────┤
│                   Core Type Layer                           │
│  GrapaValue, GrapaInt, GrapaFloat, GrapaBYTE, GrapaCHAR    │
└─────────────────────────────────────────────────────────────┘
```

### **Core Type Hierarchy**
```
GrapaValue (Base Class)
├── GrapaBYTE (Binary Data)
│   └── GrapaCHAR (Strings)
├── GrapaSS64 (Signed 64-bit)
├── GrapaSU64 (Unsigned 64-bit)
├── GrapaDU64 (Double Unsigned 64-bit)
├── GrapaInt (Unlimited Precision)
├── GrapaFloat (High Precision)
├── GrapaArray64 (64-bit Arrays)
├── GrapaVoidArray (Generic Arrays)
│   ├── GrapaCHARArray (String Arrays)
│   └── GrapaDU64Array (64-bit Arrays)
└── GrapaEvent (Event System)
    └── GrapaWorkEvent (Work Events)
```

## Implementation Details

### **1. Type Enumeration System (GrapaTokenType)**

#### **Type Classification**
```cpp
class GrapaTokenType {
public: enum {
    START = 0,
    ERR = 1,           // Error type
    RAW = 2,           // Raw binary data
    BOOL = 3,          // Boolean values
    ID = 4,            // Identifier
    STR = 5,           // String
    INPUT = 6,         // Input stream
    XML = 7,           // XML data
    INT = 8,           // Integer (unlimited precision)
    FLOAT = 9,         // Float (high precision)
    SYM = 10,          // Symbol
    SYSSYM = 11,       // System symbol
    TIME = 12,         // Time (uses unlimited precision INT)
    ARRAY = 13,        // Array
    LIST = 14,         // List
    EL = 15,           // Element
    TAG = 16,          // Tag
    TABLE = 17,        // Table
    SYSID = 18,        // System ID
    SYSSTR = 19,       // System string
    SYSINT = 20,       // System integer
    OP = 21,           // Operation
    CODE = 22,         // Code block
    VECTOR = 23,       // Vector
    CLASS = 24,        // Class
    OBJ = 25,          // Object
    REF = 26,          // Reference
    RULE = 27,         // Rule
    TOKEN = 28,        // Token
    WIDGET = 29,       // Widget
    TUPLE = 30,        // Tuple
    COMMENT = 31,      // Comment
    DOC = 32,          // Documentation
    X1 = 128,          // Extension point
    OTHER,             // Other types
    PTR,               // Pointer
    RULEREF,           // Rule reference
    RULEOP,            // Rule operation
    MAX                // Maximum type value
};
};
```

#### **Type Categories**
- **Primitive Types**: ERR, RAW, BOOL, ID, STR, INT, FLOAT, TIME
- **Collection Types**: ARRAY, LIST, TUPLE, VECTOR
- **System Types**: SYM, SYSSYM, SYSID, SYSSTR, SYSINT
- **Execution Types**: OP, CODE, RULE, TOKEN
- **Data Types**: TABLE, XML, TAG, EL
- **UI Types**: WIDGET

### **2. Base Type System (GrapaValue)**

#### **Base Class Design**
```cpp
class GrapaValue
{
public:
    u8 mToken;  // Type token from GrapaTokenType
};
```

#### **Binary Data Type (GrapaBYTE)**
```cpp
class GrapaBYTE : public GrapaValue
{
public:
    u8* mBytes;    // Binary data buffer
    u64 mSize;     // Allocated size
    u64 mLength;   // Actual data length
    u64 mPos;      // Current position
    u64 mGrow;     // Growth increment
    
    // Core operations
    virtual void FROM(u64 pSize, const void *pValue);
    virtual void FROM(const GrapaBYTE& pValue);
    virtual void FROM(const char *pStr);
    virtual void ToHex(GrapaBYTE& pValue);
    virtual void FromHex(GrapaBYTE& pValue);
    virtual void ToRaw(GrapaBYTE& pValue);
    virtual GrapaCHAR ToStr();
    
    // Memory management
    virtual void* GetPtr();
    virtual void SetNull();
    virtual u64 GetLength();
    virtual void SetLength(u64 pLength, bool pCopy = true);
    virtual void SetSize(u64 pSize, bool pCopy=true);
    virtual u64 GetSize();
    virtual void GrowSize(u64 pSize, bool pCopy=true);
    
    // Data operations
    virtual void Append(const GrapaCHAR& pValue);
    virtual void Append(const GrapaBYTE& pValue);
    virtual void Append(void *pValue, u64 pSize);
    virtual void Append(const u8 pValue);
    virtual s64 Cmp(const GrapaBYTE& pValue);
    virtual s64 Cmp(const char* pStr);
    virtual void Pad(u64 pSize, u8 pValue = 0);
    
    // Database integration
    virtual u8 ToDbType();
    virtual void FromDbType(u8 pType);
};
```

#### **String Type (GrapaCHAR)**
```cpp
class GrapaCHAR : public GrapaBYTE
{
public:
    // Constructors
    GrapaCHAR();
    GrapaCHAR(const char *pStr, u64 pLen);
    GrapaCHAR(const GrapaBYTE& pBlob);
    GrapaCHAR(s64 pNum);
    GrapaCHAR(u64 pId);
    GrapaCHAR(du64 pId);
    
    // String operations
    virtual void FROM(const char *pStr);
    virtual void FROM(const char* pStr, u64 pLen);
    virtual void FROM(s64 pNum);
    virtual void FROM(u64 pId);
    virtual void FROM(du64 pId);
    virtual void FROM(const GrapaCHAR& pChar);
    virtual void FROM(const GrapaBYTE& pValue);
    
    // String manipulation
    virtual void Append(const char* pStr, u64 pLen);
    virtual void Append(const char* pStr);
    virtual void Append(const char pChar);
    virtual void Append(const GrapaBYTE& pChar);
    virtual void Append(s64 pNum);
    virtual void Append(u64 pId);
    virtual void AppendQuoted(GrapaCHAR& pStr, bool pIsOptional);
    virtual void EscapeValue(GrapaCHAR &pTo, char* e);
    
    // String comparison
    virtual s64 StrNCmp(const char* pStr, u64 pLen);
    virtual s64 StrNCmp(const char* pStr);
    virtual s64 StrCmp(const char* pStr);
    virtual s64 StrCmp(const GrapaBYTE& pStr);
    virtual s64 StrCmp(const GrapaCHAR& pStr);
    virtual s64 StrUpperCmp(const char* pStr, u64 pLen = 0);
    virtual s64 StrUpperCmp(const GrapaBYTE& pStr);
    virtual s64 StrLowerCmp(const char* pStr, u64 pLen = 0);
    virtual s64 StrLowerCmp(const GrapaBYTE& pStr);
    
    // String transformation
    virtual void ToUpper();
    virtual void ToLower();
    virtual void RTrim(const char c = ' ');
    virtual void LTrim(const char c = ' ');
    virtual void Trim(const char c = ' ');
    virtual void RTrim(GrapaCHAR& s);
    virtual void LTrim(GrapaCHAR& s);
    virtual void Trim(GrapaCHAR& s);
    virtual void RTrim2(GrapaRuleEvent* s);
    virtual void LTrim2(GrapaRuleEvent* s);
    virtual void Trim2(GrapaRuleEvent* s);
    virtual void Pad(u64 pSize, u8 pValue = ' ');
    virtual void Replace(const GrapaBYTE& pFrom, const GrapaBYTE& pTo);
    
    // Base conversion
    virtual GrapaCHAR ToBase(u32 radix);
    virtual GrapaCHAR FromBase(u32 radix);
    
    // Boolean conversion
    static GrapaCHAR SetBool(bool isTrue) { 
        GrapaCHAR r("\0"); 
        if (isTrue) r.FROM("\1");  
        r.mToken = GrapaTokenType::BOOL; 
        return(r); 
    };
};
```

### **3. Unlimited Precision Integer (GrapaInt)**

#### **Array32 Foundation**
```cpp
class GrapaArray32
{
public:
    bool dataSigned, NaN;  // Sign and NaN flags
    u8* mBytes;            // Binary data
    u64 mSize, mLength, mGrow;  // Size management
    
    // Array operations
    void SetLength(u64 pLen = 0, bool pCopy = true);
    void SetSize(u64 pLen = 0, bool pCopy = true);
    void FROM(const GrapaArray32& pData);
    u64 GetCount() const;
    u64 GetByteCount() const;
    void SetCount(u64 pCount);
    void GrowCount(u64 pCount);
    void SetItem(u64 pItem, u32 pValue);
    u32 GetItem(u64 pItem) const;
    void RTrim();
    
    // State checking
    bool IsNull() const;
    bool IsNeg() const;
    bool IsSignNeg() const;
    bool IsZero() const;
    bool IsItem(u32 pItem) const;
};
```

#### **Unlimited Precision Implementation**
```cpp
class GrapaInt : public GrapaArray32, public GrapaCritical
{
public:
    // Constructors
    GrapaInt();
    GrapaInt(s64 value);
    GrapaInt(const GrapaInt& bi);
    GrapaInt(const char* value, s32 radix);
    GrapaInt(const u8* inData, u32 inLen);
    GrapaInt(u32* inData, u32 inCount);
    GrapaInt(const GrapaBYTE& inData);
    
    // Arithmetic operators
    GrapaInt operator +(const GrapaInt& bi2) const;
    GrapaInt operator +(s64 bi) const;
    GrapaInt operator +=(const GrapaInt& bi1);
    GrapaInt& operator ++();
    GrapaInt operator ++(int);
    GrapaInt operator -(const GrapaInt& bi2) const;
    GrapaInt operator -(s64 bi) const;
    GrapaInt operator -=(const GrapaInt& bi1);
    GrapaInt& operator --();
    GrapaInt operator --(int);
    GrapaInt operator *(const GrapaInt& bi2) const;
    GrapaInt operator *(s64 bi) const;
    GrapaInt operator *(u64 bi) const;
    
    // Bit operations
    GrapaInt operator <<(u64 shiftVal);
    GrapaInt operator >>(u64 shiftVal);
    GrapaInt operator ~() const;
    GrapaInt operator -() const;
    
    // Comparison operators
    bool operator ==(const GrapaInt& bi2) const;
    bool operator !=(const GrapaInt& bi2) const;
    bool Equals(const GrapaInt& bi) const;
    bool operator >(const GrapaInt& bi2) const;
    bool operator >(u64 i1) const;
    bool operator <(const GrapaInt& bi2) const;
    bool operator <(s64 bi) const;
    bool operator >=(const GrapaInt& bi2) const;
    bool operator <=(const GrapaInt& bi2) const;
    
    // Mathematical functions
    GrapaInt Abs() const;
    GrapaInt Div(const GrapaInt& bi2, GrapaInt& remainder);
    GrapaInt operator /(const GrapaInt& bi2);
    GrapaInt operator /(u64 bi);
    GrapaInt operator %(const GrapaInt& bi2);
    GrapaInt operator %(u64 bi);
    GrapaInt operator %=(const GrapaInt& bi1);
    
    // Bitwise operations
    GrapaInt operator &(const GrapaInt& bi2);
    GrapaInt operator |(const GrapaInt& bi2) const;
    GrapaInt operator ^(const GrapaInt& bi2) const;
    
    // Utility functions
    GrapaInt bmax(const GrapaInt& bi) const;
    GrapaInt bmin(const GrapaInt& bi) const;
    GrapaInt babs() const;
    d64 blog();
    d64 blog2() const;
};
```

### **4. High-Precision Float (GrapaFloat)**

#### **5-Part Structure**
```cpp
class GrapaFloat
{
public:
    // 5-part structure components
    bool mSigned;    // Sign bit
    bool mTrunc;     // Truncation flag
    bool mFix;       // Fixed-point flag
    u64 mExp;        // Exponent
    u64 mMax;        // Maximum precision bits
    u64 mExtra;      // Extra precision bits
    GrapaInt mData;  // Actual numeric data
    bool mNaN;       // Not-a-Number flag
    
    // Serialization format
    // Header byte: [signed][extended][trunc][fix][exponent_bits]
    // Exponent: Variable-length signed integer
    // Precision: mMax and mExtra values
    // Data: Actual binary digits
    // Tail: Future extensibility data
};
```

#### **Precision Control**
```cpp
// Fixed-point for financial calculations
GrapaFloat price(true, 32, 10, 0);  // Fixed-point, 32 fractional bits
price.FromString("123.456", 10);    // Maintains exact decimal precision

// Floating-point for scientific calculations
GrapaFloat pi(false, 64, 10, 0);    // Floating-point, 64 total bits
pi = GrapaFloat::Pi(pi);            // High-precision π calculation

// Unlimited precision operations
GrapaFloat result(false, 0, 0, 0);  // No limits (unlimited precision)
```

### **5. Array and Collection Types**

#### **64-bit Array (GrapaArray64)**
```cpp
class GrapaArray64 : public GrapaBYTE
{
public:
    // Array operations
    virtual inline u64 GetCount();
    virtual inline void SetCount(u64 pCount);
    virtual inline void GrowCount(u64 pCount);
    virtual inline void SetItem(u64 pItem, u64 pValue);
    virtual inline u64 GetItem(u64 pItem);
};
```

#### **Generic Arrays (GrapaVoidArray)**
```cpp
class GrapaVoidArray : public GrapaBYTE
{
public:
    GrapaVoidArray(u32 pCount=0);
    ~GrapaVoidArray();
    
    virtual void Init(u32 pCount=0);
    virtual void Append(void *pItem);
    virtual u32 Count();
    void* GetAt(u32 i);
};

class GrapaCHARArray : public GrapaVoidArray
{
public:
    GrapaCHARArray(u32 pCount=0);
    ~GrapaCHARArray();
    
    virtual void Append(char *pStr);
};

class GrapaDU64Array : public GrapaVoidArray
{
public:
    GrapaDU64Array(u32 pCount=0);
    ~GrapaDU64Array();
    
    virtual void Append(du64 pNum);
    virtual void Append(u64 key, u64 value);
};
```

## Type Checking and Conversion

### **Runtime Type Checking**
```cpp
// Type checking in GrapaRuleEvent
switch (e->mValue.mToken) {
case GrapaTokenType::PTR:
    // Handle pointer types
    FromGrapa(vScriptExec, e->vRulePointer, o);
    break;
case GrapaTokenType::STR:
    // Convert Grapa STR to Python string
    *o = py::str((char*)e->mValue.mBytes, e->mValue.mLength);
    break;
case GrapaTokenType::INT:
    // Convert Grapa INT to Python int
    a.FromBytes(e->mValue);
    *o = py::int_(a.LongValue());
    break;
case GrapaTokenType::FLOAT:
    // Convert Grapa FLOAT to Python float
    f.FromBytes(e->mValue);
    s = f.ToString();
    *o = py::float_(std::stod((char*)s.mBytes));
    break;
// ... other types
}
```

### **Type Conversion Patterns**
```cpp
// Automatic type conversion
GrapaCHAR str("123");
GrapaInt num;
num.FromString((char*)str.mBytes, 10);  // String to integer

// Explicit type conversion
GrapaFloat pi(3.14159);
GrapaCHAR pi_str = pi.ToString();       // Float to string

// Type safety checks
if (value.mToken == GrapaTokenType::INT) {
    // Safe to use as integer
    GrapaInt int_val;
    int_val.FromBytes(value);
}
```

## Serialization and Storage

### **Serialization Strategy**
- **Lists/Arrays/Objects**: Remain deserialized during runtime for performance
- **Primitive Types**: Serialized on-demand during use
- **Database Storage**: Native Grapa type support with automatic serialization

### **Binary Format**
```cpp
// GrapaFloat serialization format
struct GrapaFloatBinary {
    u8 header;        // [signed][extended][trunc][fix][exponent_bits]
    GrapaInt exponent; // Variable-length signed integer
    u64 max_bits;     // Maximum precision bits
    u64 extra_bits;   // Extra precision bits
    GrapaInt data;    // Actual numeric data
    GrapaBYTE tail;   // Future extensibility data
};
```

### **Database Integration**
```cpp
// Database type support
class GrapaDBField {
    u8 mType;         // GrapaTokenType value
    u64 mSize;        // Field size
    bool mNullable;   // Nullable flag
    // ... other metadata
};

// Type-aware database operations
GrapaError GrapaDB::CompareKey(s16 compareType, GrapaCursor& dataCursor, 
                               GrapaCursor& treeCursor, s8& result) {
    // Type-aware comparison based on field metadata
    switch (field.mType) {
    case GrapaTokenType::INT:
        return CompareInt(dataCursor, treeCursor, result);
    case GrapaTokenType::FLOAT:
        return CompareFloat(dataCursor, treeCursor, result);
    case GrapaTokenType::STR:
        return CompareString(dataCursor, treeCursor, result);
    // ... other types
    }
}
```

## Integration with Other Systems

### **With Execution System**
```cpp
// Type checking during execution
GrapaRuleEvent* GrapaScriptExec::EvalValue(GrapaRuleEvent* value) {
    switch (value->mValue.mToken) {
    case GrapaTokenType::INT:
        return EvalInt(value);
    case GrapaTokenType::FLOAT:
        return EvalFloat(value);
    case GrapaTokenType::STR:
        return EvalString(value);
    // ... other types
    }
}
```

### **With Python Integration**
```cpp
// Type mapping between Grapa and Python
static GrapaRuleEvent* ToGrapa(PyObject* o, GrapaCHAR pname) {
    if (PyLong_Check(o)) {
        // Python int → Grapa INT
        long long retvalue = PyLong_AsLongLong(o);
        pStr = GrapaInt(retvalue).getBytes();
        result = new GrapaRuleEvent(0, pname, pStr);
    }
    else if (PyFloat_Check(o)) {
        // Python float → Grapa FLOAT
        double retvalue = PyFloat_AS_DOUBLE(o);
        pStr = GrapaFloat(retvalue).getBytes();
        result = new GrapaRuleEvent(0, pname, pStr);
    }
    // ... other type mappings
}
```

### **With Database System**
```cpp
// Database field metadata lifecycle
class GrapaDBFieldMetadata {
    u8 mType;         // GrapaTokenType
    u64 mSize;        // Field size
    bool mNullable;   // Nullable flag
    GrapaCHAR mName;  // Field name
    
    // Type-specific operations
    GrapaError Serialize(GrapaBYTE& data);
    GrapaError Deserialize(const GrapaBYTE& data);
    GrapaError Compare(const GrapaBYTE& data1, const GrapaBYTE& data2);
};
```

## Performance Characteristics

### **Type Operations Performance**
- **Efficient Type Checking**: O(1) type token comparison
- **Optimized Conversions**: Minimal overhead for common conversions
- **Memory Management**: Automatic cleanup with reference counting
- **Serialization**: Efficient binary format for storage

### **Arithmetic Performance**
- **Unlimited Precision**: No overflow, dynamic memory allocation
- **High Precision**: 5-part float structure for accuracy
- **Optimized Algorithms**: Efficient algorithms for large numbers
- **Memory Efficiency**: Dynamic sizing based on precision needs

### **Database Performance**
- **Native Storage**: No type conversion overhead
- **Type Safety**: Compile-time and runtime type validation
- **Indexing**: Full support for type-based indexing
- **Query Optimization**: Type-aware query optimization

## Error Handling

### **Type Errors**
```cpp
// Type mismatch handling
if (expected_type != actual_type) {
    GrapaError error = GrapaError::TYPE_MISMATCH;
    error.SetMessage("Expected " + GetTypeName(expected_type) + 
                    ", got " + GetTypeName(actual_type));
    return error;
}

// Conversion error handling
try {
    GrapaInt result;
    result.FromString(input, radix);
} catch (const GrapaError& e) {
    // Handle conversion failure
    return GrapaError::CONVERSION_FAILED;
}
```

### **Precision Errors**
```cpp
// Precision limit handling
if (precision_exceeded) {
    GrapaFloat::SetTrunc(true);  // Set truncation flag
    // Continue with limited precision
}

// Memory allocation errors
if (memory_allocation_failed) {
    return GrapaError::OUT_OF_MEMORY;
}
```

## Security Considerations

### **Type Safety**
- **Input Validation**: All inputs validated against expected types
- **Conversion Safety**: Safe type conversions with error handling
- **Memory Safety**: Proper memory management and bounds checking
- **Database Safety**: Type-safe database operations

### **Precision Security**
- **Overflow Protection**: Unlimited precision prevents integer overflow
- **Precision Limits**: Configurable precision limits for performance
- **Memory Protection**: Dynamic memory allocation with bounds checking
- **Error Propagation**: Proper error handling throughout type system

## Future Enhancements

### **Planned Features**
- **Type Inference**: Advanced type inference capabilities
- **Generic Types**: Support for generic type parameters
- **Type Extensions**: User-defined type extensions
- **Type Optimization**: Further performance optimizations

### **Performance Improvements**
- **Type Caching**: Cache frequently used type information
- **Lazy Evaluation**: Lazy type checking for performance
- **Memory Pooling**: Memory pooling for type instances
- **Parallel Operations**: Parallel type operations where possible

## Related Documentation

- **[TYPE_DEFINITIONS_AND_CLASSIFICATIONS.md](TYPE_DEFINITIONS_AND_CLASSIFICATIONS.md)** - Complete type definitions
- **[SERIALIZATION_AND_DESERIALIZATION.md](SERIALIZATION_AND_DESERIALIZATION.md)** - Serialization details
- **[TYPE_CHECKING_AND_CONVERSION.md](TYPE_CHECKING_AND_CONVERSION.md)** - Type checking mechanisms
- **[../CORE/BASETYPES.md](../CORE/BASETYPES.md)** - Base type system documentation
- **[../SYSTEM_COMPONENTS/GRAPAFLOAT_IMPLEMENTATION.md](../SYSTEM_COMPONENTS/GRAPAFLOAT_IMPLEMENTATION.md)** - Float implementation
- **[../DATABASE/GRAPA_DB_IMPLEMENTATION.md](../DATABASE/GRAPA_DB_IMPLEMENTATION.md)** - Database type integration
