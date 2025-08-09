---
tags:
  - user
  - highlevel
---
# Assignment Operators
## `=`
Assign.

## `+=`
Add item to source.

## `++=`
Concatenate contents to source.

For rules, this operator concatenates additional rule alternatives, allowing dynamic grammar construction:

```
> x = rule $INT {op(a:$1){a}};
> x ++= rule $STR {op(a:$1){a}};
> x
$INT {@<[op,@<var,{a}>],{"a":$1}>}| $STR {@<[op,@<var,{a}>],{"a":$1}>}
```

See [Rule Composition and Concatenation](../type/rule.md#rule-composition-and-concatenation) for detailed examples.

## `-=`
Subtract item from source.

## See also
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Use Cases](../use_cases/index.md)
- [Language Reference](../syntax/basic_syntax.md)
