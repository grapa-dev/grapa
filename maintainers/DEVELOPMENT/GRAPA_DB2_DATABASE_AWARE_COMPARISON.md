# GrapaDB2 Database-Aware Comparison System

## Problem Statement

### **Current Limitations**
The existing GrapaDB comparison system has several critical limitations:

1. **Type-Unaware Comparison**: Uses `strcmp` for all field comparisons, causing incorrect results for numeric, date, and other non-string types
2. **Tree-Type Blindness**: Doesn't account for different storage models (ROW, COL, GROUP)
3. **Storage-Type Ignorance**: Doesn't handle different storage types (FIX, VAR, PAR) appropriately
4. **Complex Pointer Logic**: Requires complex `PtrToRec` dereferencing for pointer types
5. **Limited Extensibility**: Hard to extend for new data types or comparison logic

### **Impact on GrapaDB2**
For GrapaDB2 to be truly robust and performant, it needs a **database-aware comparison system** that understands:
- **Data Types**: INT, FLOAT, STR, TIME, BOOL, etc.
- **Tree Types**: ROW, COL, GROUP storage models
- **Storage Types**: FIX, VAR, PAR storage strategies
- **Index Types**: Single-field, multi-field, composite indexes
- **Comparison Modes**: Search, insert, delete, update operations

## Solution: Database-Aware Comparison Architecture

### **1. Enhanced Comparison Hierarchy**

#### **Base Comparison Interface**
```cpp
class GrapaDB2Comparison {
public:
    enum ComparisonMode { SEARCH, INSERT, DELETE, UPDATE };
    enum ComparisonResult { LESS_THAN = -1, EQUAL = 0, GREATER_THAN = 1 };
    
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) = 0;
    virtual bool SupportsDataType(u8 dataType) const = 0;
    virtual bool SupportsTreeType(u8 treeType) const = 0;
    virtual bool SupportsStorageType(u8 storageType) const = 0;
};
```

#### **Comparison Context**
```cpp
struct GrapaDB2ComparisonContext {
    // Operation context
    ComparisonMode mode;
    GrapaDB2Table* table;
    GrapaDB2Index* index;
    
    // Data to compare
    GrapaDB2FieldValueArray& leftValues;
    GrapaDB2FieldValueArray& rightValues;
    
    // Field metadata
    GrapaDB2FieldArray& fields;
    GrapaDU64Array& fieldOrder;  // For composite indexes
    
    // Storage context
    u8 treeType;      // ROW, COL, GROUP
    u8 storageType;   // FIX, VAR, PAR
    
    // Performance hints
    bool useCache;
    bool optimizeForRange;
};
```

### **2. Comprehensive Grapa Data Type Comparison**

#### **All Grapa Data Types Support**
Based on `GrapaTokenType` enum, GrapaDB2 will support comparison for **all Grapa data types**:

```cpp
enum GrapaDB2FieldType {
    // Primitive Types
    FIELD_TYPE_ERR = GrapaTokenType::ERR,           // Error values
    FIELD_TYPE_RAW = GrapaTokenType::RAW,           // Raw binary data
    FIELD_TYPE_BOOL = GrapaTokenType::BOOL,         // Boolean values
    FIELD_TYPE_ID = GrapaTokenType::ID,             // Identifier values
    FIELD_TYPE_STR = GrapaTokenType::STR,           // String values (Unicode)
    FIELD_TYPE_INT = GrapaTokenType::INT,           // Integer values (64-bit)
    FIELD_TYPE_FLOAT = GrapaTokenType::FLOAT,       // Floating-point values
    FIELD_TYPE_TIME = GrapaTokenType::TIME,         // Time values
    FIELD_TYPE_SYSID = GrapaTokenType::SYSID,       // System identifiers
    FIELD_TYPE_SYSSTR = GrapaTokenType::SYSSTR,     // System strings
    FIELD_TYPE_SYSINT = GrapaTokenType::SYSINT,     // System integers
    
    // Composite Types
    FIELD_TYPE_ARRAY = GrapaTokenType::ARRAY,       // Dynamic arrays
    FIELD_TYPE_LIST = GrapaTokenType::LIST,         // Linked lists
    FIELD_TYPE_VECTOR = GrapaTokenType::VECTOR,     // Mathematical vectors
    FIELD_TYPE_TABLE = GrapaTokenType::TABLE,       // Database tables
    FIELD_TYPE_XML = GrapaTokenType::XML,           // XML documents
    FIELD_TYPE_EL = GrapaTokenType::EL,             // XML elements
    FIELD_TYPE_TAG = GrapaTokenType::TAG,           // Tagged unions
    FIELD_TYPE_TUPLE = GrapaTokenType::TUPLE,       // Tuples
    FIELD_TYPE_WIDGET = GrapaTokenType::WIDGET,     // UI widgets
    
    // System Types
    FIELD_TYPE_OP = GrapaTokenType::OP,             // Operations/functions
    FIELD_TYPE_CODE = GrapaTokenType::CODE,         // Code blocks
    FIELD_TYPE_CLASS = GrapaTokenType::CLASS,       // Class definitions
    FIELD_TYPE_OBJ = GrapaTokenType::OBJ,           // Object instances
    FIELD_TYPE_REF = GrapaTokenType::REF,           // References
    FIELD_TYPE_RULE = GrapaTokenType::RULE,         // Rule definitions
    FIELD_TYPE_TOKEN = GrapaTokenType::TOKEN,       // Tokens
    FIELD_TYPE_RULEREF = GrapaTokenType::RULEREF,   // Rule references
    FIELD_TYPE_RULEOP = GrapaTokenType::RULEOP,     // Rule operations
    FIELD_TYPE_PTR = GrapaTokenType::PTR,           // Pointers
    FIELD_TYPE_OTHER = GrapaTokenType::OTHER,       // Other types
    FIELD_TYPE_MAX = GrapaTokenType::MAX            // Maximum type value
};
```

