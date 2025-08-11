---
tags:
  - maintainer
  - lowlevel
  - implementation
  - rule-system
  - bnf
  - compilation
  - execution
---

# $RULE System Architecture and Implementation

## Overview

The $RULE system is the foundational architecture of Grapa, implementing an **executable BNF (Backus-Naur Form)** that goes far beyond traditional grammar definitions. It's not just a parser generator - it's a complete programming language for defining languages, with sophisticated capabilities for ETL processing, XML/HTML parsing, and dynamic grammar mutation.

## Key Architectural Principles

### 1. **Executable BNF Beyond Traditional Limits**

Unlike traditional BNF systems that only define syntax, Grapa's $RULE system:
- **Executes arbitrary code** during parsing via action codes
- **Builds execution trees** that can be evaluated later
- **Supports dynamic grammar mutation** at runtime
- **Handles complex token types** with special behaviors
- **Manages execution context** and parameter binding

### 2. **Three-Phase Processing Model**

```
Input → Compilation → Execution Tree → Runtime Evaluation
```

1. **Compilation Phase**: Raw input is parsed against BNF rules, building execution trees
2. **Tree Construction**: Action codes create `$OP` and `$CODE` nodes with parameter binding
3. **Runtime Evaluation**: Trees are evaluated with lazy parameter resolution

## Core Components

### GrapaRuleEvent Structure

```cpp
class GrapaRuleEvent : public GrapaObjectEvent
{
public:
    GrapaRuleEvent *vValueEvent;        // Value reference
    GrapaLibraryEvent *vLibraryEvent;   // Library function
    GrapaRuleEvent *vRuleLambda;        // Action code (executable)
    GrapaRuleEvent *vRuleParent;        // Parent rule reference
    GrapaRuleEvent *vClass;             // Class definition
    bool mAbort, mVar, mLocal, mClass, mConst;
    char mQuote;
    u8 mT;                              // Token type
};
```

**Key Fields:**
- **`vRuleLambda`**: Contains the action code that executes when rule matches
- **`vRuleParent`**: Links to parent rule for hierarchical processing
- **`vLibraryEvent`**: Points to C++ library function for execution
- **`vValueEvent`**: References actual values or nested structures

### GrapaRuleQueue Management

```cpp
class GrapaRuleQueue : public GrapaObjectQueue
{
    // Rule storage and lookup
    virtual GrapaRuleEvent* Search(const GrapaCHAR& pName, s64& pIndex);
    virtual GrapaRuleEvent* Head(u64 mSkip = 0);
    virtual GrapaRuleEvent* Tail(u64 mSkip = 0);
};
```

## Compilation Phase Architecture

### 1. **Tokenization and Lexical Analysis**

**Entry Point**: `GrapaScriptExec::Plan()`

```cpp
GrapaRuleEvent* GrapaScriptExec::Plan(GrapaNames* pNameSpace, GrapaCHAR& pInput, 
                                     GrapaRuleEvent* rulexx, u64 pRuleId, GrapaCHAR pProfile)
```

**Process:**
1. **Input Processing**: Raw text converted to token stream via `GrapaItemState`
2. **Token Queue**: Tokens placed in `tokenQueue` for rule matching
3. **State Machine**: Uses START/ESCAPE/BLOCK states for different token types
4. **Rule Matching**: Tokens matched against BNF grammar rules

### 2. **Rule Matching Engine**

**Core Method**: `GrapaScriptExec::PlanRule()`

```cpp
GrapaRuleEvent* GrapaScriptExec::PlanRule(GrapaNames* pNameSpace, GrapaRuleEvent* pTokenEvent,
                                         GrapaRuleEvent* pRuleEvent, GrapaRuleQueue* pOperationQueue,
                                         bool& pMatched, GrapaRuleEvent* pLastNext, bool& pAddLastNext,
                                         s8 pLeftRecursion, GrapaKeyValue& pCache, bool pTouched)
```

**Key Features:**
- **Right Recursion**: Default parsing uses right recursion
- **Left Recursion Detection**: Special handling for left recursive rules
- **Operation Building**: Creates execution trees with `$OP` and `$CODE` nodes
- **Caching**: Performance optimization through rule result caching

### 3. **Left Recursion Solution**

Grapa implements a sophisticated two-phase approach inspired by Python's solution:

