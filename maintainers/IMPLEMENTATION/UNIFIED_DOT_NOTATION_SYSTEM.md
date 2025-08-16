# Unified Dot Notation System

## Overview

Grapa implements a unified dot notation system that provides consistent access patterns across multiple data types including `$LIST`, `$ARRAY`, `$XML`, `$TAG`, and other complex structures. This system is implemented through the `search` rule in `$grapa.grc` and the `GrapaLibraryRuleSearchEvent` class in C++.

## Grammar Definition

### Rule Definition in `$grapa.grc`
```grapa
@global["$compL3"]
    = rule <$lit> <$comppart> {@<search,{$1,$2}>}
    | <$function> <$comppart> {@<search,{$1,$2}>}
    | <$function>
    ;
```

**Key Points:**
- The `search` rule is triggered for dot notation (`<$lit> <$comppart>`)
- It handles both literal access and function calls
- The rule creates a `search` operation with the target object and the accessor

## C++ Implementation

### `GrapaLibraryRuleSearchEvent::Run`
**Location**: `source/grapa/GrapaLibRule.cpp:5992-6091`

**Core Functionality:**
- **Unified Type Handling**: Single implementation handles multiple data types
- **Class Resolution**: Automatically resolves appropriate class for each type
- **Queue Management**: Manages internal data structures for each type

### Type-Specific Handling

#### 1. Array/Tuple Handling
```cpp
case GrapaTokenType::ARRAY:
    q = (GrapaRuleQueue*)item->vQueue;
    if (item->vClass == NULL)
        item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$ARRAY"));
    c = item->vClass;
    root = item;
    break;
```

#### 2. List Handling
```cpp
case GrapaTokenType::LIST:
    q = (GrapaRuleQueue*)item->vQueue;
    if (item->vClass == NULL)
        item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$LIST"));
    c = item->vClass;
    root = item;
    break;
```

#### 3. XML/TAG Handling
```cpp
case GrapaTokenType::XML:
    q = (GrapaRuleQueue*)item->vQueue;
    if (item->vClass == NULL)
        item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$XML"));
    c = item->vClass;
    root = item;
    break;
case GrapaTokenType::TAG:
    q = (GrapaRuleQueue*)item->vQueue;
    if (item->vClass == NULL)
        item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$TAG"));
    c = item->vClass;
    root = item;
    break;
```

### Property Search Behavior

**Location**: `source/grapa/GrapaObject.cpp` - `GrapaObjectQueue::SearchCase()`

**Search Algorithm**: The property resolution system searches through `$LIST` objects in **reverse order** (tail to head):

```cpp
GrapaObjectEvent* GrapaObjectQueue::SearchCase(const GrapaCHAR& pName, s64& pIndex, bool pLower)
{
    GrapaObjectEvent* item = Tail();  // Start from TAIL (last element)
    pIndex = mCount - 1;              // Start from last index
    while (item)
    {
        // ... search logic ...
        if (pLower && item->mName.StrLowerCmp(pName) == 0)
            return(item);  // Return last match (first found when going backwards)
        else if (!pLower && item->mName.StrCmp(pName) == 0)
            return(item);  // Return last match (first found when going backwards)
        // ...
        pIndex--;
        item = item->Prev();  // Move backward
    }
    return(NULL);
}
```

**Key Characteristics**:
- **Reverse Search Order**: Searches from tail to head (last to first)
- **Last Value Wins**: When duplicate keys exist, the last occurrence takes precedence
- **JSON Compatibility**: Matches behavior of JSON in other programming languages
- **Index Access Unchanged**: Array-style indexing (`obj[0]`, `obj[1]`) uses position-based access

**Example Behavior**:
```grapa
obj = {name: "Alice", age: 30, name: "Bob"};
obj.name;  // Returns "Bob" (last value)

obj2 = {a: 1, b: 2, a: 3, c: 4, b: 5};
obj2.a;    // Returns 3 (last value)
obj2.b;    // Returns 5 (last value)
obj2.c;    // Returns 4 (only value)
```

**Implementation Date**: January 2025 - Changed from "first match" to "last match" behavior

## Internal Data Structure

### XML/HTML Internal Representation

XML and HTML in Grapa use the same internal structure as JSON (`$LIST`/`$ARRAY`), but with special handling for:

1. **Element Hierarchy**: XML elements are stored as nested structures
2. **Attribute Storage**: Attributes are stored as key-value pairs within elements
3. **Content Storage**: Text content is stored as child elements

### XML to LIST Conversion

The `.list()` method provides direct conversion from XML to LIST format:

**Implementation**: `GrapaLibraryRuleListEvent::Run` (lines 15471-15509)
```cpp
case GrapaTokenType::TAG:
case GrapaTokenType::EL:
case GrapaTokenType::XML:
case GrapaTokenType::ARRAY:
case GrapaTokenType::TUPLE:
case GrapaTokenType::LIST:
case GrapaTokenType::ERR:
    result = vScriptExec->CopyItem(r1.vVal, true);
    result->mValue.mToken = GrapaTokenType::LIST;
    break;
```

