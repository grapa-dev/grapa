# $sys
A few general utility functions that are useful, but it wasn't clear if they should be added to the native language syntax, were added to $sys.

## type (object)
```
> $sys().type(5)
$INT
```

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