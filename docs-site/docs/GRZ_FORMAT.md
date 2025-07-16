# GRZ Format in Grapa

## Overview

The `.grz` format is a compressed, versioned representation of Grapa execution trees. It extends the pure `$OP` type by adding version information and applying ZIP compression to reduce file size and enable version checking.

## Pure $OP Type vs .grz Format

### Pure $OP Type
The pure `$OP` type represents the core Grapa execution tree structure:

```grapa
/* Pure $OP execution tree */
@<[op,body],params>
```

**Characteristics:**
- **Direct representation** of execution trees
- **No compression** - trees can be large
- **No version information** - no way to check compatibility
- **Immediate execution** - can be executed directly
- **Human readable** - shows the complete tree structure

**Example:**
```grapa
/* Pure $OP tree */
f = op(x){x * 2 + 1};
f;
/* Result: @<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}> */
```

### .grz Format
The `.grz` format wraps the `$OP` tree in a versioned, compressed container:

```grapa
/* .grz structure */
{
    op: @<[op,body],params>,
    version: "0.0.38"
}
```

**Characteristics:**
- **Versioned** - includes Grapa version information
- **Compressed** - uses ZIP compression to reduce size
- **Binary format** - not human readable
- **Compatibility checking** - can verify version compatibility
- **Efficient storage** - significantly smaller than pure trees

## Creating .grz Files

### Using $sys().compile()
Creates a `.grz` object from a script string:

```grapa
/* Compile script to .grz format */
compiled = $sys().compile("a = 5 + 3; a.echo();");
compiled;
/* Result: Binary .grz data with version information */
```

### Using $sys().compilef()
Compiles a `.grc` file to a `.grz` file:

```grapa
/* Compile source file to .grz file */
$sys().compilef("test/test_basic.grc", "test/test_basic.grz");
```

### Manual Creation
You can create `.grz` objects manually:

```grapa
/* Create .grz object manually */
grz_object = {
    op: op(x){x * 2 + 1},
    version: $sys().getenv($VERSION)
};

/* Apply ZIP compression */
compressed = grz_object.encode("ZIP-GRAPA");
```

## Executing .grz Files

### ⚠️ Important: Execution Methods

**The following methods do NOT work with .grz files:**
- `include "file.grz"` - This will fail (bug to be investigated)
- `$sys().eval(grz_data)` - This will fail (documentation needs updating)

**Correct method for executing .grz files:**

```grapa
/* Load and execute .grz file */
grz_data = $file().get("test/test_basic.grz");
decoded = grz_data.decode("ZIP-GRAPA");
result = decoded["op"]();
/* Result: Executes the compiled function */
```

**Alternative syntax (more concise):**
```grapa
/* One-liner execution */
$file().get("test/test_basic.grz").decode("ZIP-GRAPA")["op"]();
```

### Runtime vs Compile-time Execution

**To avoid compile-time includes (for performance testing):**

```grapa
/* Runtime execution of .grc (avoids compile-time includes) */
op()($file().get("test/run_tests.grc"))();

/* Runtime execution of .grz (pre-compiled) */
$file().get("test/run_tests.grz").decode("ZIP-GRAPA")["op"]();
```

## Reversing .grz Files

### Extracting Version Information
To check the version of a `.grz` file:

```grapa
/* Load .grz file */
grz_data = $file().get("test/test_basic.grz");

/* Decode to extract version */
decoded = grz_data.decode("ZIP-GRAPA");
version = decoded["version"];
version.echo();
/* Result: "0.0.38" */
```

### Extracting the Original $OP
To get the pure `$OP` tree from a `.grz` file:

```grapa
/* Load and decode .grz file */
grz_data = $file().get("test/test_basic.grz");
decoded = grz_data.decode("ZIP-GRAPA");

/* Extract the pure $OP tree */
pure_op = decoded["op"];
pure_op;
/* Result: @<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}> */

/* Execute the pure $OP */
pure_op(5);
/* Result: 11 */
```

## File Format Details

### .grz File Structure
The `.grz` format uses a specific binary structure:

1. **Header**: "GRZ" magic bytes + type identifier
2. **Length**: Compressed data length
3. **Compressed Data**: ZIP-compressed versioned object
4. **Type Information**: Original data type preserved

### Compression Process
```grapa
/* Internal compression process */
original = {op: op(x){x * 2}, version: "0.0.38"};
compressed = original.encode("ZIP-GRAPA");
/* Result: Binary .grz data */
```

### Decompression Process
```grapa
/* Internal decompression process */
decoded = compressed.decode("ZIP-GRAPA");
/* Result: {op: @<[op,@<mul,{@<var,{x}>,2}>],{x}>, version: "0.0.38"} */
```

## Version Compatibility

### Version Checking
You can check if a `.grz` file is compatible with your Grapa version:

```grapa
/* Check version compatibility */
grz_data = $file().get("library.grz");
decoded = grz_data.decode("ZIP-GRAPA");
current_version = $sys().getenv($VERSION);

if (decoded["version"] == current_version) {
    "Version compatible".echo();
} else {
    ("Version mismatch: " + decoded["version"] + " vs " + current_version).echo();
}
```

### Version Extraction Utility
Create a utility function to extract version information:

```grapa
/* Version extraction utility */
get_grz_version = op(filename) {
    grz_data = $file().get(filename);
    decoded = grz_data.decode("ZIP-GRAPA");
    decoded["version"];
};

/* Usage */
version = get_grz_version("test/test_basic.grz");
version.echo();
/* Result: "0.0.38" */
```

## Build Process Integration

### Automated .grz Creation
The `buildgrapalib.grc` script demonstrates automated `.grz` creation:

