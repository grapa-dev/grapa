# $SYSINT
References:


Same as $INT, but was initialized with '$' at the front of the integer. Used for special system integers.

## Usage in Rules

`$SYSINT` is used in rule definitions to match integer tokens. When a rule matches an integer, the matched value is passed as a parameter to the rule's operation.

### Basic Rule Example

```grapa
/* Define a rule that matches integers */
x = rule $INT $INT {op(a:$1,b:$2){a*b}} | $INT {op(a:$1){a}};

/* Test with single integer */
(op()("4",x))();
/* Returns: 4 */

/* Test with two integers */
(op()("4 3",x))();
/* Returns: 12 */
```

In this example:
- `$INT` matches integer tokens in the input
- `$1` and `$2` refer to the first and second matched integers
- The rule can handle both single integers and pairs of integers

## Dynamic Dot Notation

`$SYSINT` is also used in dynamic dot notation, where you can create custom methods that operate on objects using the `$0` syntax.

### Creating Dynamic Methods

```grapa
/* Create a custom method that doubles the length */
mylen = op(){$0.len()*2;};

/* Use the dynamic method */
"hi".mylen();
/* Returns: 4 (length of "hi" is 2, doubled is 4) */
```

### Inline Dynamic Dot Notation

You can also use dynamic dot notation inline without defining a separate function:

```grapa
/* Inline dynamic dot notation */
"hi".{$0.len()*3;};
/* Returns: 6 (length of "hi" is 2, tripled is 6) */
```

### How It Works

- `$0` refers to the object that the method is being called on
- The expression inside `{}` is evaluated with `$0` as the context
- This allows you to create custom methods dynamically without modifying the class definition

### Advanced Examples

```grapa
/* Create a method that returns the object's type and length */
getinfo = op(){$0.type().str() + " with length " + $0.len().str();};

/* Use on different types */
"hello".getinfo();        /* Returns: "STR with length 5" */
[1,2,3].getinfo();        /* Returns: "ARRAY with length 3" */
{a:1,b:2}.getinfo();      /* Returns: "GOBJ with length 2" */

/* Inline version */
"test".{$0.upper() + " has " + $0.len().str() + " characters";};
/* Returns: "TEST has 4 characters" */
```

### Use Cases

- **Custom utility methods**: Create domain-specific methods for your objects
- **Data transformation**: Apply custom transformations inline
- **Debugging**: Create temporary methods to inspect object properties
- **Prototyping**: Test new functionality without modifying existing classes

This dynamic dot notation capability makes Grapa's object system highly extensible, allowing you to add custom behavior to any object at runtime. 
