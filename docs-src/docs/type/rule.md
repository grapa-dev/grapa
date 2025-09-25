# $RULE

/*
Design Influences:
Grapa's $RULE type is inspired by Prolog's executable BNF and logic programming paradigm, enabling dynamic, mutable grammars and rule-based execution. This design empowers users to experiment with language syntax and parsing, making Grapa especially valuable for educators, researchers, and anyone interested in language design. The overarching goal is to make Grapa (and GrapaPy) as inviting and usable as possible for the broadest audience.
*/
References:


This datatype is basis of the grapa language. The syntax of the language is implemented as a set of global rule variables that are accessible and changeable - making the grapa language syntax dynamically mutable, either globally, or modified within a specific function by creating local variable rules that override the global rules. Rules variables can also be defined to support parsing of a domain specific language, or defining a data ETL task as a language by defining the rules for the data and applying the data to the rules - in the same way a language would be defined.

### **The `@` vs `$` Symbols in Grapa**

#### **`@` - Runtime Evaluation**
The `@` symbol is used for **runtime evaluation** and dereferencing. In rule tokens (like `{@<assignappend,{$1,$4}>}`), `@` serves as a **dereferencing operator** that gets the actual function/operation at runtime.

**Examples:**
```grapa
{@<assignappend,{$1,$4}>}  /* @ dereferences to get the actual assignappend function */
{@<if,{$3,$5}>}            /* @ dereferences to get the actual if function */
<@variable>                 /* @ dereferences to get the actual value of variable */
```

#### **`$` - Compile-time Lexical Processing**
The `$` symbol is used for **compile-time lexical processing** in the Grapa lexer. It acts as an escape character that modifies how tokens are processed during compilation.

**Lexical Modifiers:**
```grapa
$ID      /* Converts to $SYSID (system identifier) */
$STR     /* Converts to $SYSSTR (system string) */
$INT     /* Converts to $SYSINT (system integer) */
$[       /* Lexical flag for special processing */
$]       /* Lexical flag for special processing */
$&       /* Lexical flag for XML/HTML processing */
```

**Why the distinction matters:**
- `@` operates at **runtime** - gets actual values and executes functions
- `$` operates at **compile-time** - modifies how the lexer processes tokens
- `@` is for **evaluation**, `$` is for **lexical transformation**

There are three basic steps:
* Define the rules (rules may reference other rules) with code to execute for each rule option.
* Apply raw data to the rule to generate an execution plan.
* Execute the plan, with parameters if specified.

Example system rule:
```
> @$start
rule <$command_list> $SYSID("BE") | <$command_list> ';' | <$command_list>
```

The above is the entry point to the grapa language. This entry point is a global variable that can be updated...so take caution as it is the default entry point all scripts. It is also possible to navigate the language syntax by looking up each rule that is lined from any other rule.

Example of defining a custom rule, and applying the rule:
```
> x = rule $INT $INT {op(a:$1,b:$2){a*b}} | $INT {op(a:$1){a}}
> (op()("4",x))()
4

> (op()("4 3",x))()
12

> op()("4",x)
@<[op,@<[op,@<var,{a}>],{"a":4}>],{}>

> op()("4 3",x)
@<[op,@<[op,@<mul,{@<var,{a}>,@<var,{b}>}>],{"a":4,"b":3}>],{}>
```

To simplify creating rules that depend on matching on a predefined list of values, create a list and reference the list in the rules. If the rule matches, the value of the matched item is passed in as the token value.

```
> t = {x:"matched on x",y:5};
> r = rule $INT @t $INT {op(a:$2){a}};
> (op()("1 x 2",r))();
matched on x

> (op()("1 y 2",r))();
5
```

If the lookup needs to be against something other than a list (maybe checking a database or even a web service), an operation can be added to a rule token that will run at compile time if the rule matches. Note that this will be called during the planning phase (or compile time) every time the rule is evaluated - so take caution on attaching a heavy workload. The result of the operation will be used as the result of the token matched. 

```
> x = rule $STR {@<lit,{$1}>};
> r = rule $INT <x,op(a:$1){a.len()}> $INT {op(a:$2){a}};
> (op()("44 'x' 22",r))();
1
```

Rules can also be embedded. Unfortunately, the current grammar requires an operation for a rule if the rule is to return any value - it doesn't make any assumptions. So as of the current release an embedded rule of (x|y) would match on x or y, but with no operation attached to x or y the result would not produce an output for the match. In some future version, this will be addressed to return a default. For now, an operation needs to be inserted if you want a value to be returned.

If the token handler returns an $ERR object, it will cause the rule to fail. So it not only has the ability to add additional logic to processing the token, is also can also serve as an additional component to the planning engine by validating the token against the intent of the rule - something that could not be done at a later phase and could only be done during the planning/compile phase. 

