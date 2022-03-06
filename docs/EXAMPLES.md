
## Crypt
### RSA test
The following will create an RSA key using hard coded public/private key values and verify the encode/decode functions work.

```
e = 0xa932b948feed4fb2b692609bd22164fc9edb59fae7880cc1eaff7b3c9626b7e5b241c27a974833b2622ebe09beb451917663d47232488f23a117fc97720f1e7;
d = (0x4adf2f7a89da93248509347d2ae506d683dd3a16357e859a980c4f77a4e2f7a01fae289f13a851df6e9db5adaa60bfd2b162bbbe31f7c8f828261a6839311929d2cef).uraw();
d = (d + (0x4f864dde65e556ce43c89bbbf9f1ac5511315847ce9cc8dc92470a747b8792d6a83b0092d2e5ebaf852c85cacf34278efa99160f2f8aa7ee7214de07b7).uraw()).uint();
n = (0xe8e77781f36a7b3188d711c2190b560f205a52391b3479cdb99fa010745cbeba5f2adc08e1de6bf38398a0487c4a73610d94ec36f17f3f46ad75e17bc1adfec998395).uraw();
n = (n + (0x89f45f95ccc94cb2a5c500b477eb3323d8cfab0c8458c96f0147a45d27e45a4d11d54d77684f65d48f15fafcc1ba208e71e921b9bd9017c16a5231af7f).uraw()).uint();
g = {e:e,d:d,n:n};
v = "this is a test of 95 chars to see if we can encode with RSA. It needs to be exactly 95...so need to make it so.".left(95);
v.encode("RSA-KEY",g).decode("RSA-KEY",g).str();

this is a test of 95 chars to see if we can encode with RSA. It needs to be exactly 95...so ne
```

## Strings
### String Word Length
The following returns the length of each word in a string:
```
"this is a test".split(" ").reduce(op(a,b){a+=b.len();},[])
[4,2,1,4]
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
