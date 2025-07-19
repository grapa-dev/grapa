# Education & Prototyping

Grapa's dynamic grammar and meta-programming are ideal for rapid prototyping and teaching programming concepts.

## Key Features for Education:
- **Interactive Learning**: Immediate feedback and experimentation
- **Visual Debugging**: See AST and execution flow
- **Language Design**: Create custom languages and DSLs
- **Algorithm Visualization**: Step-through execution

## Example: Teaching Recursion
```grapa
/* Visualize recursive function execution */
factorial_with_trace = op(n, depth) {
    indent = "  ".repeat(depth);
    (indent + "factorial(" + n.str() + ")").echo();
    if (n <= 1) {
        (indent + "return 1").echo();
        1;
    } else {
        result = n * factorial_with_trace(n - 1, depth + 1);
        (indent + "return " + n.str() + " * " + (n - 1).str() + "! = " + result.str()).echo();
        result;
    };
};

/* Example: Calculate 4! with trace */
"Calculating 4! with execution trace:".echo();
result = factorial_with_trace(4, 0);
("Final result: " + result.str()).echo();
```

## Example: Custom DSL for Data Processing
```grapa
/* Define a simple data processing language */
data_dsl_grammar = {
    "pipeline": "command*",
    "command": "filter_command | map_command | reduce_command",
    "filter_command": "'filter' condition",
    "map_command": "'map' expression", 
    "reduce_command": "'reduce' expression 'with' initial_value",
    "condition": "field operator value",
    "expression": "field | function_call",
    "operator": "'==' | '!=' | '>' | '<' | '>=' | '<='"
};

/* Example DSL usage */
dsl_script = "
filter age > 18
map salary * 1.1
reduce sum with 0
";

/* Execute the DSL */
execute_dsl = op(script, data) {
    ast = $rule().parse(script, data_dsl_grammar);
    $rule().execute(ast, data);
};
```

## Example: Algorithm Visualization
```grapa
/* Visualize sorting algorithms */
bubble_sort_with_trace = op(array) {
    n = array.len();
    trace = [];
    
    i = 0;
    while (i < n - 1) {
        j = 0;
        while (j < n - i - 1) {
            /* Show current state */
            current_state = array.copy();
            trace += {
                "step": "Compare " + array.get(j).str() + " and " + array.get(j + 1).str(),
                "array": current_state,
                "comparing": [j, j + 1]
            };
            
            /* Swap if needed */
            if (array.get(j) > array.get(j + 1)) {
                temp = array.get(j);
                array.set(j, array.get(j + 1));
                array.set(j + 1, temp);
                
                trace += {
                    "step": "Swap " + array.get(j).str() + " and " + array.get(j + 1).str(),
                    "array": array.copy(),
                    "swapped": [j, j + 1]
                };
            };
            j += 1;
        };
        i += 1;
    };
    
    {"sorted_array": array, "trace": trace};
};

/* Example usage */
data = [64, 34, 25, 12, 22, 11, 90];
result = bubble_sort_with_trace(data);

"Bubble Sort Visualization:".echo();
result.get("trace").map(op(step) {
    (step.get("step") + ": " + step.get("array").join(", ")).echo();
});

("Final sorted array: " + result.get("sorted_array").join(", ")).echo();
```

## Example: Interactive Language Learning
```grapa
/* Interactive calculator with step-by-step evaluation */
interactive_calculator = op(expression) {
    /* Parse the expression */
    ast = $rule().parse(expression, {
        "expression": "term (( '+' | '-' ) term)*",
        "term": "factor (( '*' | '/' ) factor)*",
        "factor": "number | '(' expression ')'",
        "number": "[0-9]+"
    });
    
    /* Evaluate with trace */
    evaluate_with_trace = op(node, depth) {
        indent = "  ".repeat(depth);
        
        if (node.type() == "number") {
            value = node.get("value").int();
            (indent + "Number: " + value.str()).echo();
            value;
        } else if (node.type() == "expression") {
            (indent + "Expression: " + node.get("text")).echo();
            
            if (node.get("terms").len() == 1) {
                evaluate_with_trace(node.get("terms").get(0), depth + 1);
            } else {
                left = evaluate_with_trace(node.get("terms").get(0), depth + 1);
                operator = node.get("operators").get(0);
                right = evaluate_with_trace(node.get("terms").get(1), depth + 1);
                
                result = 0;
                if (operator == "+") {
                    result = left + right;
                } else if (operator == "-") {
                    result = left - right;
                };
                
                (indent + "Result: " + left.str() + " " + operator + " " + right.str() + " = " + result.str()).echo();
                result;
            };
        } else {
            /* Handle other node types */
            0;
        };
    };
    
    evaluate_with_trace(ast, 0);
};

/* Example usage */
"Interactive Calculator:".echo();
result = interactive_calculator("2 + 3 * 4");
("Final result: " + result.str()).echo();
```

## Example: Prototype Web Framework
```grapa
/* Simple web framework prototype */
simple_web_framework = op() {
    routes = {};
    
    {
        "get": op(path, handler) {
            routes.set("GET:" + path, handler);
        },
        "post": op(path, handler) {
            routes.set("POST:" + path, handler);
        },
        "handle_request": op(method, path, data) {
            route_key = method + ":" + path;
            if (routes.has(route_key)) {
                handler = routes.get(route_key);
                handler(data);
            } else {
                {"status": 404, "body": "Not Found"};
            };
        }
    };
};

/* Example usage */
app = simple_web_framework();

/* Define routes */
app.get("/hello", op(data) {
    {"status": 200, "body": "Hello, World!"};
});

app.post("/echo", op(data) {
    {"status": 200, "body": "Echo: " + data.get("message")};
});

/* Handle requests */
response1 = app.handle_request("GET", "/hello", {});
response2 = app.handle_request("POST", "/echo", {"message": "Hello from Grapa!"});

("Response 1: " + response1.get("body")).echo();
("Response 2: " + response2.get("body")).echo();
```

- **See also:** [Python Education Examples](../PYTHON_USE_CASES.md#7-education-prototyping) 