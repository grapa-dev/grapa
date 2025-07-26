# Grapa Runtime Grammar Mutation Implementation

## Overview

Grapa's **runtime grammar mutation** is the core research innovation that enables scoped language evolution through mutable `$RULE` structures. The system is **bootstrapped** with the ability to compile and run the base grammar language, and when the program starts, it loads global variables that define the high-level language with an entry point `$RULE`.

### Key Architectural Insights

#### Bootstrapping Process
1. **Base Grammar Loading**: Grapa starts with a minimal grammar compiler
2. **Global Variable Initialization**: High-level language rules are loaded as system global variables (prefixed with `$`)
3. **Entry Point**: The main language syntax is defined through the `$start` rule
4. **Runtime Mutability**: All grammar rules are stored in mutable global variables

#### Compilation vs Runtime Mutation
- **Compiled Code**: Once code is compiled to Grapa's execution tree, syntax cannot be mutated
- **Runtime Compilation**: New code can be compiled using modified grammar rules at any time
- **Scoped Mutation**: Grammar changes can apply globally or within specific function contexts
- **Alternative Entry Points**: Compilation can use custom starting rules without altering the main language syntax

## Core Architecture

### 1. Rule Storage and Management

#### Global Rule Queue (`mRuleStartQueue`)
```cpp
// source/grapa/GrapaState.h:351
class GrapaScriptState : public GrapaScriptExecState
{
public:
    GrapaRuleQueue mRuleStartQueue;  // Global rule storage
    GrapaNames* vRuleVariables;      // Namespace management
    // ...
};
```

**Key Implementation:**
- **Centralized Storage**: All grammar rules are stored in `mRuleStartQueue`
- **Hash-based Lookup**: Rules are indexed by name for O(1) access
- **Mutable Structure**: Rules can be added, modified, or removed at runtime
- **System Global Variables**: Rules with `$` prefix are system-level globals
- **Bootstrap Loading**: Initial grammar loaded from `lib/grapa/$grapa.grc`

#### Rule Creation and Registration
```cpp
// source/grapa/GrapaState.cpp:2547-2560
GrapaRuleEvent* GrapaScriptState::AddRule(GrapaRuleQueue* pRuleQueue, const char* pName)
{
    if (pRuleQueue == NULL) return(NULL);
    s64 idx;
    GrapaRuleEvent* rule = pRuleQueue->Search(pName,idx);
    if (rule == NULL)
    {
        rule = new GrapaRuleEvent();
        rule->vQueue = new GrapaRuleQueue();
        rule->SetItem(pRuleQueue->HashId(pName, 0), GrapaCHAR(pName), GrapaBYTE());
        rule->mValue.mToken = GrapaTokenType::RULE;
        if (rule) pRuleQueue->PushTail(rule);
    }
    return(rule);
}
```

**Features:**
- **Automatic Creation**: Rules are created on-demand if they don't exist
- **Hash-based Naming**: Unique identification using `HashId(pName, 0)`
- **Queue Management**: Rules are stored in linked list for iteration

### 2. Rule Structure and Components

#### Rule Event Structure
```cpp
// source/grapa/GrapaState.h:87-108
class GrapaRuleEvent : public GrapaObjectEvent
{
public:
    GrapaRuleEvent *vValueEvent;     // Value reference
    GrapaLibraryEvent *vLibraryEvent; // Library function
    GrapaRuleEvent *vRuleLambda;     // Action code
    GrapaRuleEvent *vRuleParent;     // Parent rule reference
    GrapaRuleEvent *vClass;          // Class definition
    bool mAbort, mVar, mLocal, mClass, mConst;
    char mQuote;
    u8 mT;
    // ...
};
```

**Key Components:**
- **`vQueue`**: Contains rule alternatives (OR operations)
- **`vRuleLambda`**: Action code executed when rule matches
- **`vRuleParent`**: Enables rule inheritance and scoping
- **`mLocal`**: Indicates local vs global scope

