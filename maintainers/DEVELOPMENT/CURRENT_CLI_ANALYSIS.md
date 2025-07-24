# Current CLI Options Analysis

**Status:** Reference – Used for CLI redesign planning.
**Created:** January 2025  
**Purpose:** Comprehensive analysis of current CLI options before redesign

## Current CLI Options Overview

### Information Options
| Option | Purpose | Implementation | Standard? | Keep? |
|--------|---------|----------------|-----------|-------|
| `-h, --help` | Show help message | Sets `showHelp = true`, `needExit = true` | ✅ Yes | ✅ Yes |
| `-v, --version` | Show version | Sets `showVersion = true`, `needExit = true` | ✅ Yes | ✅ Yes |
| `-q, --quiet` | Suppress header | Sets `suppressHeader = true` | ✅ Yes | ✅ Yes |

### Mode Options
| Option | Purpose | Implementation | Standard? | Keep? |
|--------|---------|----------------|-----------|-------|
| `-i, --interactive` | Interactive mode | Sets `interactiveMode = true`, `showConsole = true` | ✅ Yes | ✅ Yes |
| `-c, --console` | Show console | Sets `showConsole = true` | ❌ No (conflicts with `-c` for command) | ❌ Remove |

### Environment Options
| Option | Purpose | Implementation | Standard? | Keep? |
|--------|---------|----------------|-----------|-------|
| `-e, --env` | Show environment details | Sets `showEnv = true` | ❌ No (conflicts with `-e` for eval) | ❌ Remove |

### Execution Options
| Option | Purpose | Implementation | Standard? | Keep? |
|--------|---------|----------------|-----------|-------|
| `-ccmd script` | Execute command/script | Reads next arg into `runStr`, sets `needExit = true` | ❌ No | 🔄 Replace with `-c` |
| `-cfile file` | Execute file | Reads file content into `runStr`, sets `needExit = true` | ❌ No | 🔄 Replace with `-f` |
| `-ccin` | Execute from stdin | Reads stdin into `runStr`, sets `needExit = true` | ❌ No | 🔄 Replace with `-s` |
| `-argcin` | Store stdin in `$ARGCIN` | Reads stdin into `gSystem->mArgcin` | ❌ No | 🔄 Replace with `-S` |

### GUI Options
| Option | Purpose | Implementation | Standard? | Keep? |
|--------|---------|----------------|-----------|-------|
| `-w` | Open editor | Sets `inStr = "widget.grz"`, `showWidget = true` | ❌ No | ❌ Remove (move to GUI tool) |
| `-wfile file` | Open editor with file | Sets `inStr` to file, `showWidget = true` | ❌ No | ❌ Remove (move to GUI tool) |

### Utility Options
| Option | Purpose | Implementation | Standard? | Keep? |
|--------|---------|----------------|-----------|-------|
| `-argv` | Reset argument list | Deletes args up to current position | ❌ No | ❌ Remove (internal use) |
| `-` | Force non-exit | Sets `needExit = false` | ❌ No | ❌ Remove (internal use) |

## Detailed Analysis

### Information Options

#### `-h, --help`
**Implementation:**
```cpp
if ((e->mValue.Cmp("-h") == 0) || (e->mValue.Cmp("--help") == 0))
{
    showHelp = true;
    needExit = true;
}
```
**Purpose:** Standard help display
**Assessment:** ✅ **KEEP** - Standard convention, works well

#### `-v, --version`
**Implementation:**
```cpp
else if ((e->mValue.Cmp("-v") == 0) || (e->mValue.Cmp("--version") == 0))
{
    showVersion = true;
    needExit = true;
}
```
**Purpose:** Standard version display
**Assessment:** ✅ **KEEP** - Standard convention, works well

#### `-q, --quiet`
**Implementation:**
```cpp
else if ((e->mValue.Cmp("-q") == 0) || (e->mValue.Cmp("--quiet") == 0))
{
    suppressHeader = true;
}
```
**Purpose:** Suppress header output
**Assessment:** ✅ **KEEP** - Standard convention, useful for scripting

### Mode Options

#### `-i, --interactive`
**Implementation:**
```cpp
else if ((e->mValue.Cmp("-i") == 0) || (e->mValue.Cmp("--interactive") == 0))
{
    interactiveMode = true;
    showConsole = true;
}
```
**Purpose:** Force interactive mode
**Assessment:** ✅ **KEEP** - Standard convention (`python -i`, `node -i`)

