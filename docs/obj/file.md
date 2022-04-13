# Part of $OBJ
## setfile($file, name)
Updates file or table item with value. Separate from the $file class as this one supports chaining all the way to writing the result into a file. Requires having an existing $file instance -> or passing in $file() which creates a temporary instance.

"testing".setfile($file(),"test.txt");
