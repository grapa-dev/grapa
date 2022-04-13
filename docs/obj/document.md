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
