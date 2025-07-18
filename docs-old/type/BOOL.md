# $BOOL
References:
- [$BOOL object](../../lib/grapa/$BOOL.grc)
- [$OBJ object](../../lib/grapa/$OBJ.grc)

The following are equivalent in comparisons:
- True: true, 1
- False: false, null

Examples:
Commands | Results
------------ | -------------
1==true; | true
null==false; | true
1==false; | false
"55"==(55).str(); | true
"5.54"==(5.54).str(); | true