#### **Comprehensive Type-Aware Comparison**
```cpp
class GrapaDB2ComprehensiveComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        for (u32 i = 0; i < context.fieldOrder.Count(); i++) {
            u64 fieldId = context.fieldOrder.GetAt(i);
            GrapaDB2Field& field = context.fields.GetFieldById(fieldId);
            
            GrapaValue leftValue = context.leftValues.GetValue(fieldId);
            GrapaValue rightValue = context.rightValues.GetValue(fieldId);
            
            // Handle all Grapa data types
            switch (field.mFieldType) {
                // Primitive Types
                case FIELD_TYPE_BOOL:
                    result = CompareBool(leftValue, rightValue);
                    break;
                case FIELD_TYPE_INT:
                case FIELD_TYPE_SYSINT:
                    result = CompareInt(leftValue, rightValue);
                    break;
                case FIELD_TYPE_FLOAT:
                    result = CompareFloat(leftValue, rightValue);
                    break;
                case FIELD_TYPE_STR:
                case FIELD_TYPE_SYSSTR:
                    result = CompareString(leftValue, rightValue, field.mCollation);
                    break;
                case FIELD_TYPE_TIME:
                    result = CompareTime(leftValue, rightValue);
                    break;
                case FIELD_TYPE_ID:
                case FIELD_TYPE_SYSID:
                    result = CompareId(leftValue, rightValue);
                    break;
                case FIELD_TYPE_RAW:
                    result = CompareRaw(leftValue, rightValue);
                    break;
                case FIELD_TYPE_ERR:
                    result = CompareError(leftValue, rightValue);
                    break;
                
                // Composite Types
                case FIELD_TYPE_ARRAY:
                    result = CompareArray(leftValue, rightValue);
                    break;
                case FIELD_TYPE_LIST:
                    result = CompareList(leftValue, rightValue);
                    break;
                case FIELD_TYPE_VECTOR:
                    result = CompareVector(leftValue, rightValue);
                    break;
                case FIELD_TYPE_TABLE:
                    result = CompareTable(leftValue, rightValue);
                    break;
                case FIELD_TYPE_XML:
                    result = CompareXml(leftValue, rightValue);
                    break;
                case FIELD_TYPE_EL:
                    result = CompareElement(leftValue, rightValue);
                    break;
                case FIELD_TYPE_TAG:
                    result = CompareTag(leftValue, rightValue);
                    break;
                case FIELD_TYPE_TUPLE:
                    result = CompareTuple(leftValue, rightValue);
                    break;
                case FIELD_TYPE_WIDGET:
                    result = CompareWidget(leftValue, rightValue);
                    break;
                
                // System Types
                case FIELD_TYPE_OP:
                    result = CompareOperation(leftValue, rightValue);
                    break;
                case FIELD_TYPE_CODE:
                    result = CompareCode(leftValue, rightValue);
                    break;
                case FIELD_TYPE_CLASS:
                    result = CompareClass(leftValue, rightValue);
                    break;
                case FIELD_TYPE_OBJ:
                    result = CompareObject(leftValue, rightValue);
                    break;
                case FIELD_TYPE_REF:
                    result = CompareReference(leftValue, rightValue);
                    break;
                case FIELD_TYPE_RULE:
                    result = CompareRule(leftValue, rightValue);
                    break;
                case FIELD_TYPE_TOKEN:
                    result = CompareToken(leftValue, rightValue);
                    break;
                
                default:
                    return -1; // Unsupported type
            }
            
            if (result != EQUAL) return 0; // Found difference
        }
        return 0;
    }
    
private:
    ComparisonResult CompareInt(const GrapaValue& left, const GrapaValue& right) {
        s64 leftVal = left.raw().int64();
        s64 rightVal = right.raw().int64();
        if (leftVal < rightVal) return LESS_THAN;
        if (leftVal > rightVal) return GREATER_THAN;
        return EQUAL;
    }
    
    ComparisonResult CompareFloat(const GrapaValue& left, const GrapaValue& right) {
        double leftVal = left.raw().float64();
        double rightVal = right.raw().float64();
        if (leftVal < rightVal) return LESS_THAN;
        if (leftVal > rightVal) return GREATER_THAN;
        return EQUAL;
    }
    
    ComparisonResult CompareTime(const GrapaValue& left, const GrapaValue& right) {
        u64 leftTime = left.raw().int64();
        u64 rightTime = right.raw().int64();
        if (leftTime < rightTime) return LESS_THAN;
        if (leftTime > rightTime) return GREATER_THAN;
        return EQUAL;
    }
    
    // Primitive Type Comparisons
    ComparisonResult CompareBool(const GrapaValue& left, const GrapaValue& right) {
        bool leftBool = left.bool();
        bool rightBool = right.bool();
        if (leftBool < rightBool) return LESS_THAN;
        if (leftBool > rightBool) return GREATER_THAN;
        return EQUAL;
    }
    
    ComparisonResult CompareId(const GrapaValue& left, const GrapaValue& right) {
        // ID comparison (case-sensitive string comparison)
        GrapaCHAR leftId = left.str();
        GrapaCHAR rightId = right.str();
        s64 cmp = leftId.Cmp(rightId);
        if (cmp < 0) return LESS_THAN;
        if (cmp > 0) return GREATER_THAN;
        return EQUAL;
    }
    
    ComparisonResult CompareRaw(const GrapaValue& left, const GrapaValue& right) {
        // Raw binary data comparison
        GrapaBYTE leftRaw = left.raw();
        GrapaBYTE rightRaw = right.raw();
        
        // Compare lengths first
        if (leftRaw.mLength < rightRaw.mLength) return LESS_THAN;
        if (leftRaw.mLength > rightRaw.mLength) return GREATER_THAN;
        
        // Compare content
        s64 cmp = memcmp(leftRaw.mBytes, rightRaw.mBytes, leftRaw.mLength);
        if (cmp < 0) return LESS_THAN;
        if (cmp > 0) return GREATER_THAN;
        return EQUAL;
    }
    
    ComparisonResult CompareError(const GrapaValue& left, const GrapaValue& right) {
        // Error comparison (compare error codes/messages)
        GrapaCHAR leftErr = left.str();
        GrapaCHAR rightErr = right.str();
        s64 cmp = leftErr.Cmp(rightErr);
        if (cmp < 0) return LESS_THAN;
        if (cmp > 0) return GREATER_THAN;
        return EQUAL;
    }
    
    // Composite Type Comparisons
    ComparisonResult CompareArray(const GrapaValue& left, const GrapaValue& right) {
        // Array comparison (element-wise)
        GrapaArray<GrapaValue> leftArray = left.array();
        GrapaArray<GrapaValue> rightArray = right.array();
        
        u32 leftLen = leftArray.Count();
        u32 rightLen = rightArray.Count();
        
        // Compare lengths first
        if (leftLen < rightLen) return LESS_THAN;
        if (leftLen > rightLen) return GREATER_THAN;
        
        // Compare elements
        for (u32 i = 0; i < leftLen; i++) {
            GrapaValue leftElem = leftArray.GetAt(i);
            GrapaValue rightElem = rightArray.GetAt(i);
            
            // Recursive comparison for array elements
            ComparisonResult elemResult = CompareValue(leftElem, rightElem);
            if (elemResult != EQUAL) return elemResult;
        }
        return EQUAL;
    }
    
    ComparisonResult CompareList(const GrapaValue& left, const GrapaValue& right) {
        // List comparison (element-wise)
        GrapaList<GrapaValue> leftList = left.list();
        GrapaList<GrapaValue> rightList = right.list();
        
        u32 leftLen = leftList.Count();
        u32 rightLen = rightList.Count();
        
        // Compare lengths first
        if (leftLen < rightLen) return LESS_THAN;
        if (leftLen > rightLen) return GREATER_THAN;
        
        // Compare elements
        for (u32 i = 0; i < leftLen; i++) {
            GrapaValue leftElem = leftList.GetAt(i);
            GrapaValue rightElem = rightList.GetAt(i);
            
            // Recursive comparison for list elements
            ComparisonResult elemResult = CompareValue(leftElem, rightElem);
            if (elemResult != EQUAL) return elemResult;
        }
        return EQUAL;
    }
    
    ComparisonResult CompareVector(const GrapaValue& left, const GrapaValue& right) {
        // Vector comparison (mathematical vector comparison)
        GrapaVector leftVec = left.vector();
        GrapaVector rightVec = right.vector();
        
        // Compare dimensions first
        u32 leftDim = leftVec.GetDimension();
        u32 rightDim = rightVec.GetDimension();
        
        if (leftDim < rightDim) return LESS_THAN;
        if (leftDim > rightDim) return GREATER_THAN;
        
        // Compare elements (mathematical vector comparison)
        for (u32 i = 0; i < leftDim; i++) {
            GrapaValue leftElem = leftVec.GetElement(i);
            GrapaValue rightElem = rightVec.GetElement(i);
            
            ComparisonResult elemResult = CompareValue(leftElem, rightElem);
            if (elemResult != EQUAL) return elemResult;
        }
        return EQUAL;
    }
    
    ComparisonResult CompareTable(const GrapaValue& left, const GrapaValue& right) {
        // Table comparison (compare table references or content)
        GrapaTable leftTable = left.table();
        GrapaTable rightTable = right.table();
        
        // Compare table references first
        u64 leftRef = leftTable.GetReference();
        u64 rightRef = rightTable.GetReference();
        
        if (leftRef < rightRef) return LESS_THAN;
        if (leftRef > rightRef) return GREATER_THAN;
        
        // If same reference, compare content
        return CompareTableContent(leftTable, rightTable);
    }
    
    ComparisonResult CompareXml(const GrapaValue& left, const GrapaValue& right) {
        // XML comparison (canonical XML comparison)
        GrapaXML leftXml = left.xml();
        GrapaXML rightXml = right.xml();
        
        // Convert to canonical form and compare
        GrapaCHAR leftCanonical = leftXml.ToCanonical();
        GrapaCHAR rightCanonical = rightXml.ToCanonical();
        
        s64 cmp = leftCanonical.Cmp(rightCanonical);
        if (cmp < 0) return LESS_THAN;
        if (cmp > 0) return GREATER_THAN;
        return EQUAL;
    }
    
    ComparisonResult CompareElement(const GrapaValue& left, const GrapaValue& right) {
        // XML element comparison
        GrapaElement leftEl = left.element();
        GrapaElement rightEl = right.element();
        
        // Compare tag names first
        GrapaCHAR leftTag = leftEl.GetTagName();
        GrapaCHAR rightTag = rightEl.GetTagName();
        
        s64 tagCmp = leftTag.Cmp(rightTag);
        if (tagCmp != 0) {
            if (tagCmp < 0) return LESS_THAN;
            return GREATER_THAN;
        }
        
        // Compare attributes
        ComparisonResult attrResult = CompareAttributes(leftEl, rightEl);
        if (attrResult != EQUAL) return attrResult;
        
        // Compare content
        return CompareElementContent(leftEl, rightEl);
    }
    
    ComparisonResult CompareTag(const GrapaValue& left, const GrapaValue& right) {
        // Tagged union comparison
        GrapaTag leftTag = left.tag();
        GrapaTag rightTag = right.tag();
        
        // Compare tag types first
        u8 leftType = leftTag.GetType();
        u8 rightType = rightTag.GetType();
        
        if (leftType < rightType) return LESS_THAN;
        if (leftType > rightType) return GREATER_THAN;
        
        // Compare tag values
        GrapaValue leftValue = leftTag.GetValue();
        GrapaValue rightValue = rightTag.GetValue();
        
        return CompareValue(leftValue, rightValue);
    }
    
    ComparisonResult CompareTuple(const GrapaValue& left, const GrapaValue& right) {
        // Tuple comparison (element-wise)
        GrapaTuple leftTuple = left.tuple();
        GrapaTuple rightTuple = right.tuple();
        
        u32 leftSize = leftTuple.GetSize();
        u32 rightSize = rightTuple.GetSize();
        
        // Compare sizes first
        if (leftSize < rightSize) return LESS_THAN;
        if (leftSize > rightSize) return GREATER_THAN;
        
        // Compare elements
        for (u32 i = 0; i < leftSize; i++) {
            GrapaValue leftElem = leftTuple.GetElement(i);
            GrapaValue rightElem = rightTuple.GetElement(i);
            
            ComparisonResult elemResult = CompareValue(leftElem, rightElem);
            if (elemResult != EQUAL) return elemResult;
        }
        return EQUAL;
    }
    
    ComparisonResult CompareWidget(const GrapaValue& left, const GrapaValue& right) {
        // Widget comparison (compare widget properties)
        GrapaWidget leftWidget = left.widget();
        GrapaWidget rightWidget = right.widget();
        
        // Compare widget types first
        GrapaCHAR leftType = leftWidget.GetType();
        GrapaCHAR rightType = rightWidget.GetType();
        
        s64 typeCmp = leftType.Cmp(rightType);
        if (typeCmp != 0) {
            if (typeCmp < 0) return LESS_THAN;
            return GREATER_THAN;
        }
        
        // Compare widget properties
        return CompareWidgetProperties(leftWidget, rightWidget);
    }
    
    // System Type Comparisons
    ComparisonResult CompareOperation(const GrapaValue& left, const GrapaValue& right) {
        // Operation/function comparison (compare function signatures)
        GrapaOperation leftOp = left.operation();
        GrapaOperation rightOp = right.operation();
        
        // Compare operation signatures
        GrapaCHAR leftSig = leftOp.GetSignature();
        GrapaCHAR rightSig = rightOp.GetSignature();
        
        s64 sigCmp = leftSig.Cmp(rightSig);
        if (sigCmp < 0) return LESS_THAN;
        if (sigCmp > 0) return GREATER_THAN;
        return EQUAL;
    }
    
    ComparisonResult CompareCode(const GrapaValue& left, const GrapaValue& right) {
        // Code block comparison (compare normalized code)
        GrapaCode leftCode = left.code();
        GrapaCode rightCode = right.code();
        
        // Normalize and compare code
        GrapaCHAR leftNormalized = leftCode.Normalize();
        GrapaCHAR rightNormalized = rightCode.Normalize();
        
        s64 cmp = leftNormalized.Cmp(rightNormalized);
        if (cmp < 0) return LESS_THAN;
        if (cmp > 0) return GREATER_THAN;
        return EQUAL;
    }
    
    ComparisonResult CompareClass(const GrapaValue& left, const GrapaValue& right) {
        // Class comparison (compare class definitions)
        GrapaClass leftClass = left.class();
        GrapaClass rightClass = right.class();
        
        // Compare class names first
        GrapaCHAR leftName = leftClass.GetName();
        GrapaCHAR rightName = rightClass.GetName();
        
        s64 nameCmp = leftName.Cmp(rightName);
        if (nameCmp != 0) {
            if (nameCmp < 0) return LESS_THAN;
            return GREATER_THAN;
        }
        
        // Compare class definitions
        return CompareClassDefinition(leftClass, rightClass);
    }
    
    ComparisonResult CompareObject(const GrapaValue& left, const GrapaValue& right) {
        // Object comparison (compare object properties)
        GrapaObject leftObj = left.object();
        GrapaObject rightObj = right.object();
        
        // Compare object references first
        u64 leftRef = leftObj.GetReference();
        u64 rightRef = rightObj.GetReference();
        
        if (leftRef < rightRef) return LESS_THAN;
        if (leftRef > rightRef) return GREATER_THAN;
        
        // Compare object properties
        return CompareObjectProperties(leftObj, rightObj);
    }
    
    ComparisonResult CompareReference(const GrapaValue& left, const GrapaValue& right) {
        // Reference comparison (compare reference targets)
        GrapaReference leftRef = left.reference();
        GrapaReference rightRef = right.reference();
        
        // Compare reference targets
        GrapaValue leftTarget = leftRef.GetTarget();
        GrapaValue rightTarget = rightRef.GetTarget();
        
        return CompareValue(leftTarget, rightTarget);
    }
    
    ComparisonResult CompareRule(const GrapaValue& left, const GrapaValue& right) {
        // Rule comparison (compare rule definitions)
        GrapaRule leftRule = left.rule();
        GrapaRule rightRule = right.rule();
        
        // Compare rule names first
        GrapaCHAR leftName = leftRule.GetName();
        GrapaCHAR rightName = rightRule.GetName();
        
        s64 nameCmp = leftName.Cmp(rightName);
        if (nameCmp != 0) {
            if (nameCmp < 0) return LESS_THAN;
            return GREATER_THAN;
        }
        
        // Compare rule definitions
        return CompareRuleDefinition(leftRule, rightRule);
    }
    
    ComparisonResult CompareToken(const GrapaValue& left, const GrapaValue& right) {
        // Token comparison (compare token values)
        GrapaToken leftToken = left.token();
        GrapaToken rightToken = right.token();
        
        // Compare token types first
        u8 leftType = leftToken.GetType();
        u8 rightType = rightToken.GetType();
        
        if (leftType < rightType) return LESS_THAN;
        if (leftType > rightType) return GREATER_THAN;
        
        // Compare token values
        GrapaValue leftValue = leftToken.GetValue();
        GrapaValue rightValue = rightToken.GetValue();
        
        return CompareValue(leftValue, rightValue);
    }
    
    // Helper method for recursive value comparison
    ComparisonResult CompareValue(const GrapaValue& left, const GrapaValue& right) {
        // Determine types and delegate to appropriate comparison
        u8 leftType = left.mToken;
        u8 rightType = right.mToken;
        
        if (leftType != rightType) {
            // Type conversion for comparison
            return CompareWithTypeConversion(left, right);
        }
        
        // Same type, use appropriate comparison
        switch (leftType) {
            case GrapaTokenType::BOOL: return CompareBool(left, right);
            case GrapaTokenType::INT: return CompareInt(left, right);
            case GrapaTokenType::FLOAT: return CompareFloat(left, right);
            case GrapaTokenType::STR: return CompareString(left, right, COLLATION_BINARY);
            case GrapaTokenType::TIME: return CompareTime(left, right);
            case GrapaTokenType::ARRAY: return CompareArray(left, right);
            case GrapaTokenType::LIST: return CompareList(left, right);
            case GrapaTokenType::VECTOR: return CompareVector(left, right);
            case GrapaTokenType::TABLE: return CompareTable(left, right);
            // ... other types
            default: return EQUAL; // Unknown types considered equal
        }
    }
    
    // Type conversion for comparison
    ComparisonResult CompareWithTypeConversion(const GrapaValue& left, const GrapaValue& right) {
        // Convert both to a common type for comparison
        // This is a simplified version - full implementation would be more complex
        
        // Try converting to string for comparison
        GrapaCHAR leftStr = left.str();
        GrapaCHAR rightStr = right.str();
        
        s64 cmp = leftStr.Cmp(rightStr);
        if (cmp < 0) return LESS_THAN;
        if (cmp > 0) return GREATER_THAN;
        return EQUAL;
    }
};

#### **String Comparison**
```cpp
class GrapaDB2StringComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        for (u32 i = 0; i < context.fieldOrder.Count(); i++) {
            u64 fieldId = context.fieldOrder.GetAt(i);
            GrapaDB2Field& field = context.fields.GetFieldById(fieldId);
            