```cpp
// Phase 1: Parse non-left-recursive rules
if (pLeftRecursion == 0) {
    evalEvent = PlanRule(pNameSpace, nextEvent, rulexx, &operationQueue, 
                        matched, lastNext, pushExtraToken, 1, pCache, true);
    
    // Phase 2: Recursively build left-recursive structure
    while (true) {
        GrapaRuleEvent* oldEvalEvent = evalEvent;
        evalEvent = PlanRule(pNameSpace, evalEvent, rulexx, &operationQueue,
                           matched, lastNext, pushExtraToken, 2, pCache, true);
        if (evalEvent == oldEvalEvent && !matched) {
            evalEvent = oldEvalEvent;
            break;
        }
    }
}
```

## Execution Tree Architecture

### Tree Structure

All Grapa code compiles to execution trees with the format:

```grapa
@<[op,body],params>
```

**Components:**
- **`op`**: Operation type identifier (add, sub, mul, etc.)
- **`body`**: Execution tree (can be nested operations)
- **`params`**: Parameter namespace with default values

### Tree Node Types

#### 1. **$OP Nodes** (Operator Nodes)
```cpp
// Example: Addition operation
@<add,{left,right}>
```

**Structure:**
- **Library Name**: Points to C++ library function
- **Parameters**: Queue of child operations
- **Execution**: Calls library function with resolved parameters

#### 2. **$CODE Nodes** (Code Sequence Nodes)
```cpp
// Example: Multiple operations
@<code,{op1,op2,op3}>
```

**Structure:**
- **Child Queue**: Sequence of operations to execute
- **Execution**: Processes each child in order
- **Result**: Returns result of last child

#### 3. **$RULE Nodes** (Rule Reference Nodes)
```cpp
// Example: Rule reference with parameters
@<rule,{ruleName,parameters}>
```

**Structure:**
- **Rule Name**: Reference to BNF rule
- **Parameters**: Arguments for rule execution
- **Execution**: Evaluates rule with given parameters

### Parameter Resolution System

**Key Insight**: What's passed between rules are **execution trees**, not actual values.

```cpp
// C++ libraries check for unresolved trees
if (pInput->mValue.mToken == GrapaTokenType::OP || 
    pInput->mValue.mToken == GrapaTokenType::CODE) {
    // Resolve the tree first
    result = vScriptExec->ProcessPlan(pNameSpace, pInput);
}
```

**Resolution Process:**
1. **Tree Detection**: Check if input is `$OP` or `$CODE`
2. **Recursive Resolution**: Process tree to get actual value
3. **Value Usage**: Use resolved value in operation

## Special Token Types and Capabilities

### 1. **Comment Handling**

**Example from `$grapa.grc`:**
```grapa
@global["$starcomment"]
    = rule ('/' '*') <> ('*' '/')
    ;
```

**Special Features:**
- **`<>`**: Special token that skips content until pattern match
- **Nested Comments**: Handles `/* /* nested */ */` correctly
- **Action Integration**: Comments can contain executable code

### 2. **XML/HTML Processing**

**Complex Example:**
```grapa
@global["$tagScriptValueY"]
    = rule '>' <@<join,{$1}>> ('<' '/' script '>') {@<name,{null,$2}>}
    ;
```

**Special Capabilities:**
- **Content Skipping**: `<@<join,{$1}>>` captures content between tags
- **Nested Language Support**: Handles JavaScript within HTML
- **Dynamic Tag Matching**: Supports any tag name
- **Attribute Processing**: Complex attribute parsing with special characters

### 3. **ETL-Specific Tokens**

**Special Token Types:**
- **`$SKIP`**: Skip tokens during processing
- **`$ESC`**: Escape sequence handling
- **`$RUN`**: Execute action immediately
- **`$START`/`$END`**: Delimit processing blocks
- **`$EMPTY`**: Handle empty content gracefully

### 4. **Rule Token (`< >`) - Advanced Reference System**

The rule token `< >` is a sophisticated reference mechanism with multiple capabilities:

#### **Basic Rule References**
```grapa
/* Reference another rule */
@global["$expression"] = rule <$term> '+' <$expression> {@<add,{$1,$3}>}
```

#### **Variable References with Complex Lookups**
```grapa
/* Simple variable reference */
@global["$lookup"] = rule @variable {@<var,{$1}>}

/* Array/List indexing */
@global["$array_lookup"] = rule @tb["d"] {@<var,{$1}>}
@global["$list_lookup"] = rule @tb.d {@<var,{$1}>}
@global["$index_lookup"] = rule @tb[8] {@<var,{$1}>}

/* Database/File object references */
@global["$db_lookup"] = rule @$file().table("ROW") {@<var,{$1}>}
```