#### Rule Operations and Parameters
```cpp
// source/grapa/GrapaState.cpp:2572-2646
GrapaRuleEvent* GrapaScriptState::AddRuleOperation(GrapaRuleQueue* pRuleEvent, 
    const char* pName, const char* pValue, s64 a, s64 b, s64 c, s64 d)
{
    // Creates rule operation with action code
    GrapaRuleEvent* operation = new GrapaRuleEvent(GrapaTokenType::RULEOP, 
        (pName&&*pName) ? pRuleEvent->HashId(pName, 0) : 0, pName, pValue);
    
    // Sets up action code lambda
    operation->vRuleLambda = new GrapaRuleEvent(GrapaTokenType::OP, 0, "", "");
    // ...
}
```

### 3. Scope Management and Mutation

#### Namespace Hierarchy
```cpp
// source/grapa/GrapaState.h:351-395
class GrapaScriptState : public GrapaScriptExecState
{
public:
    GrapaNames* vRuleVariables;      // Current namespace
    // ...
    virtual void SetNameSpace(GrapaNames* pRuleVariables);
    virtual GrapaNames* GetNameSpace() { return vRuleVariables; };
};
```

**Scope Levels:**
1. **Global Scope**: `mRuleStartQueue` - affects entire system
2. **Namespace Scope**: `vRuleVariables` - affects current execution context
3. **Local Scope**: Function-local rule modifications
4. **Block Scope**: Temporary rule changes within code blocks

#### Compilation Context Management
```cpp
// source/grapa/GrapaState.cpp:6144-6170
GrapaRuleEvent *GrapaScriptExec::Plan(GrapaNames* pNameSpace, GrapaCHAR& pInput, 
    GrapaRuleEvent* rulexx, u64 pRuleId, GrapaCHAR pProfile)
{
    // Default to $start rule if no custom rule provided
    if (rulexx == NULL)
    {
        rulexx = vScriptState->SearchVariable(pNameSpace, GrapaCHAR("custom_start"));
        if (rulexx == NULL) rulexx = vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$start"));
        if (rulexx == NULL)
        {
            if (pRuleId == 0) pRuleId = vScriptState->mRuleStartQueue.SearchId("custom_start");
            if (pRuleId == 0) pRuleId = vScriptState->mRuleStartQueue.SearchId("$start");
            rulexx = vScriptState->mRuleStartQueue.Search(pRuleId);
        }
    }
    // ... compilation proceeds with selected rule
}
```

**Key Features:**
- **Custom Entry Points**: Can specify alternative starting rules for compilation
- **Context Preservation**: Grammar mutations don't affect already-compiled code
- **Scoped Compilation**: New code can be compiled with modified grammar rules

#### Rule Mutation Process
```cpp
// source/grapa/GrapaLibRule.cpp:3809-3950
GrapaRuleEvent* GrapaLibraryRuleRuleEvent::Run(GrapaScriptExec *vScriptExec, 
    GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    // 1. Extract rule name
    GrapaCHAR name;
    if (GrapaRuleEvent* r1 = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(0)))
    {
        if (r1->mValue.mToken == GrapaTokenType::SYSID) name.Append("$");
        name.Append(r1->mValue);
    }
    
    // 2. Create or modify rule
    if (name.mLength)
    {
        rule = vScriptExec->vScriptState->AddRule(&vScriptExec->vScriptState->mRuleStartQueue, 
            (char*)name.mBytes);
        ruleDelCount = rule->vQueue->mCount;  // Track existing operations
    }
    
    // 3. Add new rule operations
    while (option)
    {
        operation = vScriptExec->vScriptState->AddRuleOperation(
            (GrapaRuleQueue*)rule->vQueue, (char*)name.mBytes, "");
        // Add parameters and action code
        // ...
    }
    
    // 4. Clean up old operations if replacing
    if (rule && ruleDelCount)
    {
        while (ruleDelCount--)
        {
            operation = (GrapaRuleEvent*)rule->vQueue->PopHead();
            if (operation) { operation->CLEAR(); delete operation; }
        }
    }
}
```

### 4. Execution Tree Building

