# GRZ Format Specification

## Overview

GRZ (Grapa Compiled) is the binary format for compiled Grapa scripts. GRZ files are created by compiling `.grc` source files and provide faster loading and execution compared to parsing source code.

## File Extension

- **Source files**: `.grc` (Grapa source code)
- **Compiled files**: `.grz` (Grapa compiled binary)

## Format Details

GRZ files use the **ZIP-GRAPA** encoding format, which compresses and encodes Grapa scripts for efficient storage and loading. The format includes:

- **Compiled script data**: The parsed and compiled Grapa code
- **Version information**: Grapa version used for compilation
- **Compression**: ZIP-based compression for reduced file size

## Creating GRZ Files

### Using Grapa Compiler

```grapa
/* Compile a .grc file to .grz using compilef */
$sys().compilef("my_script.grc", "my_script.grz");

/* Alternative: compile from string and save */
compiled = $sys().compile("my_script.grc");
$file().set("my_script.grz", compiled);
```

### Command Line

```bash
# Windows
.\grapa.exe -q -ccmd "$sys().compilef('my_script.grc', 'my_script.grz')"

# Linux/Mac
./grapa -q -ccmd "$sys().compilef('my_script.grc', 'my_script.grz')"
```

## Running GRZ Files

GRZ files can be executed directly by Grapa:

```bash
# Windows
.\grapa.exe my_script.grz

# Linux/Mac
./grapa my_script.grz
```

## Benefits

- **Faster loading**: No parsing required
- **Smaller file size**: Compressed format
- **Version compatibility**: Includes version information
- **Distribution**: Easy to distribute compiled scripts

## Implementation

The GRZ format is implemented in the `$sys().compilef()` function:

```grapa
compilef = op(name1,name2){
    $local.r={
        op:op()($file().get(@name1)),
        version:$sys().getenv($VERSION)
    }; 
    if (r.op.type()!="$ERR") 
        r.encode("ZIP-GRAPA").setfile($file(),@name2); 
    else 
        r.op;
};
```

This function:
1. Reads the source `.grc` file
2. Compiles it to bytecode
3. Adds version information
4. Encodes using ZIP-GRAPA format
5. Saves to the target `.grz` file

## Decoding GRZ Files

To reverse the process and extract information from a GRZ file:

```grapa
/* Read and decode a GRZ file */
grz_data = $file().get("my_script.grz");
decoded = grz_data.decode("ZIP-GRAPA");

/* Extract components from the decoded array */
compiled_script = decoded.op;      /* The compiled script */
version_info = decoded.version;    /* Grapa version used */

/* Display information */
echo("Compiled script:", compiled_script);
echo("Version:", version_info);
```

### Command Line Decoding

```bash
# Windows
.\grapa.exe -q -ccmd "grz=$file().get('my_script.grz'); decoded=grz.decode('ZIP-GRAPA'); echo('Script:',decoded.op); echo('Version:',decoded.version)"

# Linux/Mac
./grapa -q -ccmd "grz=$file().get('my_script.grz'); decoded=grz.decode('ZIP-GRAPA'); echo('Script:',decoded.op); echo('Version:',decoded.version)"
```

## File Structure

A decoded GRZ file contains an array with:
- **`op`**: The compiled script bytecode
- **`version`**: The Grapa version used for compilation 