#### **Optional Post-Processing $OP**
```grapa
/* Rule with post-processing operation */
@global["$processed_rule"] = rule <$data,op(a:$1){a.len()}> {@<var,{$1}>}
```

**Key Features:**
- **Execution Tree Return**: Always returns an execution tree that must be evaluated before use
- **Namespace Resolution**: Follows Grapa's namespace hierarchy (local → function → global)
- **Dynamic Lookup**: Can reference variables, arrays, lists, objects, and database objects
- **Compile-Time Evaluation**: Variable resolution happens during compilation phase
- **Post-Processing**: Optional `$OP` can be added for compile-time data transformation
- **Token Lookback**: Post-processing `$OP` can access previous tokens via `$1`, `$2`, etc.

#### **Post-Processing $OP Execution Timing**

**Compile-Time Execution:**
```grapa
/* This $OP runs during compilation/planning phase */
@global["$filtered_data"] = rule <$raw_data,op(a:$1){a.grep("pattern")}> {@<var,{$1}>}
```

**Key Characteristics:**
- **Planning Phase**: Executes during `PlanRule()` method
- **Every Evaluation**: Runs each time the rule is evaluated (not cached)
- **Performance Impact**: Heavy operations should be avoided
- **Data Transformation**: Can filter, transform, or validate data before rule matching
- **Error Handling**: Can return `$ERR` to cause rule failure

#### **Advanced Variable Lookup Capabilities**

**Supported Lookup Patterns:**
```grapa
/* Direct variable access */
@variable

/* Array indexing */
@array[index]
@array["key"]

/* Object property access */
@object.property
@object["property"]

/* Database object access */
@$file().table("ROW")
@$file().get("filename")

/* Nested lookups */
@config.database.host
@data[0].name
```

**Implementation Details:**
- **SearchVariable Algorithm**: Uses `GrapaScriptState::SearchVariable()` for resolution
- **Namespace Chain**: Traverses up namespace hierarchy until variable found
- **Type Support**: Handles all Grapa data types (arrays, lists, objects, files, tables)
- **Error Handling**: Returns `$ERR` if variable not found

#### **Predefined Function Names in Rule Tokens**

**Current Implementation:**
```grapa
/* Using op() wrapper (current approach) */
@global["$filtered_data"] = rule <$raw_data,op(a:$1){a.grep("pattern")}> {@<var,{$1}>}
```

**Working Solution - Wrapper Function Pattern:**
```grapa
/* Define function using op(){} syntax */
my_func = op(p){p.len()};

/* Use wrapper function in rule token */
@global["$processed_data"] = rule <$raw_data,op(b:$1){my_func(b)}> {@<var,{$1}>}
```

**Key Benefits:**
- **Explicit Parameter Passing**: `op(b:$1){my_func(b)}` clearly shows parameter flow
- **Works with Any Function**: Can wrap any function definition
- **No Grammar Changes**: Uses existing `$op` syntax
- **ETL-Friendly**: Perfect for data transformation pipelines
- **Reusable**: Functions can be defined once and used in multiple rules

**ETL Processing Example:**
```grapa
/* Define ETL processing functions */
validate_data = op(p){p.grep("valid")};
transform_data = op(p){p.upper()};
filter_data = op(p){p.len() > 10 ? p : null};

/* Use in ETL pipeline rules */
@global["$etl_pipeline"] = rule 
    <$raw_data,op(b:$1){validate_data(b)}>
    <$validated,op(b:$1){transform_data(b)}>
    <$transformed,op(b:$1){filter_data(b)}>
    {@<var,{$1}>}
```

**Potential Implementation (Future Enhancement):**
```grapa
/* Define predefined function using @<op,{parameters}> syntax */
filter_function = @<grep,{@<this>,@<lit,{"pattern"}>}>;

/* Use predefined function name in rule token */
@global["$filtered_data"] = rule <$raw_data,filter_function> {@<var,{$1}>}
```

**Alternative Syntax (Requires Investigation):**
```grapa
/* Define predefined function */
filter_function = @<grep,{@<this>,@<lit,{"pattern"}>}>;

/* Use predefined function in rule token with @ reference */
@global["$filtered_data"] = rule <$raw_data,@filter_function> {@<var,{$1}>}
```