            GrapaValue leftValue = context.leftValues.GetValue(fieldId);
            GrapaValue rightValue = context.rightValues.GetValue(fieldId);
            
            if (field.mFieldType == FIELD_TYPE_STR) {
                result = CompareString(leftValue, rightValue, field.mCollation);
                if (result != EQUAL) return 0;
            }
        }
        return 0;
    }
    
private:
    ComparisonResult CompareString(const GrapaValue& left, const GrapaValue& right, u8 collation) {
        GrapaCHAR leftStr = left.str();
        GrapaCHAR rightStr = right.str();
        
        switch (collation) {
            case COLLATION_BINARY:
                return CompareBinary(leftStr, rightStr);
            case COLLATION_CASE_INSENSITIVE:
                return CompareCaseInsensitive(leftStr, rightStr);
            case COLLATION_LOCALE:
                return CompareLocale(leftStr, rightStr);
            default:
                return CompareBinary(leftStr, rightStr);
        }
    }
};
```

### **3. Tree-Type Aware Comparison**

#### **ROW Storage Comparison**
```cpp
class GrapaDB2RowComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        // ROW storage: compare entire records as units
        if (context.mode == SEARCH) {
            // For search, compare only indexed fields
            return CompareIndexedFields(context, result);
        } else {
            // For insert/delete, compare record IDs
            return CompareRecordIds(context, result);
        }
    }
    
