Create custom types using the class routine. The underlying structure is a $LIST where variables are stored, and the class can inherit other classes, including system types/classes (each system type is initiated as a class instance).

The following example defines a class with a single method. The method setdiv2 updates data to the parameter divided by 2.
```
> myClass = class {data = 0; setdiv2 = op(p){data = @p/2;}; get = op(){@data;};};

> test = myClass();

> test.setdiv2(5);

> test.get();
2

> test
{"data":2}
```