**Design Intent:**
- **BNF Flow Control**: Variable references in `< >` tokens act as "BNF flags" to control grammar execution flow
- **Generic Implementation**: Designed to be as generic as possible for various use cases
- **Function Reuse**: Predefined functions can be defined once and reused across multiple rules
- **Cleaner Syntax**: Avoids embedding raw code in grammar rules
- **Maintainability**: Centralizes processing logic in named functions

## Runtime Execution Architecture

### ProcessPlan Method

**Entry Point**: `GrapaScriptExec::ProcessPlan()`

```cpp
GrapaRuleEvent* GrapaScriptExec::ProcessPlan(GrapaNames* pNameSpace, 
                                            GrapaRuleEvent* pOperation, 
                                            GrapaRuleEvent* pParam, u64 pCount)
```

### Execution Flow

#### 1. **$CODE Node Execution**
```cpp
if (pOperation->mValue.mToken == GrapaTokenType::CODE) {
    GrapaRuleEvent *item = (GrapaRuleEvent*)pOperation->vQueue->Head();
    while (item) {
        result = ProcessPlan(pNameSpace, item, pParam, pCount);
        item = item->Next();
    }
}
```

**Process:**
- Execute each child operation in sequence
- Pass results between children
- Return result of last child

#### 2. **$OP Node Execution**
```cpp
else if (pOperation->mValue.mToken == GrapaTokenType::OP) {
    GrapaRuleEvent *libName = &gSystem->mLib;
    GrapaRuleEvent *libParam = (GrapaRuleEvent*)pOperation->vQueue->Head();
    
    LoadLib(libName);
    if (libName->vLibraryEvent) {
        result = libName->vLibraryEvent->Run(this, pNameSpace, libParam, input);
    }
}
```

**Process:**
- Extract library name and parameters
- Load appropriate library
- Execute library function with parameters
- Return function result

#### 3. **$RULE Node Execution**
```cpp
// Rule execution happens through GrapaLibraryRuleRuleEvent::Run()
GrapaRuleEvent* GrapaLibraryRuleRuleEvent::Run(GrapaScriptExec *vScriptExec,
                                               GrapaNames* pNameSpace, 
                                               GrapaRuleEvent *pOperation, 
                                               GrapaRuleQueue* pInput)
```

**Process:**
- Parse rule name and parameters
- Look up rule in global rule queue
- Execute rule with given parameters
- Return rule result

## Advanced Features

### 1. **Dynamic Grammar Mutation**

**Runtime Rule Modification:**
```grapa
// Modify existing rule at runtime
@global["$custom_rule"] = rule $INT '+' $INT {$1 + $3} | $INT '*' $INT {$1 * $3} | $INT {$1};
```

**Scoped Changes:**
```grapa
// Local rule modification within function
function customLanguage() {
    @local["$custom_rule"] = rule $STR '+' $STR {$1 + $3};
    "hello" + "world"  // Uses local rule
}
```

### 2. **Alternative Entry Points**

**Custom Grammar Compilation:**
```grapa
// Define custom grammar without modifying main language
@global["$custom_start"] = rule <$custom_expression> $SYSID("BE") | <$custom_expression>;
@global["$custom_expression"] = rule $INT '+' $INT {$1 + $3} | $INT {$1};

// Compile using custom grammar
customCode = "5 + 3";
result = $sys().compile(customCode, "$custom_start");
```

### 3. **Domain-Specific Language Creation**

**ETL Language Example:**
```grapa
// Create ETL processing language
@global["$etl_pipeline"] = rule <$etl_source> <$etl_transform> <$etl_sink>;
@global["$etl_source"] = rule 'FROM' $STR {op(source:$2){load_source($2)}};
@global["$etl_transform"] = rule 'TRANSFORM' $STR {op(transform:$2){apply_transform($2)}};
@global["$etl_sink"] = rule 'TO' $STR {op(sink:$2){save_to($2)}};
```

## Performance Optimizations

### 1. **Rule Caching**

**Cache Implementation:**
```cpp
GrapaKeyValue& pCache  // Rule result caching
```

**Benefits:**
- Avoids re-parsing identical inputs
- Significant performance improvement for repeated patterns
- Memory-efficient storage of parse results

### 2. **Bottom-Up Optimization**

**Optimization Order:**
```cpp
// Child parameters optimized first
if (pParam && pParam->vLibraryEvent) 
    pOperation = pParam->vLibraryEvent->Optimize(vScriptExec, pNameSpace, pOperation, pParam->Next());
```

**Process:**
1. Optimize child parameters
2. Optimize parent operation
3. Results flow back up call chain

