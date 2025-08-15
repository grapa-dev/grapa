# $sys.grc Parameter Renaming Proposal

## Overview
This document proposes improved parameter names for methods in `$sys.grc` that currently have ambiguous parameter identifiers.

## Methods Requiring Parameter Renaming

### 1. type(b)
**Current parameters:**
- `b` - Object to get type of

**Proposed parameters:**
- `object` - Object to get type of

**Rationale:** Based on C++ implementation analysis:
- `GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);` (b) → The object whose type to determine
- The method returns the type name as a string (e.g., "$INT", "$STR", "$ARRAY", etc.)

### 2. describe(b)
**Current parameters:**
- `b` - Object to describe

**Proposed parameters:**
- `object` - Object to describe

**Rationale:** Based on C++ implementation analysis:
- `GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);` (b) → The object to describe
- Currently returns error -1 (not fully implemented)

### 3. getenv(b)
**Current parameters:**
- `b` - Environment variable name

**Proposed parameters:**
- `name` - Environment variable name

**Rationale:** Based on C++ implementation analysis:
- `GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);` (b) → The environment variable name to retrieve
- Handles special system variables like "$PATH", "$VERSION", "$PLATFORM", etc.

### 4. putenv(b, c)
**Current parameters:**
- `b` - Environment variable name
- `c` - Value to set

**Proposed parameters:**
- `name` - Environment variable name
- `value` - Value to set

**Rationale:** Based on C++ implementation analysis:
- `GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);` (b) → The environment variable name to set
- `GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);` (c) → The value to set for the environment variable
- Handles special system variables and regular environment variables

### 5. encode(a, b, c)
**Current parameters:**
- `a` - Data to encode
- `b` - Method/options
- `c` - Additional parameters

**Proposed parameters:**
- `data` - Data to encode
- `method` - Encoding method/options
- `params` - Additional parameters

**Rationale:** Based on C++ implementation analysis:
- `GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL, needEval);` (a) → The data to encode
- `GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);` (b) → The encoding method (e.g., "RSA", "AES256", "SHAKE256", etc.)
- `GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);` (c) → Additional parameters for the encoding method

### 6. sleep(b)
**Current parameters:**
- `b` - Milliseconds to sleep

**Proposed parameters:**
- `milliseconds` - Milliseconds to sleep

**Rationale:** Based on C++ implementation analysis:
- `GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);` (b) → The number of milliseconds to sleep
- `rInt.FromBytes(r1.vVal->mValue);` → Converts to integer milliseconds
- `std::this_thread::sleep_for(std::chrono::milliseconds(rInt.GetItem(0)));` → Sleeps for the specified milliseconds

## Methods with No Parameters (No Changes Needed)

The following methods have no parameters and do not need changes:
- `compilef()` - No parameters (internal compilation function)
- `compile()` - No parameters (internal compilation function)
- `eval()` - No parameters (internal evaluation function)

## Implementation Plan

1. **Baseline Testing** ✅ - Completed
2. **Parameter Renaming** - Update `lib/grapa/$sys.grc`
3. **Regression Testing** - Create and run test with new parameter names
4. **Documentation Update** - Check if documentation exists and needs updates

## Expected Changes in $sys.grc

```diff
- type = @<[op,@<type,{@<var,{b}>}>],{b}>;
+ type = @<[op,@<type,{@<var,{object}>}>],{object}>;

- describe = @<[op,@<describe,{@<var,{b}>}>],{b}>;
+ describe = @<[op,@<describe,{@<var,{object}>}>],{object}>;

- getenv = @<[op,@<getenv,{@<var,{b}>}>],{b}>;
+ getenv = @<[op,@<getenv,{@<var,{name}>}>],{name}>;

- putenv = @<[op,@<putenv,{@<var,{b}>,@<var,{c}>}>],{b,c}>;
+ putenv = @<[op,@<putenv,{@<var,{name}>,@<var,{value}>}>],{name,value}>;

- encode = @<[op,@<encode,{@<var,{a}>,@<var,{b}>,@<var,{c}>}>],{a,b,c}>;
+ encode = @<[op,@<encode,{@<var,{data}>,@<var,{method}>,@<var,{params}>}>],{data,method,params}>;

- sleep = @<[op,@<sleep,{@<var,{b}>}>],{b}>;
+ sleep = @<[op,@<sleep,{@<var,{milliseconds}>}>],{milliseconds}>;
```

## Impact Assessment

- **Low Risk**: Only parameter names are changing, not functionality
- **Backward Compatibility**: No breaking changes to method signatures
- **Documentation**: Will need to check if documentation exists and needs updates
- **Testing**: Comprehensive baseline and regression tests will validate no functional changes
