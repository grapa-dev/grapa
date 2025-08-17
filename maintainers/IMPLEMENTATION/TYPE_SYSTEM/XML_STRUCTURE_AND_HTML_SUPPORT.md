---
tags:
  - maintainer
  - implementation
  - type-system
  - xml
  - html
  - markup
  - structure
---

# XML Structure and HTML Support

This document provides comprehensive implementation details for Grapa's XML type structure and its ability to represent HTML documents.

## Overview

**Purpose:** XML document representation with HTML support
**Implementation:** Hierarchical tree structure using GrapaRuleQueue
**HTML Support:** Full HTML parsing and representation capabilities
**Status:** Complete XML/HTML implementation

## XML Type Structure

### **Core XML Implementation**

#### **Type Hierarchy**
```
XML (GrapaTokenType::XML = 7)
├── EL (GrapaTokenType::EL = 15) - XML Elements
└── TAG (GrapaTokenType::TAG = 16) - XML/HTML Tags
```

#### **Class Definitions**
```grapa
@global["$XML"] = class ($ARRAY) {
    // XML inherits from $ARRAY for collection behavior
};

@global["$EL"] = class ($XML) {
    // Elements inherit from XML
};

@global["$TAG"] = class ($XML) {
    // Tags inherit from XML
};
```

#### **Internal Structure**
```cpp
// XML is stored as a GrapaRuleEvent with:
class GrapaRuleEvent {
    GrapaValue mValue;           // Token type = GrapaTokenType::XML
    GrapaRuleQueue* vQueue;      // Child elements/tags
    GrapaRuleEvent* vClass;      // Class reference ($XML, $EL, $TAG)
    GrapaCHAR mName;             // Element/tag name
    // ... other fields
};
```

### **XML Creation and Parsing**

#### **Grammar Rules for XML Creation**
```grapa
// XML creation grammar from $grapa.grc
@global["$xmlcreate"]
    = rule '<' '/' '>' {@<createxml,{}>}           // Empty closing tag
    | '<' '>' {@<createxml,{}>}                    // Empty tag
    | <$tagHtmlHead> <$htmlcreate> {@<prepend,{$2,$1}>}  // HTML DOCTYPE
    | <$tagHtmlHead> {@<createxml,{$1}>}           // HTML head
    | <$tagcreate> <$xmlcreate> {@<prepend,{$2,$1}>}     // Tag with content
    | <$tagcreate> <@<join,{$1}>> <$xmlcreate> {@<prepend,{$3,$2,$1}>}  // Tag with text
    | <$tagcreate> {@<createxml,{$1}>}             // Single tag
    ;
```

#### **Tag Creation Grammar**
```grapa
@global["$tagcreate"]
    = rule <$tagValue> {@<createtag,{$1}>}
    ;

@global["$tagValue"]
    = rule '<' '!' '-' <$elSkipValue> {@<name,{"!--",$4}>}           // Comments
    | '<' '!' '[' CDATA <$tagDataValue> {@<name,{"![CDATA[",$5}>}    // CDATA sections
    | '<' '!' DOCTYPE <$tagDocValue> '>' {@<name,{"!DOCTYPE",$4}>}   // DOCTYPE
    | '<' script <$tagScriptValue> {@<name,{"script",$3}>}           // Script tags
    | '<' <$elSelfClosingID> <$elQvalue> '/' '>' {@<name,{$2,$3}>}   // Self-closing tags
    | '<' <$elQname> <$elQvalue> '?' '>' {@<name,{$2,$3}>}           // Processing instructions
    | '<' <$elID> <$elValue> {@<name,{$2,$3}>}                       // Regular elements
    ;
```

## HTML Support

### **HTML-Specific Grammar Rules**

#### **HTML Creation**
```grapa
@global["$htmlcreate"]
    = rule '<' '/' '>' {@<createxml,{}>}           // Empty closing tag
    | '<' '>' {@<createxml,{}>}                    // Empty tag
    | <$tagHtmlHead> <$htmlcreate> {@<prepend,{$2,$1}>}  // HTML DOCTYPE
    | <$tagHtmlHead> {@<createxml,{$1}>}           // HTML head
    | <$tagHtmlcreate> <$htmlcreate> {@<prepend,{$2,$1}>} // HTML tag with content
    | <$tagHtmlcreate> <@<join,{$1}>> <$htmlcreate> {@<prepend,{$3,$2,$1}>} // HTML with text
    | <$tagHtmlcreate> {@<createxml,{$1}>}         // Single HTML tag
    ;
```

#### **HTML Tag Values**
```grapa
@global["$tagHtmlValue"]
    = rule '<' '!' '-' <$elSkipValue> {@<name,{"!--",$4}>}           // HTML comments
    | '<' '!' '[' CDATA <$tagDataValue> {@<name,{"![CDATA[",$5}>}    // CDATA sections
    | '<' '!' DOCTYPE <$tagDocHtmlValue> '>' {@<name,{"!DOCTYPE",$4}>} // HTML DOCTYPE
    | '<' script <$tagScriptValue> {@<name,{"script",$3}>}           // Script tags
    | '<' <$elSelfClosingID> <$elQvalue> '/' '>' {@<name,{$2,$3}>}   // Self-closing HTML tags
    | '<' <$elSelfClosingID> <$elQvalue> '>' {@<name,{$2,$3}>}       // Self-closing HTML tags
    | '<' <$elQname> <$elQvalue> '?' '>' {@<name,{$2,$3}>}           // Processing instructions
    | '<' <$elID> <$elHtmlValue> {@<name,{$2,$3}>}                   // HTML elements
    ;
```

