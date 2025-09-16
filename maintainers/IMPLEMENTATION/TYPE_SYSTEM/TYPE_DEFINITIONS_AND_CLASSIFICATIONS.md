---
tags:
  - maintainer
  - implementation
  - type-system
  - type-definitions
  - classifications
---

# Type Definitions and Classifications

This document provides comprehensive implementation details for all Grapa types, including their C++ implementations, Grapa language mappings, and usage patterns.

## Overview

**Total Types:** 32+ built-in types with comprehensive implementations
**Coverage:** Complete type system with detailed implementation documentation
**Status:** All types documented with implementation details

## Complete Type Enumeration

### **GrapaTokenType Enumeration**
```cpp
class GrapaTokenType {
public: enum {
    START = 0,         // Start marker
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

## Type Classifications

### **1. Primitive Types**
Types that represent fundamental data values.

#### **ERR (Error Type)**
- **Token:** `GrapaTokenType::ERR = 1`
- **Purpose:** Error values and error handling
- **Implementation:** Error state representation
- **Usage:** Error propagation and handling

#### **RAW (Raw Binary Data)**
- **Token:** `GrapaTokenType::RAW = 2`
- **Purpose:** Raw binary data storage
- **Implementation:** `GrapaBYTE` class
- **Usage:** Binary data manipulation, file I/O

#### **BOOL (Boolean Values)**
- **Token:** `GrapaTokenType::BOOL = 3`
- **Purpose:** True/false values
- **Implementation:** `GrapaCHAR` with boolean conversion
- **Usage:** Conditional logic, flags

```cpp
// Boolean implementation via GrapaCHAR
static GrapaCHAR SetBool(bool isTrue) { 
    GrapaCHAR r("\0"); 
    if (isTrue) r.FROM("\1");  
    r.mToken = GrapaTokenType::BOOL; 
    return(r); 
};
```

#### **ID (Identifier)**
- **Token:** `GrapaTokenType::ID = 4`
- **Purpose:** Variable and function identifiers
- **Implementation:** String-based identifier system
- **Usage:** Variable names, function names, symbols

#### **STR (String)**
- **Token:** `GrapaTokenType::STR = 5`
- **Purpose:** Text data
- **Implementation:** `GrapaCHAR` class
- **Usage:** Text manipulation, I/O, data representation

```cpp
class GrapaCHAR : public GrapaBYTE
{
public:
    // String operations
    virtual void FROM(const char *pStr);
    virtual void FROM(const char* pStr, u64 pLen);
    virtual void Append(const char* pStr);
    virtual void ToUpper();
    virtual void ToLower();
    virtual void Trim(const char c = ' ');
    virtual s64 StrCmp(const char* pStr);
    // ... extensive string manipulation methods
};
```

#### **INPUT (Input Stream)**
- **Token:** `GrapaTokenType::INPUT = 6`
- **Purpose:** Input stream handling
- **Implementation:** Stream-based input processing
- **Usage:** File input, network input, user input

#### **INT (Integer - Unlimited Precision)**
- **Token:** `GrapaTokenType::INT = 8`
- **Purpose:** Unlimited precision integers
- **Implementation:** `GrapaInt` class using `GrapaArray32`
- **Usage:** Mathematical calculations, counting, indexing

```cpp
class GrapaInt : public GrapaArray32, public GrapaCritical
{
public:
    // Unlimited precision arithmetic
    GrapaInt operator +(const GrapaInt& bi2) const;
    GrapaInt operator *(const GrapaInt& bi2) const;
    GrapaInt operator /(const GrapaInt& bi2);
    GrapaInt operator %(const GrapaInt& bi2);
    // ... full arithmetic operation set
};
```

#### **FLOAT (Float - High Precision)**
- **Token:** `GrapaTokenType::FLOAT = 9`
- **Purpose:** High-precision floating-point numbers
- **Implementation:** `GrapaFloat` class with 5-part structure
- **Usage:** Scientific calculations, financial calculations

```cpp
class GrapaFloat
{
public:
    // 5-part structure
    bool mSigned;    // Sign bit
    bool mTrunc;     // Truncation flag
    bool mFix;       // Fixed-point flag
    u64 mExp;        // Exponent
    u64 mMax;        // Maximum precision bits
    u64 mExtra;      // Extra precision bits
    GrapaInt mData;  // Actual numeric data
    bool mNaN;       // Not-a-Number flag
};
```

### **2. Data Types**
Types for structured data representation.

#### **XML (XML Data)**
- **Token:** `GrapaTokenType::XML = 7`
- **Purpose:** XML document and element handling
- **Implementation:** XML parsing and manipulation system
- **Usage:** XML document processing, web services, data exchange

```cpp
// XML handling in GrapaLibRule
GrapaLibraryEvent* HandleCreateXml(GrapaCHAR& pName);
GrapaLibraryEvent* HandleXml(GrapaCHAR& pName);
```

**XML Features:**
- **Document Parsing:** Parse XML documents into Grapa structures
- **Element Manipulation:** Create, modify, and delete XML elements
- **Attribute Handling:** XML attribute management
- **XPath Support:** XML path querying capabilities
- **Serialization:** XML document serialization and deserialization

#### **TIME (Time - Uses Unlimited Precision INT)**
- **Token:** `GrapaTokenType::TIME = 12`
- **Purpose:** Time representation with unlimited precision
- **Implementation:** `GrapaTime` class using `GrapaInt` for seconds
- **Usage:** Timestamp handling, date/time calculations

```cpp
class GrapaTime : public GrapaValue
{
public:
    GrapaInt mSec;  // Seconds using unlimited precision INT
    
