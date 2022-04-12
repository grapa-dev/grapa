# Part of $OBJ
## setfile($file, name)
Updates file or table item with value. Separate from the $file class as this one supports chaining all the way to writing the result into a file. Requires having an existing $file instance -> or passing in $file() which creates a temporary instance.

"testing".setfile($file(),"test.txt");

# $file()
Provides the ability to navigate either the file system or a database, querying data and updating data. This class/libraries will be enhanced over time to support navigating data types beyond the file system and the grapa database - such as JSON/XML and unstructured data where a mapping can be defined (maybe with a set of rules). With a few additional enhancements, this class/library will also enable extending the grapa syntax to include SQL with $file for the underlying data.

Each example below assumes the following command has been issued:
<pre><code> f = $file();</code></pre>
Which assignes f an instance of the $file class. The following are then operations that can be used from the $file class.

The name field for the commands can include a path relative to the "working directory" (see pwd()). If the "working directory" is a OS filesystem directory, than the path must reference a file within the OS filesystem. If the "working directory" is a grapa table, than the path and data item would be within the grapa table. What is not currently supported is referencing a grapa table item when the "working directory" is not within a grapa table.

## type()
```
> f.type()
$file
```
## table()
The table function creates an in memory database. 

```
> t = f.table()
> t.mkrow("test","data for test")
> t.getrow("test")
data for test
```
## pwd()
Returns the current working directory, relative to the current home directory. 
```
> f.pwd()
/

> f.cd("lib")
> f.pwd()
/lib
```
## cd([name])
Changes the current working directory, relative to the current home directory.  Using ".." will result in moving back 1 level.
```
> f.cd("lib")
> f.pwd()
/lib
```
## phd()
Returns the current home directory.
```
> f.phd()
C:\Projects\Test
```
## chd(filesystempath)
Changes the current home directory.
```
> f.chd("lib")
> f.phd()
C:\Projects\Test\lib
```
## ls([name])
Retrieves a list of files/directories in the current working directory.

```
> f.chd("/Microsoft/AndroidNDK64/android-ndk-r16b/python-packages");
> f.ls();
[
  {"$KEY":"adb","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"Android.bp","$TYPE":"FILE","$BYTES":784},
  {"$KEY":"fastboot","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"gdbrunner","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"NOTICE","$TYPE":"FILE","$BYTES":577},
  {"$KEY":"repo.prop","$TYPE":"FILE","$BYTES":4465}
]

> f.chd("/Microsoft/AndroidNDK64/android-ndk-r16b");
> f.ls("python-packages");
[
  {"$KEY":"adb","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"Android.bp","$TYPE":"FILE","$BYTES":784},
  {"$KEY":"fastboot","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"gdbrunner","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"NOTICE","$TYPE":"FILE","$BYTES":577},
  {"$KEY":"repo.prop","$TYPE":"FILE","$BYTES":4465}
]
```

## mk(name [,type]) 
Creates a directory at the current working directory within the file system.

type is one of:
- "", if in the file system will default to "DIR"
- "DIR", if in the file system, a new directory will be created. If in the database, will use "GROUP"
- "GROUP", create a database of GROUP type
- "ROW", create a ROW store database
- "COL", create a COL store database

```
> f.mk("test")
> f.cd("test")
> f.ls()
[]

> f.mk("testg","GROUP")
> f.cd("testg")
> f.ls()
[]
```

## rm(name)
Removes a directory or file.
```
> f.rm("test")
```

## set(name, value [, field])
Updates the column in a row. By default the $VALUE column is updated. But an alternate column can be specified.

field defaults to $VALUE.

```
> f.set("test","value of test")
```

## get(name [, field])
Gets the column value in a row. By default the $VALUE column is retrieved. But an alternate column can be specified.

field defaults to $VALUE.

```
> f.set("test","value of test")
> f.get("test")
value of test
```

## split

## info

## mkfield(name [,fieldType[, storeType[, storeSize[, storeGrow]]]])
Creates a field within the current working directory.

```
> f.mkfield("test")
```

Default for all fields is fieldType=STR and storeType=VAR. 

fieldType | Description
------------ | -------------
BOOL | Fixed size for $BOOL
TIME | Stores an $INT. Size depeds on storeType and storeSize
INT | Stores an $INT. Size depeds on storeType and storeSize
FLOAT | Stores a $FLOAT. Size depends on storeType and storeSize
STR | Stores a $STR. Size depends on storeType and storeSize
TABLE | Stores a $TABE. Size depends on storeType and storeSize
RAW | Stores a $RAW. Size depends on storeType and storeSize

storeType | Description
------------ | -------------
FIX | Fixed field size, set storeSize and storeGrow. Data embedded in row/col.
VAR | Variable field size. Uses an extra reference for the data. Reference stored in row/col.
PAR | Partitioned field. Best for very large data requiring updates to portions of the data. Also used for COL store $TABLE types.

storeGrow is used by variable fields to determine how much to grow the field by when extra space is needed.

## rmfield(name)
Deletes a field within the current working directory.

## debug()
Used for debugging the database during development. Displays the BTree structure of the data dictionary and fields and indexes for the current working directory when in a database (either in memory or on the file system).