private:
    GrapaError CompareIndexedFields(const GrapaDB2ComparisonContext& context, ComparisonResult& result) {
        // Use field-specific comparison for indexed fields
        GrapaDB2FieldComparison fieldComp;
        return fieldComp.CompareFields(context, result);
    }
    
    GrapaError CompareRecordIds(const GrapaDB2ComparisonContext& context, ComparisonResult& result) {
        // Simple record ID comparison for ROW storage
        u64 leftId = context.leftValues.GetRecordId();
        u64 rightId = context.rightValues.GetRecordId();
        
        if (leftId < rightId) result = LESS_THAN;
        else if (leftId > rightId) result = GREATER_THAN;
        else result = EQUAL;
        
        return 0;
    }
};
```

#### **COL Storage Comparison**
```cpp
class GrapaDB2ColumnComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        // COL storage: compare column values directly
        if (context.mode == SEARCH) {
            // For search, compare column values
            return CompareColumnValues(context, result);
        } else {
            // For insert/delete, compare row positions
            return CompareRowPositions(context, result);
        }
    }
    
private:
    GrapaError CompareColumnValues(const GrapaDB2ComparisonContext& context, ComparisonResult& result) {
        // Direct column value comparison
        GrapaDB2FieldComparison fieldComp;
        return fieldComp.CompareFields(context, result);
    }
    
    GrapaError CompareRowPositions(const GrapaDB2ComparisonContext& context, ComparisonResult& result) {
        // Compare row positions in column storage
        u64 leftPos = context.leftValues.GetRowPosition();
        u64 rightPos = context.rightValues.GetRowPosition();
        
        if (leftPos < rightPos) result = LESS_THAN;
        else if (leftPos > rightPos) result = GREATER_THAN;
        else result = EQUAL;
        
        return 0;
    }
};
```

### **4. Storage-Type Aware Comparison**

#### **Fixed Storage Comparison**
```cpp
class GrapaDB2FixedComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        // Fixed storage: direct memory comparison
        for (u32 i = 0; i < context.fieldOrder.Count(); i++) {
            u64 fieldId = context.fieldOrder.GetAt(i);
            GrapaDB2Field& field = context.fields.GetFieldById(fieldId);
            
            if (field.mStorageType == STORAGE_TYPE_FIX) {
                result = CompareFixedField(context.leftValues, context.rightValues, field);
                if (result != EQUAL) return 0;
            }
        }
        return 0;
    }
    
private:
    ComparisonResult CompareFixedField(const GrapaDB2FieldValueArray& left, 
                                      const GrapaDB2FieldValueArray& right,
                                      const GrapaDB2Field& field) {
        // Direct memory comparison for fixed fields
        const u8* leftData = left.GetFixedData(field.mFieldId);
        const u8* rightData = right.GetFixedData(field.mFieldId);
        
        return memcmp(leftData, rightData, field.mSize);
    }
};
```

#### **Variable Storage Comparison**
```cpp
class GrapaDB2VariableComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        // Variable storage: length + data comparison
        for (u32 i = 0; i < context.fieldOrder.Count(); i++) {
            u64 fieldId = context.fieldOrder.GetAt(i);
            GrapaDB2Field& field = context.fields.GetFieldById(fieldId);
            
            if (field.mStorageType == STORAGE_TYPE_VAR) {
                result = CompareVariableField(context.leftValues, context.rightValues, field);
                if (result != EQUAL) return 0;
            }
        }
        return 0;
    }
    
private:
    ComparisonResult CompareVariableField(const GrapaDB2FieldValueArray& left,
                                         const GrapaDB2FieldValueArray& right,
                                         const GrapaDB2Field& field) {
        // Variable field comparison with length prefix
        u32 leftLen = left.GetVariableLength(field.mFieldId);
        u32 rightLen = right.GetVariableLength(field.mFieldId);
        
        if (leftLen != rightLen) {
            return (leftLen < rightLen) ? LESS_THAN : GREATER_THAN;
        }
        
        const u8* leftData = left.GetVariableData(field.mFieldId);
        const u8* rightData = right.GetVariableData(field.mFieldId);
        
        return memcmp(leftData, rightData, leftLen);
    }
};
```

### **5. Composite Comparison Strategy**

#### **Comparison Strategy Factory**
```cpp
class GrapaDB2ComparisonStrategy {
public:
    static GrapaDB2Comparison* CreateStrategy(const GrapaDB2ComparisonContext& context) {
        // Create composite strategy based on context
        GrapaDB2CompositeComparison* strategy = new GrapaDB2CompositeComparison();
        
        // Add type-aware comparison
        strategy->AddComparison(new GrapaDB2NumericComparison());
        strategy->AddComparison(new GrapaDB2StringComparison());
        
        // Add tree-type aware comparison
        switch (context.treeType) {
            case TREE_TYPE_ROW:
                strategy->AddComparison(new GrapaDB2RowComparison());
                break;
            case TREE_TYPE_COL:
                strategy->AddComparison(new GrapaDB2ColumnComparison());
                break;
            case TREE_TYPE_GROUP:
                strategy->AddComparison(new GrapaDB2GroupComparison());
                break;
        }
        
        // Add storage-type aware comparison
        switch (context.storageType) {
            case STORAGE_TYPE_FIX:
                strategy->AddComparison(new GrapaDB2FixedComparison());
                break;
            case STORAGE_TYPE_VAR:
                strategy->AddComparison(new GrapaDB2VariableComparison());
                break;
            case STORAGE_TYPE_PAR:
                strategy->AddComparison(new GrapaDB2PartitionedComparison());
                break;
        }
        
        return strategy;
    }
};
```

#### **Composite Comparison**
```cpp
class GrapaDB2CompositeComparison : public GrapaDB2Comparison {
public:
    void AddComparison(GrapaDB2Comparison* comparison) {
        mComparisons.Append(comparison);
    }
    
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        // Try each comparison strategy in order
        for (u32 i = 0; i < mComparisons.Count(); i++) {
            GrapaDB2Comparison* comp = mComparisons.GetAt(i);
            
            if (comp->SupportsDataType(context.fields.GetFieldType()) &&
                comp->SupportsTreeType(context.treeType) &&
                comp->SupportsStorageType(context.storageType)) {
                
                GrapaError err = comp->Compare(context, result);
                if (!err) return 0; // Success
            }
        }
        
        // Fallback to default comparison
        return DefaultComparison(context, result);
    }
    
private:
    GrapaArray<GrapaDB2Comparison*> mComparisons;
    
    GrapaError DefaultComparison(const GrapaDB2ComparisonContext& context, ComparisonResult& result) {
        // Fallback comparison logic
        result = EQUAL;
        return 0;
    }
};
```

### **6. Enhanced GrapaDB2 Integration**

#### **GrapaDB2 Comparison Override**
```cpp
class GrapaDB2 : public GrapaBtree {
public:
    // Override BTree comparison for database-aware logic
    virtual GrapaError CompareKey(s16 compareType, GrapaCursor& dataCursor, GrapaCursor& treeCursor, s8& result) override {
        // Create comparison context
        GrapaDB2ComparisonContext context;
        context.mode = ConvertCompareType(compareType);
        context.table = GetCurrentTable();
        context.index = GetCurrentIndex();
        
        // Extract values from cursors
        ExtractValuesFromCursor(dataCursor, context.leftValues);
        ExtractValuesFromCursor(treeCursor, context.rightValues);
        
        // Get field metadata
        context.fields = context.table->GetFields();
        context.fieldOrder = context.index->GetFieldOrder();
        
        // Set storage context
        context.treeType = context.table->GetTreeType();
        context.storageType = GetStorageType(context.fields);
        
        // Create and execute comparison strategy
        GrapaDB2Comparison* strategy = GrapaDB2ComparisonStrategy::CreateStrategy(context);
        GrapaError err = strategy->Compare(context, result);
        delete strategy;
        
        return err;
    }
    
private:
    ComparisonMode ConvertCompareType(s16 compareType) {
        switch (compareType) {
            case SEARCH_MODE: return SEARCH;
            case INSERT_MODE: return INSERT;
            case DELETE_MODE: return DELETE;
            default: return SEARCH;
        }
    }
    
    void ExtractValuesFromCursor(GrapaCursor& cursor, GrapaDB2FieldValueArray& values) {
        // Extract field values from cursor based on table schema
        // This replaces the complex PtrToRec logic
        GrapaDB2Table* table = GetCurrentTable();
        GrapaDB2FieldArray& fields = table->GetFields();
        
        for (u32 i = 0; i < fields.Count(); i++) {
            GrapaDB2Field& field = fields.GetFieldAt(i);
            GrapaValue value = ExtractFieldValue(cursor, field);
            values.Add(field.mFieldId, value);
        }
    }
};
```

### **7. Performance Optimizations**

#### **Comparison Caching**
```cpp
class GrapaDB2ComparisonCache {
public:
    struct CachedComparison {
        GrapaDB2ComparisonContext context;
        ComparisonResult result;
        u64 timestamp;
    };
    