    // Time operations
    static GrapaTime Now();
    virtual void SetNow();
    virtual void ToBytes(GrapaCHAR& pChar);
    virtual void FromBytes(const GrapaBYTE& pChar);
    virtual void ToString(GrapaCHAR& pChar);
    virtual void FromString(const GrapaCHAR& pChar);
    
    // Date calculations
    static bool isLeap(s64 y);
    static s64 offsetDays(s64 d, s64 m, s64 y);
    static void revoffsetDays(s64 offset, s64 y, s64* d, s64* m);
    static void addDays(s64 d1, s64 m1, s64 y1, s64 x, s64& d2, s64& m2, s64& y2, s64& x2);
    
    // Utility methods
    GrapaCHAR getBytes();
    GrapaCHAR getString();
    virtual s64 LocalOffset();
};
```

**Time Features:**
- **Unlimited Precision:** Uses `GrapaInt` for second representation
- **Date Calculations:** Leap year, day offset calculations
- **Time Zones:** Local offset handling
- **Serialization:** Binary and string serialization
- **Format Conversion:** Multiple time format support

#### **TABLE (Table)**
- **Token:** `GrapaTokenType::TABLE = 17`
- **Purpose:** Database table and record management
- **Implementation:** `GrapaDB` system with table structures
- **Usage:** Database operations, data storage, record management

```cpp
// Table implementation via GrapaDB
class GrapaDBTable
{
public:
    u64 mTableId;           // Table identifier
    u8 mTreeType;           // Storage type (ROW, COL, GROUP)
    GrapaDBFieldArray mFields;  // Field definitions
    GrapaDBIndexArray mIndexes; // Index definitions
    // ... table management methods
};
```

**Table Features:**
- **Schema Management:** Field definitions and metadata
- **Indexing:** Multiple index types and strategies
- **Storage Types:** ROW, COL, GROUP storage formats
- **Query Operations:** Complex query support
- **Transaction Support:** ACID transaction handling

#### **EL (Element)**
- **Token:** `GrapaTokenType::EL = 15`
- **Purpose:** XML/HTML element representation
- **Implementation:** Element node system
- **Usage:** XML/HTML parsing, DOM manipulation

#### **TAG (Tag)**
- **Token:** `GrapaTokenType::TAG = 16`
- **Purpose:** XML/HTML tag handling
- **Implementation:** Tag parsing and manipulation
- **Usage:** Markup language processing

### **3. Collection Types**
Types for managing collections of data.

#### **ARRAY (Array)**
- **Token:** `GrapaTokenType::ARRAY = 13`
- **Purpose:** Fixed-size array collections
- **Implementation:** `GrapaArray64` and `GrapaVoidArray` classes
- **Usage:** Indexed data storage, performance-critical operations

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

class GrapaVoidArray : public GrapaBYTE
{
public:
    // Generic array operations
    virtual void Init(u32 pCount=0);
    virtual void Append(void *pItem);
    virtual u32 Count();
    void* GetAt(u32 i);
};
```