```grapa
/* From buildgrapalib.grc */
f = $file();
f.chd($sys().getenv($LIB));

/* Find all .grc files */
a = f.ls().reduce(op(a,b){
    if ((b.$KEY.left(1)=="$")&&(b.$KEY.right(1)=="c")) {
        a += b.$KEY.rtrim("c");
    }
}, {});

/* Compile each .grc file to .grz */
a.reduce(op(x,b,g){
    $sys().encode(op()(g.get(b+"c")), "ZIP-GRAPA").setfile(g,b+"z");
}, {}, f);
```

### Static Library Creation
The build process also creates static libraries from `.grz` files:

```grapa
/* Create static library from .grz files */
f = $file();
f.chd($sys().getenv($LIB));
x = f.ls().reduce(op(a,b,g){
    n = b.$KEY;
    a += (@n:g.get(n));
}, {}, f).encode("ZIP-GRAPA").encode("BASE64");

/* Generate C++ static library code */
y = x.split(x.len()/80);
z = y.join('",\n"');
a = 'class GrapaStaticLib {public:const static char* staticlist[];};\n' +
    'const char* GrapaStaticLib::staticlist[] = {\n"' + z + '",""\n};';
f.chd($sys().getenv($WORK));
f.set("source/grapa/GrapaStaticLib.c", a);
```

## Performance Characteristics

### File Size Comparison
- **Pure $OP**: Large, uncompressed execution trees
- **.grz format**: Significantly smaller due to ZIP compression
- **Compression ratio**: Typically 60-80% size reduction

### Execution Performance
- **Pure $OP**: Immediate execution, no decompression overhead
- **.grz format**: Minimal decompression overhead, then same execution speed
- **Runtime performance**: Identical after decompression

### Compilation Performance
- **Pure $OP**: No compilation needed
- **.grz format**: Fast loading, no parsing required
- **Development**: Use `.grc` for development, `.grz` for deployment

## Best Practices

### Development Workflow
1. **Development**: Use `.grc` files for easy debugging and modification
2. **Testing**: Use `.grz` files for faster loading during testing
3. **Production**: Use `.grz` files for optimal performance and version control

### Version Management
1. **Always include version information** in `.grz` files
2. **Check version compatibility** before loading `.grz` files
3. **Document version requirements** for libraries and applications

### File Organization
1. **Source files**: Keep `.grc` files in version control
2. **Compiled files**: Distribute `.grz` files for deployment
3. **Clear naming**: Use descriptive names for both formats

## Examples

### Complete Workflow Example
```grapa
/* 1. Create pure $OP function */
pure_func = op(x,y){x * y + 1};

/* 2. Create .grz object */
grz_object = {
    op: pure_func,
    version: $sys().getenv($VERSION)
};

/* 3. Compress to .grz format */
compressed = grz_object.encode("ZIP-GRAPA");

/* 4. Save to file */
$file().set("my_function.grz", compressed);

/* 5. Load and use later (correct method) */
loaded_data = $file().get("my_function.grz");
decoded = loaded_data.decode("ZIP-GRAPA");
extracted_func = decoded["op"];

/* 6. Execute the function */
result = extracted_func(5, 3);
result.echo();
/* Result: 16 */

/* 7. Check version */
decoded["version"].echo();
/* Result: "0.0.38" */
```

### Library Distribution Example
```grapa
/* Create library with version checking */
load_library = op(filename) {
    grz_data = $file().get(filename);
    decoded = grz_data.decode("ZIP-GRAPA");
    
    /* Check version compatibility */
    current_version = $sys().getenv($VERSION);
    if (decoded["version"] != current_version) {
        ("Warning: Library version " + decoded["version"] + 
         " may not be compatible with Grapa " + current_version).echo();
    }
    
    /* Return the library functions */
    decoded["op"];
};

/* Usage */
math_lib = load_library("lib/math_functions.grz");
result = math_lib.multiply(4, 5);
result.echo();
/* Result: 20 */
```

### Performance Testing Example
```grapa
/* Performance comparison between .grc and .grz */
"=== PERFORMANCE TEST ===\n".echo();

/* Time runtime execution of .grc */
grc_start = $TIME().utc();
op()($file().get("test/run_tests.grc"))();
grc_end = $TIME().utc();
grc_time = ((grc_end - grc_start) / 1000000).int();

/* Time execution of .grz */
grz_start = $TIME().utc();
$file().get("test/run_tests.grz").decode("ZIP-GRAPA")["op"]();
grz_end = $TIME().utc();
grz_time = ((grz_end - grz_start) / 1000000).int();

/* Compare results */
("Grc time: " + grc_time + " ms").echo();
("Grz time: " + grz_time + " ms").echo();

if (grz_time < grc_time) {
    ("Grz is " + ((grc_time - grz_time) * 100 / grc_time) + "% faster").echo();
} else {
    ("Grc is " + ((grz_time - grc_time) * 100 / grz_time) + "% faster").echo();
}
```

## Conclusion

The `.grz` format provides a robust solution for distributing Grapa code with:
- **Version control** for compatibility checking
- **Compression** for efficient storage and transmission
- **Binary format** for fast loading
- **Backward compatibility** through version extraction

**Key points:**
- Use `$file().get("file.grz").decode("ZIP-GRAPA")["op"]()` to execute `.grz` files
- Avoid `include` and `$sys().eval()` with `.grz` files (they don't work)
- Use runtime execution for performance testing to avoid compile-time includes
- Always check version compatibility before loading `.grz` files

Understanding the difference between pure `$OP` trees and `.grz` files is essential for effective Grapa development, deployment, and library distribution. 