# $INT
References:
- [GrapaInt.cpp source](../../source/grapa/GrapaInt.cpp)
- [$INT object](../../lib/grapa/$INT.grc)
- [$math object](../../lib/grapa/$math.grc)

Supports signed and unsigned. Unsigned is essentially an $INT that is not negative. Given this is unlimited precision, a separate $UINT type is not required (grapa originally had one, but it's been removed as it's essentially redundant and adds unnecessary complexity). The reason other languages have a $UINT type is to handle to bit overflow issue where the high order bit may become set during math operations. In grapa, when the high order bit is set, it automatically increases the precision and there is no overflow. Still, there is sometimes a need for specific $UINT type handling of raw bytes, and to support this there is a uint() function to ensure that a raw byte stream with a high order bit is not incorrectly interpreted as a negative number. 

The $INT class inherits $math class - see the the $math class for additional functions supported.

Examples:
Commands | Results
------------ | -------------
52322; | 52322
"52322".int(); | 52322
(52322).raw(); | 0x0CC62