**Array Features:**
- **Fixed Size:** Pre-allocated memory for performance
- **Type Safety:** Type checking for array operations
- **Indexed Access:** O(1) random access
- **Memory Efficiency:** Optimized memory layout

#### **LIST (List)**
- **Token:** `GrapaTokenType::LIST = 14`
- **Purpose:** Dynamic list collections
- **Implementation:** Linked list or dynamic array
- **Usage:** Dynamic data storage, sequential access

**List Features:**
- **Dynamic Sizing:** Grows and shrinks as needed
- **Sequential Access:** Optimized for sequential operations
- **Insertion/Deletion:** Efficient at ends, O(n) in middle
- **Memory Management:** Automatic memory allocation

#### **VECTOR (Vector)**
- **Token:** `GrapaTokenType::VECTOR = 23`
- **Purpose:** Mathematical vector operations
- **Implementation:** Vector mathematics system
- **Usage:** Mathematical calculations, graphics, physics

#### **TUPLE (Tuple)**
- **Token:** `GrapaTokenType::TUPLE = 30`
- **Purpose:** Fixed-size ordered collections
- **Implementation:** Tuple data structure
- **Usage:** Multi-value returns, structured data

### **4. System Types**
Types for system-level operations.

#### **SYM (Symbol)**
- **Token:** `GrapaTokenType::SYM = 10`
- **Purpose:** Symbol table entries
- **Implementation:** Symbol management system
- **Usage:** Variable resolution, function lookup

#### **SYSSYM (System Symbol)**
- **Token:** `GrapaTokenType::SYSSYM = 11`
- **Purpose:** System-level symbols
- **Implementation:** System symbol table
- **Usage:** System function calls, built-in operations

#### **SYSID (System ID)**
- **Token:** `GrapaTokenType::SYSID = 18`
- **Purpose:** System identifier management
- **Implementation:** System ID allocation
- **Usage:** Object identification, system references

#### **SYSSTR (System String)**
- **Token:** `GrapaTokenType::SYSSTR = 19`
- **Purpose:** System string management
- **Implementation:** System string pool
- **Usage:** String interning, system text

#### **SYSINT (System Integer)**
- **Token:** `GrapaTokenType::SYSINT = 20`
- **Purpose:** System integer operations
- **Implementation:** System integer management
- **Usage:** System counters, internal calculations

### **5. Execution Types**
Types for code execution and control flow.

#### **OP (Operation)**
- **Token:** `GrapaTokenType::OP = 21`
- **Purpose:** Single operation execution
- **Implementation:** `GrapaRuleEvent` with operation data
- **Usage:** Function calls, method execution

```cpp
// Operation execution tree format
@<[op,body],params>
// Example: @<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}>
```

#### **CODE (Code Block)**
- **Token:** `GrapaTokenType::CODE = 22`
- **Purpose:** Code block execution
- **Implementation:** Code block management
- **Usage:** Script execution, code compilation

#### **RULE (Rule)**
- **Token:** `GrapaTokenType::RULE = 27`
- **Purpose:** Rule-based execution
- **Implementation:** Rule engine system
- **Usage:** Pattern matching, rule processing

#### **TOKEN (Token)**
- **Token:** `GrapaTokenType::TOKEN = 28`
- **Purpose:** Lexical token representation
- **Implementation:** Token management system
- **Usage:** Parsing, lexical analysis

### **6. Object-Oriented Types**
Types for object-oriented programming.

#### **CLASS (Class)**
- **Token:** `GrapaTokenType::CLASS = 24`
- **Purpose:** Class definition and instantiation
- **Implementation:** Class management system
- **Usage:** Object-oriented programming, type definitions

#### **OBJ (Object)**
- **Token:** `GrapaTokenType::OBJ = 25`
- **Purpose:** Object instances
- **Implementation:** Object management system
- **Usage:** Object-oriented programming, data encapsulation

#### **REF (Reference)**
- **Token:** `GrapaTokenType::REF = 26`
- **Purpose:** Reference to objects
- **Implementation:** Reference counting system
- **Usage:** Object references, memory management