#### Rule Matching and Execution
```cpp
// source/grapa/GrapaState.cpp:3270-4104
GrapaRuleEvent* GrapaScriptExec::PlanRule(GrapaNames* pNameSpace, GrapaRuleEvent* pTokenEvent, 
    GrapaRuleEvent* pRuleEvent, GrapaRuleQueue* pOperationQueue, bool& pMatched, 
    GrapaRuleEvent* pLastNext, bool& pAddLastNext, s8 pLeftRecursion, 
    GrapaKeyValue& pCache, bool pTouched)
{
    // 1. Find matching rule operation
    operation = (GrapaRuleEvent*)pRuleEvent->vQueue->Head();
    while (operation)
    {
        // Check left recursion constraints
        if (pLeftRecursion)
        {
            parameter = (GrapaRuleEvent*)operation->vQueue->Head();
            if (pLeftRecursion == 1 && parameter->mName.StrCmp(pRuleEvent->mName) != 0) break;
            if (pLeftRecursion == 2 && parameter->mName.StrCmp(pRuleEvent->mName) == 0) break;
        }
        operation = operation->Next();
    }
    
    // 2. Build execution tree
    operationEvent = new GrapaRuleEvent(operation->mValue.mToken, operation->mId, 
        (char*)operation->mName.mBytes, (char*)operation->mValue.mBytes);
    parameterQueue = new GrapaRuleQueue();
    operationEvent->vQueue = parameterQueue;
    
    // 3. Process parameters and build tree
    parameter = (GrapaRuleEvent*)operation->vQueue->Head();
    nextEvent = pTokenEvent;
    while (parameter)
    {
        // Handle different parameter types (RULEREF, RULE, TOKEN, etc.)
        // Build execution tree recursively
        // ...
    }
}
```

### 5. Left Recursion Handling

#### Two-Phase Parsing Algorithm
```cpp
// source/grapa/GrapaState.cpp:3270-4104 (continued)
if (pLeftRecursion == 0)
{
    // Phase 1: Try non-left-recursive rules first
    evalEvent = PlanRule(pNameSpace, nextEvent, rulexx, &operationQueue, 
        matched, lastNext, pushExtraToken, 1, pCache, true);
    
    // Phase 2: Build left-recursive structure recursively
    while (true)
    {
        GrapaRuleEvent* oldEvalEvent = evalEvent;
        evalEvent = PlanRule(pNameSpace, evalEvent, rulexx, &operationQueue, 
            matched, lastNext, pushExtraToken, 2, pCache, true);
        if (evalEvent == oldEvalEvent && !matched)
        {
            evalEvent = oldEvalEvent;
            break;
        }
    }
}
```

**Innovation:**
- **Automatic Detection**: Left recursion is detected by comparing rule names
- **Two-Phase Resolution**: Non-recursive rules first, then recursive building
- **Infinite Loop Prevention**: Termination condition prevents infinite recursion

## Performance Characteristics

### Time Complexity
- **Rule Lookup**: O(1) average case (hash-based)
- **Rule Matching**: O(n) where n = number of rule alternatives
- **Tree Building**: O(m) where m = number of tokens in input
- **Left Recursion**: O(k) where k = recursion depth

### Space Complexity
- **Rule Storage**: O(r) where r = number of rules
- **Execution Tree**: O(t) where t = number of tokens
- **Scope Stack**: O(s) where s = scope nesting depth

### Memory Management
- **Automatic Cleanup**: Rule operations are cleaned up when replaced
- **Reference Counting**: Shared rule references prevent memory leaks
- **Scope Isolation**: Local rule changes don't affect global state

## Academic Research Contributions

### 1. Runtime Grammar Evolution
**Innovation**: Grammar rules can be modified during program execution
- **Bootstrap Architecture**: System starts with minimal grammar, loads full language as mutable globals
- **Global Evolution**: System-wide syntax changes through `$` prefixed global variables
- **Scoped Evolution**: Function or block-local syntax modifications that don't affect compiled code
- **Incremental Evolution**: Rules can be extended or replaced without restarting the system
- **Context Preservation**: Already-compiled code remains unaffected by grammar mutations

### 2. Scope-Aware Mutation
**Innovation**: Rule changes respect lexical scoping
- **Namespace Isolation**: Changes don't leak across scopes
- **Temporary Modifications**: Block-scoped rule changes
- **Inheritance**: Local rules can extend global rules

