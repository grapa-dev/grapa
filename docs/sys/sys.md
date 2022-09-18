# $sys
A few general utility functions that are useful, but it wasn't clear if they should be added to the native language syntax, were added to $sys.

## type (object)
```
> $sys().type(5)
$INT
```

## describe (object)
Not implemented yet...more details than type. 

## getenv (type)

getenv/putenv types:
Type | Description
------------ | -------------
$PATH | 
$STATICLIB |
$ARGCIN |
$ARGV |
$LIB |
$BIN |
$NAME |
$WORK |
$HOME |
$TEMP |
$VERSION |
$LICENCE |
$PLATFORM | C++/G++ compile flags
Any value not starting with '$' will be directed to the native OS getenv/putenv |

```
> $sys().getenv($VERSION)
{"major":0,"minor":0,"micro":2,"releaselevel":"alpha","serial":63,"date":2020-04-24T16:30:37.000000}
```

Values for $PLATFORM 
* `__APPLE__`
* `_WIN32`
* `_WIN64`
* `__linux__`
* `__ANDROID__`
* `_MSC_VER`
* `_MSC_FULL_VER`
* `__GNUC__`
* `__GNUC_MINOR__`
* `__clang__`
* `__MINGW32__`
* `__MINGW64__`
* `__i386__`
* `__x86_64__`
* `__arm__`
* `__ARM_ARCH_5T__`
* `__ARM_ARCH_7A__`
* `__powerpc64__`
* `__aarch64__`

## putenv (type, value)
See getenv.

## compilef(scriptfilename,compiledfilename)
Compiles the script. Execution of a compiled script is faster. 

Reads in scriptfilename, compiles, and writes output to compiledfilename.

## compile(script)
Compiles the script.

## exec(cscript,param="")
Runs a compiled script. Taks a single optional param, which is accessed using @$ARG in the compiled script.

Example.

```
grapa: />x = $sys().compile("@$ARG+5;")

grapa: />$sys().exec(x)
5
grapa: />$sys().exec(x,44)
49
```
## eval(script,param="")
Runs a script. Taks a single optional param, which is accessed using @$ARG in the script.

Example.

```
grapa: />x = "@$ARG+5;";

grapa: />$sys().eval(x)
5
grapa: />$sys().eval(x,44)
49
```
## sleep(ms)