**Conversion Pattern**:
- **Simple Element**: `<item>test</item>` → `{{"item":[{},["test"]]}}`
- **Element with Attributes**: `<div class="main">content</div>` → `{{"div":[{"class":"main"},["content"]]}}`
- **Nested Structure**: Complex XML hierarchies are converted to nested LIST structures

**Usage**:
```grapa
xml = <user><name>Alice</name><age>30</age></user>;
list = xml.list();  /* Convert XML to LIST */
```

### XML and LIST Integration

Grapa provides seamless integration between XML and LIST types, allowing XML structures to be embedded within LIST objects and accessed using dot notation.

**Implementation**: `GrapaLibraryRuleCreateXmlEvent::Run` (lines 5534-5536)
```cpp
GrapaRuleEvent* GrapaLibraryRuleCreateXmlEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
    return ItemPrependRun(vScriptExec, pNameSpace, pOperation, pInput, GrapaCHAR("createxml"));
}
```

**Grammar Rules**: `lib/grapa/$grapa.grc` (lines 440-467)
```grapa
@global["$xmlterm"]
    = rule '<' '/' <$elID> '>' {@<createxml,{}>}
    | '<' '/' '>' {@<createxml,{}>}
    | '<' '>' {@<createxml,{}>}
    | <$tagcreate> <$xmlterm> {@<prepend,{$2,$1}>}
    | <$tagcreate> <@<join,{$1}>> <$xmlterm> {@<prepend,{$3,$2,$1}>}
    ;
```

**Integration Patterns**:

#### 1. XML Embedding in LIST
```grapa
/* XML can be embedded directly in LIST */
arr = {a:5, b:<div class="main"><h1>Title</h1></div>, c:"hi"};
/* Result: {"a":5,"b":<div class="main"><h1>Title</h1></div><>,"c":"hi"} */
```

#### 2. Dot Notation Access
```grapa
/* Access XML elements using dot notation from LIST */
arr = {a:5, b:<div><h1>Title</h1><p>Content</p></div>, c:"hi"};
arr.b[0].h1[0].echo();  /* "Title" */
arr.b[0].p[0].echo();   /* "Content" */
```

#### 3. LIST Embedding in XML
```grapa
/* LIST can be embedded within XML */
xml = <root><data>{a:1, b:2, c:3}</data></root>;
/* Note: LIST is treated as string content within XML */
```

**Key Features**:
- **Automatic `<>` Syntax**: XML embedded in LIST automatically gets `<>` appended
- **Direct Element Access**: Use dot notation to access XML elements from LIST
- **Mixed Content Support**: Combine structured data with XML templates
- **Unified Access Patterns**: Same dot notation works across both types

**Internal Structure Example:**
```grapa
/* User sees: */
<div class="main"><h1>Title</h1></div>

/* Internal representation (simplified): */
{
    "tag": "div",
    "attributes": {"class": "main"},
    "children": [
        {
            "tag": "h1",
            "content": "Title"
        }
    ]
}
```

### Access Patterns

#### 1. Array/List Access
```grapa
/* Standard array access */
arr = [1, 2, 3, 4, 5];
arr[0];     /* Direct index access */
arr.first;  /* Property access (if defined) */
```

#### 2. XML/HTML Access
```grapa
/* XML element access */
xml = <root><item>1</item><item>2</item></root>;
xml[0];           /* Root element */
xml[0][0];        /* First child element */
xml[0][0][0];     /* Text content of first child */

/* HTML structure access */
html = <html><body><div class="main"><h1>Title</h1></div></body></html>;
html[0][0][0][0][0];  /* Navigate to "Title" */
```

## Interchangeable Access Methods

### Dot vs Bracket Notation
The system supports both dot notation and bracket notation interchangeably:

```grapa
/* These are equivalent for arrays/lists */
data = {a:1, b:2, c:3};
data.a;     /* Dot notation */
data["a"];  /* Bracket notation */

/* These work for XML structures */
xml = <root><item>1</item></root>;
xml[0];     /* Bracket notation */
xml.root;   /* Dot notation (if element name matches) */
```

## Special XML/HTML Considerations

### 1. Element Name Access
```grapa
/* Access by element name */
xml = <root><item>1</item><item>2</item></root>;
xml.item;   /* Access all 'item' elements */
```

### 2. Attribute Access
```grapa
/* Access attributes */
div = <div class="main" id="content">Text</div>;
div.class;  /* Access class attribute */
div.id;     /* Access id attribute */
```

### 3. Content Access
```grapa
/* Access text content */
h1 = <h1>Title</h1>;
h1[0];      /* Access text content directly */
```

## Implementation Details

### Class Resolution
Each data type automatically gets its appropriate class assigned:

```cpp
if (item->vClass == NULL)
    item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$XML"));
```

### Queue Management
The system uses `GrapaRuleQueue` for managing internal data structures:

```cpp
q = (GrapaRuleQueue*)item->vQueue;
```

### Error Handling
The system gracefully handles missing properties and invalid access:

