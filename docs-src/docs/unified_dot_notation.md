---
tags:
  - user
  - highlevel
---
# Unified Dot Notation System

> **See also:** [Object Access Patterns](obj/transform.md), [XML and HTML Processing](type/xml.md)
>
> **Best Practices:**
> - Use dot notation for simple property access
> - Use bracket notation for dynamic property names or special characters
> - The same access patterns work across JSON, XML, and HTML structures

## Overview

Grapa provides a unified dot notation system that allows you to access data consistently across different types including JSON objects, arrays, XML documents, and HTML structures. This system uses the same syntax patterns regardless of the underlying data type.

## Basic Access Patterns

### JSON Object Access
```grapa
/* Object property access */
user = {name:"Alice", age:30, city:"New York"};
user.name;      /* "Alice" */
user["name"];   /* "Alice" - bracket notation */
user.age;       /* 30 */

/* Object property assignment */
user.name = "Bob";          /* Change name to "Bob" */
user["age"] = 25;           /* Change age to 25 */
user.city = "Boston";       /* Change city to "Boston" */

/* Duplicate key resolution - last value wins */
user2 = {name:"Alice", age:30, name:"Bob"};
user2.name;     /* "Bob" (last value) */

user3 = {a:1, b:2, a:3, c:4, b:5};
user3.a;        /* 3 (last value) */
user3.b;        /* 5 (last value) */
user3.c;        /* 4 (only value) */
```

### Array Access
```grapa
/* Array element access */
numbers = [1, 2, 3, 4, 5];
numbers[0];     /* 1 */
numbers[1];     /* 2 */
numbers[-1];    /* 5 - negative indexing */

/* Array element assignment */
numbers[0] = 10;            /* Change first element to 10 */
numbers[-1] = 50;           /* Change last element to 50 */
numbers[2] = 30;            /* Change third element to 30 */
```

### XML/HTML Access
```grapa
/* XML element access */
xml = <root><item>1</item><item>2</item></root>;
xml[0];         /* Root element */
xml[0][0];      /* First item element */
xml[0][0][0];   /* Text content: "1" */

/* HTML structure access */
html = <html><body><div class="main"><h1>Title</h1></div></body></html>;
html[0][0][0][0][0];  /* Navigate to "Title" */
```

## Interchangeable Notation

### Dot vs Bracket Notation
Both dot notation and bracket notation work for most access patterns:

```grapa
/* These are equivalent */
data = {name:"Alice", age:30};
data.name;      /* Dot notation */
data["name"];   /* Bracket notation */

/* Bracket notation is required for special characters */
data = {"user-name":"Alice", "age":30};
data["user-name"];  /* Must use brackets for hyphens */
data.age;           /* Works for simple names */
```

### Dynamic Property Access
Bracket notation is essential for dynamic property names:

```grapa
/* Dynamic property access */
data = {name:"Alice", age:30, city:"New York"};
property = "name";
data[property];     /* "Alice" */

/* Loop through properties */
properties = ["name", "age", "city"];
properties.map(op(prop){data[prop];}).echo();
```

### Duplicate Key Resolution
When objects contain duplicate keys, Grapa follows "last value wins" behavior:

```grapa
/* Duplicate keys - last value wins */
config = {debug: false, port: 8080, debug: true};
config.debug;   /* true (last value) */
config.port;    /* 8080 (only value) */

/* Multiple duplicates */
settings = {theme: "dark", timeout: 30, theme: "light", timeout: 60};
settings.theme;    /* "light" (last value) */
settings.timeout;  /* 60 (last value) */
```

**Key Points:**
- **Search Order**: Property resolution searches from tail to head (reverse order)
- **Last Value Wins**: When duplicate keys exist, the last occurrence takes precedence
- **JSON Compatibility**: Matches behavior of JSON in other programming languages
- **Index Access Unchanged**: Array-style indexing (`obj[0]`, `obj[1]`) uses position-based access
- **Property Assignment**: Assignment also targets the last occurrence of duplicate keys

## XML and HTML Navigation

### Element Hierarchy
XML and HTML structures can be navigated using the same indexing patterns:

```grapa
/* Simple XML navigation */
xml = <root><item>1</item><item>2</item></root>;
xml[0];         /* <root> element */
xml[0][0];      /* First <item> element */
xml[0][1];      /* Second <item> element */
xml[0][0][0];   /* Text content: "1" */

/* Complex HTML navigation */
html = <html><body><div class="main"><h1>Title</h1><p>Content</p></div></body></html>;
html[0];        /* <html> element */
html[0][0];     /* <body> element */
html[0][0][0];  /* <div> element */
html[0][0][0][0]; /* <h1> element */
html[0][0][0][0][0]; /* "Title" */
html[0][0][0][1]; /* <p> element */
html[0][0][0][1][0]; /* "Content" */
```

### Element Name Access
You can access elements by their tag names:

```grapa
/* Access by element name */
xml = <root><item>1</item><item>2</item><header>Title</header></root>;
xml.item;       /* Access all 'item' elements */
xml.header;     /* Access 'header' element */
```

### Attribute Access
XML and HTML attributes can be accessed using dot notation:

```grapa
/* Attribute access */
div = <div class="main" id="content">Text</div>;
div.class;      /* Access class attribute */
div.id;         /* Access id attribute */

/* Complex attribute access */
form = <form method="post" action="/submit"><input type="text" name="username"/></form>;
form.method;    /* "post" */
form.action;    /* "/submit" */
form[0].type;   /* "text" */
form[0].name;   /* "username" */
```

## Advanced Access Patterns

