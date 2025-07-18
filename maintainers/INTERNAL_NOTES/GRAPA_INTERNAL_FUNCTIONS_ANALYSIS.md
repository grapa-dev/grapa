# Grapa Internal Functions Analysis

This document provides a comprehensive analysis of all internal functions available in Grapa, discovered by examining the source code in `source/grapa/GrapaLibRule.cpp`.

## Function Discovery Method

The internal functions are registered in a `handlerMap` table at lines 2282-2569 in `GrapaLibRule.cpp`. Each function has:
1. A string identifier (the function name)
2. A handler method pointer (the implementation)

## Complete Function List

### Language Control Functions
- **rule** - Rule definition and manipulation
- **ruleval** - Rule value handling
- **ruleref** - Rule reference handling
- **token** - Token manipulation
- **grammar** - Grammar definition
- **reset** - Reset execution state
- **shell** - Execute shell commands
- **eval** - Evaluate expressions
- **exec** - Execute code
- **post** - Post events
- **sleep** - Sleep/delay execution
- **exit** - Exit execution
- **getenv** - Get environment variables
- **putenv** - Set environment variables
- **include** - Include files
- **name** - Name handling
- **getname** - Get names
- **lit** - Literal values
- **ref** - References
- **setnull** - Set null values
- **var** - Variable handling
- **assign** - Assignment
- **assignappend** - Append assignment
- **assignextend** - Extend assignment

### Data Structure Creation
- **createarray** - Create arrays
- **createtuple** - Create tuples
- **createlist** - Create lists
- **createxml** - Create XML structures
- **createel** - Create elements
- **createtag** - Create tags
- **createop** - Create operations
- **createcode** - Create code blocks
- **createrule** - Create rules

### Data Manipulation
- **extend** - Extend data structures
- **remove** - Remove elements
- **prepend** - Prepend elements
- **pop** - Pop elements
- **call** - Function calls
- **search** - Search operations
- **findall** - Find all matches
- **sort** - Sort data
- **argsort** - Sort by arguments
- **unique** - Get unique elements
- **group** - Group data
- **class** - Class definition
- **obj** - Object creation
- **global** - Global scope
- **parent** - Parent scope
- **this** - Current object
- **local** - Local scope
- **static** - Static scope
- **const** - Constants
- **setconst** - Set constants
- **return** - Return values
- **break** - Break execution
- **if** - Conditional execution
- **while** - While loops
- **scope** - Scope definition
- **switch** - Switch statements
- **case** - Case statements
- **plan** - Execution planning
- **wrap** - Wrap operations
- **op** - Operations
- **map** - Map operations
- **reduce** - Reduce operations
- **filter** - Filter operations
- **range** - Range generation
- **isint** - Integer type checking
- **iferr** - Error handling
- **message** - Message handling
- **string** - String operations
- **echo** - Output
- **console** - Console operations
- **prompt** - User prompts

### Mathematical Functions
- **genprime** - Generate prime numbers
- **staticprime** - Static prime generation
- **isprime** - Prime number testing
- **isaks** - AKS primality test
- **random** - Random number generation
- **setbit** - Set bits
- **clearbit** - Clear bits
- **genbits** - Generate bits
- **genkeys** - Generate keys
- **encode** - Encoding
- **encoderaw** - Raw encoding
- **decode** - Decoding
- **sign** - Digital signatures
- **signadd** - Add signatures
- **verify** - Verify signatures
- **verifyrecover** - Recover from signatures
- **secret** - Secret handling
- **setfloat** - Set float precision
- **setfix** - Set fixed precision
- **root** - Root calculations
- **pow** - Power operations
- **mod** - Modulo operations
- **modpow** - Modular exponentiation
- **modinv** - Modular inverse
- **abs** - Absolute value
- **gcd** - Greatest common divisor
- **e** - Euler's number
- **pi** - Pi constant
- **ln** - Natural logarithm
- **log** - Logarithm
- **add** - Addition
- **sub** - Subtraction
- **mul** - Multiplication
- **div** - Division
- **sin** - Sine
- **cos** - Cosine
- **tan** - Tangent
- **cot** - Cotangent
- **sec** - Secant
- **csc** - Cosecant
- **asin** - Arcsine
- **acos** - Arccosine
- **atan** - Arctangent
- **acot** - Arccotangent
- **asec** - Arcsecant
- **acsc** - Arccosecant
- **sinh** - Hyperbolic sine
- **cosh** - Hyperbolic cosine
- **tanh** - Hyperbolic tangent
- **coth** - Hyperbolic cotangent
- **sech** - Hyperbolic secant
- **csch** - Hyperbolic cosecant
- **asinh** - Inverse hyperbolic sine
- **acosh** - Inverse hyperbolic cosine
- **atanh** - Inverse hyperbolic tangent
- **acoth** - Inverse hyperbolic cotangent
- **asech** - Inverse hyperbolic secant
- **acsch** - Inverse hyperbolic cosecant
- **atan2** - Two-argument arctangent
- **hypot** - Hypotenuse

