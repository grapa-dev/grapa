# Grammar Design Guide

## Overview

Grapa was originally designed as a **language for creating languages**. The current Grapa syntax you're familiar with was actually just an example that evolved into something powerful! This guide shows you how to use Grapa's original purpose - creating your own grammars and languages.

> **Note**: This guide uses Grapa syntax for action codes (e.g., `{$1 + $3}`) which can contain any valid Grapa code. The action codes are processed through Grapa's full compilation and execution pipeline.

## Key Concepts

### 1. **Left Recursion is Your Friend**

Traditional parsers can't handle left recursion, but Grapa automatically detects and handles it using a sophisticated two-phase approach:

```grapa
// ✅ GOOD: Natural left-associative expressions
@global["$additive"]
    = rule <$additive> '+' <$multiplicative> {$1 + $3}
    | <$additive> '-' <$multiplicative> {$1 - $3}
    | <$multiplicative>
    ;
```

This enables natural mathematical expressions like `a + b + c` without complex precedence tables.

### 2. **Operator Precedence Through Grammar Structure**

Precedence emerges from grammar hierarchy - it's not hardcoded:

```grapa
// ✅ CORRECT: Precedence through grammar levels
@global["$expr_or"]      // Lowest precedence
    = rule <$expr_or> '||' <$expr_and> {$1 || $3}
    | <$expr_and>
    ;

@global["$expr_and"]     // Higher precedence
    = rule <$expr_and> '&&' <$expr_equality> {$1 && $3}
    | <$expr_equality>
    ;

@global["$expr_equality"] // Even higher precedence
    = rule <$expr_equality> '==' <$expr_comparison> {$1 == $3}
    | <$expr_equality> '!=' <$expr_comparison> {$1 != $3}
    | <$expr_comparison>
    ;
```

**Precedence Rules:**
- **Higher precedence** = **Lower in grammar hierarchy**
- **Left-associative** = **Left-recursive rules**
- **Right-associative** = **Right-recursive rules** (rare)

### 3. **Action Codes Build Execution Trees**

When a rule matches, Grapa creates execution tree nodes:

```grapa
// Action code format: {grapa_expression}
@global["$addition"]
    = rule <$term> '+' <$addition> {$1 + $3}
    | <$term>
    ;
```

**Action Code Best Practices:**
```grapa
// ✅ GOOD: Clear operation names
@global["$string_concat"]
    = rule <$string> '+' <$string_concat> {$1 + $3}
    | <$string>
    ;

// ✅ GOOD: Multiple parameters
@global["$function_call"]
    = rule <$identifier> '(' <$argument_list> ')' {$1($3)}
    | <$identifier> '(' ')' {$1()}
    ;

// ✅ GOOD: Conditional operations
@global["$conditional"]
    = rule <$condition> '?' <$true_expr> ':' <$false_expr> {$1 ? $3 : $5}
    | <$condition>
    ;
```

### 4. **Token Types and Matching**

Grapa uses a sophisticated token type system:

```grapa
// Built-in token types
@global["$identifier"]
    = rule <$litname>  // Matches literal names

@global["$number"]
    = rule <$lit>      // Matches literals (INT, FLOAT, STR)

@global["$operator"]
    = rule '+' | '-' | '*' | '/'  // Direct character matching
```

**Custom Token Types:**
```grapa
// ✅ GOOD: Define custom tokens for clarity
@global["$custom_operator"]
    = rule '++' | '--' | '**' | '*/'  // Multi-character operators

@global["$keyword"]
    = rule 'if' | 'while' | 'for' | 'return'  // Reserved words
```

### 5. **Error Handling and Recovery**

Grapa's parser is designed for graceful error recovery:

```grapa
// ✅ GOOD: Provide fallback alternatives
@global["$statement"]
    = rule <$if_statement>
    | <$while_statement>
    | <$assignment>
    | <$expression> ';'  // Expression as statement
    | ';'  // Empty statement
    ;
```

## Common Problems and Solutions

### **Problem 1: Infinite Recursion**
```grapa
// ❌ BAD: Direct left recursion without base case
@global["$bad_rule"]
    = rule <$bad_rule> '+' <$term> {$1 + $3}
    ;

// ✅ GOOD: Include base case
@global["$good_rule"]
    = rule <$good_rule> '+' <$term> {$1 + $3}
    | <$term>  // Base case
    ;
```

