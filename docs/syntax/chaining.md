# Function Chaining
Any object that returns an object can be chained.

For example "4.4".float(300,4).pi() will convert the string 4.4 to a float with 300 bit precision and will return pi() to the power of the input, at the precision specified in the input. So, a 400 bit precision of pi to the power of 4.4. 

If an existing function/command doesn't support chaining, an OP can be inserted in the chain to make it work using the $$ variable for the result of the prior operation. For example, the following will get the length of a list, generate that many bits, and output the result in binary form. The 3 examples all accomplish the same result.
```
> {1,2,3,5}.{(@$$).len()}.genbits().bin();
1111

> {1,2,3,5}.{(op(a){a.len()})(@$$)}.genbits().bin();
1111

> f = op(a){a.len()};
> {1,2,3,5}.{f(@$$)}.genbits().bin();
1111
```