### Bitwise Operations
- **bsl** - Bit shift left
- **bsr** - Bit shift right
- **bor** - Bitwise OR
- **band** - Bitwise AND
- **or** - Logical OR
- **and** - Logical AND
- **xor** - Logical XOR
- **inv** - Bitwise inversion
- **neg** - Negation
- **not** - Logical NOT

### Matrix/Vector Operations
- **t** - Transpose
- **rref** - Reduced row echelon form
- **det** - Determinant
- **rank** - Matrix rank
- **solve** - Solve linear systems
- **cov** - Covariance
- **shape** - Get shape
- **reshape** - Reshape arrays
- **dot** - Dot product
- **identity** - Identity matrix
- **diagonal** - Diagonal matrix
- **triu** - Upper triangular
- **tril** - Lower triangular
- **eigh** - Eigenvalues/eigenvectors
- **sum** - Summation
- **mean** - Mean calculation

### Type Conversion
- **bits** - Convert to bits
- **bytes** - Convert to bytes
- **len** - Get length
- **bool** - Boolean conversion
- **int** - Integer conversion
- **raw** - Raw conversion
- **base** - Base conversion
- **str** - String conversion
- **list** - List conversion
- **array** - Array conversion
- **vector** - Vector conversion
- **xml** - XML conversion
- **float** - Float conversion
- **fix** - Fixed-point conversion
- **time** - Time conversion
- **type** - Type checking
- **describe** - Type description

### String Operations
- **left** - Left substring
- **right** - Right substring
- **mid** - Middle substring
- **midtrim** - Trimmed middle substring
- **rtrim** - Right trim
- **ltrim** - Left trim
- **trim** - Trim whitespace
- **rrot** - Right rotation
- **lrot** - Left rotation
- **rpad** - Right padding
- **lpad** - Left padding
- **reverse** - String reversal
- **replace** - String replacement
- **grep** - Pattern matching
- **split** - String splitting
- **join** - String joining
- **lower** - Convert to lowercase
- **upper** - Convert to uppercase

### Comparison Operations
- **eq** - Equal
- **neq** - Not equal
- **gteq** - Greater than or equal
- **gt** - Greater than
- **lteq** - Less than or equal
- **lt** - Less than
- **cmp** - Compare

### Time Operations
- **utc** - UTC time
- **tz** - Timezone operations

### File System Operations
- **file_table** - Create file tables
- **file_pwd** - Print working directory
- **file_cd** - Change directory
- **file_phd** - Print home directory
- **file_chd** - Change home directory
- **file_ls** - List directory contents
- **file_mk** - Make directories
- **file_rm** - Remove directories/files
- **file_mkfield** - Make fields
- **file_rmfield** - Remove fields
- **file_info** - Get file information
- **file_set** - Set file data
- **file_get** - Get file data
- **file_split** - Split files
- **file_debug** - Debug file operations

### Network Operations
- **net_mac** - MAC address operations
- **net_interfaces** - Network interfaces
- **net_connect** - Network connections
- **net_bind** - Network binding
- **net_listen** - Network listening
- **net_onlisten** - Listen event handling
- **net_disconnect** - Disconnect
- **net_proxy** - Proxy operations
- **net_certificate** - Certificate handling
- **net_private** - Private key operations
- **net_trusted** - Trusted certificate operations
- **net_verify** - Certificate verification
- **net_chain** - Certificate chain operations
- **net_host** - Host operations
- **net_send** - Send data
- **net_receive** - Receive data
- **net_pending** - Check pending data
- **net_onreceive** - Receive event handling
- **http_read** - HTTP read operations
- **http_send** - HTTP send operations
- **http_message** - HTTP message handling

