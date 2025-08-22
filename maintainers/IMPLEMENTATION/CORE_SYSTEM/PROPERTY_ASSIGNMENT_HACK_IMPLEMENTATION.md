# Property Assignment "Hack" Implementation

## Overview
The property assignment "hack" is a mechanism that enables automatic variable creation when assigning to non-existent properties on objects that support property assignment.

## Purpose
When a user attempts to assign to a property that doesn't exist (e.g., `obj.new_prop = 42`), Grapa automatically creates the property rather than throwing an error.

## Implementation Details

### The "Hack" Mechanism
In the property access logic (likely in `GrapaLibraryRuleNameEvent::Run`), when a property doesn't exist:

```cpp
if (attrList->Next() == NULL && root->mValue.mToken != GrapaTokenType::ERR)
{
    // this is a HACK to pass the item back to Assign...look at the Assign that checks for ERR
    // ended up needing to put code in various places to remove the vRulePointer and vClass if not used
    result = Error(vScriptExec, pNameSpace, -1);
    result->mVar = true;
    result->mLocal = true;
    result->vRulePointer = attr;
    result->vClass = root;
    result->mName.FROM(attr->mValue);
    if (result->vClass->mValue.mToken == GrapaTokenType::TAG)
    {
        result->vClass = root->vQueue->Head()->vQueue->Head(1);
    }
    break;
}
```

### How It Works
1. **Detection**: When property access fails (`attrList->Next() == NULL`)
2. **Setup**: Creates a special `$ERR` event with specific flags:
   - `mVar = true` - Marks as a variable reference
   - `mLocal = true` - Marks as local scope
   - `vRulePointer = attr` - Points to the property name
   - `vClass = root` - Points to the target object
   - `mName` - Contains the property name
3. **Integration**: `ItemAssignRun` detects this setup and calls `AddRawParameter` to create the new property

### Type Restrictions
The hack is limited to types that naturally support property assignment:
- `$OBJ` - Objects
- `$LIST` - Lists/Objects  
- `$TAG` - Tagged objects

Other types (primitives, system types) are excluded to prevent crashes from temporary object deletion.

### Critical Safety Considerations
- **Temporary Object Issue**: The original implementation caused crashes when used with types that don't support property assignment
- **Scope Management**: The `mLocal` flag helps manage object lifetime
- **Type Validation**: Only certain types are allowed to use this mechanism

## Related Functions
- `ItemAssignRun()` - Main assignment logic that processes the hack
- `AddRawParameter()` - Creates the actual property in the target object

## Recent Fix
The fix implemented in 2024 limits the hack to only `$OBJ`, `$LIST`, and `$TAG` types to prevent crashes from temporary object deletion on primitive and system types.