### 3. **Lazy Evaluation**

**Tree Resolution:**
- Execution trees built during compilation
- Actual evaluation deferred until runtime
- Parameters resolved only when needed
- Memory efficient for complex operations

## Integration with C++ Libraries

### Library Function Interface

**Standard Interface:**
```cpp
virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, 
                           GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
```

**Parameter Access:**
```cpp
// Access parameters safely
GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
```

### Error Handling

**Error Propagation:**
```cpp
virtual GrapaRuleEvent* Error(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaError err);
```

**Debug Support:**
```cpp
// Debug output for execution tracking
if (vScriptState->mDebug.ShouldDebug("executor", 1)) {
    char debugMsg[256];
    snprintf(debugMsg, sizeof(debugMsg), "EXEC: Starting execution - operation type=%d", 
             pOperation ? pOperation->mValue.mToken : -1);
    vScriptState->mDebug.DebugPrint(this, pNameSpace, "executor", debugMsg, 1);
}
```

## Use Cases and Applications

### 1. **Language Extensibility**

**Syntax Extensions:**
```grapa
// Add new language constructs at runtime
@global["$custom_loop"] = rule 'repeat' $INT 'times' '{' <$command_list> '}' {
    op(count:$2, body:$5) {
        for(i=0; i<count; i++) { body() }
    }
};
```

### 2. **Custom Command and Function Integration**

**Critical Distinction:**
- **`custom_command`**: For actions that perform operations (no return value)
- **`custom_function`**: For expressions that return values

**Integration Points:**
```grapa
@global["$command"]
    = rule '{' <$command_list> ';' '}' {@<scope,{$2}>}
    // ... other commands ...
    | <custom_command>  /* ← Actions (loops, commands, statements) */
    // ... other commands ...
    ;

@global["$function"]
    = rule '[' ']' {@<createarray,{}>}
    // ... other functions/expressions ...
    | <custom_function>  /* ← Expressions (operators, functions) */
    // ... other functions/expressions ...
    ;
```

**Execution Pattern:**
```grapa
/* Define custom syntax */
custom_command = rule select $STR from $STR {op(fields:$2,table:$4){
    /* Implementation */
}};

custom_function = rule count '(' $STR ')' from $STR {op(field:$3,table:$6){
    /* Implementation */
    return result;
}};

/* Execute using op(parse)() */
op(parse)("select * from users")();           /* Action */
result = op(parse)("count(*) from users")();  /* Expression */
```

**Key Implementation Pattern:**
The `op(parse)()` pattern is crucial for executing custom syntax:
1. `op()` creates an `$OP` object
2. `op()(script)` compiles the script string into an executable operation
3. `op()(script)()` executes the compiled operation

This pattern enables dynamic code execution and custom syntax injection.

### 2. **Data Processing Pipelines**

**ETL Processing:**
```grapa
// Define data transformation rules
@global["$csv_parser"] = rule <$csv_row> ('\n' <$csv_parser> | '\n' | ) {
    op(row:$1, rest:$3) { [row] + (rest ? rest : []) }
};
```

### 3. **Configuration Languages**

**Dynamic Configuration:**
```grapa
// Create configuration parser
@global["$config_entry"] = rule $ID '=' <$config_value> ';' {
    op(key:$1, value:$3) { set_config(key, value) }
};
```

### 4. **Protocol Parsing**

**Network Protocol Support:**
```grapa
// Define protocol grammar
@global["$http_request"] = rule <$http_method> ' ' <$http_path> ' ' <$http_version> '\r\n' <$http_headers> {
    op(method:$1, path:$3, version:$5, headers:$7) { create_request(method, path, version, headers) }
};
```

## Conclusion

The $RULE system represents a fundamental innovation in language design and implementation. It provides:

1. **Unprecedented Flexibility**: Dynamic grammar mutation and runtime language evolution
2. **Powerful ETL Capabilities**: Built-in support for complex data processing
3. **Performance Optimization**: Sophisticated caching and lazy evaluation
4. **Extensibility**: Easy addition of new language constructs and capabilities
5. **Integration**: Seamless connection between high-level grammar and low-level C++ libraries

This architecture makes Grapa uniquely suited for:
- **Language research and experimentation**
- **Complex data processing tasks**
- **Dynamic system configuration**
- **Protocol implementation**
- **Educational language design**

The system's complexity is justified by its power and flexibility, enabling use cases that would be impossible with traditional parsing approaches. 