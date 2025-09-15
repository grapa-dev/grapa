---
title: XML and TAG Operations
description: Comprehensive XML and HTML support with powerful querying capabilities
---

# XML and TAG Operations

Grapa provides **enterprise-grade** XML and HTML support with sophisticated querying capabilities that rival XPath in many ways.

## Overview

XML and TAG types in Grapa are designed for structured document processing and web content generation. They support hierarchical structures with attributes, content, and nested elements using **unified dot notation** and **powerful query patterns**.

## Basic Operations

### XML/HTML Creation
```grapa
/* Create XML element */
xml = <root><item id="1">Hello</item><item id="2">World</item></root>;

/* Create HTML element */
html = <html><head><title>Test</title></head><body><h1>Hello</h1><p>World</p></body></html>;

/* Create from string */
xml_str = "<root><item>1</item></root>";
xml = xml_str.xml();
```

### Type Information
```grapa
xml = <root><item>1</item></root>;
xml.type();                    /* Returns: $XML */
xml[0].type();                 /* Returns: $TAG */
```

## XML Structure Access

### Array-like Element Access
```grapa
xml = <root><item id="1">Hello</item><item id="2">World</item></root>;

/* Access root element (required for $XML) */
xml[0]                         /* Returns: <root><item id="1">Hello</item><item id="2">World</item></root> */

/* Access child elements */
xml[0][0]                      /* Returns: <item id="1">Hello</item> */
xml[0][1]                      /* Returns: <item id="2">World</item> */

/* Access element content */
xml[0][0][0]                   /* Returns: "Hello" */
xml[0][1][0]                   /* Returns: "World" */

/* Get element count */
xml[0].len()                   /* Returns: 2 (number of child elements) */
xml[0][0].len()                /* Returns: 1 (content count) */
```

### Dot Notation Access
```grapa
xml = <root><item id="1">Hello</item><item id="2">World</item></root>;

/* Direct dot notation access */
xml.root                       /* Returns: <root><item id="1">Hello</item><item id="2">World</item></root> */
xml.root.item                  /* Returns: <item id="1">Hello</item> */
xml.root.item[0]               /* Returns: "Hello" */

/* Access nested structures */
xml.root.item.attr             /* Returns: {"id":"1"} */
xml.root.item.attr.id          /* Returns: "1" */
```

### Attribute Access
```grapa
xml = <root><item id="1" class="test">Hello</item><item id="2">World</item></root>;

/* Access all attributes of an element */
xml[0][0].attr                 /* Returns: {"id":"1","class":"test"} */

/* Access specific attributes */
xml[0][0].attr.id              /* Returns: "1" */
xml[0][0].attr.class           /* Returns: "test" */

/* Access attributes via dot notation */
xml.root.item.attr.id          /* Returns: "1" */
xml.root.item.attr.class       /* Returns: "test" */
```

### XML Rotation

XML structures support left and right rotation operations for reordering child elements:

#### Left Rotation (.lrot())
```grapa
xml = <root><item id="1">First</item><item id="2">Second</item><item id="3">Third</item></root>;

xml.lrot();                              /* <root><item id="2">Second</item><item id="3">Third</item><item id="1">First</item></root> */
xml.lrot(2);                             /* <root><item id="3">Third</item><item id="1">First</item><item id="2">Second</item></root> */
xml.lrot(0);                             /* <root><item id="1">First</item><item id="2">Second</item><item id="3">Third</item></root> (no change) */
```

#### Right Rotation (.rrot())
```grapa
xml = <root><item id="1">First</item><item id="2">Second</item><item id="3">Third</item></root>;

xml.rrot();                              /* <root><item id="3">Third</item><item id="1">First</item><item id="2">Second</item></root> */
xml.rrot(2);                             /* <root><item id="2">Second</item><item id="3">Third</item><item id="1">First</item></root> */
xml.rrot(0);                             /* <root><item id="1">First</item><item id="2">Second</item><item id="3">Third</item></root> (no change) */
```

> **Note:** Rotation methods preserve all child elements and their attributes while reordering them. For empty XML or single-element XML, rotation has no effect.

## Advanced Element Finding (.findall())

The `.findall()` method provides **enterprise-grade querying capabilities** with support for complex patterns and logical operations.

### Basic Queries
```grapa
xml = <root><item id="1">Hello</item><item id="2">World</item></root>;

/* Find by tag name */
xml.findall({name:"item"})     /* Returns: <item id="1">Hello</item><item id="2">World</item> */

/* Find by content value */
xml.findall({value:"Hello"})   /* Returns: <item id="1">Hello</item> */

/* Find by attributes */
xml.findall({attr:{id:"2"}})   /* Returns: <item id="2">World</item> */

/* Find by tag name and attributes */
xml.findall({name:"item", attr:{id:"1"}}) /* Returns: <item id="1">Hello</item> */
```

### Complex Attribute Queries
```grapa
xml = <root><item id="1" class="test">Hello</item><item id="2" class="prod">World</item></root>;

/* Multiple attributes */
xml.findall({attr:{id:"1", class:"test"}}) /* Returns: <item id="1" class="test">Hello</item> */

/* Tag name with multiple attributes */
xml.findall({name:"item", attr:{id:"1", class:"test"}}) /* Returns: <item id="1" class="test">Hello</item> */
```

### Logical Operators
```grapa
xml = <root><item id="1">Hello</item><title>Test</title></root>;

/* AND logic */
xml.findall({and:[{name:"item"}, {attr:{id:"1"}}]}) /* Returns: <item id="1">Hello</item> */

/* OR logic */
xml.findall({or:[{name:"item"}, {name:"title"}]}) /* Returns: <item id="1">Hello</item><title>Test</title> */

/* NAND logic */
xml.findall({nand:[{name:"item"}, {attr:{id:"2"}}]}) /* Returns: <item id="1">Hello</item> */
```

