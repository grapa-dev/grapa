---
tags:
  - maintainer
  - implementation
  - widget
  - callback
  - gui
---

# Grapa Widget Implementation

## Overview

This document captures the implementation details and patterns for Grapa's widget system, particularly focusing on the callback mechanism that enables rich context-aware execution.

## Key Design Pattern: Object Reference Callbacks

### Core Insight
The widget callback system passes the **actual Grapa object reference** as the first parameter to callbacks, enabling direct object manipulation using dot notation.

### Callback Signature Pattern
```grapa
callback: op(o, cbdata, item) {
    // o = the actual widget object reference
    // cbdata = callback data (if any)
    // item = menu item (for menu callbacks)
    
    // Direct object access via dot notation
    o.get("property");           // Get widget property
    o.set({"property": "value"}); // Set widget property
    o.parent();                  // Access parent widget
    o.child("name");             // Access child widget
    o.hide();                    // Call widget methods
    o.show();
}
```

## Implementation Architecture

### 1. Widget Post System
The `widget_post` method enables asynchronous execution with rich context:

```grapa
// In $WIDGET.grc
post = @<[op,@<"widget_post",{@<this>,@<var,{postop}>,@<var,{postparams}>,@<var,{doneop}>}>],{postop,postparams,doneop}>;
```

**C++ Implementation** (`GrapaLibRule.cpp`):
```cpp
GrapaRuleEvent* GrapaLibraryRuleWidgetPostEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
    // Extract parameters: widget, runCode, params, doneCode
    GrapaLibraryParam r1(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL); // widget
    GrapaLibraryParam r2(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL); // runCode
    GrapaLibraryParam r3(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL); // params
    GrapaLibraryParam r4(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL); // doneCode

    // Get widget object
    GrapaRuleEvent* objEvent = vScriptExec->vScriptState->SearchTarget(pNameSpace, r1.vVal);
    
    // Execute in widget's thread
    err = objEvent->vWidget->Post(r2.vVal, r3.vVal, r4.vVal);
}
```

### 2. Worker Thread Execution
The `GrapaWorker` class manages callback execution in separate threads:

```cpp
// In GrapaState.h
class GrapaWorker : public GrapaThread
{
public:
    GrapaRuleEvent mWorkerEvent;
    GrapaNames mRuleVariables;  // Worker's namespace
    GrapaRuleEvent *vRunCode, *vDoneCode, *vResult;
    GrapaRuleEvent *vParam;
    
    void Set(GrapaScriptExec* pTokenExec, GrapaNames* pNameSpace)
    {
        mWorkerEvent.vClass = mScriptState.GetClass(pNameSpace, GrapaCHAR("$thread"));
        mScriptState.Stop();
        mRuleVariables.SetParrent(pNameSpace);  // Key: Set parent namespace
        mScriptState.vConsoleSend = pTokenExec->vScriptState->vConsoleSend;
        mScriptState.Start();
    }
    
    virtual void Running()
    {
        // Create parameter queue with widget object reference
        GrapaRuleQueue p;
        GrapaRuleEvent* p1 = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
        p.PushTail(p1);
        p1->mValue.mToken = GrapaTokenType::PTR;
        p1->vRulePointer = vParam;  // This contains the widget object reference
        
        // Execute callback with widget context
        vResult = mScriptExec.ProcessPlan(&mRuleVariables, vRunCode, p1);
    }
};
```

### 3. Object Reference Injection
The key mechanism is injecting the widget object reference into the callback's parameter list:

```cpp
// In GrapaWidgetThread::SendM() - shows how widget object is passed
GrapaRuleEvent* GrapaWidgetThread::SendM(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
    GrapaRuleQueue q;
    GrapaRuleEvent wid;
    wid.mValue.mToken = GrapaTokenType::WIDGET;
    wid.vWidget = widget;  // ← Widget object reference
    wid.vClass = vScriptExec->vScriptState->GetClass(pNameSpace, GrapaCHAR("$WIDGET"));
    q.PushTail(&wid);  // ← First parameter: widget object
    
    // Add data parameter
    GrapaRuleEvent wid2;
    wid2.mValue.mToken = GrapaTokenType::PTR;
    wid2.vRulePointer = pValue;
    q.PushTail(&wid2);  // ← Second parameter: data
    
    // Execute callback with widget context
    result = vScriptExec->ProcessPlan(pNameSpace, messageCB, &wid);
}
```

## Editor Implementation Examples

### 1. Post Callback Setup
The editor demonstrates rich callback setup with multiple event handlers:

```grapa
// In $editor.grc
on_post_start: op(o){
    o.set({"color":"YELLOW"});
    o.redraw();
},
on_post_echo: op(o,data){
    o.set({"append":data.str(),"key":"end"});
},
on_post_prompt: op(o,data){
    o.set({"append":"\ngrapa> ","key":"end"});
},
on_post_end: op(o,data){
    o.set({"append":"\n"+data.str(),"key":"end","color":"white","cursor_state":"show","cursor_color":"black"});
    o.redraw();
}
```

### 2. Menu Callback Pattern
Menu callbacks receive the widget object and can manipulate the entire widget hierarchy:

```grapa
callback: op(o,cbdata,item) {
    $local.label = "Grapa Editor " + $global.editors.len().str();
    createEditor($local.label);
}
```

### 3. Key Event Handling
Key event callbacks can access widget properties and methods:

```grapa
on_keydown: op(o,event) {
    $local.handled = 0;
    if ((o.event_key() & 0x7f) == 0x0d) {
        $local.s = o.get("selection");
        // ... handle enter key
        $local.t = s.post();  // Execute script in widget context
    }
    handled;
}
```

## Key Learnings for Formula Fields

### 1. Object Reference Pattern
The widget system passes the **actual object reference** as the first callback parameter, enabling:
- Direct property access: `o.get("property")`
- Direct method calls: `o.method()`
- Object hierarchy navigation: `o.parent()`, `o.child("name")`

### 2. Namespace Inheritance
The worker thread's namespace inherits from the parent namespace:
```cpp
mRuleVariables.SetParrent(pNameSpace);  // Enables access to parent context
```

### 3. Parameter Injection
The system injects the object reference into the callback's parameter list:
```cpp
p1->vRulePointer = vParam;  // Object reference becomes first parameter
```

### 4. Rich Context Access
Callbacks have access to the entire object context, not just isolated data.

## Application to Formula Fields

The formula field system should follow the same pattern:

1. **Database Context Object**: Pass a database context object (like `dbx`) as the first parameter
2. **Rich Context**: The context object should provide access to record fields, table info, etc.
3. **Method Access**: Enable direct method calls like `dbx.getField("name")`
4. **Namespace Inheritance**: Formula execution should inherit from the database context

## Implementation Requirements

### For Formula Fields
1. Create a database context object that represents the current record/table
2. Register database functions (`getField`, `getFieldPartial`) in the formula execution namespace
3. Pass the database context object as the first parameter to formula callbacks
4. Enable direct object access via dot notation in formulas

### C++ Changes Needed
1. Create `GrapaDBXContext` class similar to `GrapaWidget`
2. Register database functions in `GrapaLibRule.cpp`
3. Implement formula execution using `GrapaWorker` pattern
4. Inject database context object into formula parameter list

## Conclusion

The widget callback system demonstrates a powerful pattern for context-aware execution:
- **Object reference injection** enables rich context access
- **Namespace inheritance** provides access to parent context
- **Direct object manipulation** via dot notation simplifies callback code
- **Asynchronous execution** with completion callbacks

This pattern should be applied to formula fields to enable similar rich context access for database operations. 