### Nested Object Access
```grapa
/* Deep nested access */
data = {
    user: {
        profile: {
            name: "Alice",
            address: {
                city: "New York",
                country: "USA"
            }
        }
    }
};

data.user.profile.name;           /* "Alice" */
data.user.profile.address.city;   /* "New York" */
data["user"]["profile"]["name"];  /* Bracket notation */
```

### Array of Objects
```grapa
/* Access properties in arrays of objects */
users = [
    {id:1, name:"Alice", age:30},
    {id:2, name:"Bob", age:25},
    {id:3, name:"Charlie", age:35}
];

users[0].name;   /* "Alice" */
users[1].age;    /* 25 */

/* Extract all names */
names = users.map(op(user){user.name;});
names.echo();    /* ["Alice", "Bob", "Charlie"] */
```

### Mixed Data Types
```grapa
/* Access patterns work across mixed types */
data = {
    users: [
        {name:"Alice", preferences:{theme:"dark", lang:"en"}},
        {name:"Bob", preferences:{theme:"light", lang:"es"}}
    ],
    settings: {
        api_url: "https://api.example.com",
        timeout: 30
    }
};

data.users[0].preferences.theme;  /* "dark" */
data.settings.api_url;            /* "https://api.example.com" */
```

## Error Handling

### Graceful Property Access
The system gracefully handles missing properties:

```grapa
/* Missing properties return error */
data = {name:"Alice"};
data.age;       /* Returns error */
data["age"];    /* Returns error */

/* Safe access with error checking */
if (data.age.type() != $ERR) {
    data.age.echo();
} else {
    "Age not found".echo();
}
```

### Null/Undefined Handling
```grapa
/* Handle null/undefined values */
data = {name:"Alice", details:null};
data.name;      /* "Alice" */
data.details;   /* null */
data.details.age; /* Error - can't access property of null */
```

## Performance Considerations

### Efficient Access
- **Direct Access**: Dot notation is slightly faster than bracket notation
- **Caching**: Frequently accessed properties are cached internally
- **Lazy Loading**: Properties are resolved only when accessed

### Best Practices
```grapa
/* Prefer dot notation for known properties */
user.name;      /* Faster */
user["name"];   /* Slower but more flexible */

/* Use bracket notation for dynamic access */
property = "name";
user[property]; /* Required for dynamic access */

/* Avoid deep nesting in loops */
/* Instead of: */
users.map(op(user){user.profile.address.city;});

/* Consider: */
users.map(op(user){
    profile = user.profile;
    profile ? profile.address.city : null;
});
```

### ⚠️ Critical: Number Method Calls

**IMPORTANT:** When calling methods on numbers using dot notation, you must enclose the number in parentheses:

```grapa
/* ✅ Correct - Use parentheses for number method calls */
(20).random();    /* Works: Random number 0-19 */
(3.14).floor();   /* Works: Floor to 3 */
(42).str();       /* Works: Convert to string */

/* ❌ Incorrect - Lexer consumes the dot as part of float literal */
20.random();      /* Fails: Lexer sees "20." as float */
3.14.floor();     /* Fails: Lexer sees "3.14." as float */
42.str();         /* Fails: Lexer sees "42." as float */
```

**Why this happens:**
- The lexer processes `20.` as a float literal and consumes the dot
- No dot token reaches the parser, so method calls fail
- Parentheses force the lexer to treat the number as a complete token first
- Then the dot becomes available for method calls

**Common patterns:**
```grapa
/* Working examples */
result = (20).random();     /* Random number 0-19 */
rounded = (3.14159).floor(); /* Floor: 3 */
text = (42).str();          /* Convert to string: "42" */

/* In functional programming */
numbers = [(10).random(), (20).random(), (30).random()];
text_numbers = [(1).str(), (2).str(), (3).str()];
```

## Use Cases

### 1. Configuration Access
```grapa
/* Access configuration settings */
config = {
    database: {
        host: "localhost",
        port: 5432,
        credentials: {
            username: "admin",
            password: "secret"
        }
    },
    api: {
        base_url: "https://api.example.com",
        timeout: 30
    }
};

db_host = config.database.host;
api_url = config.api.base_url;
```

### 2. Web Scraping
```grapa
/* Extract data from HTML */
html = $file().get("page.html").str().html();
title = html[0][0][0][0][0];  /* Extract page title */
links = html[0][0].findall({name:"a"});  /* Find all links */
```

### 3. JSON Processing
```grapa
/* Process JSON API responses */
response = $file().get("api.json").str().json();
users = response.data.users;
first_user = users[0];
user_name = first_user.name;
```

### 4. XML Document Processing
```grapa
/* Process XML documents */
xml = $file().get("data.xml").str().xml();
items = xml[0].findall({name:"item"});
item_count = items.len();

/* Convert XML to LIST for easier processing */
xml = <root><item>1</item><item>2</item></root>;
list = xml.list();  /* Convert to LIST structure */
list.echo();        /* View converted structure */
```

## Tips and Tricks

### 1. Debugging Access
```grapa
/* Debug object structure */
data = {user:{name:"Alice", age:30}};
data.echo();        /* See full structure */
data.user.echo();   /* See user object */
```

### 2. Safe Navigation
```grapa
/* Safe navigation pattern */
data = {user:{profile:{name:"Alice"}}};
name = data.user && data.user.profile ? data.user.profile.name : null;
```

### 3. Dynamic Property Building
```grapa
/* Build property paths dynamically */
data = {a:{b:{c:"value"}}};
path = ["a", "b", "c"];
result = path.reduce(op(acc, prop){acc[prop];}, data);
result.echo();  /* "value" */
```

## See Also
- [Object Transformation Methods](../obj/transform.md)
- [XML and HTML Processing](../type/xml.md)
- [Array Operations](../type/array.md)
- [List Operations](../type/list.md)
- [Basic Syntax](../syntax/basic_syntax.md) 