#### `-c, --console`
**Implementation:**
```cpp
else if ((e->mValue.Cmp("-c") == 0) || (e->mValue.Cmp("--console") == 0))
{
    showConsole = true;
}
```
**Purpose:** Show console (but not interactive)
**Assessment:** ❌ **REMOVE** - Confuses users, conflicts with standard `-c` for command execution

### Environment Options

#### `-e, --env`
**Implementation:**
```cpp
else if ((e->mValue.Cmp("-e") == 0) || (e->mValue.Cmp("--env") == 0))
{
    showEnv = true;
}
```
**Purpose:** Show environment details
**Assessment:** ❌ **REMOVE** - Conflicts with standard `-e` for eval/expression execution

### Execution Options

#### `-ccmd script`
**Implementation:**
```cpp
else if (e->mValue.Cmp("-ccmd") == 0)
{
    e = e->Next();
    if (e)
        runStr.FROM(e->mValue);
    needExit = true;
}
```
**Purpose:** Execute command/script from argument
**Assessment:** 🔄 **REPLACE** with `-c` - Standard convention (`bash -c`, `python -c`)

#### `-cfile file`
**Implementation:**
```cpp
else if (e->mValue.Cmp("-cfile") == 0)
{
    e = e->Next();
    GrapaCHAR fn;
    if (e)
    {
        fn.FROM(e->mValue);
        GrapaFileIO fp;
        if (fp.Open((char*)fn.mBytes) == 0)
        {
            u64 sz = 0;
            fp.GetSize(sz);
            runStr.SetLength(sz);
            if (sz)
            {
                if (fp.Read(0, 0, 0, sz, runStr.mBytes))
                    runStr.SetLength(0);
            }
        }
    }
    needExit = true;
}
```
**Purpose:** Execute file content
**Assessment:** 🔄 **REPLACE** with `-f` - Standard convention (`bash -f`, `python -f`)

#### `-ccin`
**Implementation:**
```cpp
else if (e->mValue.Cmp("-ccin") == 0)
{
    needExit = true;
    char c;
    runStr.SetLength(0);
    while (std::cin >> c && !std::cin.eof())
        runStr.Append((char)c);
}
```
**Purpose:** Execute stdin content
**Assessment:** 🔄 **REPLACE** with `-s` - Standard convention for stdin execution

#### `-argcin`
**Implementation:**
```cpp
else if (e->mValue.Cmp("-argcin") == 0)
{
    needExit = true;
    char c;
    gSystem->mArgcin.SetLength(0);
    while (std::cin >> c && !std::cin.eof())
        gSystem->mArgcin.Append((char)c);
}
```
**Purpose:** Store stdin in `$ARGCIN` environment variable
**Usage:** `echo "data" | grapa -S -c "$sys().getenv('$ARGCIN').echo()"`
**Assessment:** 🔄 **REPLACE** with `-S` or `--stdin-var` - Useful feature but non-standard name

### GUI Options

#### `-w`
**Implementation:**
```cpp
else if ((e->mValue.Cmp("-w") == 0) || (e->mValue.Cmp("--widget") == 0))
{
    inStr.FROM("widget.grz");
    showWidget = true;
}
```
**Purpose:** Open GUI editor
**Assessment:** ❌ **REMOVE** - Should be in separate GUI tool, not CLI

#### `-wfile file`
**Implementation:**
```cpp
else if (e->mValue.Cmp("-wfile") == 0)
{
    e = e->Next();
    if (e)
        inStr.FROM(e->mValue);
    if (inStr.mLength==0)
        inStr.FROM("widget.grz");
    showWidget = true;
}
```
**Purpose:** Open GUI editor with file
**Assessment:** ❌ **REMOVE** - Should be in separate GUI tool, not CLI

### Utility Options

#### `-argv`
**Implementation:**
```cpp
else if (e->mValue.Cmp("-argv") == 0)
{
    e = e->Next();
    while (gSystem->mArgv->Head() != e)
    {
        delete gSystem->mArgv->PopHead();
    }
    break;
}
```
**Purpose:** Reset argument list (internal use)
**Assessment:** ❌ **REMOVE** - Internal implementation detail, not user-facing

