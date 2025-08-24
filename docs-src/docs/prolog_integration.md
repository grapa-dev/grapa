---
tags:
  - user
  - highlevel
  - prolog
  - logic
  - ai
  - knowledge
---
# PROLOG Integration in Grapa

Grapa's executable BNF architecture allows you to add **true native PROLOG syntax** to the language using `custom_command` and `custom_function` rules. This is **not** manual parsing - it's **native syntax** that integrates seamlessly with Grapa's grammar system.

## True Native Syntax Implementation

The PROLOG integration uses Grapa's **rule-based grammar system** exclusively. All parsing is handled by Grapa's grammar engine - **no manual string manipulation** is required.

### Example PROLOG Syntax Implementation

```grapa
/* Initialize custom_command and custom_function for PROLOG syntax */
custom_command = rule $STR '(' $STR ',' $STR ')' '.' {op(predicate:$1,arg1:$3,arg2:$5){
    /* Store fact in knowledge base - NO MANUAL PARSING */
    fact_key = predicate + "_" + arg1 + "_" + arg2;
    fact_value = predicate + "(" + arg1 + "," + arg2 + ")";
    kb.set(fact_key, fact_value);
}};

/* Fact with single argument */
custom_command ++= rule $STR '(' $STR ')' '.' {op(predicate:$1,arg1:$3){
    /* Store single-argument fact - NO MANUAL PARSING */
    fact_key = predicate + "_" + arg1;
    fact_value = predicate + "(" + arg1 + ")";
    kb.set(fact_key, fact_value);
}};

/* Rule storage with native grammar */
custom_command ++= rule $STR '(' $STR ',' $STR ')' ':-' $STR '(' $STR ',' $STR ')' '.' {op(head_pred:$1,head_arg1:$3,head_arg2:$5,body_pred:$7,body_arg1:$9,body_arg2:$11){
    /* Store rule in knowledge base - NO MANUAL PARSING */
    rule_key = "rule_" + head_pred + "_" + head_arg1 + "_" + head_arg2;
    rule_value = head_pred + "(" + head_arg1 + "," + head_arg2 + ") :- " + body_pred + "(" + body_arg1 + "," + body_arg2 + ")";
    kb.set(rule_key, rule_value);
}};

/* Rule with single argument */
custom_command ++= rule $STR '(' $STR ')' ':-' $STR '(' $STR ')' '.' {op(head_pred:$1,head_arg1:$3,body_pred:$5,body_arg1:$7){
    /* Store single-argument rule - NO MANUAL PARSING */
    rule_key = "rule_" + head_pred + "_" + head_arg1;
    rule_value = head_pred + "(" + head_arg1 + ") :- " + body_pred + "(" + body_arg1 + ")";
    kb.set(rule_key, rule_value);
}};

/* Query execution with native grammar */
custom_function = rule '?-' $STR '(' $STR ',' $STR ')' {op(predicate:$2,arg1:$4,arg2:$6){
    /* PROLOG query processing - NO MANUAL PARSING */
    /* Implementation receives already-parsed tokens from Grapa's grammar */
}};

/* Query with single variable */
custom_function ++= rule '?-' $STR '(' $STR ',' 'X' ')' {op(predicate:$2,arg1:$4){
    /* PROLOG query processing - NO MANUAL PARSING */
}};

/* Query with single argument */
custom_function ++= rule '?-' $STR '(' $STR ')' {op(predicate:$2,arg1:$4){
    /* PROLOG query processing - NO MANUAL PARSING */
}};
```

**Key Point**: The `++=` operator **appends** to existing rules, allowing multiple PROLOG syntax patterns to coexist.

### Using PROLOG Syntax

Once defined, PROLOG syntax works **natively** in Grapa:

```grapa
/* Execute PROLOG using op()() - TRUE NATIVE SYNTAX */
op()("parent(john,mary)")();
op()("ancestor(X,Y) :- parent(X,Y)")();
results = op()("?- parent(john,X)")();

/* Use in expressions */
parent_count = op()("?- parent(john,X)")();
if (parent_count > 1) {
    ("John has multiple children").echo();
};
```

## Power of Syntax Extension

This demonstrates **true native syntax** - not string storage or manual parsing:

- **Grammar Integration**: PROLOG syntax is processed by the same parser that handles all Grapa syntax
- **Token Capture**: Individual components (predicates, arguments, variables) are captured as separate tokens
- **Structured Data**: Implementation code receives already-parsed, structured data
- **No Manual Parsing**: Zero string manipulation or manual parsing required
- **Seamless Integration**: PROLOG expressions work in Grapa expressions, conditionals, and functions
- **Rule Addition**: Uses `|` operator to add to existing grammar rules without replacement

## Two Implementation Approaches

### 1. Syntax Extension (Recommended)
Uses `custom_command ++= rule` and `custom_function ++= rule` to define **true native syntax**:

```grapa
/* TRUE NATIVE SYNTAX - No manual parsing */
custom_command = rule $STR '(' $STR ')' '.' {op(predicate:$1,args:$3){
    /* Grapa grammar provides already-parsed tokens */
}};
```

### 2. String-based (Not Recommended)
Uses manual string parsing and manipulation:

```grapa
/* MANUAL PARSING - Not true native syntax */
prolog_fact = op(predicate, args) {
    /* Manual parsing required */
    fact_str = predicate + "(" + args + ")";
    /* ... manual string manipulation ... */
};
```

**Recommendation**: Always use **syntax extension** for true native language integration.

## PROLOG Syntax Extension Pattern

The key features for true native syntax are:

- **`custom_command = rule`**: Defines initial fact and rule syntax
- **`custom_command ++= rule`**: Adds additional fact and rule syntax patterns
- **`custom_function = rule`**: Defines initial query syntax (`?-`)
- **`custom_function ++= rule`**: Adds additional query syntax patterns
- **`op()(script)()`**: Executes the custom syntax
- **`script.exec()`**: Alternative syntax for executing custom syntax (more readable)
- **Token Capture**: Use `$STR` to capture predicates and arguments
- **No Manual Parsing**: Let Grapa's grammar handle all parsing
- **Rule Addition**: Use `|` operator to add to existing rules

## Comparison with Other Languages

Unlike other programming languages that require:
- Manual parser implementation
- String manipulation
- Token extraction
- Custom lexers

Grapa provides **true native syntax** through its executable BNF architecture, making PROLOG (and other languages) **first-class citizens** in the language.

## Use Cases

- **Logic Programming**: Native PROLOG syntax for rule-based reasoning
- **Knowledge Representation**: Facts and rules in PROLOG syntax
- **Query Processing**: PROLOG queries embedded in Grapa applications
- **Multi-Syntax Applications**: Mix PROLOG, SQL, JSON, and custom DSLs seamlessly

This demonstrates Grapa's unique capability to add **true native syntax** for other programming languages and domain-specific languages through its rule-based grammar system.