    GrapaError GetCachedResult(const GrapaDB2ComparisonContext& context, ComparisonResult& result) {
        u64 hash = HashContext(context);
        CachedComparison* cached = mCache.Search(hash);
        
        if (cached && (GetCurrentTime() - cached->timestamp) < CACHE_TTL) {
            result = cached->result;
            return 0; // Cache hit
        }
        
        return -1; // Cache miss
    }
    
    GrapaError CacheResult(const GrapaDB2ComparisonContext& context, ComparisonResult result) {
        u64 hash = HashContext(context);
        CachedComparison* cached = new CachedComparison();
        cached->context = context;
        cached->result = result;
        cached->timestamp = GetCurrentTime();
        
        mCache.Insert(hash, cached);
        return 0;
    }
    
private:
    GrapaHashTable<u64, CachedComparison*> mCache;
    static const u64 CACHE_TTL = 1000; // 1 second
};
```

#### **Optimized Field Access**
```cpp
class GrapaDB2OptimizedFieldAccess {
public:
    // Pre-compute field offsets for fixed storage
    virtual GrapaError PrecomputeOffsets(const GrapaDB2FieldArray& fields) {
        u64 offset = 0;
        for (u32 i = 0; i < fields.Count(); i++) {
            GrapaDB2Field& field = fields.GetFieldAt(i);
            if (field.mStorageType == STORAGE_TYPE_FIX) {
                mFieldOffsets[field.mFieldId] = offset;
                offset += field.mSize;
            }
        }
        return 0;
    }
    
    // Fast field access for fixed storage
    virtual const u8* GetFixedFieldData(const u8* recordData, u64 fieldId) {
        u64 offset = mFieldOffsets[fieldId];
        return recordData + offset;
    }
    
private:
    GrapaHashMap<u64, u64> mFieldOffsets; // fieldId -> offset
};
```

## RAW Field Comparison Support

### **Current RAW Field Behavior**

RAW fields in Grapa have special characteristics that must be preserved in the database-aware comparison system:

#### **1. RAW Field Storage**
- **Type**: `GrapaTokenType::RAW`
- **Special Handling**: RAW fields store an additional type byte to preserve the original data type
- **Conversion**: Most data types can be converted to and from RAW using `.raw()`, `.hex()`, `.str()`, `.int()`
- **Comparison**: Byte-by-byte comparison with type preservation

#### **2. RAW Field Characteristics**
```grapa
/* RAW field creation and usage */
f.mkfield("binary_data", "RAW", "VAR");

/* Store raw bytes */
f.set("record1", "hello".raw(), "binary_data");
f.set("record2", (0x6869).raw(), "binary_data");

/* Retrieve and convert */
raw_data = f.get("record1", "binary_data");
hex_string = raw_data.hex();  /* "0x6869" */
string_value = raw_data.str(); /* "hi" */
int_value = raw_data.int();   /* 26729 */
```

### **GrapaDB2 RAW Field Comparison**

#### **1. RAW Field Comparison Implementation**
```cpp
// GrapaDB2 RAW field comparison
class GrapaDB2RawComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        GrapaBYTE& left = context.leftValue;
        GrapaBYTE& right = context.rightValue;
        
        // Preserve type information for RAW fields
        u8 leftType = left.ToDbType();
        u8 rightType = right.ToDbType();
        
        // Compare raw bytes first
        result = CompareRawBytes(left, right);
        
        // If bytes are equal, compare types (preserve original type order)
        if (result == COMPARISON_EQUAL) {
            result = (leftType < rightType) ? COMPARISON_LESS_THAN : 
                     (leftType > rightType) ? COMPARISON_GREATER_THAN : 
                     COMPARISON_EQUAL;
        }
        
        return 0;
    }
    
    virtual bool SupportsDataType(u8 dataType) const override {
        return dataType == GrapaTokenType::RAW;
    }
    
private:
    ComparisonResult CompareRawBytes(const GrapaBYTE& left, const GrapaBYTE& right) {
        // Byte-by-byte comparison (current GrapaDB behavior)
        u64 minLen = (left.mLength < right.mLength) ? left.mLength : right.mLength;
        
        for (u64 i = 0; i < minLen; i++) {
            if (left.mBytes[i] < right.mBytes[i]) return COMPARISON_LESS_THAN;
            if (left.mBytes[i] > right.mBytes[i]) return COMPARISON_GREATER_THAN;
        }
        
        // If all bytes match up to minLen, compare lengths
        if (left.mLength < right.mLength) return COMPARISON_LESS_THAN;
        if (left.mLength > right.mLength) return COMPARISON_GREATER_THAN;
        return COMPARISON_EQUAL;
    }
};
```

#### **2. RAW Field Type Preservation**
```cpp
// RAW field type preservation in GrapaDB2
class GrapaDB2RawTypePreservation {
public:
    // Store RAW field with type preservation
    virtual GrapaError StoreRawField(const GrapaCHAR& tableName, const GrapaCHAR& fieldName, 
                                    const GrapaBYTE& rawData, u8 originalType) {
        // Create RAW field with type preservation
        GrapaBYTE storedData = rawData;
        storedData.mToken = GrapaTokenType::RAW;  // Set as RAW type
        
        // Store the original type in metadata
        GrapaDB2FieldMetadata metadata;
        metadata.originalType = originalType;
        metadata.isRawField = true;
        
        return StoreFieldWithMetadata(tableName, fieldName, storedData, metadata);
    }
    
    // Retrieve RAW field with type restoration
    virtual GrapaError RetrieveRawField(const GrapaCHAR& tableName, const GrapaCHAR& fieldName,
                                       GrapaBYTE& rawData, u8& originalType) {
        GrapaDB2FieldMetadata metadata;
        GrapaError err = RetrieveFieldWithMetadata(tableName, fieldName, rawData, metadata);
        if (err) return err;
        
        if (metadata.isRawField) {
            originalType = metadata.originalType;
            rawData.mToken = originalType;  // Restore original type
        }
        
        return 0;
    }
};
```

#### **3. RAW Field Conversion Support**
```cpp
// RAW field conversion functions
class GrapaDB2RawConversion {
public:
    // Convert to RAW
    virtual GrapaError ToRaw(const GrapaValue& value, GrapaBYTE& rawData) {
        switch (value.type()) {
            case GrapaTokenType::STR:
                rawData.FROM(value.str());
                rawData.mToken = GrapaTokenType::RAW;
                break;
            case GrapaTokenType::INT:
                rawData.FROM(value.int());
                rawData.mToken = GrapaTokenType::RAW;
                break;
            case GrapaTokenType::FLOAT:
                rawData.FROM(value.float());
                rawData.mToken = GrapaTokenType::RAW;
                break;
            default:
                rawData = value.ToRaw();
                break;
        }
        return 0;
    }
    
    // Convert from RAW
    virtual GrapaError FromRaw(const GrapaBYTE& rawData, GrapaValue& value, u8 targetType) {
        switch (targetType) {
            case GrapaTokenType::STR:
                value = rawData.str();
                break;
            case GrapaTokenType::INT:
                value = rawData.int();
                break;
            case GrapaTokenType::FLOAT:
                value = rawData.float();
                break;
            case GrapaTokenType::RAW:
                value = rawData;
                break;
            default:
                return -1;  // Unsupported conversion
        }
        return 0;
    }
};
```

### **RAW Field Comparison in SQL**

#### **1. RAW Field SQL Grammar**
```grapa
/* SQL grammar rules for RAW field comparison */
@global["$sql_raw_value"]
    = rule $STR '.raw()' {@<sql_raw_from_string,{$1}>}
    | $INT '.raw()' {@<sql_raw_from_int,{$1}>}
    | '0x' $STR {@<sql_raw_from_hex,{$2}>}
    | $RAW {@<sql_raw_literal,{$1}>}
    ;

@global["$sql_raw_comparison"]
    = rule <$sql_column_name> '=' <$sql_raw_value> {@<sql_raw_eq_comparison,{$1,$3}>}
    | <$sql_column_name> '!=' <$sql_raw_value> {@<sql_raw_neq_comparison,{$1,$3}>}
    | <$sql_column_name> '>' <$sql_raw_value> {@<sql_raw_gt_comparison,{$1,$3}>}
    | <$sql_column_name> '<' <$sql_raw_value> {@<sql_raw_lt_comparison,{$1,$3}>}
    | <$sql_column_name> '>=' <$sql_raw_value> {@<sql_raw_gte_comparison,{$1,$3}>}
    | <$sql_column_name> '<=' <$sql_raw_value> {@<sql_raw_lte_comparison,{$1,$3}>}
    ;
```

#### **2. RAW Field SQL Examples**
```grapa
/* RAW field SQL operations */
f = $file();
f.cd("binary_database");

/* Create table with RAW fields */
CREATE TABLE binary_data (
    id INT,
    name STR,
    binary_content RAW,
    metadata RAW
);