```
> r = rule $INT (x{@<lit,{"found x"}>}|y{@<lit,{"found y"}>}) $INT {op(a:$2){a}};
> (op()("44 y 22",r))();
found y
```

If there is a well known end sequence, and you want to collect all the tokens in between, you can insert an empty rule. All of the tokens, including any white space, will be passed on. The risk of using this in a rule is that if the end token does not exist, all tokens will be processed in the empty rule. 

```
> r = rule $INT <> $INT {op(a:$2){a}};
> (op()("44 'x' 22",r))();
[" ","x"," "]
```

The stop rule for a catchall can be either a token, a rule reference, or an embedded rule.

```
> e = rule $INT {@<lit,{$1}>} | $ID {@<lit,{$1}>};
> r = rule $INT <> <e> {op(a:$3){a}};
> (op()("44 'x' 22",r))();
22

> (op()("44 'x' hi",r))();
hi

> r = rule $INT <> ($INT{@<lit,{$1}>}|$ID{@<lit,{$1}>}) {op(a:$3){a}};
> (op()("44 'x' 22",r))();
22

> (op()("44 'x' hi",r))();
hi
```

### **Start/Stop Token Sequences with Parentheses**

When you need a sequence of tokens as the start or stop markers, use parentheses to define embedded rules. This allows you to specify multi-token sequences without defining separate rules.

**Simple single tokens:**
```grapa
/* Single token start/stop */
catch_sq_bracket_rule = rule '[' <> ']' {@<lit,{$2}>};
```

**Multi-token sequences with parentheses:**
```grapa
/* Multi-token sequence start/stop */
catch_sq_bracket_rule = rule ('[') <> (']') {@<lit,{$2}>};
/* This is equivalent to: rule '[' <> ']' {@<lit,{$2}>} */

/* More complex multi-token sequences */
xml_comment_rule = rule ('<' '!' '-' '-') <> ('-' '-' '>') {@<lit,{$2}>};
/* Captures everything between <!-- and --> */
```

**Advanced XML parsing example from Grapa's grammar:**
```grapa
/* Sophisticated XML comment parsing with token joining */
$global["$elSkipValueY"]
    = rule '-' <@<join,{$1}>> ('-' '-' '>') {@<lit,{$2}>}
    ;
$global["$elSkipValueX"]
    = rule <$elSkipValueY> {@<createxml,{$1}>}
    ;
$global["$elSkipValue"]
    = rule <$elSkipValueX> {@<createel,{{},$1}>}
    ;
$global["$tagHtmlValue"]
    = rule '<' '!' '-' <$elSkipValue> {@<name,{"!--",$4}>}
    ;
```

**Key Points:**
- **Parentheses `()`**: Define embedded rules for multi-token sequences
- **Single tokens**: Can be used directly without parentheses
- **Token joining**: Use `<@<join,{$1}>>` to concatenate collected tokens
- **Complex sequences**: Parentheses allow complex start/stop patterns

A operation can also be associated with the empty rule above, which can process the tokens and either pass through the list, or pass through some transformed version. The following is an example f transforming.

```
> r = rule $INT <op(a:$1){a.len().str()+" raw characters:"+a.join("")}> $INT {op(a:$2){a}};
> (op()("44 'x' 22",r))();
3 raw characters: x
```

If the token handler returns an $ERR object, the corresponding rule option will fail and the next rule option will be evaluated. The following is an example of causing the first rule option to fail.

```
> r = rule $INT <op(a:$1){$ERR()}> $INT {op(a:$2){a}} | $INT <> $INT {op(a:$2){a}};
> (op()("44 'x' 22",r))();
[" ","x"," "]
```

The catchall token can also be used to add support for comments in a grammar. The following is used in the grapa syntax. This rule is then inserted in places in the grammar where comments can be included. 

```
$starcomment = rule ('/' '*') <> ('*' '/');
```

Rules can include both right recursion and left recursion.  The following is an example of right recursion. Note that an internal function "prepend" is used to build the list. This is because grapa uses recursive decent, which results in the last match being processed first.

```
> r1 = rule $ID <r1> {@<prepend,{$2,$1}>} | $ID {@<createarray,{$1}>};
> (op()("a b c",r1))();
[a,b,c]
```

But right recursion may not be desirable in some cases. One example is order of operations for additon and subtraction, where the evaluation needs to happen from left to right. Here is an example using right recursion. 

```
r1 = rule 
    $INT '+' <r1>             {@<add,{$1,$3}>} 
  | $INT '-' <r1>              {@<sub,{$1,$3}>} 
  | $INT                            {@<lit,{$1}>}
  ;

> op()("5-3+2",r1);
@<[op,@<sub,{5,@<add,{3,2}>}>],{}>

> (op()("5-3+2",r1))();
0
```

