# Grapa Base System Functions

## Overview

Grapa provides a comprehensive set of base system functions that form the foundation of the language. These functions are defined in the `handlerMap` array in `GrapaLibraryRuleEvent::LoadLib` and can be called from:

1. **BNF Grammar**: Directly referenced in language grammar rules
2. **Library Files**: Called from `.grc` files in `lib/grapa/`
3. **Scripts**: Used in user-written Grapa scripts

## Function Categories

### Language Core Functions

#### Grammar and Rules
- **`rule`** - Define grammar rules
- **`ruleval`** - Rule value operations
- **`ruleref`** - Rule reference operations
- **`token`** - Token handling
- **`grammar`** - Grammar management

#### System Control
- **`reset`** - Reset system state
- **`shell`** - Shell command execution
- **`eval`** - Evaluate expressions
- **`exec`** - Execute commands
- **`post`** - Post operations
- **`sleep`** - Sleep/delay execution
- **`exit`** - Exit program
- **`getenv`** - Get environment variable
- **`putenv`** - Set environment variable
- **`include`** - Include files

### Variable Management

#### Variable Operations
- **`name`** - Variable naming
- **`getname`** - Get variable name
- **`lit`** - Literal values
- **`ref`** - References
- **`setnull`** - Set null value
- **`var`** - Variable declaration

#### Assignment Operations
- **`assign`** - Basic assignment
- **`assignappend`** - Append assignment
- **`assignextend`** - Extend assignment

#### Scope Management
- **`global`** - Global scope
- **`parent`** - Parent scope
- **`this`** - Current object
- **`local`** - Local scope
- **`static`** - Static scope
- **`const`** - Constant declaration
- **`setconst`** - Set constant

### Data Structure Creation

#### Container Creation
- **`createarray`** - Create arrays
- **`createtuple`** - Create tuples
- **`createlist`** - Create lists (JSON objects)
- **`createxml`** - Create XML structures
- **`createel`** - Create elements
- **`createtag`** - Create tags

#### Code Structure Creation
- **`createop`** - Create operations
- **`createcode`** - Create code blocks
- **`createrule`** - Create rules

### Data Structure Operations

#### Basic Operations
- **`extend`** - Extend structures
- **`remove`** - Remove elements
- **`prepend`** - Prepend elements
- **`pop`** - Pop elements
- **`call`** - Function calls

#### Search and Analysis
- **`search`** - Search operations
- **`findall`** - Find all matches
- **`sort`** - Sort data
- **`argsort`** - Sort by arguments
- **`unique`** - Remove duplicates
- **`group`** - Group data

### Control Flow

#### Basic Control
- **`return`** - Return from function
- **`break`** - Break loops
- **`if`** - Conditional execution
- **`while`** - While loops
- **`scope`** - Scope blocks
- **`switch`** - Switch statements
- **`case`** - Case statements

#### Advanced Control
- **`plan`** - Planning operations
- **`wrap`** - Wrapper operations
- **`op`** - Operation handling

### Functional Programming

#### Higher-Order Functions
- **`map`** - Map function over data
- **`reduce`** - Reduce data with function
- **`filter`** - Filter data
- **`range`** - Generate ranges

### Type Checking and I/O

#### Type Operations
- **`isint`** - Check if integer
- **`iferr`** - Error handling
- **`message`** - Message handling
- **`string`** - String operations
- **`echo`** - Output to console
- **`console`** - Console operations
- **`prompt`** - User prompts

### Cryptography and Advanced Math

#### Prime Operations
- **`genprime`** - Generate prime numbers
- **`staticprime`** - Static prime operations
- **`isprime`** - Check if prime
- **`isaks`** - AKS primality test

#### Random and Bits
- **`random`** - Random number generation
- **`setbit`** - Set bit
- **`clearbit`** - Clear bit
- **`genbits`** - Generate bits
- **`genkeys`** - Generate keys

#### Cryptographic Operations
- **`encode`** - Encode data
- **`encoderaw`** - Raw encoding
- **`decode`** - Decode data
- **`sign`** - Digital signatures
- **`signadd`** - Add signatures
- **`verify`** - Verify signatures
- **`verifyrecover`** - Recover from signatures
- **`secret`** - Secret operations

### Mathematical Functions

#### Basic Math
- **`setfloat`** - Set float precision
- **`setfix`** - Set fixed precision
- **`root`** - Root operations
- **`pow`** - Power operations
- **`mod`** - Modulo operations
- **`modpow`** - Modular exponentiation
- **`modinv`** - Modular inverse
- **`abs`** - Absolute value
- **`gcd`** - Greatest common divisor

