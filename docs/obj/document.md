# From $OBJ

# data = json(rawdata)

# data = xml(rawxml)

# data = html(rawhtml)

# data = findall(term)
Searches for all occurances of a match against JSON, XML or HTML data.

Each item in search is optional. 

The has, nhas, and, nand, or, nor are all recursive. 

n in nhas, nand, and nor indicates negation. 

```
search = {
  name:"", 
  value:"", 
  attr:attrlist, 
  has:searchlist, 
  nhas:searchlist, 
  and:searchlist, 
  nand:searchlist, 
  or:searchlist, 
  nor:searchlist
  }
 attrlist = {attr, attr, etc}
 attr = name: value
 searchlist = [search, search, etc]
```

# Advanced Usage: Extracting Data from HTML

```grapa
// Parse HTML and extract all <a> tags with an <img> child
html = $file().get("page.html").str().html();
anchors_with_images = html.body.findall({name:"a", has:{name:"img"}});

// Extract hrefs, filtering out any <a> without an href attribute
hrefs = anchors_with_images.reduce(op(acc, a) {
    if (a.$LIST.href.type() != $ERR) {
        acc += a.$LIST.href;
    }
}, []);
```

**Explanation:**
- `findall({name:"a", has:{name:"img"}})` finds all `<a>` tags that contain an `<img>` child.
- The `.reduce()` collects the `href` attributes, skipping any that are missing.

# Query Syntax Recap
- `name`: Tag name to match (e.g., `"a"`, `"div"`)
- `attr`: Match attributes (e.g., `{id:"main", class:"header"}`)
- `has`: Require child/descendant elements matching a query
- `nhas`: Require absence of child/descendant elements
- `and`, `or`, `nand`, `nor`: Combine multiple queries (logical operations)

# Best Practices
- Chain `.findall()` calls to traverse nested structures.
- Use functional methods (`.map()`, `.reduce()`) to process results.
- Always check for `$ERR` when accessing properties that may not exist.

# Note on JSON and XML
The same `.findall()` principles apply to complex JSON and XML documents. You can use the same query patterns to extract nested data, filter by attributes/keys, and process results with functional methods.