/* Insert RAW data */
INSERT INTO binary_data (id, name, binary_content, metadata) VALUES 
    (1, 'file1', 'hello'.raw(), (0x6869).raw()),
    (2, 'file2', 'world'.raw(), (0x776f).raw());

/* Query RAW data with byte-by-byte comparison */
SELECT * FROM binary_data WHERE binary_content = 'hello'.raw();
SELECT * FROM binary_data WHERE metadata > (0x5000).raw();

/* RAW field functions */
SELECT id, name, binary_content.hex() as hex_content FROM binary_data;
SELECT id, name, binary_content.str() as string_content FROM binary_data;
SELECT id, name, binary_content.int() as int_content FROM binary_data;
```

#### **3. RAW Field Comparison Modes**
```grapa
/* RAW field comparison modes */
/* Byte-by-byte comparison (default) */
SELECT * FROM binary_data WHERE binary_content = 'hello'.raw();

/* Type-aware comparison */
SELECT * FROM binary_data WHERE binary_content.str() = 'hello';
SELECT * FROM binary_data WHERE binary_content.int() > 1000;

/* Hex pattern matching */
SELECT * FROM binary_data WHERE binary_content.hex() LIKE '0x68%';

/* Length-based comparison */
SELECT * FROM binary_data WHERE binary_content.len() > 10;
```

### **RAW Field Indexing and Performance**

#### **1. RAW Field Indexing**
```cpp
// RAW field indexing in GrapaDB2
class GrapaDB2RawIndexing {
public:
    // Create index on RAW field
    virtual GrapaError CreateRawIndex(const GrapaCHAR& tableName, const GrapaCHAR& fieldName) {
        // RAW fields can be indexed for efficient byte-by-byte comparison
        GrapaDB2IndexOptions options;
        options.type = INDEX_TYPE_RAW;
        options.isByteOrdered = true;  // Maintain byte order for comparison
        
        return CreateIndex(tableName, fieldName, options);
    }
    
    // Search using RAW field index
    virtual GrapaError SearchRawIndex(const GrapaCHAR& tableName, const GrapaCHAR& fieldName, 
                                     const GrapaBYTE& searchValue, GrapaDB2ResultSet& results) {
        // Use byte-by-byte comparison for RAW field searches
        GrapaDB2ComparisonContext context;
        context.comparisonMode = COMPARISON_MODE_RAW;
        context.leftValue = searchValue;
        
        return SearchIndex(tableName, fieldName, context, results);
    }
};
```

#### **2. RAW Field Performance Optimization**
```cpp
// RAW field performance optimization
class GrapaDB2RawPerformance {
public:
    // Optimize RAW field storage
    virtual GrapaError OptimizeRawStorage(const GrapaCHAR& tableName, const GrapaCHAR& fieldName) {
        // Analyze RAW field usage patterns
        GrapaDB2FieldStats stats = GetFieldStats(tableName, fieldName);
        
        // Optimize storage based on patterns
        if (stats.avgLength < 16) {
            // Use fixed storage for small RAW fields
            return OptimizeToFixedStorage(tableName, fieldName, stats.maxLength);
        } else if (stats.avgLength < 1024) {
            // Use variable storage for medium RAW fields
            return OptimizeToVariableStorage(tableName, fieldName);
        } else {
            // Use partitioned storage for large RAW fields
            return OptimizeToPartitionedStorage(tableName, fieldName);
        }
    }
    
    // RAW field compression
    virtual GrapaError CompressRawField(const GrapaCHAR& tableName, const GrapaCHAR& fieldName) {
        // Apply compression to RAW fields
        return ApplyCompression(tableName, fieldName, COMPRESSION_TYPE_RAW);
    }
};
```

### **RAW Field Compatibility and Migration**

#### **1. Backward Compatibility**
```cpp
// Ensure backward compatibility with existing RAW fields
class GrapaDB2RawCompatibility {
public:
    // Migrate existing RAW fields
    virtual GrapaError MigrateRawFields(const GrapaCHAR& tableName) {
        // Detect existing RAW fields
        GrapaArray<GrapaDB2Field> fields = GetTableFields(tableName);
        
        for (const auto& field : fields) {
            if (field.type == GrapaTokenType::RAW) {
                // Preserve existing RAW field behavior
                GrapaDB2FieldMetadata metadata;
                metadata.isRawField = true;
                metadata.originalType = field.originalType;
                
                // Update field metadata
                UpdateFieldMetadata(tableName, field.name, metadata);
            }
        }
        return 0;
    }
    
    // Validate RAW field compatibility
    virtual GrapaError ValidateRawCompatibility(const GrapaCHAR& tableName) {
        // Ensure all RAW fields maintain their original behavior
        return ValidateFieldCompatibility(tableName, GrapaTokenType::RAW);
    }
};
```

#### **2. RAW Field Testing**
```grapa
/* RAW field compatibility test */
f = $file();
f.cd("test_database");

/* Create RAW field */
f.mkfield("binary_data", "RAW", "VAR");

/* Test current behavior */
f.set("test1", "hello".raw(), "binary_data");
f.set("test2", (0x6869).raw(), "binary_data");

/* Verify retrieval */
raw1 = f.get("test1", "binary_data");
raw2 = f.get("test2", "binary_data");

/* Verify conversion methods */
hex1 = raw1.hex();  /* Should be "0x6869" */
str1 = raw1.str();  /* Should be "hi" */
int1 = raw1.int();  /* Should be 26729 */

/* Test SQL integration */
SELECT * FROM test_database WHERE binary_data = 'hello'.raw();
SELECT * FROM test_database WHERE binary_data.hex() = '0x6869';
SELECT * FROM test_database WHERE binary_data.str() = 'hi';
```

### **RAW Field Advanced Features**

#### **1. RAW Field Pattern Matching**
```grapa
/* RAW field pattern matching */
SELECT * FROM binary_data WHERE RAW_STARTS_WITH(binary_content, 'hello'.raw());
SELECT * FROM binary_data WHERE RAW_ENDS_WITH(binary_content, 'world'.raw());
SELECT * FROM binary_data WHERE RAW_CONTAINS(binary_content, 'test'.raw());
SELECT * FROM binary_data WHERE RAW_MATCHES(binary_content, '0x68%');  /* Hex pattern */
```

#### **2. RAW Field Aggregation**
```grapa
/* RAW field aggregation functions */
SELECT 
    COUNT(*) as total_records,
    AVG(RAW_LENGTH(binary_content)) as avg_size,
    MAX(RAW_LENGTH(binary_content)) as max_size,
    MIN(RAW_LENGTH(binary_content)) as min_size,
    SUM(RAW_LENGTH(binary_content)) as total_size
FROM binary_data;
```

#### **3. RAW Field Conversion Functions**
```grapa
/* RAW field conversion functions */
SELECT 
    id,
    name,
    binary_content,
    RAW_TO_HEX(binary_content) as hex_string,
    RAW_TO_STR(binary_content) as string_value,
    RAW_TO_INT(binary_content) as integer_value,
    RAW_LENGTH(binary_content) as byte_count
FROM binary_data;
```

### **Summary**

RAW fields are fully supported in the GrapaDB2 database-aware comparison system with:

1. **Complete Type Preservation**: Original data types preserved in RAW fields
2. **Byte-by-Byte Comparison**: Maintains current GrapaDB behavior
3. **SQL Integration**: RAW fields work seamlessly with SQL syntax
4. **Enhanced Functions**: New SQL functions for RAW field manipulation
5. **Performance Optimization**: Leverages GrapaDB2's enhanced features
6. **Backward Compatibility**: All existing RAW field functionality preserved
7. **Advanced Features**: Pattern matching, aggregation, and conversion functions

This ensures that RAW fields work exactly as they do now, while gaining the benefits of the enhanced comparison system and SQL integration. 

## Unicode, Regex, and JSON/XML Comparison Support

### **Unicode Comparison Support**

#### **1. Unicode-Aware String Comparison**
```cpp
// Unicode-aware string comparison in GrapaDB2
class GrapaDB2UnicodeComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        GrapaCHAR& left = context.leftValue;
        GrapaCHAR& right = context.rightValue;
        
        // Unicode-aware comparison with normalization
        GrapaCHAR normalizedLeft, normalizedRight;
        UnicodeNormalize(left, normalizedLeft, "NFC");
        UnicodeNormalize(right, normalizedRight, "NFC");
        
        // Compare normalized strings
        result = UnicodeCompare(normalizedLeft, normalizedRight);
        
        return 0;
    }
    
    virtual bool SupportsDataType(u8 dataType) const override {
        return dataType == GrapaTokenType::STR;
    }
    