### **HTML DOCTYPE Support**
```grapa
@global["$tagDocHtmlValue"]
    = rule html <$tagDocHtmlValueY> {@<prepend,{$2,$1}>}     // HTML root
    | html {@<createarray,{$1}>}                              // HTML root
    | HTML <$tagDocHtmlValueY> {@<prepend,{$2,$1}>}          // HTML root (uppercase)
    | HTML {@<createarray,{$1}>}                              // HTML root (uppercase)
    ;

@global["$tagDocHtmlValueY"]
    = rule <$litname> <$tagDocHtmlValueY> {@<prepend,{$2,$1}>} // DOCTYPE attributes
    | <$litname> {@<createarray,{$1}>}                         // DOCTYPE attributes
    ;
```

## XML/HTML Element Structure

### **Element Attributes**
```grapa
@global["$elQvalue"]
    = rule <$elAttrList> {@<createel,{$1}>}
    ;

@global["$elAttrList"]
    = rule <$elAttr> <$elAttrList> {@<prepend,{$2,$1}>}  // Multiple attributes
    | <$elAttr> {@<createarray,{$1}>}                     // Single attribute
    ;
```

### **Element Content**
```grapa
@global["$elValue"]
    = rule <$elContent> <$elValue> {@<prepend,{$2,$1}>}  // Multiple content items
    | <$elContent> {@<createarray,{$1}>}                  // Single content item
    ;

@global["$elContent"]
    = rule <$elID> {@<createel,{$1}>}                     // Child element
    | <$elText> {@<createxml,{$1}>}                       // Text content
    | <$elSkipValue> {@<createel,{{},$1}>}                // Comment/CDATA
    ;
```

## Special Content Types

### **Comments**
```grapa
@global["$elSkipValue"]
    = rule <$elSkipValueX> {@<createel,{{},$1}>}
    ;

@global["$elSkipValueX"]
    = rule <$elSkipValueY> {@<createxml,{$1}>}
    ;

@global["$elSkipValueY"]
    = rule '-' <@<join,{$1}>> ('-' '-' '>') {@<lit,{$2}>}  // <!-- comment -->
    ;
```

### **CDATA Sections**
```grapa
@global["$tagDataValue"]
    = rule <$tagDataValueX> {@<createel,{{},$1}>}
    ;

@global["$tagDataValueX"]
    = rule <$tagDataValueY> {@<createxml,{$1}>}
    ;

@global["$tagDataValueY"]
    = rule '[' <@<join,{$1}>> (']' ']' '>') {@<lit,{$2}>}  // <![CDATA[ content ]]>
    ;
```

### **Script Content**
```grapa
@global["$tagScriptValue"]
    = rule <$elAttrList> <$tagScriptValueX> {@<createel,{$1,$2}>}  // Script with attributes
    | <$elAttrList> '/' '>' {@<createel,{$1}>}                     // Empty script
    ;

@global["$tagScriptValueX"]
    = rule <$tagScriptValueY> {@<createxml,{$1}>}
    ;

@global["$tagScriptValueY"]
    = rule '>' <@<join,{$1}>> ('<' '/' script '>') {@<name,{null,$2}>}  // <script>content</script>
    ;
```

## Implementation Details

### **C++ XML Handling**

#### **XML Event Classes**
```cpp
// XML creation event
class GrapaLibraryRuleCreateXmlEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleCreateXmlEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};

// XML processing event
class GrapaLibraryRuleXmlEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleXmlEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
```

#### **XML Processing Implementation**
```cpp
GrapaRuleEvent* GrapaLibraryRuleXmlEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
    GrapaRuleEvent* result = NULL;
    GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
    if (r1.vVal)
    {
        switch (r1.vVal->mValue.mToken)
        {
        case GrapaTokenType::RAW:
        case GrapaTokenType::STR:
            if (r1.vVal->mValue.mLength)
            {
                // Parse XML string using $function
                GrapaRuleEvent* rulexx = vScriptExec->vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$function"));
                GrapaCHAR val;
                val.Append("$&");
                val.Append(r1.vVal->mValue);
                val.Append("$&");
                GrapaRuleEvent* plan = vScriptExec->Plan(pNameSpace, val, rulexx, 0, GrapaCHAR());
                result = vScriptExec->ProcessPlan(pNameSpace, plan);
                if (plan)
                {
                    plan->CLEAR();
                    delete plan;
                }
            }
            break;
        default:
            break;
        }
    }
    return(result);
}
```

### **Database Storage**

#### **XML Field Metadata**
```cpp
// XML requires field reference size of 5
case GrapaTokenType::XML:
    if (field->mRef < 5)
        field->mRef = 5;
    break;
```

