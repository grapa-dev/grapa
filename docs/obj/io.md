# From $OBJ

## exec(op)
Executes string in grapa.

```
> "4*5".exec()
20
```

## shell()
Executes string in the shell of the operating system. 

```
> "ver".exec()
Microsoft Windows [Version 10.0.18363.778]
```

## post()
Sends string to console input, processing as if it came in from the console. 

## echo()
Outputs item to stdio.

## setfile($file, name)
Updates file or table item with value. Separate from the $file class as this one supports chaining all the way to writing the result into a file. Requires having an existing $file instance -> or passing in $file() which creates a temporary instance.

"testing".setfile($file(),"test.txt");