private:
    ComparisonResult UnicodeCompare(const GrapaCHAR& left, const GrapaCHAR& right) {
        // Use Unicode-aware comparison (ICU or similar)
        return UnicodeCollation::Compare(left, right);
    }
    
    GrapaError UnicodeNormalize(const GrapaCHAR& input, GrapaCHAR& output, const GrapaCHAR& form) {
        // Normalize Unicode string (NFC, NFD, NFKC, NFKD)
        return UnicodeNormalization::Normalize(input, output, form);
    }
};
```

#### **2. Unicode Collation Support**
```cpp
// Unicode collation support
class GrapaDB2UnicodeCollation {
public:
    // Create collation for specific locale
    virtual GrapaError CreateCollation(const GrapaCHAR& name, const GrapaCHAR& locale) {
        // Create Unicode collation for specific locale
        return UnicodeCollation::Create(name, locale);
    }
    
    // Compare with specific collation
    virtual GrapaError CompareWithCollation(const GrapaCHAR& left, const GrapaCHAR& right, 
                                           const GrapaCHAR& collation, s8& result) {
        // Compare strings using specific collation
        return UnicodeCollation::Compare(left, right, collation, result);
    }
    
    // Unicode character count
    virtual u64 UnicodeCharCount(const GrapaCHAR& str) {
        // Count Unicode characters (not bytes)
        return UnicodeLength::Count(str);
    }
};
```

### **Regex Comparison Support**

#### **1. Regex Pattern Comparison**
```cpp
// Regex pattern comparison in GrapaDB2
class GrapaDB2RegexComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        GrapaCHAR& text = context.leftValue;
        GrapaCHAR& pattern = context.rightValue;
        
        // Check if text matches regex pattern
        bool matches;
        GrapaError err = RegexMatch(text, pattern, matches);
        if (err) return err;
        
        // Convert boolean result to comparison result
        result = matches ? COMPARISON_EQUAL : COMPARISON_NOT_EQUAL;
        
        return 0;
    }
    
    virtual bool SupportsDataType(u8 dataType) const override {
        return dataType == GrapaTokenType::STR;
    }
    
private:
    GrapaError RegexMatch(const GrapaCHAR& text, const GrapaCHAR& pattern, bool& matches) {
        // Use PCRE2 or similar regex engine
        return RegexEngine::Match(text, pattern, matches);
    }
};
```

#### **2. Regex with Flags Support**
```cpp
// Regex with flags support
class GrapaDB2RegexFlagsComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        GrapaCHAR& text = context.leftValue;
        GrapaCHAR& pattern = context.rightValue;
        GrapaCHAR& flags = context.regexFlags;
        
        // Check if text matches regex pattern with flags
        bool matches;
        GrapaError err = RegexMatchWithFlags(text, pattern, flags, matches);
        if (err) return err;
        
        result = matches ? COMPARISON_EQUAL : COMPARISON_NOT_EQUAL;
        
        return 0;
    }
    
private:
    GrapaError RegexMatchWithFlags(const GrapaCHAR& text, const GrapaCHAR& pattern, 
                                  const GrapaCHAR& flags, bool& matches) {
        // Support case-insensitive, multiline, etc.
        return RegexEngine::MatchWithFlags(text, pattern, flags, matches);
    }
};
```

#### **3. Regex Capture Group Support**
```cpp
// Regex capture group support
class GrapaDB2RegexCaptureComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        GrapaCHAR& text = context.leftValue;
        GrapaCHAR& pattern = context.rightValue;
        u32 captureGroup = context.captureGroup;
        
        // Extract capture group and compare
        GrapaCHAR captured;
        GrapaError err = RegexCapture(text, pattern, captureGroup, captured);
        if (err) return err;
        
        // Compare captured value
        result = StringCompare(captured, context.compareValue);
        
        return 0;
    }
    
private:
    GrapaError RegexCapture(const GrapaCHAR& text, const GrapaCHAR& pattern, 
                           u32 captureGroup, GrapaCHAR& captured) {
        // Extract specific capture group
        return RegexEngine::Capture(text, pattern, captureGroup, captured);
    }
    
    ComparisonResult StringCompare(const GrapaCHAR& left, const GrapaCHAR& right) {
        // Standard string comparison
        return GrapaDB2StringComparison::Compare(left, right);
    }
};
```

### **JSON Comparison Support**

#### **1. JSON Path Comparison**
```cpp
// JSON path comparison in GrapaDB2
class GrapaDB2JsonPathComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        GrapaCHAR& json = context.leftValue;
        GrapaCHAR& path = context.jsonPath;
        GrapaCHAR& compareValue = context.rightValue;
        
        // Extract value from JSON path
        GrapaCHAR extractedValue;
        GrapaError err = JsonPathQuery(json, path, extractedValue);
        if (err) return err;
        
        // Compare extracted value
        result = StringCompare(extractedValue, compareValue);
        
        return 0;
    }
    
    virtual bool SupportsDataType(u8 dataType) const override {
        return dataType == GrapaTokenType::STR;  // JSON stored as string
    }
    
private:
    GrapaError JsonPathQuery(const GrapaCHAR& json, const GrapaCHAR& path, GrapaCHAR& result) {
        // Execute JSON path query
        return JsonEngine::PathQuery(json, path, result);
    }
    
    ComparisonResult StringCompare(const GrapaCHAR& left, const GrapaCHAR& right) {
        return GrapaDB2StringComparison::Compare(left, right);
    }
};
```

#### **2. JSON Array Contains Comparison**
```cpp
// JSON array contains comparison
class GrapaDB2JsonArrayContainsComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        GrapaCHAR& json = context.leftValue;
        GrapaCHAR& path = context.jsonPath;
        GrapaCHAR& searchValue = context.rightValue;
        
        // Check if array contains value
        bool contains;
        GrapaError err = JsonArrayContains(json, path, searchValue, contains);
        if (err) return err;
        
        result = contains ? COMPARISON_EQUAL : COMPARISON_NOT_EQUAL;
        
        return 0;
    }
    
private:
    GrapaError JsonArrayContains(const GrapaCHAR& json, const GrapaCHAR& path, 
                                const GrapaCHAR& searchValue, bool& contains) {
        // Check if JSON array contains specific value
        return JsonEngine::ArrayContains(json, path, searchValue, contains);
    }
};
```

#### **3. JSON Object Key Comparison**
```cpp
// JSON object key comparison
class GrapaDB2JsonKeyComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        GrapaCHAR& json = context.leftValue;
        GrapaCHAR& key = context.jsonKey;
        
        // Check if JSON object has key
        bool hasKey;
        GrapaError err = JsonHasKey(json, key, hasKey);
        if (err) return err;
        
        result = hasKey ? COMPARISON_EQUAL : COMPARISON_NOT_EQUAL;
        
        return 0;
    }
    
private:
    GrapaError JsonHasKey(const GrapaCHAR& json, const GrapaCHAR& key, bool& hasKey) {
        // Check if JSON object has specific key
        return JsonEngine::HasKey(json, key, hasKey);
    }
};
```

### **XML Comparison Support**

#### **1. XPath Comparison**
```cpp
// XPath comparison in GrapaDB2
class GrapaDB2XPathComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        GrapaCHAR& xml = context.leftValue;
        GrapaCHAR& xpath = context.xpathExpression;
        GrapaCHAR& compareValue = context.rightValue;
        
        // Execute XPath query
        GrapaArray<GrapaCHAR> results;
        GrapaError err = XPathQuery(xml, xpath, results);
        if (err) return err;
        
        // Compare first result
        if (results.Count() > 0) {
            result = StringCompare(results.GetAt(0), compareValue);
        } else {
            result = COMPARISON_NOT_EQUAL;
        }
        
        return 0;
    }
    
    virtual bool SupportsDataType(u8 dataType) const override {
        return dataType == GrapaTokenType::STR;  // XML stored as string
    }
    
private:
    GrapaError XPathQuery(const GrapaCHAR& xml, const GrapaCHAR& xpath, GrapaArray<GrapaCHAR>& results) {
        // Execute XPath query
        return XmlEngine::XPathQuery(xml, xpath, results);
    }
    
    ComparisonResult StringCompare(const GrapaCHAR& left, const GrapaCHAR& right) {
        return GrapaDB2StringComparison::Compare(left, right);
    }
};
```

#### **2. XML Element Comparison**
```cpp
// XML element comparison
class GrapaDB2XmlElementComparison : public GrapaDB2Comparison {
public:
    virtual GrapaError Compare(const GrapaDB2ComparisonContext& context, ComparisonResult& result) override {
        GrapaCHAR& xml = context.leftValue;
        GrapaCHAR& elementPath = context.xmlElementPath;
        GrapaCHAR& compareValue = context.rightValue;
        
        // Extract element value
        GrapaCHAR elementValue;
        GrapaError err = XmlExtract(xml, elementPath, elementValue);
        if (err) return err;
        
        // Compare element value
        result = StringCompare(elementValue, compareValue);
        
        return 0;
    }
    
private:
    GrapaError XmlExtract(const GrapaCHAR& xml, const GrapaCHAR& elementPath, GrapaCHAR& result) {
        // Extract value from XML element
        return XmlEngine::Extract(xml, elementPath, result);
    }
    
