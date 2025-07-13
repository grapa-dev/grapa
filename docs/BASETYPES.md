# Base Types

References:
- [$OP](type/OP.md)
- [$CODE](type/CODE.md)

## Overview

$OP and $CODE provide the foundation of Grapa's execution model. While most users interact with the high-level language defined in `$grapa.grc`, all syntax ultimately compiles down to combinations of these two core types, which support an execution tree linking library calls.

## For Most Users

If you're new to Grapa, you typically won't need to work directly with $OP and $CODE. Instead, focus on the high-level language features described in the [Getting Started](GETTING_STARTED.md) guide and [Examples](EXAMPLES.md).

## For Language Extenders

$OP and $CODE are essential for understanding how to extend the language or create custom grammar rules. An $OP represents a single operation (like a function call), while a $CODE represents a sequence of operations.

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
