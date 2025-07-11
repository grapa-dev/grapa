
## Crypt
### RSA test
The following will create an RSA key using hard coded public/private key values and verify the encode/decode functions work.

```
e = 65537;
d = (0xA4954A19EA13D5FF06C86E7719AB6B6D5F37EF2D0A453713F08CD3FF2847C7BE80F2A3D0159C2EA862997DEE93DC08CD18CDFBD96DCC077B978904D05B2FCA5988A63).uraw();
d = (d + (0x2CEB3B7286ACEF97682F418172B30509BD29A1C56E0244D79CFADD5A60C83D2F28A8B72643CB82AABA08871E094B36E6613B108166768B33F5C4860BD21).uraw()).uint();
n = (0xB47D8ABBD7A33B5AD29C8EFD291433D62695BA043F4130F8E677CD179315FD0DD39989539EC710EB3BCBCACBDED2230DE8575FAEE3ED2835B256ADB1AAEF2F34C430E).uraw();
n = (n + (0xAB9934FAF1977FA83E12B947806CE1F8D06692F02068FD18E4DE952895DDD2D9BDEE923E94257E84B63D21C92EC1F74B8AF4AF9A33608C7E8247F1C820F).uraw()).uint();
g = {"method":"RSA","n":n,"e":e, "d":d};
len = g.n.bytes()-42;
v = "this is a test of 86 chars to see if we can encode with RSA. It needs to be exact size...so need to make it so.".left(len);
v.encode(g).decode(g).str();

this is a test of 86 chars to see if we can encode with RSA. It needs to be exact size
```

## Strings
### String Word Length
The following returns the length of each word in a string:
```
"this is a test".split(" ").reduce(op(a,b){a+=b.len();},[])
[4,2,1,4]
```

## Grep - Pattern Matching

### Basic Pattern Matching
```
"Hello world".grep("world")
["Hello world"]
```

### Match-Only Output
```
"Hello world".grep("world", "o")
["world"]
```

### Case-Insensitive Matching
```
"Hello WORLD".grep("world", "i")
["Hello WORLD"]
```

### Context Lines
```
"Header\nLine 1\nLine 2\nLine 3\nFooter".grep("Line 2", "A1B1")
["Line 1\n", "Line 2\n", "Line 3\n"]
```

### Unicode Support
```
"café résumé".grep("cafe", "d")
["café résumé"]
```

### Custom Delimiters
```
"Line 1|Line 2|Line 3".grep("Line 2", "", "|")
["Line 2"]
```

### Error Handling
```
result = input.grep("invalid[", "o");
if (result.type() == $ERR) {
    "Error occurred: " + result.str() + "\n".echo();
}
```

## Grammer Updating

The standard grammer loaded by default includes suport for 2 user defined rules that assist in easily modifying the grammer.

custom_start - The engine will evaulate the $start rule before evaluating the predefined $start rule. Setting a variable "start" with a rule will cause the engine to call the user defined rule first. If this variable is set locally within a function, it will only apply to scripts parsed from within the scope of that function.

custom_function - The engine will evaluate the variable "function" if it is a $RULE as a part of the $function rule. To verify, check the $function variable. Used in contexts where a value is returned.

custom_command - Same as above. Used in contexts where a value is not returned.

Alternative, redefine any of the predefined rules for the default grammer. Take caution on changing the global variable, unless the change is inteded for the entire session. Restart the app to revert back. 

If any of the above is set as a local variable within some scope, such as a class instance or a function, the modification will only apply for exectution within that scope. You can, for example, have muliple threads with unique modifications that apply separately. This is bascially the result of having the rules implemented as variables.

## PCA - Principal Component Analysis

Center the data around the mean, calcualte covariance, and than the eigenvalues and eigenvectors. Identify the number of components based on the eigenvalues, and select that number of eigenvectors. Use the eigenvectors to calculate PCA. Use the result for the new feature (replacing the old features). Use the same method for generating the feature from new values (need to save the mean and eigenvectors for this).

```
X=[[3.4,9.1,1.4],[9.1,1.2,8.2],[1.4,8.2,5.2]];
M=X.mean(1);
R=(X-M).cov(1).eigh();

R.w;
[41.7879952005570829404677683153157,4.4920047994429170595322314913591,0.000000000000000000000000193325118422339381744140663363374750345332]

Y=R.v.left(1);
(Y.t() .* (X-M).t()).t();

[[4.3153778898305898473304814235512],[-7.4322629041586428646647322175959],[3.1168850143280530173342507940447]]

(Y.t() .* ([[3.4,9.1,1.4]]-M).t()).t();
[[4.3153778898305898473304814235512]]

(Y.t() .* ([[3.4,9.23,2.4]]-M).t()).t();
[[3.9393492470695862312625754586492]]

```
