# Base Types

References:
- [$OP](type/OP.md)
- [$CODE](type/CODE.md)

$OP and $CODE provide the basis of the grapa core language. All other syntax compiles down to a combination of these 2 types - which support an execution tree linking library calls.

The following creates an $OP that takes a parameter x and adds the numbers from 1 to x, and returns the result. 

```
> op(x){i=1;n=0;while(i<=x){n+=i;i+=1;};n;}

@<
  [op,
    @[
      @<assign,{i,1}>,
      @<assign,{n,0}>,
      @<while,
      {
        @<lteq,
        {
          @<var,{i}>,
          @<var,{x}>
        }>,
        @<scope,
        {
          @[
            @<assignappend,{n,@<var,{i}>}>,
            @<assignappend,{i,1}>
          ]
        }>
      }>,
      @<var,{n}>
    ]
  ],
  {x}
>
```
