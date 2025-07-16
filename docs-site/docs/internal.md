## Grapa Variable Scoping Best Practice
- Always use $local. only at the top of a function for variable declarations.
- Never use $local. inside loops or blocks, as this creates a new variable in a new namespace and can cause subtle bugs.
- After the first assignment, use the variable name directly everywhere in the function.

## Backend TODO
- Fix table.get for INT/FLOAT types to return proper types, not $STR with raw bytes. 