#### `-`
**Implementation:**
```cpp
else if (e->mValue.Cmp("-") == 0)
{
    needExit = false;
}
```
**Purpose:** Force non-exit mode (internal use)
**Assessment:** ❌ **REMOVE** - Internal implementation detail, not user-facing

## Direct Execution Features (Already Implemented)

### Quoted Command Detection
**Implementation:**
```cpp
if (e->mValue.mLength >= 2 && 
    ((e->mValue.mBytes[0] == '"' && e->mValue.mBytes[e->mValue.mLength - 1] == '"') ||
     (e->mValue.mBytes[0] == '\'' && e->mValue.mBytes[e->mValue.mLength - 1] == '\'')))
{
    GrapaCHAR cmd;
    cmd.FROM((char*)&e->mValue.mBytes[1], e->mValue.mLength - 2);
    runStr.FROM(cmd);
    needExit = true;
}
```
**Purpose:** Execute quoted commands directly
**Assessment:** ✅ **KEEP** - Excellent feature, makes CLI intuitive

### Method Call Detection
**Implementation:**
```cpp
else if (e->mValue.mLength > 0 && 
    (strstr((char*)e->mValue.mBytes, ".echo()") != NULL ||
     strstr((char*)e->mValue.mBytes, ".print()") != NULL ||
     strstr((char*)e->mValue.mBytes, ".len()") != NULL ||
     strstr((char*)e->mValue.mBytes, ".get(") != NULL))
{
    runStr.FROM(e->mValue);
    needExit = true;
}
```
**Purpose:** Execute method calls directly
**Assessment:** ✅ **KEEP** - Excellent feature, makes CLI intuitive

### Script File Detection
**Implementation:**
```cpp
else if (e->mValue.mLength > 4 && 
    (strstr((char*)e->mValue.mBytes, ".grc") != NULL ||
     strstr((char*)e->mValue.mBytes, ".grz") != NULL))
{
    inStr.FROM(e->mValue);
    needExit = true;
}
```
**Purpose:** Execute script files directly
**Assessment:** ✅ **KEEP** - Excellent feature, makes CLI intuitive

### Pipe Input Detection
**Implementation:**
```cpp
if (isPipeInput && runStr.mLength == 0 && !interactiveMode && !showWidget)
{
    char c;
    runStr.SetLength(0);
    while (std::cin >> c && !std::cin.eof())
        runStr.Append((char)c);
    if (runStr.mLength > 0)
    {
        needExit = true;
    }
}
```
**Purpose:** Auto-detect pipe input
**Assessment:** ✅ **KEEP** - Excellent feature, follows Unix conventions

## Missing Standard Options

### Output Control
- `-o, --output <file>` - Redirect output to file
- `-a, --append` - Append to file
- `--verbose` - Verbose output (different from `-v` version)

### Debugging
- `-d, --debug` - Debug mode
- `--trace` - Execution trace
- `--profile` - Performance profiling

### Performance
- `-j, --jobs <N>` - Parallel workers
- `--no-parallel` - Disable parallelism

### Environment
- `-E, --env <VAR=value>` - Set environment variable
- `--env-file <file>` - Load environment file

### Error Handling
- `--strict` - Strict mode
- `--continue` - Continue on errors
- `--max-errors <N>` - Stop after N errors

## Recommendations

### Keep (Standard Conventions)
- `-h, --help` ✅
- `-v, --version` ✅
- `-q, --quiet` ✅
- `-i, --interactive` ✅
- Direct execution features ✅
- Pipe input detection ✅

### Replace (Standard Names)
- `-ccmd` → `-c` (command execution)
- `-cfile` → `-f` (file execution)
- `-ccin` → `-s` (stdin execution)
- `-argcin` → `-S` (stdin to variable)

### Remove (Conflicts/Non-Standard)
- `-c, --console` (conflicts with `-c` for command)
- `-e, --env` (conflicts with `-e` for eval)
- `-w, -wfile` (GUI features)
- `-argv, -` (internal use)

### Add (Missing Standard)
- `-o, --output` (output redirection)
- `-a, --append` (append output)
- `-d, --debug` (debug mode)
- `--verbose` (verbose output)
- `-j, --jobs` (parallel workers)
- `-E, --env` (environment variables)

---

**Next:** ✅ Phase 1 completed! This analysis was used to implement the CLI redesign. 