### **7. UI Types**
Types for user interface components.

#### **WIDGET (Widget)**
- **Token:** `GrapaTokenType::WIDGET = 29`
- **Purpose:** User interface widgets
- **Implementation:** `GrapaWidget` class
- **Usage:** GUI development, user interface

```cpp
class GrapaWidget
{
public:
    // Widget management
    virtual void Create();
    virtual void Destroy();
    virtual void Show();
    virtual void Hide();
    virtual void Update();
    // ... widget-specific methods
};
```

**Widget Features:**
- **Cross-platform:** Platform-independent widget system
- **Event Handling:** User interaction event processing
- **Layout Management:** Automatic layout and positioning
- **Theme Support:** Customizable appearance

### **8. Documentation Types**
Types for documentation and comments.

#### **COMMENT (Comment)**
- **Token:** `GrapaTokenType::COMMENT = 31`
- **Purpose:** Code comments and documentation
- **Implementation:** Comment processing system
- **Usage:** Code documentation, documentation generation

#### **DOC (Documentation)**
- **Token:** `GrapaTokenType::DOC = 32`
- **Purpose:** Documentation blocks
- **Implementation:** Documentation processing
- **Usage:** API documentation, help system

### **9. Special Types**
Types for special purposes and extensions.

#### **X1 (Extension Point)**
- **Token:** `GrapaTokenType::X1 = 128`
- **Purpose:** Extension point for custom types
- **Implementation:** Extension system
- **Usage:** Custom type development, language extensions

#### **OTHER (Other Types)**
- **Token:** `GrapaTokenType::OTHER`
- **Purpose:** Miscellaneous types
- **Implementation:** Generic type handling
- **Usage:** Unknown types, fallback handling

#### **PTR (Pointer)**
- **Token:** `GrapaTokenType::PTR`
- **Purpose:** Pointer references
- **Implementation:** Pointer management system
- **Usage:** Memory references, indirection

#### **RULEREF (Rule Reference)**
- **Token:** `GrapaTokenType::RULEREF`
- **Purpose:** References to rules
- **Implementation:** Rule reference system
- **Usage:** Rule chaining, rule composition

#### **RULEOP (Rule Operation)**
- **Token:** `GrapaTokenType::RULEOP`
- **Purpose:** Rule operations
- **Implementation:** Rule operation system
- **Usage:** Rule execution, rule composition

## Type Relationships

### **Inheritance Hierarchy**
```
GrapaValue (Base)
├── GrapaBYTE (Binary Data)
│   ├── GrapaCHAR (Strings)
│   ├── GrapaArray64 (64-bit Arrays)
│   └── GrapaVoidArray (Generic Arrays)
│       ├── GrapaCHARArray (String Arrays)
│       └── GrapaDU64Array (64-bit Arrays)
├── GrapaSS64 (Signed 64-bit)
├── GrapaSU64 (Unsigned 64-bit)
├── GrapaDU64 (Double Unsigned 64-bit)
├── GrapaInt (Unlimited Precision)
├── GrapaFloat (High Precision)
├── GrapaTime (Time with Unlimited Precision)
└── GrapaEvent (Event System)
    └── GrapaWorkEvent (Work Events)
```

### **Type Conversion Matrix**
```
From\To    STR     INT     FLOAT   TIME    ARRAY   LIST    TABLE   XML
STR        -       ✓       ✓       ✓       ✗       ✗       ✗       ✗
INT        ✓       -       ✓       ✓       ✗       ✗       ✗       ✗
FLOAT      ✓       ✓       -       ✗       ✗       ✗       ✗       ✗
TIME       ✓       ✓       ✗       -       ✗       ✗       ✗       ✗
ARRAY      ✗       ✗       ✗       ✗       -       ✓       ✗       ✗
LIST       ✗       ✗       ✗       ✗       ✓       -       ✗       ✗
TABLE      ✗       ✗       ✗       ✗       ✗       ✗       -       ✗
XML        ✗       ✗       ✗       ✗       ✗       ✗       ✗       -
```

## Type Usage Patterns

