---
tags:
  - user
  - highlevel
---
# XML and TAG Operations

Grapa provides comprehensive support for XML and TAG data types with specialized operations for document manipulation and processing.

## Overview

XML and TAG types in Grapa are designed for structured document processing, web content generation, and data serialization. They support hierarchical structures with attributes, content, and nested elements.

## Basic Operations

### Creation
```grapa
/* Create XML element */
xml = <root><item>1</item></root>;

/* Create TAG element */
tag = <div class="container"><p>Hello World</p></div>;

/* Create from string */
xml_str = "<root><item>1</item></root>";
xml = xml_str.xml();

/* Create from list */
data = {tag:"root", children:[{tag:"item", content:"1"}]};
xml = data.to_xml();
```

### Addition Operations (`+=`)
```grapa
xml = <root><item>1</item></root>;

/* Add XML element */
xml += <newitem>2</newitem>;               /* Add element to end */

/* Add at specific position */
xml += <child>3</child> xml[0];            /* Add before first element */

/* Add with attributes */
xml += <item id="new" class="highlight">4</item>;
```

### Concatenation Operations (`++=`)
```grapa
xml1 = <root><item>1</item></root>;
xml2 = <root><item>2</item><item>3</item></root>;

/* Concatenate XML documents */
xml1 ++= xml2;                             /* Combine XML structures */
```

## XML Structure

### Element Properties
```grapa
xml = <div id="main" class="container">
    <h1>Title</h1>
    <p>Content</p>
</div>;

/* Access element properties */
tag_name = xml.tag;                        /* "div" */
attributes = xml.attributes;                /* {id:"main", class:"container"} */
content = xml.content;                     /* Text content */
children = xml.children;                   /* Child elements */
```

### Attribute Access
```grapa
xml = <div id="main" class="container" data-value="123"></div>;

/* Access attributes */
id_value = xml.attributes.id;              /* "main" */
class_value = xml.attributes.class;        /* "container" */
data_value = xml.attributes["data-value"]; /* "123" */

/* Set attributes */
xml.attributes.id = "new_id";
xml.attributes["data-value"] = "456";
```

## XML Navigation

### Element Selection
```grapa
xml = <root>
    <header>
        <title>Page Title</title>
    </header>
    <body>
        <section>
            <h1>Section Title</h1>
            <p>Content</p>
        </section>
    </body>
</root>;

/* Find elements by tag name */
titles = xml.find_all("title");            /* All title elements */
headers = xml.find_all("header");          /* All header elements */

/* Find element by ID */
main_section = xml.find("#main");

/* Find elements by class */
highlighted = xml.find_all(".highlight");

/* Find by path */
section_title = xml.find("body/section/h1");
```

### Element Traversal
```grapa
xml = <root><item>1</item><item>2</item></root>;

/* Access child elements */
first_item = xml.children[0];              /* First item */
last_item = xml.children[-1];              /* Last item */

/* Get parent element */
parent = xml.parent;

/* Get sibling elements */
siblings = xml.siblings;

/* Get next/previous sibling */
next_sibling = xml.next_sibling;
prev_sibling = xml.previous_sibling;
```

## XML Manipulation

### Adding Elements
```grapa
xml = <root></root>;

/* Add child element */
xml += <item>1</item>;

/* Add element with attributes */
xml += <item id="new" class="highlight">2</item>;

/* Add at specific position */
xml += <header>Title</header> xml[0];      /* Add at beginning */

/* Add multiple elements */
xml += [<item>3</item>, <item>4</item>];
```

### Removing Elements
```grapa
xml = <root><item>1</item><item>2</item><item>3</item></root>;

/* Remove by index */
xml -= xml[1];                             /* Remove second item */

/* Remove by tag name */
xml.remove_all("item");                    /* Remove all item elements */

/* Remove by condition */
xml.remove(op(element){
    return element.content == "2";
});
```

### Modifying Elements
```grapa
xml = <div class="old">Content</div>;

/* Change tag name */
xml.tag = "span";

/* Change content */
xml.content = "New Content";

/* Change attributes */
xml.attributes.class = "new";
xml.attributes.id = "main";

/* Add new attributes */
xml.attributes["data-value"] = "123";
```

## XML Processing

### Content Extraction
```grapa
xml = <article>
    <title>Article Title</title>
    <content>
        <p>First paragraph</p>
        <p>Second paragraph</p>
    </content>
</article>;

/* Extract text content */
title = xml.find("title").text();          /* "Article Title" */
all_text = xml.text();                     /* All text content */

/* Extract attribute values */
class_values = xml.find_all("[class]").map(op(el){el.attributes.class;});
```