### **Problem 2: Ambiguous Grammar**
```grapa
// ❌ BAD: Ambiguous - both rules can match same input
@global["$ambiguous"]
    = rule <$expr> '+' <$expr> {$1 + $3}
    | <$expr> '*' <$expr> {$1 * $3}
    ;

// ✅ GOOD: Clear precedence hierarchy
@global["$unambiguous"]
    = rule <$additive> '+' <$multiplicative> {$1 + $3}
    | <$multiplicative>
    ;
```

### **Problem 3: Missing Error Recovery**
```grapa
// ❌ BAD: No error recovery
@global["$fragile"]
    = rule <$perfect_match>  // Fails on any error

// ✅ GOOD: Graceful error handling
@global["$robust"]
    = rule <$perfect_match>
    | <$error_recovery>  // Handle errors gracefully
    ;
```

## Performance Considerations

### **Grammar Optimization**
```grapa
// ✅ GOOD: Minimize backtracking
@global["$efficient_parsing"]
    = rule <$unique_prefix> <$rest>  // Unique first token
    | <$another_unique> <$rest2>
    ;

// ❌ AVOID: Ambiguous prefixes
@global["$inefficient_parsing"]
    = rule <$ambiguous> <$rest>  // Multiple rules start same way
    | <$ambiguous> <$rest2>
    ;
```

## Testing Your Grammar

### **Test Cases to Include**
```grapa
// Test precedence
"a + b * c"  // Should be: a + (b * c)

// Test associativity
"a - b - c"  // Should be: (a - b) - c

// Test error recovery
"a + ; b"    // Should recover and parse "b"

// Test edge cases
""           // Empty input
"a"          // Single token
"a + b + c"  // Multiple operations
```

### **Validation Checklist**
- [ ] **Precedence** works correctly
- [ ] **Associativity** is as expected
- [ ] **Error recovery** handles bad input
- [ ] **Performance** is acceptable
- [ ] **Edge cases** are handled

## Practical Example: Calculator Grammar

Here's a complete example of a simple calculator grammar:

```grapa
// calculator.grc
@global["$expression"]
    = rule <$expression> '+' <$term> {$1 + $3}
    | <$expression> '-' <$term> {$1 - $3}
    | <$term>
    ;

@global["$term"]
    = rule <$term> '*' <$factor> {$1 * $3}
    | <$term> '/' <$factor> {$1 / $3}
    | <$factor>
    ;

@global["$factor"]
    = rule '(' <$expression> ')' {$2}
    | <$number>
    ;

@global["$number"]
    = rule <$lit>
    ;
```

## Integration with Grapa System

### **Loading Your Grammar**
```grapa
// Load grammar file
include "my_grammar.grc"

// Use your rules
result = parse_my_language("input text")
```

### **Extending Existing Grammar**
```grapa
// Extend existing rules
@global["$extended_expression"]
    = rule <$existing_expression>
    | <$my_new_operator> <$extended_expression>
    ;
```

## Advanced Features

### **Complex Action Codes**
Since action codes can contain any valid Grapa code, you can do complex operations:

```grapa
@global["$complex_operation"]
    = rule <$value> '+' <$value> {
        // Complex Grapa code here
        if ($1.type() == $STR && $3.type() == $STR) {
            $1 + $3  // String concatenation
        } else {
            $1.int() + $3.int()  // Numeric addition
        }
    }
    | <$value>
    ;
```

### **Conditional Parsing**
```grapa
@global["$conditional_parse"]
    = rule <$condition> '?' <$true_expr> ':' <$false_expr> {
        $1 ? $3 : $5
    }
    | <$condition>
    ;
```

## Conclusion

Key takeaways for grammar authors:

1. **Left recursion is powerful** - use it for natural expression parsing
2. **Precedence emerges from structure** - design your grammar hierarchy carefully
3. **Action codes can contain any Grapa code** - they're processed through the full pipeline
4. **Error recovery is essential** - make your grammar robust
5. **Test thoroughly** - precedence and associativity are critical
6. **Performance matters** - avoid ambiguous or inefficient patterns

The Grapa grammar system is surprisingly powerful and flexible, but understanding these principles will help you create effective, maintainable grammars.

## Next Steps

- **Try the calculator example** above
- **Experiment with your own grammars**
- **Check the [API Reference](../API_REFERENCE.md)** for available functions
- **Look at [Examples](../EXAMPLES.md)** for more complex patterns
- **Join the community** to share your grammar designs

---

*Grapa: A language for creating languages, evolved into something much more.* 