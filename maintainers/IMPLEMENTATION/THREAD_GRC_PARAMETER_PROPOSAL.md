# $thread.grc Parameter Renaming Proposal

## Overview
This document proposes improved parameter names for methods in `$thread.grc` that currently have ambiguous parameter identifiers.

## Methods Requiring Parameter Renaming

### 1. start(o, p, c)
**Current parameters:**
- `o` - Run code/function to execute
- `p` - Parameter to pass to the run code
- `c` - Done code/completion callback

**Proposed parameters:**
- `runCode` - Run code/function to execute
- `param` - Parameter to pass to the run code
- `doneCode` - Done code/completion callback

**Rationale:** Based on C++ implementation analysis:
- `GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);` (o) → `pRunCode` - The function to run
- `GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);` (p) → `pParam` - The parameter to pass
- `GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);` (c) → `pDoneCode` - The completion callback
- `objEvent->vWorker->Run(r2.vVal, r3.vVal, r4.vVal);` - Calls `GrapaWorker::Run(pRunCode, pParam, pDoneCode)`

## Methods with No Parameters (No Changes Needed)

The following methods have no parameters and do not need changes:
- `trylock()` - No parameters
- `lock()` - No parameters
- `unlock()` - No parameters
- `wait()` - No parameters
- `signal()` - No parameters
- `waiting()` - No parameters
- `stop()` - No parameters
- `started()` - No parameters
- `suspend()` - No parameters
- `resume()` - No parameters
- `suspended()` - No parameters

## Implementation Plan

1. **Baseline Testing** ✅ - Completed
2. **Parameter Renaming** - Update `lib/grapa/$thread.grc`
3. **Regression Testing** - Create and run test with new parameter names
4. **Documentation Update** - Check if documentation exists and needs updates

## Expected Changes in $thread.grc

```diff
- start = @<[op,@<thread_start,{null,@<var,{o}>,@<var,{p}>,@<var,{c}>}>],{o,p,c}>;
+ start = @<[op,@<thread_start,{null,@<var,{runCode}>,@<var,{param}>,@<var,{doneCode}>}>],{runCode,param,doneCode}>;
```

## Impact Assessment

- **Low Risk**: Only parameter names are changing, not functionality
- **Backward Compatibility**: No breaking changes to method signatures
- **Documentation**: Will need to check if documentation exists and needs updates
- **Testing**: Comprehensive baseline and regression tests will validate no functional changes