### Threading Operations
- **thread_trylock** - Try to acquire lock
- **thread_lock** - Acquire lock
- **thread_unlock** - Release lock
- **thread_wait** - Wait for condition
- **thread_signal** - Signal condition
- **thread_waiting** - Check if waiting
- **thread_start** - Start thread
- **thread_stop** - Stop thread
- **thread_started** - Check if started
- **thread_suspend** - Suspend thread
- **thread_resume** - Resume thread
- **thread_suspended** - Check if suspended

### Widget Operations
- **widget_new** - Create widgets
- **widget_show** - Show widgets
- **widget_hide** - Hide widgets
- **widget_redraw** - Redraw widgets
- **widget_resizable** - Set resizable
- **widget_resize** - Resize widgets
- **widget_parent** - Get parent widget
- **widget_child** - Get child widget
- **widget_next** - Get next widget
- **widget_focus** - Set focus
- **widget_get** - Get widget properties
- **widget_set** - Set widget properties
- **widget_handle** - Get widget handle
- **widget_callback** - Set callbacks
- **widget_event_key** - Key event handling
- **widget_append** - Append to widget
- **widget_post** - Post to widget
- **widget_clear** - Clear widget

## Compile-Time Optimization System

Grapa includes a sophisticated compile-time optimization system that operates on the execution tree. Many internal functions have an `Optimize` method that is called during compilation to improve performance.

### Optimization Mechanism
- **Location**: `source/grapa/GrapaState.cpp` line 6115
- **Trigger**: Called during `Plan()` execution when processing library events
- **Pattern**: `code->vLibraryEvent->Optimize(this, pNameSpace, result2, param)`

### Optimization Types

#### 1. **Constant Folding**
Functions like `createarray`, `createtuple`, `createlist` check if all parameters are constants (not operations or code blocks):
```cpp
bool noLambda = true;
while (v) {
    if (v->mValue.mToken == GrapaTokenType::OP || v->mValue.mToken == GrapaTokenType::CODE) {
        noLambda = false;
        break;
    }
    v = v->Next();
}
if (noLambda) {
    // Convert to immediate value at compile time
    v->mValue.mToken = GrapaTokenType::ARRAY;
}
```

#### 2. **Literal Optimization**
The `lit` function optimizes by removing unnecessary wrapper operations when the parameter is already a literal value.

#### 3. **Structure Flattening**
Functions like `prepend` can flatten nested structures when all elements are constants, reducing runtime overhead.

#### 4. **Code Simplification**
The system automatically removes unnecessary `CODE` wrappers when they contain only single elements.

### Optimization Benefits
- **Performance**: Reduces runtime overhead by pre-computing constant expressions
- **Memory**: Eliminates unnecessary intermediate structures
- **Efficiency**: Flattens execution trees for better performance

## Key Insights

### Function Categories
1. **Core Language**: Control flow, variable handling, scope management
2. **Mathematics**: Comprehensive mathematical library including crypto functions
3. **Data Structures**: Array, vector, matrix operations with linear algebra
4. **String Processing**: Rich text manipulation capabilities
5. **File System**: Database-like file operations
6. **Networking**: Full network stack with HTTP support
7. **Concurrency**: Threading and synchronization
8. **GUI**: Widget system for user interfaces

### Notable Capabilities
- **Cryptography**: Prime generation, digital signatures, key management
- **Linear Algebra**: Matrix operations, eigenvalues, solving systems
- **Database**: File-based database with table operations
- **Networking**: Full TCP/IP stack with HTTP support
- **GUI**: Cross-platform widget system
- **Unicode**: Full Unicode support throughout
- **Compile-Time Optimization**: Sophisticated tree optimization system

### Implementation Pattern
Each function follows a consistent pattern:
1. Extract parameters using `GrapaLibraryParam`
2. Check parameter types and validity
3. Perform the operation
4. Return result or error

Many functions also include an `Optimize` method for compile-time optimization:
1. Check if parameters are constants
2. Perform constant folding when possible
3. Simplify execution tree structure
4. Return optimized operation

This analysis reveals Grapa as a comprehensive programming language with capabilities spanning from low-level bit operations to high-level GUI development, with particular strength in mathematical computing, data processing, and compile-time optimization. 