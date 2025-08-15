# List Comprehension Implementation

## Overview

List comprehension is a native C++ feature in Grapa that allows creating arrays from expressions evaluated over iterables, with optional conditional filtering. The implementation follows established Grapa patterns for temporary namespace management and variable scoping.

## Grammar Integration

### BNF Rules Added to `$grapa.grc`

```grapa
@global["$array"]
    = rule ',' ',' <$array> {@<prepend,{$3,null,null}>}
    | ',' <$array> {@<prepend,{$2,null}>}
    | <$comp> ',' <$array> {@<prepend,{$3,$1}>}
    | <$comp> ',' {@<createarray,{$1,null}>}
    | <$comp> {@<createarray,{$1}>}
    | ',' {@<createarray,{null,null}>}
    | '[' <$comp> for $ID in <$comp> if <$comp> ']' {@<arraycomp,{$2,$4,$6,$8}>}
    | '[' <$comp> for $ID in <$comp> ']' {@<arraycomp,{$2,$4,$6}>}
    ;
```

### Event Registration

The `arraycomp` event is registered in the grammar and maps to `GrapaLibraryRuleArrayCompEvent`.

## Core Implementation

### Event Class Structure

```cpp
class GrapaLibraryRuleArrayCompEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleArrayCompEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
private:
    GrapaRuleEvent* HandleListComprehension(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleQueue* pInput, bool hasCondition);
};
```

### Main Dispatch Logic

```cpp
GrapaRuleEvent* GrapaLibraryRuleArrayCompEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
    // List comprehension handler based on parameter count
    switch (pInput->mCount)
    {
        case 3:
            // [expression for variable in iterable]
            // Parameters: [0]=expression, [1]=variable, [2]=iterable
            return HandleListComprehension(vScriptExec, pNameSpace, pInput, false);

        case 4:
            // [expression for variable in iterable if condition]
            // Parameters: [0]=expression, [1]=variable, [2]=iterable, [3]=condition
            return HandleListComprehension(vScriptExec, pNameSpace, pInput, true);

        default:
            return Error(vScriptExec, pNameSpace, -1);
    }
}
```

## Key Implementation Patterns

### 1. Temporary Namespace Management

**Pattern**: Follow `EvalEvent`/`OpEvent` pattern for proper scoping

```cpp
// Create temporary namespace for list comprehension
GrapaRuleEvent* operation = vScriptExec->vScriptState->AddRuleOperation(pNameSpace->GetNameQueue(), "", "");
GrapaRuleEvent* vLocals = new GrapaRuleEvent();
vLocals->mValue.mToken = GrapaTokenType::LIST;
vLocals->vQueue = new GrapaRuleQueue();
pNameSpace->GetNameQueue()->PushTail(vLocals);

// ... loop execution ...

// Clean up temporary namespace
pNameSpace->GetNameQueue()->PopEvent(vLocals);
if (vLocals) {
    vLocals->CLEAR();
    delete vLocals;
}

if (pNameSpace->GetNameQueue()->PopEvent(operation)) {
    operation->CLEAR();
    delete operation;
    operation = NULL;
}
```

### 2. Variable Management (Key Insight)

**Pattern**: Create variable once, update value in place (NOT PTR pattern)

```cpp
// Create variable once at the beginning
GrapaRuleEvent* actualVar = new GrapaRuleEvent(0, varName, GrapaCHAR());
vLocals->vQueue->PushTail(actualVar);

// Update value in each iteration (NOT create new variables)
while (current < rangeInt) {
    actualVar->mValue.FROM(current.getBytes());
    // ... evaluate expression ...
    current += 1;
}
```

**Why this works**: 
- Single variable in namespace, so `SearchVariable` always finds the same variable
- Direct value updates avoid PTR complexity
- More efficient than creating/destroying variables each iteration

### 3. Iteration Types

#### Numeric Ranges
```cpp
if (iterParam.vVal->mValue.mToken == GrapaTokenType::INT) {
    GrapaInt rangeInt;
    rangeInt.FromBytes(iterParam.vVal->mValue);
    GrapaInt current = 0;
    
    while (current < rangeInt) {
        actualVar->mValue.FROM(current.getBytes());
        // ... evaluate expression ...
        current += 1;
    }
}
```