Note that for "5-3+2", the additon of 3+2 happens first resulting in 5-(3+2), which produces the wrong answer. 

Left recursion is required to address this.

```
r2 = rule 
    <r2> '+' $INT       {@<add,{$1,$3}>} 
  | <r2> '-' $INT        {@<sub,{$1,$3}>} 
  | $INT                      {@<lit,{$1}>}
  ;

> op()("5-3+2",r2);
@<[op,@<add,{@<sub,{5,3}>,2}>],{}>

> (op()("5-3+2",r2))();
4
```

Note that with left recursion, the subtraction happens first resulting in (5-3)+2, which produces the correct answer.

## Rule Composition and Concatenation

Rules can be dynamically composed and extended using the `++=` operator (see [Assignment Operators](../operators/assignment.md#)), which allows you to concatenate additional rule alternatives to an existing rule. This enables dynamic grammar construction and runtime rule modification.

### Appending to Existing Rules

**Important**: To add additional alternatives to an existing rule, use the `++=` operator, not the `|` operator. The `|` operator only works within a single rule definition, while `++=` allows you to append to an existing rule variable.

#### Self-Modifying Grammar System

Grapa's grammar system is **self-modifying** - the BNF (Backus-Naur Form) in `lib/grapa/$grapa.grc` defines the rules for creating and modifying grammar rules themselves. This creates a powerful meta-programming capability where:

- **The grammar defines how to define grammar rules**
- **Rules can be modified at runtime using the same grammar system**
- **The `|` operator creates alternatives within a single rule definition**
- **The `++=` operator appends new alternatives to existing rule variables**

#### Token Types and Syntax

Based on the Grapa grammar system in `lib/grapa/$grapa.grc`, there are important distinctions between token types:

**Identifier Tokens (automatic conversion):**
```grapa
/* Define initial rule with identifier */
$custom_command = rule test {op(){'matched first'}};

/* Append additional alternatives using ++= */
$custom_command ++= rule test2 {op(){'matched second'}};
$custom_command ++= rule test3 {op(){'matched third'}};

/* Test the rule - use identifiers directly */
test;   /* Returns: 'matched first' */
test2;  /* Returns: 'matched second' */
test3;  /* Returns: 'matched third' */
```

**String Tokens (explicit conversion required):**
```grapa
/* Define initial rule with string token */
$custom_command = rule $STR('first') {op(){'matched first'}};

/* Append additional alternatives using ++= */
$custom_command ++= rule $STR('second') {op(){'matched second'}};
$custom_command ++= rule $STR('third') {op(){'matched third'}};

/* Test the rule - use string literals */
"first";   /* Returns: 'matched first' */
"second";  /* Returns: 'matched second' */
"third";   /* Returns: 'matched third' */
```

**Key Points:**
- **`++=` operator**: Appends new rule alternatives to existing rule variables
- **`|` operator**: Only works within a single rule definition for alternatives
- **`$ID` tokens**: Automatically converted (e.g., `test` becomes identifier)
- **`$STR` tokens**: Require explicit conversion (e.g., `$STR('test')` for strings)
- **`$INT` tokens**: Match integer literals (e.g., `42`, `100`, `-5`)
- **`$FLOAT` tokens**: Match floating-point literals (e.g., `3.14`, `-2.5`)
- **Dynamic grammar construction**: Enables building complex grammars incrementally
- **Runtime rule modification**: Rules can be extended during program execution
- **Self-modifying grammar**: The BNF defines how to modify the BNF itself

**Token Usage Guidelines:**
- **Function/predicate names**: Use `$ID` for unquoted identifiers or `$STR` for quoted strings
- **Arguments**: Use `$STR` for quoted strings, `$INT` for integers, or `$ID` for unquoted identifiers
- **Choice depends on calling syntax**: `$ID` requires unquoted calls, `$STR` requires quoted calls

#### BNF Structure for Rule Definition

The grammar system uses this pattern for defining rules:

```grapa
$global["$option_list"]
	= rule <$option> '|' <$option_list> {@<prepend,{$3,$1}>}
	| <$option> {@<createrule,{$1}>}
	;
```

This shows:
- **`= rule`**: Defines a new rule
- **`|`**: Creates alternatives within the same rule definition
- **`{@<createrule,{$1}>}`**: Creates a rule from the parsed tokens
- **`{@<prepend,{$3,$1}>}`**: Prepends new alternatives to existing rules

#### Rule Alternative Ordering for Backtracking

**Critical**: Rule alternatives must be ordered from **most specific to least specific** (longest to shortest token sequences). This is essential for proper backtracking in the rule engine.

**Correct Ordering Pattern:**
```grapa
$global["$list2"]
	= rule ',' ',' <$list2> {@<prepend,{$3,null,null}>}    /* 3 tokens */
	| ',' <$list2> {@<prepend,{$2,null}>}                  /* 2 tokens */
	| <$param2> ',' <$list2> {@<prepend,{$3,$1}>}          /* 3 tokens */
	| <$param2> ',' {@<createlist,{$1,null}>}              /* 2 tokens */
	| <$param2> {@<createlist,{$1}>}                       /* 1 token */
	| ',' {@<createlist,{null,null}>}                      /* 1 token */
	;
```

**Why This Ordering Matters:**
- **Backtracking**: The rule engine tries alternatives in order
- **Greedy Matching**: Longer patterns must come first to be matched
- **Prevention of Short-Circuiting**: If a simple rule comes first, it will always match and prevent longer, more specific rules from being reached
- **Ambiguity Resolution**: More specific rules should take precedence

**Example of Incorrect Ordering:**
```grapa
/* WRONG - Simple rule first will prevent longer rules from matching */
$custom_command = rule test {op(){'simple'}};                    /* 1 token - too early! */
$custom_command ++= rule test $STR {op(arg:$2){'with arg'}};     /* 2 tokens - never reached */
$custom_command ++= rule test $STR $STR {op(arg1:$2,arg2:$3){'with 2 args'}}; /* 3 tokens - never reached */
```

**Correct Ordering:**
```grapa
/* CORRECT - Most specific rules first */
$custom_command = rule test $STR $STR {op(arg1:$2,arg2:$3){'with 2 args'}}; /* 3 tokens */
$custom_command ++= rule test $STR {op(arg:$2){'with arg'}};     /* 2 tokens */
$custom_command ++= rule test {op(){'simple'}};                  /* 1 token - last */
```

#### Testing Custom Rules

**Important**: Custom rules must be tested using string execution since they cannot be used in the same script where they are defined (due to compilation timing).

```grapa
/* Define custom rules */
$custom_command = rule test {op(){'simple'}};
$custom_command ++= rule test $STR {op(arg:$2){'with arg: ' + arg}};

/* Test using string execution */
op()("test")();                    /* Returns: 'simple' */
op()("test hello")();              /* Returns: 'with arg: hello' */

/* Alternative testing method - more readable */
script1 = "test";
script2 = "test hello";
script1.exec();                    /* Returns: 'simple' */
script2.exec();                    /* Returns: 'with arg: hello' */

/* Both methods are equivalent:
   op()("test")()     ==  "test".exec()
   op()("test hello")() ==  "test hello".exec()
   
   Note: exec() cannot take parameters, but op()() can.
   For most testing scenarios, exec() provides better readability.
*/
```

**Note**: Custom rules do not persist when the application exits. They must be redefined in each session.

### Language Integration Example

This pattern is commonly used for implementing domain-specific languages and syntax extensions:

```grapa
/* Initialize SQL syntax */
$custom_command = rule select $STR from $STR {op(fields:$2,table:$4){
    ("SELECT " + fields + " FROM " + table).echo();
}};

/* Add INSERT syntax */
$custom_command ++= rule insert into $STR values $STR ',' $INT ',' $STR {op(table:$3,name:$6,age:$8,city:$10){
    ("INSERT INTO " + table + " VALUES " + name + "," + age + "," + city).echo();
}};

/* Add UPDATE syntax */
$custom_command ++= rule update $STR set $STR '=' $STR where $STR '=' $STR {op(table:$2,field:$4,value:$6,where_field:$8,where_value:$10){
    ("UPDATE " + table + " SET " + field + "=" + value + " WHERE " + where_field + "=" + where_value).echo();
}};

/* Add DELETE syntax */
$custom_command ++= rule delete from $STR where $STR '=' $STR {op(table:$3,where_field:$5,where_value:$7){
    ("DELETE FROM " + table + " WHERE " + where_field + "=" + where_value).echo();
}};
```

This approach enables **true native syntax** implementation where multiple language constructs can coexist using Grapa's grammar system.

### Basic Rule Concatenation

```
> x = rule $INT $INT {op(a:$1,b:$2){a*b}}
rule $INT $INT {@<[op,@<mul,{@<var,{a}>,@<var,{b}>}>],{"a":$1,"b":$2}>}

> x ++= rule $INT {op(a:$1){a}}
$INT $INT {@<[op,@<mul,{@<var,{a}>,@<var,{b}>}>],{"a":$1,"b":$2}>}| $INT {@<[op,@<var,{a}>],{"a":$1}>}

> (op()("4",x))()
4

> (op()("4 3",x))()
12
```

### Dynamic Grammar Building

This feature is particularly powerful for building domain-specific languages incrementally:

```
> // Start with a basic rule
> parser = rule $ID {op(a:$1){a}};

> // Add support for numbers
> parser ++= rule $INT {op(a:$1){a}};

> // Add support for string literals
> parser ++= rule $STR {op(a:$1){a}};

> // Add support for function calls
> parser ++= rule $ID '(' $ID ')' {op(a:$1,b:$3){a+" calls "+b}};

> // Test the expanded parser
> (op()("hello",parser))();
hello

> (op()("42",parser))();
42

> (op()("func(arg)",parser))();
func calls arg
```

### Use Cases

- **Incremental Language Development**: Build complex grammars step by step
- **Plugin Systems**: Allow extensions to add new syntax rules
- **Dynamic Parsing**: Modify parsing behavior based on runtime conditions
- **Domain-Specific Languages**: Create specialized parsers that can be extended

For those interested in how to modify a recursive decent parser to support left recursion, see the following on how this was addressed for Python:

https://medium.com/@gvanrossum_83706/left-recursive-peg-grammars-65dab3c580e1

A simular approach is used for Grapa - but with a few improvements on the appropach to support more complex scenarios than what's required to support the Python syntax alone.

## Language Syntax Extension

Grapa's most powerful feature is its ability to dynamically extend the language syntax at runtime. This allows you to create domain-specific languages, add custom commands, and modify the grammar without restarting the application.

### Custom Commands and Functions

The Grapa parser includes two special variables that allow you to inject custom syntax:

- **`$custom_command`**: For domain-specific commands that perform actions but don't return values
- **`$custom_function`**: For functions that return values

#### Basic Syntax Extension

```grapa
// Define a custom function that returns a value
$custom_function = rule select $INT {op(p:$2){p*5}};

// Now you can use it directly
select 4;        // Returns 20
x = select 8;    // x = 40

// Define a custom command that performs an action
$custom_command = rule reset_data {op(){clear_database()}};

// Use the custom command
reset_data;      // Executes clear_database()
```

#### How It Works

When you define `$custom_function` or `$custom_command`, you're essentially creating isolated rule execution for domain-specific processing. The parser:

1. **Falls through** the built-in command patterns
2. **Matches your custom rule** when the input fits the pattern
3. **Executes your code** with the matched parameters
4. **Returns results** (for functions) or performs actions (for commands)

### Scoping and Namespace Control

Custom syntax can be defined at different scopes, giving you precise control over where extensions are available:

#### Local Scope (Function-Level)

```grapa
function_with_custom_syntax() {
    // Custom syntax only available within this function
    $custom_function = rule local_cmd $INT {op(n:$2){n*2}};
    
    result = local_cmd 5;  // Works here
    // Function exits, local_cmd is gone
}

// Global scope - local_cmd is undefined
// local_cmd 5 would cause an error
```

#### Global Scope (Permanent)

```grapa
function_with_permanent_syntax() {
    // Promote to global namespace - available everywhere
    $global["$custom_function"] = rule global_cmd $INT {op(n:$2){n*3}};
    
    result = global_cmd 5;  // Works here
    // Function exits, but global_cmd is still available globally
}

// Global scope - global_cmd is still available
result = global_cmd 10;  // Works here too
```

#### Conditional Global Promotion

```grapa
function_with_conditional_syntax() {
    // Create local syntax first
    $custom_function = rule temp_cmd $INT {op(n:$2){n*4}};
    
    // Test it locally
    if (temp_cmd 5 == 20) {
        // If it works well, promote to global
        $global["$custom_function"] = rule temp_cmd $INT {op(n:$2){n*4}};
        echo "Syntax promoted to global";
    }
    
    // temp_cmd is now available globally
}
```

### Dynamic Compilation with Custom Syntax

**Important**: Custom syntax changes don't affect already-compiled code. They only apply to new code compiled with `op()`():

#### Compilation vs. Runtime

```grapa
// This script was compiled with the original grammar
function test() {
    // Even if we change $custom_function here, it won't affect
    // the already-compiled code in this function
    $custom_function = rule new_syntax $INT {op(n:$2){n*10}};
    
    // This line was compiled before the $custom_function change
    // so it still uses the old grammar
    old_syntax 5;  // Uses whatever was defined when script was compiled
}
```

#### Dynamic Compilation

```grapa
function dynamic_compilation() {
    // Set up custom syntax
    $custom_function = rule custom_cmd $INT {op(n:$2){n*5}};
    
    // Compile and execute new code dynamically
    script_text = "result = custom_cmd 10; echo result;";
    op()(script_text)();  // Uses current custom syntax
    
    // Or compile to a function for reuse
    compiled_function = op()(script_text);
    compiled_function();  // Executes with custom syntax
}
```

### Real-World Use Cases

#### Domain-Specific Languages

```grapa
function create_sql_dsl() {
    // Define SQL-like syntax
    $custom_function = rule SELECT $STR FROM $STR {op(fields:$2,table:$4){
        build_select_query(fields, table)
    }};
    
    $custom_function = rule WHERE $STR {op(condition:$2){
        build_where_clause(condition)
    }};
    
    // Now you can write SQL-like code
    script = "SELECT name,age FROM users WHERE age > 18;";
    query = op()(script)();
    return query;
}
```

#### Configuration-Driven Scripts

```grapa
function load_plugin(plugin_config) {
    // Set up plugin-specific syntax
    $custom_function = rule plugin_cmd $STR {op(cmd:$2){execute_plugin(cmd)}};
    
    // Compile plugin script with new syntax
    plugin_script = plugin_config.script;
    plugin_function = op()(plugin_script);
    
    // Execute with plugin syntax available
    plugin_function();
}
```

#### Thread-Safe Syntax Extensions

```grapa
// Each thread can have its own syntax extensions
function thread_with_custom_syntax() {
    // Local custom syntax for this thread
    $custom_function = rule thread_cmd $INT {op(n:$2){n*thread_id()}};
    
    // This syntax is isolated to this thread
    result = thread_cmd 5;
}
```

### Best Practices

1. **Start Local**: Define custom syntax locally first, then promote to global when ready
2. **Use Descriptive Names**: Choose names that clearly indicate what your syntax does
3. **Document Patterns**: Include examples of how to use your custom syntax
4. **Test Thoroughly**: Custom syntax can affect parsing, so test edge cases
5. **Consider Scope**: Use local scope for experimental features, global for stable ones
6. **Performance**: Remember that custom syntax is evaluated at parse time

### Limitations and Considerations

- **Parse-Time Execution**: Custom syntax rules are evaluated during parsing, not execution
- **No Recursive Extensions**: You can't extend the grammar to support extending itself
- **Performance Impact**: Complex custom syntax can slow down parsing
- **Debugging**: Custom syntax errors can be harder to debug than regular code errors

This syntax extension capability makes Grapa uniquely powerful for creating domain-specific languages, implementing custom DSLs, and adapting the language to specific use cases without modifying the core implementation.

## Rule Organization Patterns

For complex parsing scenarios, organizing rules in data structures provides better maintainability and reusability.

### Object-Based Rule Organization

Organize related rules in `$GOBJ` objects with a common parsing interface:

```grapa
/* Define ETL rules in a $GOBJ object */
etl_rules = {
    /* Common parsing interface */
    parse = op(parse_data, parse_rule){op()(parse_data, parse_rule)()},

    /* CSV parsing rule */
    csv_parser = rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){
        {name: name, age: age, city: city};
    }},

    /* Log parsing rules */
    catch_sq_bracket_rule = rule '[' <> ']' {@<lit,{$2}>},
    id_list_rule = rule $ID <id_list_rule> {@<prepend,{$2,$1}>} | $ID {@<createarray,{$1}>},
    log_parser = rule <catch_sq_bracket_rule> <id_list_rule> {op(timestamp:$1,message:$2){
        {timestamp: timestamp, message: message, level: "INFO"};
    }},
    
    /* Config parsing rule */
    config_parser = rule $ID '=' $ID {op(key:$1,value:$3){
        {key: key, value: value};
    }}
};

/* Execute with custom rule set */
csv_result = etl_rules.parse("John,25,NY", etl_rules.csv_parser);
log_result = etl_rules.parse("[2024-01-15 10:30:00] User login successful", etl_rules.log_parser);
config_result = etl_rules.parse("server_name=myapp", etl_rules.config_parser);
```

**Key Benefits:**
- **Organized Structure**: Rules grouped logically in objects
- **Common Interface**: Shared parsing method across all rules
- **Easy Extension**: Add new rules to the object
- **Clean Syntax**: Simple method calls for rule execution

### Class-Based Rule Organization

Use classes for more structured rule organization with methods:

```grapa
/* Define rules in a class */
etl_rules_class = class {
    /* Common parsing interface */
    parse = op(parse_data, parse_rule){op()(parse_data, parse_rule)()};

    /* CSV parsing rule */
    csv_parser = rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){
        {name: name, age: age, city: city};
    }};

    /* Log parsing rules */
    catch_sq_bracket_rule = rule '[' <> ']' {@<lit,{$2}>};
    id_list_rule = rule $ID <id_list_rule> {@<prepend,{$2,$1}>} | $ID {@<createarray,{$1}>};
    log_parser = rule <catch_sq_bracket_rule> <id_list_rule> {op(timestamp:$1,message:$2){
        {timestamp: timestamp, message: message, level: "INFO"};
    }};
    
    /* Config parsing rule */
    config_parser = rule $ID '=' $ID {op(key:$1,value:$3){
        {key: key, value: value};
    }};
};

/* Create instance and execute */
etl_rules = etl_rules_class();
csv_result = etl_rules.parse("John,25,NY", etl_rules.csv_parser);
log_result = etl_rules.parse("[2024-01-15 10:30:00] User login successful", etl_rules.log_parser);
config_result = etl_rules.parse("server_name=myapp", etl_rules.config_parser);
```

**Key Benefits:**
- **Object-Oriented**: Use classes for better organization
- **Instance Methods**: Each instance has its own rule set
- **Encapsulation**: Rules are contained within the class
- **Reusability**: Create multiple instances for different contexts

## Helper Rule Design Patterns

For complex parsing scenarios, break down parsing into modular helper rules that work together.

### Modular Rule Composition

Design helper rules that can be reused across multiple main rules:

```grapa
/* Helper rules for complex parsing */
catch_sq_bracket_rule = rule '[' <> ']' {@<lit,{$2}>};
id_list_rule = rule $ID <id_list_rule> {@<prepend,{$2,$1}>} | $ID {@<createarray,{$1}>};

/* Main rule using helper rules */
log_rule = rule <catch_sq_bracket_rule> <id_list_rule> {op(timestamp:$1,message:$2){
    return {
        timestamp: timestamp,
        message: message,
        level: "INFO"
    };
}};

/* Another rule using the same helper */
config_rule = rule $ID '=' $ID {op(key:$1,value:$3){
    return {key: key, value: value};
}};

/* Execute with helper rules */
log_result = op()("[2024-01-15 10:30:00] User login successful", log_rule)();
config_result = op()("server_name=myapp", config_rule)();
```

**Design Principles:**
- **Single Responsibility**: Each helper rule handles one parsing concern
- **Reusability**: Helper rules can be used by multiple main rules
- **Composability**: Main rules combine helper rules for complex parsing
- **Maintainability**: Changes to helper rules affect all dependent rules

### Rule Dependency Management

Structure rules to show clear dependencies and relationships:

```grapa
/* Base parsing rules */
field_rule = rule $ID {op(value:$1){value}};
separator_rule = rule ',' {op(){","}};

/* Composite rules using base rules */
csv_row_rule = rule <field_rule> (<separator_rule> <csv_row_rule> | ) {op(first:$1,rest:$3){
    if (rest) {
        return [first] + rest;
    } else {
        return [first];
    };
}};

/* Complex rule using composite rules */
structured_data_rule = rule <csv_row_rule> '|' <csv_row_rule> {op(row1:$1,row2:$3){
    return {
        primary: row1,
        secondary: row2,
        combined: row1 + row2
    };
}};
```

## Dynamic Rule Management

Add rules to existing rule collections dynamically using different patterns.

### Object Extension with `+=` Operator

Add new rules to existing `$GOBJ` objects:

```grapa
/* Start with base ETL rules object */
etl_rules = {
    parse = op(parse_data, parse_rule){op()(parse_data, parse_rule)()},
};

/* Dynamically add new rules using += operator */
etl_rules += (csv_parser : rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){{name: name, age: age, city: city};}});

/* Execute with dynamically added rule */
csv_result = etl_rules.parse("John,25,NY", etl_rules.csv_parser);
/* Result: {name: "John", age: 25, city: "NY"} */
```

**Key Features:**
- **Object Extension**: Add rules to existing `$GOBJ` objects using `+=` operator
- **Immediate Availability**: New rules are immediately available for use
- **Flexible Addition**: Add rules based on runtime conditions

### Class-Based Dynamic Rule Addition

Use class methods to add rules dynamically to `$CLASS` instances:

```grapa
/* Define base ETL rules class with add_rule method */
etl_rules_class = class {
    parse = op(parse_data, parse_rule){op()(parse_data, parse_rule)()};
    add_rule = op(pName,pRule) {$this.@pName=pRule;};
};

/* Create instance and add rules dynamically */
etl_rules = etl_rules_class();
etl_rules.add_rule("csv_parser", rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){{name: name, age: age, city: city};}});

/* Execute with dynamically added rule */
csv_result = etl_rules.parse("John,25,NY", etl_rules.csv_parser);
/* Result: {name: "John", age: 25, city: "NY"} */
```

**Key Features:**
- **Class Methods**: Use class methods to add rules dynamically to `$CLASS` instances
- **Encapsulated Management**: Rule addition is controlled by class methods
- **Instance Isolation**: Each class instance has its own rule set

### Comparison: `+=` vs `++=` vs `add_rule()`

| Method | Use Case | Target | Syntax |
|--------|----------|---------|---------|
| `++=` | Rule concatenation | Rule variables | `rule_var ++= rule ...` |
| `+=` | Object extension | `$GOBJ` objects | `obj += (key : rule ...)` |
| `add_rule()` | Class method | `$CLASS` instances | `instance.add_rule("name", rule ...)` |

## Return Value Patterns

Rules can use different patterns for returning values, each with specific use cases.

### Explicit Return Pattern

Use explicit `return` statements for clarity and control:

```grapa
/* Explicit return pattern */
csv_rule = rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){
    return {
        name: name,
        age: age,
        city: city
    };
}};
```

**When to Use:**
- **Complex Logic**: When the rule body has multiple statements
- **Conditional Returns**: When different conditions return different values
- **Debugging**: When you need to add logging or validation before returning

### Implicit Return Pattern

Use implicit return (last expression) for simple, single-expression rules:

```grapa
/* Implicit return pattern */
csv_parser = rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){
    {name: name, age: age, city: city};
}};
```

**When to Use:**
- **Simple Rules**: When the rule body is a single expression
- **Functional Style**: When following functional programming patterns
- **Conciseness**: When you want more concise rule definitions

### Mixed Return Patterns

Combine both patterns based on complexity:

```grapa
/* Simple rules use implicit return */
field_rule = rule $ID {op(value:$1){value}};

/* Complex rules use explicit return */
complex_rule = rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){
    /* Validation logic */
    if (age < 0 || age > 150) {
        return $err("Invalid age: " + age);
    };
    
    /* Return structured data */
    return {
        name: name,
        age: age,
        city: city,
        valid: true
    };
}};
```

## Rule Testing and Debugging

Systematic testing and debugging techniques for rule definitions.

### Testing Rule Definitions

Test rules systematically to ensure they work correctly:

```grapa
/* Define test rules */
test_rules = {
    parse = op(parse_data, parse_rule){op()(parse_data, parse_rule)()},
    
    csv_parser = rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){
        {name: name, age: age, city: city};
    }},
    
    log_parser = rule ('[') <> (']') $ID {op(timestamp:$1,message:$3){
        {timestamp: timestamp, message: message, level: "INFO"};
    }}
};

/* Test cases */
test_cases = [
    ("John,25,NY", test_rules.csv_parser, {name: "John", age: 25, city: "NY"}),
    ("[2024-01-15] User login", test_rules.log_parser, {timestamp: "2024-01-15", message: "User login", level: "INFO"})
];

/* Run tests */
i = 0;
while (i < test_cases.len()) {
    test_case = test_cases[i];
    input = test_case[0];
    rule = test_case[1];
    expected = test_case[2];
    
    result = test_rules.parse(input, rule);
    if (result == expected) {
        ("✓ Test " + (i + 1) + " passed").echo();
    } else {
        ("✗ Test " + (i + 1) + " failed: expected " + expected + ", got " + result).echo();
    };
    i += 1;
};
```

### Debugging Rule Execution

Use debugging techniques to understand rule behavior:

```grapa
/* Debug rule execution */
debug_rule = rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){
    ("Debug: name=" + name + ", age=" + age + ", city=" + city).echo();
    return {name: name, age: age, city: city};
}};

/* Test with debugging */
result = op()("John,25,NY", debug_rule)();
/* Output: Debug: name=John, age=25, city=NY */
```

### Rule Validation Patterns

Validate rule definitions before use:

```grapa
/* Rule validation function */
validate_rule = op(rule_name, rule_definition, test_input, expected_output) {
    try result = op()(test_input, rule_definition)();
    catch (error) return {valid: false, rule: rule_name, message: "Rule execution failed", error: error};
    
    if (result == expected_output) {
        return {valid: true, rule: rule_name, message: "Rule works correctly"};
    } else {
        return {valid: false, rule: rule_name, message: "Rule returned unexpected result", expected: expected_output, actual: result};
    };
};

/* Validate rules */
csv_rule = rule $ID ',' $INT ',' $ID {op(name:$1,age:$3,city:$5){
    {name: name, age: age, city: city};
}};

validation_result = validate_rule("csv_parser", csv_rule, "John,25,NY", {name: "John", age: 25, city: "NY"});
validation_result.echo();
```

### Best Practices for Rule Testing

1. **Test Edge Cases**: Test with empty input, malformed input, and boundary conditions
2. **Validate Output**: Ensure rules return expected data structures
3. **Test Rule Combinations**: Test how rules work together in complex scenarios
4. **Performance Testing**: Measure execution time for performance-critical rules
5. **Error Handling**: Test how rules handle invalid input gracefully