### 3. Left Recursion Resolution
**Innovation**: Automatic handling of left-recursive grammars
- **Detection**: Automatic identification of left-recursive rules
- **Resolution**: Two-phase parsing algorithm
- **Efficiency**: Linear time complexity for left recursion

### 4. Execution Tree Semantics
**Innovation**: Dynamic execution tree building from mutable rules
- **Lazy Evaluation**: Trees built on-demand during parsing
- **Action Code Integration**: Arbitrary code execution during tree building
- **Parameter Binding**: Dynamic parameter resolution
- **Compilation Context**: Trees built using current grammar state, preserving compilation context
- **Bootstrap Compatibility**: Execution trees work with both base grammar and evolved language constructs

## Implementation Examples

### Basic Rule Definition
```grapa
// Define a simple rule
@global["$custom_rule"] = rule $INT '+' $INT {$1 + $3} | $INT {$1};
```

### Rule Mutation
```grapa
// Modify existing rule
@global["$custom_rule"] = rule $INT '+' $INT {$1 + $3} | $INT '*' $INT {$1 * $3} | $INT {$1};
```

### Scoped Rule Changes
```grapa
// Local rule modification within function context
function customLanguage() {
    @local["$custom_rule"] = rule $STR '+' $STR {$1 + $3};  // String concatenation
    "hello" + "world"  // Uses local rule for this compilation
}
// Outside function, original rule applies to new compilations
```

### Alternative Entry Point Compilation
```grapa
// Define custom grammar without modifying main language
@global["$custom_start"] = rule <$custom_expression> $SYSID("BE") | <$custom_expression>;
@global["$custom_expression"] = rule $INT '+' $INT {$1 + $3} | $INT {$1};

// Compile code using custom grammar
customCode = "5 + 3";
result = $sys().compile(customCode, "$custom_start");  // Use custom entry point
```

### DSL Creation
```grapa
// Create a simple calculator DSL
@global["$calc_expr"] = rule <$calc_term> '+' <$calc_expr> {$1 + $3} | <$calc_term>;
@global["$calc_term"] = rule <$calc_factor> '*' <$calc_term> {$1 * $3} | <$calc_factor>;
@global["$calc_factor"] = rule '(' <$calc_expr> ')' {$2} | $INT {$1};
```

## Research Applications

### 1. Language Extensibility
- **Syntax Extensions**: Add new language constructs at runtime
- **Operator Overloading**: Define custom operators for specific domains
- **Macro Systems**: Implement powerful macro facilities

### 2. Domain-Specific Languages
- **ETL Languages**: Create data transformation languages
- **Configuration Languages**: Build flexible configuration systems
- **Query Languages**: Implement custom query syntax

### 3. Meta-Programming
- **Code Generation**: Generate code using grammar rules
- **Reflection**: Introspect and modify language behavior
- **Compile-time Computation**: Perform computations during parsing

### 4. Educational Applications
- **Language Learning**: Experiment with different syntax designs
- **Compiler Education**: Teach parsing and compilation concepts
- **Research Prototyping**: Rapidly prototype language features

## Future Research Directions

### 1. Formal Semantics
- **Type Safety**: Ensure type safety during grammar evolution
- **Semantic Preservation**: Guarantee semantic correctness of mutations
- **Concurrency Safety**: Handle concurrent grammar modifications

### 2. Performance Optimization
- **Caching**: Cache frequently used rule combinations
- **JIT Compilation**: Compile hot grammar paths to native code
- **Parallel Parsing**: Parallelize rule matching and tree building

### 3. Advanced Features
- **Grammar Versioning**: Track and manage grammar evolution history
- **Conflict Resolution**: Handle conflicting rule modifications
- **Grammar Composition**: Combine multiple grammar fragments

## Conclusion

Grapa's runtime grammar mutation system represents a significant advance in programming language design, enabling unprecedented flexibility in language evolution and meta-programming. The combination of mutable rules, scope-aware modifications, and automatic left recursion handling provides a powerful foundation for language experimentation and DSL creation.

The system's academic contributions lie in its novel approach to grammar evolution, providing a practical implementation of theoretical concepts in language design and compilation theory. This makes Grapa an excellent platform for research in programming languages, compilers, and language engineering. 