### Content Transformation
```grapa
xml = <root><item>1</item><item>2</item></root>;

/* Transform elements */
xml.transform(op(element){
    element.content = element.content.int() * 2;
    return element;
});

/* Map over elements */
doubled = xml.map(op(element){
    return <item>{element.content.int() * 2}</item>;
});
```

## XML Serialization

### Export Formats
```grapa
xml = <root><item>1</item><item>2</item></root>;

/* Export to string */
xml_string = xml.to_string();              /* XML string */
pretty_string = xml.to_string(true);       /* Pretty-printed */

/* Export to JSON */
json_data = xml.to_json();                 /* Convert to JSON structure */

/* Export to list */
list_data = xml.to_list();                 /* Convert to list structure */
```

### Import Formats
```grapa
/* Import from string */
xml_str = "<root><item>1</item></root>";
xml = xml_str.xml();

/* Import from JSON */
json_data = {tag:"root", children:[{tag:"item", content:"1"}]};
xml = json_data.to_xml();

/* Import from list */
list_data = {tag:"root", children:[{tag:"item", content:"1"}]};
xml = list_data.to_xml();
```

## Advanced Features

### XPath-like Queries
```grapa
xml = <root>
    <section id="main">
        <h1>Title</h1>
        <p>Content</p>
    </section>
    <section id="sidebar">
        <h2>Sidebar</h2>
    </section>
</root>;

/* Query by path */
main_content = xml.query("section[@id='main']");
titles = xml.query("//h1|//h2");           /* All h1 and h2 elements */

/* Query with conditions */
highlighted = xml.query("//*[@class='highlight']");
numbered = xml.query("//item[@id>5]");
```

### Template Processing
```grapa
/* XML template with placeholders */
template = <div class="user-card">
    <h2>{name}</h2>
    <p>Email: {email}</p>
    <p>Age: {age}</p>
</div>;

/* Fill template with data */
user_data = {name:"Alice", email:"alice@example.com", age:30};
filled_template = template.fill(user_data);
```

### Validation
```grapa
xml = <form>
    <input type="text" name="username" required="true"/>
    <input type="email" name="email" required="true"/>
    <button type="submit">Submit</button>
</form>;

/* Validate XML structure */
is_valid = xml.validate();

/* Validate against schema */
schema = <schema>...</schema>;
is_valid = xml.validate(schema);

/* Get validation errors */
errors = xml.get_validation_errors();
```

## Performance Considerations

- **Lazy Parsing**: XML is parsed only when accessed
- **Memory Efficiency**: Optimized storage for large XML documents
- **Streaming Support**: Processing of large XML files without loading entire document
- **Caching**: Frequently accessed elements are cached for performance

## Integration with Other Types

### XML and Arrays
```grapa
/* Convert array to XML */
data = [1, 2, 3, 4, 5];
xml = <root>{data.map(op(item){<item>{item}</item>;})}</root>;

/* Convert XML to array */
xml = <root><item>1</item><item>2</item><item>3</item></root>;
array = xml.children.map(op(child){child.content.int();});
```

### XML and Lists
```grapa
/* Convert list to XML */
data = {name:"Alice", age:30, city:"New York"};
xml = <person>
    <name>{data.name}</name>
    <age>{data.age}</age>
    <city>{data.city}</city>
</person>;

/* Convert XML to list using .list() method */
xml = <person><name>Alice</name><age>30</age></person>;
list = xml.list();  /* Converts XML to LIST structure */

/* Convert XML to list manually */
xml = <person><name>Alice</name><age>30</age></person>;
list = {
    name: xml.find("name").content,
    age: xml.find("age").content.int()
};
```

### XML to LIST Conversion
The `.list()` method converts XML structures to LIST format:

```grapa
/* Simple element conversion */
xml = <item>test</item>;
list = xml.list();  /* {{"item":[{},["test"]]}} */

/* Element with attributes */
xml = <div class="main">content</div>;
list = xml.list();  /* {{"div":[{"class":"main"},["content"]]}} */

/* Complex nested structure */
xml = <html><body><div class="main"><h1>Title</h1></div></body></html>;
list = xml.list();  /* Complex nested LIST structure */

/* Access converted data */
xml = <user><name>Alice</name><age>30</age></user>;
list = xml.list();
user_name = list.user[1][0].name[1][0];  /* "Alice" */
user_age = list.user[1][1].age[1][0];    /* "30" */
```

## See also
- [$ARRAY object](array.md)
- [$LIST object](list.md)
- [$WIDGET object](widget.md)
- [Assignment Operators](../operators/assignment.md)
- [String Operations](../obj/transform.md)
- [Unified Dot Notation System](../unified_dot_notation.md)