### **Common Type Combinations**
```grapa
/* Database operations with multiple types */
table = $TABLE().create("users", {
    "id": $INT,           // Unlimited precision integer
    "name": $STR,         // String
    "balance": $FLOAT,    // High precision float
    "created": $TIME,     // Time with unlimited precision
    "data": $XML          // XML data
});

/* Collection operations */
numbers = $LIST().create(100);  // Fixed-size array
names = $GOBJ().create();        // Dynamic list
config = $XML().parse(xml_data); // XML parsing

/* Time operations with unlimited precision */
current_time = $TIME().utc();    // Current time
future_time = current_time + 86400; // Add seconds (unlimited precision)
```

### **Type-Specific Operations**
```grapa
/* Integer operations (unlimited precision) */
large_number = 123456789012345678901234567890;
result = large_number * large_number;  // No overflow

/* Float operations (high precision) */
pi = 3.141592653589793238462643383279;
area = pi * radius * radius;  // High precision calculation

/* Time operations */
now = $TIME().utc();
tomorrow = now + 86400;  // Add seconds (unlimited precision)
formatted = now.toString("YYYY-MM-DD HH:mm:ss");

/* XML operations */
xml_doc = $XML().parse(xml_string);
elements = xml_doc.find("//user");
for (element in elements) {
    name = element.get("name");
    age = element.get("age").int();
}
```

## Performance Characteristics

### **Type Performance Summary**
| Type | Memory | Access | Conversion | Serialization |
|------|--------|--------|------------|---------------|
| STR | O(n) | O(1) | Fast | Fast |
| INT | Dynamic | O(1) | Fast | Medium |
| FLOAT | Fixed | O(1) | Medium | Medium |
| TIME | Dynamic | O(1) | Fast | Fast |
| ARRAY | Fixed | O(1) | Slow | Fast |
| LIST | Dynamic | O(n) | Slow | Fast |
| TABLE | Dynamic | O(log n) | Slow | Medium |
| XML | Dynamic | O(n) | Slow | Slow |

### **Memory Usage Patterns**
- **Primitive Types**: Fixed or predictable memory usage
- **Collection Types**: Dynamic memory allocation
- **Complex Types**: Variable memory usage based on content
- **System Types**: Minimal memory overhead

## Error Handling

### **Type-Specific Errors**
```grapa
/* Type conversion errors */
try {
    result = "not_a_number".int();
} catch (error) {
    // Handle conversion failure
}

/* Precision errors */
if (float_value.trunc()) {
    // Handle precision loss
}

/* Memory errors */
if (large_array.memory_error()) {
    // Handle memory allocation failure
}
```

### **Type Safety**
- **Compile-time checking**: Type validation during compilation
- **Runtime checking**: Type validation during execution
- **Conversion safety**: Safe type conversions with error handling
- **Memory safety**: Proper memory management and cleanup

## Future Enhancements

### **Planned Type Improvements**
- **Generic Types**: Support for generic type parameters
- **Type Inference**: Advanced type inference capabilities
- **Type Extensions**: User-defined type extensions
- **Type Optimization**: Further performance optimizations

### **New Type Proposals**
- **Complex Numbers**: Mathematical complex number support
- **Decimal Types**: Fixed-point decimal arithmetic
- **UUID Types**: Universally unique identifier support
- **JSON Types**: Native JSON data type support

## Related Documentation

- **[TYPE_SYSTEM_ARCHITECTURE.md](TYPE_SYSTEM_ARCHITECTURE.md)** - Overall type system architecture
- **[SERIALIZATION_AND_DESERIALIZATION.md](SERIALIZATION_AND_DESERIALIZATION.md)** - Type serialization details
- **[TYPE_CHECKING_AND_CONVERSION.md](TYPE_CHECKING_AND_CONVERSION.md)** - Type checking mechanisms
- **[../CORE/BASETYPES.md](../CORE/BASETYPES.md)** - Base type system documentation
- **[../SYSTEM_COMPONENTS/GRAPAFLOAT_IMPLEMENTATION.md](../SYSTEM_COMPONENTS/GRAPAFLOAT_IMPLEMENTATION.md)** - Float implementation
- **[../DATABASE/GRAPA_DB_IMPLEMENTATION.md](../DATABASE/GRAPA_DB_IMPLEMENTATION.md)** - Database type integration