#### Constants
- **`e`** - Euler's number
- **`pi`** - Pi constant
- **`ln`** - Natural logarithm
- **`log`** - Logarithm

#### Arithmetic
- **`add`** - Addition
- **`sub`** - Subtraction
- **`mul`** - Multiplication
- **`div`** - Division

#### Trigonometric Functions
- **`sin`** - Sine
- **`cos`** - Cosine
- **`tan`** - Tangent
- **`cot`** - Cotangent
- **`sec`** - Secant
- **`csc`** - Cosecant

#### Inverse Trigonometric
- **`asin`** - Arcsine
- **`acos`** - Arccosine
- **`atan`** - Arctangent
- **`acot`** - Arccotangent
- **`asec`** - Arcsecant
- **`acsc`** - Arccosecant

#### Hyperbolic Functions
- **`sinh`** - Hyperbolic sine
- **`cosh`** - Hyperbolic cosine
- **`tanh`** - Hyperbolic tangent
- **`coth`** - Hyperbolic cotangent
- **`sech`** - Hyperbolic secant
- **`csch`** - Hyperbolic cosecant

#### Inverse Hyperbolic
- **`asinh`** - Inverse hyperbolic sine
- **`acosh`** - Inverse hyperbolic cosine
- **`atanh`** - Inverse hyperbolic tangent
- **`acoth`** - Inverse hyperbolic cotangent
- **`asech`** - Inverse hyperbolic secant
- **`acsch`** - Inverse hyperbolic cosecant

#### Special Functions
- **`atan2`** - Two-argument arctangent
- **`hypot`** - Hypotenuse

### Bit Operations

#### Bit Manipulation
- **`bsl`** - Bit shift left
- **`bsr`** - Bit shift right
- **`bor`** - Bitwise OR
- **`band`** - Bitwise AND
- **`or`** - Logical OR
- **`and`** - Logical AND
- **`xor`** - Exclusive OR
- **`inv`** - Bitwise inversion

### Matrix Operations

#### Linear Algebra
- **`t`** - Transpose
- **`rref`** - Reduced row echelon form
- **`det`** - Determinant
- **`rank`** - Matrix rank
- **`solve`** - Solve linear systems
- **`cov`** - Covariance
- **`neg`** - Negation

### Data Type Operations

#### Type Conversion
- **`bits`** - Bit operations
- **`bytes`** - Byte operations
- **`len`** - Length
- **`bool`** - Boolean conversion
- **`not`** - Logical NOT
- **`int`** - Integer conversion
- **`raw`** - Raw data
- **`base`** - Base conversion
- **`str`** - String conversion
- **`list`** - List conversion
- **`array`** - Array conversion
- **`vector`** - Vector conversion
- **`xml`** - XML conversion
- **`float`** - Float conversion
- **`fix`** - Fixed-point conversion
- **`time`** - Time operations
- **`type`** - Type information
- **`describe`** - Describe object

### String Operations

#### String Manipulation
- **`left`** - Left substring
- **`right`** - Right substring
- **`mid`** - Middle substring
- **`midtrim`** - Trim middle
- **`rtrim`** - Right trim
- **`ltrim`** - Left trim
- **`trim`** - Trim both sides
- **`rrot`** - Right rotation
- **`lrot`** - Left rotation
- **`rpad`** - Right padding
- **`lpad`** - Left padding
- **`reverse`** - Reverse string
- **`replace`** - Replace text
- **`grep`** - Pattern matching
- **`split`** - Split string
- **`join`** - Join strings
- **`lower`** - Convert to lowercase
- **`upper`** - Convert to uppercase

### Array/Matrix Operations

#### Array Manipulation
- **`shape`** - Get shape
- **`reshape`** - Reshape array
- **`dot`** - Dot product
- **`identity`** - Identity matrix
- **`diagonal`** - Diagonal operations
- **`triu`** - Upper triangular
- **`tril`** - Lower triangular
- **`eigh`** - Eigenvalues/eigenvectors
- **`sum`** - Sum elements
- **`mean`** - Mean value

### Comparison Operations

#### Relational Operators
- **`eq`** - Equal
- **`neq`** - Not equal
- **`gteq`** - Greater than or equal
- **`gt`** - Greater than
- **`lteq`** - Less than or equal
- **`lt`** - Less than
- **`cmp`** - Compare

### Time Operations

#### Time Management
- **`utc`** - UTC time
- **`tz`** - Timezone operations

### File System Operations