#### **XML Serialization**
```cpp
// XML is serialized as a queue of child elements
case GrapaTokenType::XML:
    if (vQueue) ((GrapaRuleQueue*)vQueue)->TO(pValue, vClass, mValue.mToken);
    pValue.mToken = mValue.mToken;
    break;
```

## Usage Patterns

### **XML Creation**
```grapa
/* Create XML document */
xml_doc = $XML().create();

/* Parse XML string */
xml_doc = $XML().parse("<root><item>value</item></root>");

/* Create HTML document */
html_doc = $XML().parse("<!DOCTYPE html><html><head><title>Page</title></head><body>Content</body></html>");
```

### **Element Manipulation**
```grapa
/* Create elements */
element = $EL().create("div", {"class": "container", "id": "main"});
text_element = $EL().create("p", "This is a paragraph");

/* Add content */
element.append(text_element);
element.append("Additional text content");

/* Access attributes */
class_name = element.get("class");
element.set("style", "color: red;");
```

### **HTML-Specific Operations**
```grapa
/* HTML document structure */
html_doc = $XML().parse("""
<!DOCTYPE html>
<html>
<head>
    <title>My Page</title>
    <script>console.log('Hello');</script>
</head>
<body>
    <div class="container">
        <h1>Title</h1>
        <p>Content</p>
    </div>
</body>
</html>
""");

/* Access HTML elements */
title = html_doc.find("head/title").text();
scripts = html_doc.find("script");
container = html_doc.find("body/div.container");
```

### **Comment and CDATA Handling**
```grapa
/* XML comments */
xml_doc = $XML().parse("""
<root>
    <!-- This is a comment -->
    <item>value</item>
    <![CDATA[<special>content</special>]]>
</root>
""");

/* Access comments */
comments = xml_doc.find("comment()");
cdata_sections = xml_doc.find("text()[contains(., 'CDATA')]");
```

## Performance Characteristics

### **XML Operations Performance**
| Operation | Complexity | Memory | Notes |
|-----------|------------|--------|-------|
| Creation | O(1) | Fixed | Simple XML creation |
| Parsing | O(n) | Dynamic | String to XML parsing |
| Element Access | O(n) | Fixed | Tree traversal |
| Attribute Access | O(n) | Fixed | Linear search |
| Serialization | O(n) | Dynamic | Tree to string |

### **Memory Usage**
- **XML Structure**: Hierarchical tree using GrapaRuleQueue
- **Element Storage**: Each element stored as GrapaRuleEvent
- **Attribute Storage**: Attributes stored as child elements
- **Text Content**: Text stored as XML nodes with string values

## Error Handling

### **XML Parsing Errors**
```grapa
/* Handle parsing errors */
try {
    xml_doc = $XML().parse(malformed_xml);
} catch (error) {
    // Handle parsing failure
    error_message = error.message();
}
```

### **Validation**
```grapa
/* Validate XML structure */
if (xml_doc.is_valid()) {
    // XML is well-formed
    process_xml(xml_doc);
} else {
    // Handle validation errors
    errors = xml_doc.validation_errors();
}
```

## Integration with Other Systems

### **Database Integration**
```grapa
/* Store XML in database */
table = $TABLE().create("documents", {
    "id": $INT,
    "title": $STR,
    "content": $XML
});

table.insert({
    "id": 1,
    "title": "Sample Document",
    "content": xml_doc
});
```

### **Network Integration**
```grapa
/* XML over HTTP */
response = $net().get("https://api.example.com/data.xml");
xml_data = $XML().parse(response.body());

/* HTML scraping */
html_page = $net().get("https://example.com");
page_content = $XML().parse(html_page.body());
title = page_content.find("head/title").text();
```

## Future Enhancements

### **Planned Features**
- **XPath Support**: Full XPath query language implementation
- **XSLT Support**: XML transformation capabilities
- **Schema Validation**: XML Schema and DTD validation
- **Namespace Support**: XML namespace handling
- **HTML5 Support**: Enhanced HTML5 element support

### **Performance Improvements**
- **Indexed Access**: Faster element access with indexing
- **Lazy Parsing**: Parse XML on-demand for large documents
- **Memory Pooling**: Efficient memory management for XML operations
- **Parallel Processing**: Parallel XML parsing for large documents

## Related Documentation

- **[TYPE_DEFINITIONS_AND_CLASSIFICATIONS.md](TYPE_DEFINITIONS_AND_CLASSIFICATIONS.md)** - Complete type definitions
- **[TYPE_SYSTEM_ARCHITECTURE.md](TYPE_SYSTEM_ARCHITECTURE.md)** - Type system architecture
- **[../LANGUAGE_FEATURES/UNIFIED_DOT_NOTATION_SYSTEM.md](../LANGUAGE_FEATURES/UNIFIED_DOT_NOTATION_SYSTEM.md)** - Dot notation for XML access
- **[../NETWORKING/WEB_SCRAPING_IMPLEMENTATION.md](../NETWORKING/WEB_SCRAPING_IMPLEMENTATION.md)** - Web scraping with XML/HTML
