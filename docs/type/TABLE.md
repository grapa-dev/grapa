# $TABLE
A $TABLE is a hierarchical database with columns, rows, with both row store and columns store. 

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
