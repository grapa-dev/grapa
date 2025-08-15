# $net.grc Parameter Renaming Proposal

## Overview
This document proposes improved parameter names for methods in `$net.grc` that currently have ambiguous parameter identifiers.

## Methods Requiring Parameter Renaming

### 1. onlisten(o, p, c, n)
**Current parameters:**
- `o` - URL/binding address
- `p` - Message handler function
- `c` - Connect handler function  
- `n` - Count of listeners (default: 1)

**Proposed parameters:**
- `url` - URL/binding address
- `messageHandler` - Message handler function
- `connectHandler` - Connect handler function
- `count` - Count of listeners (default: 1)

**Rationale:** Based on C++ implementation analysis:
- `r1` (o) → `objEvent` (the net object itself)
- `r2` (p) → `pUrl` (URL parameter)
- `r3` (c) → `pReceiveLambda` (message handler)
- `r4` (n) → `pConnectLambda` (connect handler)

### 2. send(d)
**Current parameters:**
- `d` - Data/message to send

**Proposed parameters:**
- `data` - Data/message to send

**Rationale:** Based on C++ implementation analysis:
- `r2` (d) → `r2.vVal->mValue` (the data being sent via `Send()`)

### 3. onreceive(o)
**Current parameters:**
- `o` - Receive handler function

**Proposed parameters:**
- `handler` - Receive handler function

**Rationale:** Based on C++ implementation analysis:
- `r2` (o) → `pReceiveLambda` (the handler function for receive events)

## Methods with Good Parameter Names (No Changes Needed)

The following methods already have descriptive parameter names and do not need changes:
- `connect(url, cert, proxy)` - All descriptive
- `bind(url)` - Descriptive
- `listen(net)` - Descriptive
- `proxy(ptype, pid, ph)` - Mostly descriptive
- `certificate(cert)` - Descriptive
- `private(key, pass, param)` - Descriptive
- `trusted(file, path)` - Descriptive
- `httpsend(method, entity, header, body)` - All descriptive
- `httpmessage(raw)` - Descriptive

## Implementation Plan

1. **Baseline Testing** ✅ - Completed
2. **Parameter Renaming** - Update `lib/grapa/$net.grc`
3. **Regression Testing** - Create and run test with new parameter names
4. **Documentation Update** - Update `docs-src/docs/sys/net.md`

## Expected Changes in $net.grc

```diff
- onlisten = @<[op,@<net_onlisten,{this,@<var,{o}>,@<var,{p}>,@<var,{c}>,@<var,{n}>}>],{o,p,c,n:1}>;
+ onlisten = @<[op,@<net_onlisten,{this,@<var,{url}>,@<var,{messageHandler}>,@<var,{connectHandler}>,@<var,{count}>}>],{url,messageHandler,connectHandler,count:1}>;

- send = @<[op,@<net_send,{this,@<var,{d}>}>],{d}>;
+ send = @<[op,@<net_send,{this,@<var,{data}>}>],{data}>;

- onreceive = @<[op,@<net_onreceive,{this,@<var,{o}>}>],{o}>;
+ onreceive = @<[op,@<net_onreceive,{this,@<var,{handler}>}>],{handler}>;
```

## Impact Assessment

- **Low Risk**: Only parameter names are changing, not functionality
- **Backward Compatibility**: No breaking changes to method signatures
- **Documentation**: Will need updates to reflect new parameter names
- **Testing**: Comprehensive baseline and regression tests will validate no functional changes