    ComparisonResult StringCompare(const GrapaCHAR& left, const GrapaCHAR& right) {
        return GrapaDB2StringComparison::Compare(left, right);
    }
};
```

### **Enhanced Comparison Context**

#### **1. Extended Comparison Context**
```cpp
// Enhanced comparison context for Unicode, regex, and JSON/XML
struct GrapaDB2ComparisonContext {
    // Basic comparison data
    GrapaValue leftValue;
    GrapaValue rightValue;
    u8 leftDataType;
    u8 rightDataType;
    u8 treeType;
    u8 storageType;
    ComparisonMode mode;
    
    // Unicode support
    GrapaCHAR unicodeCollation;
    GrapaCHAR unicodeNormalization;
    
    // Regex support
    GrapaCHAR regexPattern;
    GrapaCHAR regexFlags;
    u32 captureGroup;
    GrapaCHAR compareValue;
    
    // JSON support
    GrapaCHAR jsonPath;
    GrapaCHAR jsonKey;
    GrapaCHAR jsonOperator;  // '->', '->>', '?', etc.
    
    // XML support
    GrapaCHAR xpathExpression;
    GrapaCHAR xmlElementPath;
    GrapaCHAR xmlNamespace;
    
    // Performance optimization
    bool useCache;
    u64 cacheKey;
};
```

#### **2. Comparison Strategy Factory**
```cpp
// Enhanced comparison strategy factory
class GrapaDB2ComparisonStrategyFactory {
public:
    // Get appropriate comparison strategy
    static GrapaDB2Comparison* GetStrategy(const GrapaDB2ComparisonContext& context) {
        // Unicode comparison
        if (context.unicodeCollation.mLength > 0) {
            return new GrapaDB2UnicodeComparison();
        }
        
        // Regex comparison
        if (context.regexPattern.mLength > 0) {
            if (context.regexFlags.mLength > 0) {
                return new GrapaDB2RegexFlagsComparison();
            } else if (context.captureGroup > 0) {
                return new GrapaDB2RegexCaptureComparison();
            } else {
                return new GrapaDB2RegexComparison();
            }
        }
        
        // JSON comparison
        if (context.jsonPath.mLength > 0) {
            if (context.jsonOperator.StrCmp("?") == 0) {
                return new GrapaDB2JsonArrayContainsComparison();
            } else if (context.jsonOperator.StrCmp("?") == 0) {
                return new GrapaDB2JsonKeyComparison();
            } else {
                return new GrapaDB2JsonPathComparison();
            }
        }
        
        // XML comparison
        if (context.xpathExpression.mLength > 0) {
            return new GrapaDB2XPathComparison();
        } else if (context.xmlElementPath.mLength > 0) {
            return new GrapaDB2XmlElementComparison();
        }
        
        // Default to comprehensive comparison
        return new GrapaDB2ComprehensiveComparison();
    }
};
```

### **Performance Optimizations**

#### **1. Unicode Performance**
```cpp
// Unicode performance optimizations
class GrapaDB2UnicodePerformance {
public:
    // Cache normalized Unicode strings
    virtual GrapaError CacheNormalizedString(const GrapaCHAR& original, const GrapaCHAR& normalized) {
        u64 hash = HashString(original);
        return UnicodeCache::Store(hash, normalized);
    }
    
    // Get cached normalized string
    virtual GrapaError GetCachedNormalized(const GrapaCHAR& original, GrapaCHAR& normalized) {
        u64 hash = HashString(original);
        return UnicodeCache::Get(hash, normalized);
    }
    
    // Pre-compute collation tables
    virtual GrapaError PrecomputeCollation(const GrapaCHAR& locale) {
        return UnicodeCollation::Precompute(locale);
    }
};
```

#### **2. Regex Performance**
```cpp
// Regex performance optimizations
class GrapaDB2RegexPerformance {
public:
    // Compile and cache regex patterns
    virtual GrapaError CompileRegex(const GrapaCHAR& pattern, u64& patternId) {
        return RegexEngine::Compile(pattern, patternId);
    }
    
    // Get compiled regex pattern
    virtual GrapaError GetCompiledRegex(u64 patternId, void*& compiledPattern) {
        return RegexEngine::GetCompiled(patternId, compiledPattern);
    }
    
    // Create regex indexes for common patterns
    virtual GrapaError CreateRegexIndex(const GrapaCHAR& tableName, const GrapaCHAR& fieldName, 
                                       const GrapaCHAR& pattern) {
        return RegexIndex::Create(tableName, fieldName, pattern);
    }
};
```

#### **3. JSON/XML Performance**
```cpp
// JSON/XML performance optimizations
class GrapaDB2StructuredDataPerformance {
public:
    // Index common JSON paths
    virtual GrapaError IndexJsonPath(const GrapaCHAR& tableName, const GrapaCHAR& fieldName, 
                                    const GrapaCHAR& path) {
        return JsonIndex::Create(tableName, fieldName, path);
    }
    
    // Index common XPath expressions
    virtual GrapaError IndexXPath(const GrapaCHAR& tableName, const GrapaCHAR& fieldName, 
                                 const GrapaCHAR& xpath) {
        return XmlIndex::Create(tableName, fieldName, xpath);
    }
    
    // Cache parsed JSON/XML structures
    virtual GrapaError CacheParsedStructure(const GrapaCHAR& data, void* parsedStructure) {
        u64 hash = HashString(data);
        return StructureCache::Store(hash, parsedStructure);
    }
};
```

### **SQL Integration Examples**

#### **1. Unicode SQL Examples**
```grapa
/* Unicode comparison in SQL */
SELECT * FROM unicode_data WHERE greeting = 'Hello 世界' COLLATE unicode;
SELECT * FROM unicode_data WHERE greeting ILIKE '%мир%' COLLATE unicode;
SELECT * FROM unicode_data WHERE greeting LIKE '%世界%' COLLATE unicode;

/* Unicode functions */
SELECT id, name, UNICODE_NORMALIZE(greeting, 'NFC') as normalized FROM unicode_data;
SELECT id, name, greeting.len() as char_count FROM unicode_data;
```

#### **2. Regex SQL Examples**
```grapa
/* Regex comparison in SQL */
SELECT * FROM regex_data WHERE email REGEXP '^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$';
SELECT * FROM regex_data WHERE phone REGEXP '^(\+?1-)?555-\d{3}-\d{4}$';
SELECT * FROM regex_data WHERE name REGEXP '^[A-Z][a-z]+ [A-Z][a-z]+$';

/* Regex with flags */
SELECT * FROM regex_data WHERE email REGEXP 'i' '^[a-z]+@';
SELECT * FROM regex_data WHERE description REGEXP 'm' '^engineer$';

/* Regex capture groups */
SELECT id, name, REGEXP_CAPTURE(email, '^([^@]+)@([^@]+)$', 1) as username FROM regex_data;
```

#### **3. JSON SQL Examples**
```grapa
/* JSON comparison in SQL */
SELECT * FROM json_data WHERE metadata->>'city' = 'New York';
SELECT * FROM json_data WHERE metadata->>'age' > 30;
SELECT * FROM json_data WHERE metadata->'skills' ? 'Python';
SELECT * FROM json_data WHERE settings->>'theme' = 'dark';

/* JSON functions */
SELECT id, name, JSON_EXTRACT(metadata, '$.age') as age FROM json_data;
SELECT id, name, JSON_LENGTH(metadata->'skills') as skill_count FROM json_data;
```

#### **4. XML SQL Examples**
```grapa
/* XML comparison in SQL */
SELECT * FROM xml_data WHERE config->>'theme' = 'dark';
SELECT * FROM xml_data WHERE document->>'role' = 'admin';
SELECT * FROM xml_data WHERE document->'permissions'->>'write' = 'true';

/* XPath queries */
SELECT id, name, XPATH(document, '/user/name') as user_name FROM xml_data;
SELECT * FROM xml_data WHERE XPATH(document, '/user/role') = 'admin';

/* XML functions */
SELECT id, name, XML_EXTRACT(document, '/user/name') as extracted_name FROM xml_data;
SELECT id, name, XML_VALID(document) as is_valid_xml FROM xml_data;
```

### **Summary**

The enhanced database-aware comparison system now supports:

1. **Unicode Comparison**: Full Unicode string handling with normalization, collation, and character counting
2. **Regex Comparison**: Powerful regex pattern matching with flags, capture groups, and performance optimization
3. **JSON Comparison**: JSON path queries, array contains, object key checks, and validation
4. **XML Comparison**: XPath queries, element extraction, validation, and transformation
5. **Performance Optimization**: Caching, indexing, and compilation for all comparison types
6. **SQL Integration**: Seamless integration with SQL syntax for all comparison types

These enhancements make GrapaDB2's comparison system competitive with modern database systems while maintaining Grapa's unique capabilities and performance characteristics. 