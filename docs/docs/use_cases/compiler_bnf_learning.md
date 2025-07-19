# Compiler/BNF Learning

With executable BNF and mutable grammar, Grapa is perfect for teaching and experimenting with language design, parsing, and compiler construction.

## Key Features for BNF/Compiler Work:
- **Executable BNF**: Define and test grammars directly in Grapa
- **Mutable Grammar**: Modify language syntax at runtime
- **AST Manipulation**: Build and transform abstract syntax trees
- **Meta-programming**: Generate code and modify language behavior

## Example: Simple Expression Parser
```grapa
/* Define a simple arithmetic expression grammar */
expression_grammar = {
    "expression": "term (( '+' | '-' ) term)*",
    "term": "factor (( '*' | '/' ) factor)*",
    "factor": "number | '(' expression ')',",
    "number": "[0-9]+"
};

/* Parse and evaluate expressions */
parse_expression = op(expr) {
    ast = $rule().parse(expr, expression_grammar);
    $rule().evaluate(ast);
};

/* Test the parser */
test_expressions = ["2+3*4", "(1+2)*3", "10/2+5"];
results = test_expressions.map(op(expr) { parse_expression(expr); });
```

## Example: Custom Language Extension
```grapa
/* Extend Grapa with a custom "repeat" construct */
extend_grammar = {
    "statement": "original_statement | repeat_statement",
    "repeat_statement": "'repeat' number 'times' '{' statement* '}'"
};

/* Implement the repeat functionality */
execute_repeat = op(ast) {
    if (ast.get("type") == "repeat_statement") {
        count = ast.get("count");
        body = ast.get("body");
        i = 0;
        while (i < count) {
            $rule().execute(body);
            i += 1;
        };
    };
};
```

- **See also:** [Python BNF/Compiler Examples](../PYTHON_USE_CASES.md#2-compilerbnf-learning) 