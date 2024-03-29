# $TABLE
References:
- [$TABLE object](../../lib/grapa/$TABLE.grc)
- [$file object](../../lib/grapa/$file.grc)
- [GrapaDB.cpp](../../source/grapa/GrapaDB.cpp)
- [GrapaDatabase.cpp](../../source/grapa/GrapaDatabase.cpp)
- [GrapaDatabase.cpp](../../source/grapa/GrapaDatabase.cpp)
- [GrapaBtree.cpp](../../source/grapa/GrapaBtree.cpp)

A $TABLE is a hierarchical database with columns, rows, with both row store and columns store. 

A $TABLE can be in memory or stored on the file system. 

See $file commands for creating, updated, and navigating.
```
> f=$file().table()
> f.ls()
[]

> f.set("test","value")
> f.ls()
[{"$KEY":"test","$TYPE":"ROW","$BYTES":5}]

> f.get("test")
value

> f.type()
$TABLE
```