### Content Matching
```grapa
xml = <root><item id="1">Hello</item><item id="2">World</item></root>;

/* Exact content match */
xml.findall({value:"Hello"})   /* Returns: <item id="1">Hello</item> */

/* Multiple content values */
xml.findall({value:["Hello", "World"]}) /* Returns: <item id="1">Hello</item><item id="2">World</item> */
```

### Recursive Search
```grapa
xml = <root><header><item id="1">Hello</item></header><body><item id="2">World</item></body></root>;

/* Searches through ALL nested elements */
xml.findall({name:"item"})     /* Returns: <item id="1">Hello</item><item id="2">World</item> */
```

### Working with .findall() Results
```grapa
xml = <root><item id="1">Hello</item><item id="2">World</item><item id="3">Another</item></root>;

/* Get all results */
results = xml.findall({name:"item"});
results.len()                  /* Returns: 3 */

/* Access individual results */
first = results[0];            /* Returns: <item id="1">Hello</item> */
second = results[1];           /* Returns: <item id="2">World</item> */

/* Access attributes of results */
first.attr                     /* Returns: {"id":"1"} */
first.attr.id                  /* Returns: "1" */
first[0]                       /* Returns: "Hello" */
```

### HTML Element Finding
```grapa
html = <html><head><title>Test</title></head><body><h1>Hello</h1><p>World</p><p>Another</p><div><p>Nested</p></div></body></html>;

/* Find all paragraph elements (including nested) */
html.findall({name:"p"})       /* Returns: <p>World</p><p>Another</p><p>Nested</p> */

/* Find all heading elements */
html.findall({name:"h1"})      /* Returns: <h1>Hello</h1> */

/* Find elements by class */
html.findall({name:"div", attr:{class:"container"}})
```

## XML to LIST Conversion

### Using .list() Method
```grapa
xml = <root><item id="1">Hello</item><item id="2">World</item></root>;

/* Convert XML to LIST structure */
list = xml.list();             /* Returns: {{"root":[{},[{"item":[{"id":"1"},["Hello"]]},{"item":[{"id":"2"},["World"]]}]]}} */

/* Access converted data */
list.root[1][0].item[1][0]     /* Returns: "Hello" */
list.root[1][1].item[1][0]     /* Returns: "World" */
```

### Complex Structure Conversion
```grapa
xml = <html><head><title>Test</title></head><body><h1>Hello</h1></body></html>;

/* Convert complex nested structure */
list = xml.list();             /* Complex nested LIST structure */

/* Access nested elements */
list.html[1][0].head[1][0].title[1][0]  /* Returns: "Test" */
list.html[1][1].body[1][0].h1[1][0]     /* Returns: "Hello" */
```

## String Output

### Converting to String
```grapa
xml = <root><item id="1">Hello</item><item id="2">World</item></root>;

/* Get XML string representation */
xml.echo();                    /* Outputs: <root><item id="1">Hello</item><item id="2">World</item></root> */
xml.str();                     /* Returns: <root><item id="1">Hello</item><item id="2">World</item></root> */

/* Get string representation of individual elements */
xml[0][0].str();               /* Returns: <item id="1">Hello</item> */
```

## Integration with Other Types

### XML and Arrays
```grapa
xml = <root><item>1</item><item>2</item><item>3</item></root>;

/* Iterate over elements */
for (i = 0; i < xml[0].len(); i++) {
    xml[0][i].echo();          /* Output each item element */
}
```

### XML and Lists
```grapa
/* XML elements behave like lists for attribute access */
first_item = xml[0][0];        /* First item element */
first_item.attr.id             /* Access attribute like a list property */

/* Convert XML to LIST for complex processing */
list_data = xml.list();        /* Full LIST conversion */
```

## Performance Considerations

- **Efficient Parsing**: XML is parsed efficiently during creation
- **Array-like Access**: Direct indexing provides fast element access
- **Powerful Finding**: `.findall()` method supports complex queries with logical operators
- **Memory Efficient**: Optimized storage for XML structures
- **Recursive Search**: Efficient traversal of nested structures

## Limitations

The following features are **not currently implemented or fully supported**:
- **`.find()` method** - Use `findall()[0]` instead (not needed given `.findall()`'s power)
- **`.tag` property** - Tag names are embedded in string representation, direct `.tag` property is not available
- **`.children` property** - Use array indexing `[0]`, `[1]`, etc.
- **`.parent` property** - Not implemented
- **`.siblings` property** - Not implemented
- **`.to_json()` method** - Not implemented
- **XML manipulation operators** (`+=`, `-=`, `++=`) - Not implemented

## Best Practices

### Element Access
```grapa
/* Use array indexing for direct access to children */
xml[0][0]                      /* First child element */

/* Use dot notation for direct access to named children */
xml.root.item                  /* First 'item' child of 'root' */

/* Use .findall() for complex queries and recursive searches */
xml.findall({name:"item", attr:{class:"highlight"}})

/* Use .attr for attribute access */
xml[0][0].attr.id              /* Access specific attribute */
```

### Error Handling
```grapa
/* Check for existence before accessing properties or elements */
if (xml.root) {
    if (xml.root.item) {
        xml.root.item.echo();
    }
}
```

## See also
- [$ARRAY object](array.md)
- [$GOBJ object](gobj.md)
- [$TAG object](tag.md)
- [Assignment Operators](../operators/assignment.md)
- [String Operations](../obj/transform.md)
