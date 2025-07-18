# $ID
References:
- [$ID object](../../lib/grapa/$ID.grc)
- [$OBJ object](../../lib/grapa/$OBJ.grc)

Any identifier (starts with a letter and can follow with letters or numbers will be initialized as an $ID. And an $ID can be used for many things, including associating a value. Optionally, the '@' symbol is used to de-reference an $ID to retrieve the data stored - which is useful when being used in a location that requires and $ID (such as a label). When used in a location that requires a value, an $ID will be automatically dereferenced. 

Example of using an $ID as a variable:
```
> x = "hi"

> x
hi

> x + " test"
hi test

> {x:"test"}
{"x":"test"}

> {@x:"test"}
{"hi":"test"}
```
