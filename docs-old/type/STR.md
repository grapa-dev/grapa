# $STR
References:
- [$STR object](../../lib/grapa/$STR.grc)
- [$OBJ object](../../lib/grapa/$OBJ.grc)

ASCII only - Unicode will eventually be added. Can initialize with either double quotes or single quotes - which is easier than escaping a string that includes a quoted string - such as "this 'is' a test", or 'this "is" a test'. $STR inherits the $obj class - see $obj for functions supported. 

Examples:
Commands | Results
------------ | -------------
"this is a test".len(); | 14
"this is a test".split(""); | ["t","h","i","s"," ","i","s"," ","a"," ","t","e","s","t"]
"this is a test".raw(); | 0x7468697320697320612074657374
"this is a test".raw().int(); | 2361031878030638688519054699098996
