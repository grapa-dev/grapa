# Grammar Design Guide for Grapa $RULE Authors

## Overview

This guide provides practical insights for anyone defining their own grammar using Grapa's `$RULE` system, based on learnings from the core compilation and execution process.

> **Note**: This guide uses Grapa syntax for action codes (e.g., `{$1 + $3}`) instead of internal action codes (e.g., `{@<add,{$1,$3}>}`). This makes the examples more accessible and allows this guide to be included in user documentation.

## Key Learnings for Grammar Authors

### 1. **Left Recursion is Your Friend (But Tricky)**

#### **The Problem**
Traditional recursive descent parsers can't handle left recursion:
```grapa
// This would cause infinite recursion in naive parsers
@global["$expression"]
    = rule <$expression> '+' <$term> {@<add,{$1,$3}>}
    | <$term>
    ;
```

#### **Grapa's Solution**
Grapa automatically detects and handles left recursion using a sophisticated two-phase approach:

**Phase 1**: Parse non-left-recursive alternatives first
**Phase 2**: Build left-recursive structure recursively

#### **Best Practices for Left Recursion**
```grapa
// ✅ GOOD: Natural left-associative expressions
@global["$additive"]
    = rule <$additive> '+' <$multiplicative> {$1 + $3}
    | <$additive> '-' <$multiplicative> {$1 - $3}
    | <$multiplicative>
    ;

// ✅ GOOD: Clear precedence hierarchy
@global["$multiplicative"]
    = rule <$multiplicative> '*' <$power> {$1 * $3}
    | <$multiplicative> '/' <$power> {$1 / $3}
    | <$power>
    ;

@global["$power"]
    = rule <$power> '**' <$primary> {$1 ** $3}
    | <$primary>
    ;
```

### 2. **Operator Precedence Through Grammar Structure**

#### **The Insight**
Operator precedence is **not hardcoded** but emerges from grammar structure:

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

#### **Precedence Rules**
1. **Higher precedence** = **Lower in grammar hierarchy**
2. **Left-associative** = **Left-recursive rules**
3. **Right-associative** = **Right-recursive rules** (rare)

### 3. **Action Codes and Tree Building**

#### **Understanding Action Codes**
When a rule matches, Grapa creates execution tree nodes:

```grapa
// Action code format: {grapa_expression}
@global["$addition"]
    = rule <$term> '+' <$addition> {$1 + $3}
    | <$term>
    ;
```

#### **Action Code Best Practices**
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

#### **Token Type System**
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

#### **Custom Token Types**
```grapa
// ✅ GOOD: Define custom tokens for clarity
@global["$custom_operator"]
    = rule '++' | '--' | '**' | '*/'  // Multi-character operators

@global["$keyword"]
    = rule 'if' | 'while' | 'for' | 'return'  // Reserved words
```

### 5. **Error Handling and Recovery**

#### **Graceful Failure**
Grapa's parser is designed to handle errors gracefully:

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

#### **Error Recovery Patterns**
```grapa
// ✅ GOOD: Skip to next statement on error
@global["$statement_list"]
    = rule <$statement> <$statement_list>
    | <$statement>
    | <$error_recovery> <$statement_list>  // Skip bad tokens
    ;
```

### 6. **Performance Considerations**

#### **Grammar Optimization**
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

#### **Caching and Memoization**
- Grapa automatically caches rule lookups
- **Rule caching** improves performance for repeated patterns
- **Token caching** reduces re-parsing overhead

### 7. **Debugging Grammar Issues**

#### **Common Problems and Solutions**

**Problem 1: Infinite Recursion**
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

**Problem 2: Ambiguous Grammar**
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

**Problem 3: Missing Error Recovery**
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

### 8. **Testing Your Grammar**

#### **Test Cases to Include**
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

#### **Validation Checklist**
- [ ] **Precedence** works correctly
- [ ] **Associativity** is as expected
- [ ] **Error recovery** handles bad input
- [ ] **Performance** is acceptable
- [ ] **Edge cases** are handled

### 9. **Integration with Grapa System**

#### **Loading Your Grammar**
```grapa
// Load grammar file
include "my_grammar.grc"

// Use your rules
result = parse_my_language("input text")
```

#### **Extending Existing Grammar**
```grapa
// Extend existing rules
@global["$extended_expression"]
    = rule <$existing_expression>
    | <$my_new_operator> <$extended_expression>
    ;
```

## Practical Example: Custom DSL

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

## Conclusion

Key takeaways for grammar authors:

1. **Left recursion is powerful** - use it for natural expression parsing
2. **Precedence emerges from structure** - design your grammar hierarchy carefully
3. **Action codes build execution trees** - they're not just for decoration
4. **Error recovery is essential** - make your grammar robust
5. **Test thoroughly** - precedence and associativity are critical
6. **Performance matters** - avoid ambiguous or inefficient patterns

The Grapa grammar system is surprisingly powerful and flexible, but understanding these principles will help you create effective, maintainable grammars.

## User Documentation Potential

This guide has been written using Grapa syntax instead of internal action codes, making it suitable for inclusion in user documentation. The examples are:

- **Immediately understandable** to Grapa users
- **Directly executable** as grammar definitions
- **Self-contained** with complete working examples
- **Educational** for learning grammar design principles

This approach bridges the gap between internal implementation details and user-facing documentation, providing valuable insights while remaining accessible. 