#### File Management
- **`file_table`** - File table operations
- **`file_pwd`** - Print working directory
- **`file_cd`** - Change directory
- **`file_phd`** - Push directory
- **`file_chd`** - Change home directory
- **`file_ls`** - List files
- **`file_mk`** - Make directory/file
- **`file_rm`** - Remove file/directory
- **`file_mkfield`** - Make field
- **`file_rmfield`** - Remove field
- **`file_info`** - File information
- **`file_set`** - Set file properties
- **`file_get`** - Get file properties
- **`file_split`** - Split files
- **`file_debug`** - File debugging

### Networking Operations

#### Network Management
- **`net_mac`** - MAC address operations
- **`net_interfaces`** - Network interfaces
- **`net_connect`** - Network connection
- **`net_bind`** - Bind to address
- **`net_listen`** - Listen for connections
- **`net_onlisten`** - On listen callback
- **`net_disconnect`** - Disconnect
- **`net_proxy`** - Proxy operations
- **`net_certificate`** - Certificate operations
- **`net_private`** - Private key operations
- **`net_trusted`** - Trusted certificate operations
- **`net_verify`** - Certificate verification
- **`net_chain`** - Certificate chain
- **`net_host`** - Host operations
- **`net_send`** - Send data
- **`net_receive`** - Receive data
- **`net_pending`** - Pending operations
- **`net_onreceive`** - On receive callback

#### HTTP Operations
- **`http_read`** - HTTP read
- **`http_send`** - HTTP send
- **`http_message`** - HTTP message handling

### Threading Operations

#### Thread Management
- **`thread_trylock`** - Try to acquire lock
- **`thread_lock`** - Acquire lock
- **`thread_unlock`** - Release lock
- **`thread_wait`** - Wait for condition
- **`thread_signal`** - Signal condition
- **`thread_waiting`** - Check if waiting
- **`thread_start`** - Start thread
- **`thread_stop`** - Stop thread
- **`thread_started`** - Check if started
- **`thread_suspend`** - Suspend thread
- **`thread_resume`** - Resume thread
- **`thread_suspended`** - Check if suspended

### GUI Widget Operations

#### Widget Management
- **`widget_new`** - Create new widget
- **`widget_show`** - Show widget
- **`widget_hide`** - Hide widget
- **`widget_redraw`** - Redraw widget
- **`widget_resizable`** - Set resizable
- **`widget_resize`** - Resize widget
- **`widget_parent`** - Get parent widget
- **`widget_child`** - Get child widget
- **`widget_next`** - Get next widget
- **`widget_focus`** - Set focus
- **`widget_get`** - Get widget property
- **`widget_set`** - Set widget property
- **`widget_handle`** - Widget handle
- **`widget_callback`** - Widget callback
- **`widget_event_key`** - Key event handling
- **`widget_append`** - Append to widget
- **`widget_post`** - Post to widget
- **`widget_clear`** - Clear widget

### Object-Oriented Programming

#### Class and Object Management
- **`class`** - Class definition
- **`obj`** - Object operations

## Usage Examples

### Basic Function Call
```grapa
/* Call a system function directly */
result = $sys.add(5, 3);  /* Returns 8 */
```

### In BNF Grammar
```grapa
/* Function can be referenced in grammar rules */
expression = $sys.add(term, expression) | term;
```

### In Library Files
```grapa
/* Functions can be called from lib/grapa/*.grc files */
add = op(){@this.$sys.add(@var.{term1},@var.{term2});};
```

## Implementation Details

### Handler Map Structure
The `handlerMap` is an `std::unordered_map<std::string, Handler>` that maps function names to their handler methods:

```cpp
static const std::unordered_map<std::string, Handler> handlerMap = {
    { "add", &GrapaLibraryRuleEvent::HandleAdd },
    { "sub", &GrapaLibraryRuleEvent::HandleSub },
    // ... all other functions
};
```

### Dynamic Loading
When a function is called:
1. `LoadLib` looks up the function name in `handlerMap`
2. If found, returns the appropriate handler
3. The handler is cached for future use
4. The function can then be executed

### Extensibility
This architecture allows:
- Easy addition of new system functions
- Consistent interface across all function types
- Efficient function lookup and caching
- Integration with both BNF grammar and library files

## Notes

- All functions are available system-wide
- Functions can be called from any context (BNF, library files, scripts)
- The system provides comprehensive coverage of programming needs
- Functions are organized by domain for easy discovery
- Each function has a specific handler implementation in the C++ code 

## See Also
- [System Include Reference](INCLUDE_SYSTEM.md) — Details on system include files and integration 