#### Collections (Lists/Arrays)
```cpp
else if (iterParam.vVal->mValue.mToken == GrapaTokenType::LIST || 
         iterParam.vVal->mValue.mToken == GrapaTokenType::ARRAY) {
    GrapaInt length = iterParam.vVal->vQueue ? iterParam.vVal->vQueue->mCount : 0;
    GrapaInt i = 1;
    
    while (i <= length) {
        GrapaRuleEvent* elementEvent = iterParam.vVal->vQueue->Head((u64)(i.LongValue() - 1));
        if (elementEvent) {
            actualVar->mValue.FROM(elementEvent->mValue);
            // ... evaluate expression ...
        }
        i += 1;
    }
}
```

#### Strings
```cpp
else if (iterParam.vVal->mValue.mToken == GrapaTokenType::STR) {
    GrapaCHAR strValue = iterParam.vVal->mValue;
    GrapaInt length = strValue.mLength;
    GrapaInt i = 1;
    
    while (i <= length) {
        GrapaCHAR charValue;
        charValue.FROM((char*)&strValue.mBytes[(int)(i.LongValue() - 1)], 1);
        actualVar->mValue.FROM(charValue);
        // ... evaluate expression ...
        i += 1;
    }
}
```

### 4. Conditional Filtering

```cpp
if (hasCondition) {
    GrapaRuleEvent* condResult = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(3));
    bool conditionMet = false;
    
    if (condResult) {
        GrapaRuleEvent* condVal = condResult->mValue.mToken == GrapaTokenType::PTR ? 
                                condResult->vRulePointer : condResult;
        
        if (condVal && condVal->mValue.mLength && condVal->mValue.mBytes[0] && 
            condVal->mValue.mBytes[0] != '0' && !condVal->IsNull()) {
            conditionMet = true;
        }
        
        condResult->CLEAR();
        delete condResult;
    }
    
    if (!conditionMet) {
        continue; // Skip this iteration
    }
}
```

### 5. Expression Evaluation

```cpp
// Evaluate expression and add to result
GrapaRuleEvent* exprResult = vScriptExec->ProcessPlan(pNameSpace, pInput->Head(0));
if (exprResult) {
    // Handle PTR tokens (dereference if needed)
    GrapaRuleEvent* r1 = exprResult->mValue.mToken == GrapaTokenType::PTR ? 
                       exprResult->vRulePointer : exprResult;
    
    // Copy result to avoid memory issues
    GrapaRuleEvent* copy = vScriptExec->CopyItem(r1);
    result->vQueue->PushTail(copy);
    
    exprResult->CLEAR();
    delete exprResult;
}
```

## Usage Examples

### Basic List Comprehension
```grapa
[x for x in 3]           // [0,1,2]
[x*2 for x in 3]         // [0,2,4]
[x+1 for x in 5]         // [1,2,3,4,5]
```

### Conditional Filtering
```grapa
[x for x in 5 if x > 1]  // [2,3,4]
[x for x in 10 if x % 2 == 0]  // [0,2,4,6,8]
```

### Collection Iteration
```grapa
arr = [10, 20, 30];
[x*2 for x in arr]       // [20,40,60]
```

### String Iteration
```grapa
[x for x in "abc"]       // ["a","b","c"]
```

## Key Learnings

### 1. Variable Scoping Pattern
- Use `AddRuleOperation` + `vLocals` pattern for temporary namespaces
- Create variables once, update values in place
- Avoid PTR complexity when direct updates work

### 2. Memory Management
- Always clean up temporary namespaces
- Use `vScriptExec->CopyItem()` for expression results
- Clear and delete intermediate results

### 3. Expression Evaluation
- `vScriptExec->CopyItem()` handles PTR dereferencing automatically
- No need for manual PTR chain following
- Process results immediately to avoid memory leaks

### 4. Iteration Patterns
- Follow `ForEvent` patterns for consistency
- Handle different iterable types appropriately
- Use 1-based indexing for collections (Grapa convention)

## Testing

The implementation has been tested with:
- Basic numeric ranges
- Complex expressions
- Conditional filtering
- Variable scoping (outer variables unaffected)
- Collection and string iteration

All tests pass and produce expected results.

## Future Enhancements

Potential improvements:
- Support for multiple variables (nested loops)
- Support for multiple conditions
- Performance optimizations for large iterables
- Integration with other Grapa features (lambdas, etc.)