```cpp
case GrapaTokenType::ERR:
    q = (GrapaRuleQueue*)item->vQueue;
    if (item->vRulePointer)
    {
        item->vRulePointer = NULL;
        item->vClass = NULL;
    }
    break;
```

## Performance Considerations

### 1. Lazy Class Loading
Classes are loaded only when needed:
```cpp
if (item->vClass == NULL)
    item->vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$XML"));
```

### 2. Pointer Optimization
The system uses pointer dereferencing for efficient access:
```cpp
while (item->mValue.mToken == GrapaTokenType::PTR && item->vRulePointer) 
    item = item->vRulePointer;
```

### 3. Memory Management
Proper cleanup of temporary objects:
```cpp
if (r1Del) { r1Del->CLEAR(); delete r1Del; }
```

## Design Philosophy

### 1. Unified Interface
- **Consistency**: Same access patterns across all data types
- **Simplicity**: Users don't need to learn different syntax for different types
- **Flexibility**: Support for both dot and bracket notation

### 2. Late-Binding
- **Runtime Resolution**: Type checking and class loading at runtime
- **Dynamic Access**: Support for dynamic property access
- **Error Tolerance**: Graceful handling of missing properties

### 3. Extensibility
- **Type Agnostic**: Easy to add new data types
- **Class System**: Leverages Grapa's class system for type-specific behavior
- **Plugin Architecture**: New access patterns can be added through classes

## Future Enhancements

### 1. Enhanced XML Support
- **XPath-like Queries**: More sophisticated XML navigation
- **Attribute Filtering**: Advanced attribute-based selection
- **Content Search**: Text-based content searching

### 2. Performance Optimizations
- **Caching**: Cache frequently accessed properties
- **Indexing**: Build indexes for large structures
- **Lazy Evaluation**: Defer computation until needed

### 3. Advanced Features
- **Method Chaining**: Support for method chaining on accessed properties
- **Type Inference**: Automatic type detection and conversion
- **Validation**: Runtime validation of access patterns

## .unique() Function Enhancement

The `.unique()` function was enhanced to properly handle duplicate keys in objects while maintaining the "last value wins" principle.

### Implementation Details

**File:** `source/grapa/GrapaLibRule.cpp`  
**Method:** `GrapaLibraryRuleUniqueEvent::Run` (lines 8336-8450)

**Key Changes:**
- **Post-Processing Approach**: Added special post-processing logic for `$LIST` and `$OBJ` types
- **Sort Preservation**: Kept existing sort logic unchanged to avoid impacting `.sort()` behavior
- **Reverse Traversal**: Post-processing traverses sorted items in reverse order to keep last occurrences
- **Type-Specific Logic**: Different behavior for objects (key-based) vs lists (value-based)

**Code Structure:**
```cpp
// For LIST and OBJ types, use post-processing to ensure "last value wins" behavior
if (result->mValue.mToken == GrapaTokenType::LIST || result->mValue.mToken == GrapaTokenType::OBJ)
{
    // Post-process: traverse in reverse order to keep last occurrence of each key
    GrapaRuleQueue* tempQueue = new GrapaRuleQueue();
    GrapaRuleEvent* last = NULL;
    
    // Traverse from end to beginning to keep last occurrence
    for (u64 j = i; j > 0; j -= 3)
    {
        u64 idx = j - 3;
        bool match = false;
        
        if (last)
        {
            // For LIST/OBJ, compare only by name (simple string comparison)
            match = (last->mName.StrCmp(rq[idx]->mName) == 0);
        }
        
        if (!match)
        {
            // Keep this item (it's the last occurrence of its key)
            tempQueue->PushHead((GrapaRuleEvent*)rq[idx]);
            last = (GrapaRuleEvent*)rq[idx];
            rq[idx] = NULL;
        }
        else
        {
            // Remove this duplicate (earlier occurrence)
            delete (GrapaRuleEvent*)rq[idx];
            rq[idx] = NULL;
        }
    }
    
    // Replace the original queue with our post-processed one
    delete r;
    result->vQueue = tempQueue;
}
```

### Behavior

**For Objects (`$OBJ`):**
- Removes duplicate keys, keeping the last value for each key
- Consistent with property access "last value wins" behavior
- Example: `{a:1, b:2, a:3, c:4, b:5}.unique()` → `{"a":3, "b":5, "c":4}`

**For Lists (`$LIST`):**
- Removes duplicate values, keeping the last occurrence of each value
- Works with mixed data types
- Example: `[1, 2, 1, 3, 2, 4].unique()` → `[1, 2, 3, 4]`

### Implementation Date

This enhancement was implemented as part of the Phase 2B user experience improvements to fix the bug where `.unique()` was not properly removing duplicate keys in objects.

## See Also
- [Rule System Architecture](RULE_SYSTEM_ARCHITECTURE.md)
- [Array and List Operators Analysis](ARRAY_LIST_OPERATORS_ANALYSIS.md)
- [Op Function Parameter Matching](OP_FUNCTION_PARAMETER_MATCHING.md) 