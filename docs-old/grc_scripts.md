# Writing and Running Grapa Scripts

## Syntax Reminders
- Every statement and every block (including after closing braces) must end with a semicolon (`;`).
- Use block comments (`/* ... */`), not line comments (`// ...`).
- To append to arrays, use the `+=` operator (not `.push()` or `.append()`).
- See [Syntax Quick Reference](syntax/basic_syntax.md) for more.

---

## Writing .grc Files
- Use `.grc` files for Grapa scripts and tests.
- End every statement and block with a semicolon.
- Use block comments for documentation and explanations.
- Use `.echo()` for output.
- Use `while` loops (not `for`).
- Access arrays/lists with `[index]`, objects with `.get("key")`.
- See [Examples](EXAMPLES.md) for idiomatic code patterns.

### Example .grc File
```grapa
/* Example Grapa script */
input = "Hello world\nGoodbye world\nHello again";
matches = input.grep("Hello");
matches.echo();
```

---

## Running .grc Files
- Use the `-cfile` option to run a Grapa script file:
  - Windows: `.\grapa.exe -q -cfile "my_script.grc"`
  - Linux/Mac: `./grapa -q -cfile "my_script.grc"`
- Use `-ccmd` for short, inline commands (not for files).
- Always use `-cfile` for multi-line scripts and tests.

---

## Creating and Using .grz Files
- `.grz` files are compiled Grapa scripts for faster execution.
- To create a `.grz` file, use the Grapa compiler or the `$sys().compile()` function:
  ```grapa
  compiled = $sys().compile("my_script.grc");
  $file().set("my_script.grz", compiled);
  ```
- To run a `.grz` file:
  - Windows: `.\grapa.exe -q -cfile "my_script.grz"`
  - Linux/Mac: `./grapa -q -cfile "my_script.grz"`
- `.grz` files are portable and can be distributed for faster loading and execution.
- See [GRZ Format Specification](GRZ_FORMAT.md) for technical details.

---

## Best Practices
- Validate your `.grc` scripts with known-good examples before running in production.
- Use `.grz` files for deployment or performance-critical scenarios.
- Use [Testing](TESTING.md) to ensure your scripts behave as expected.

---

## Related Topics
- [Syntax Quick Reference](syntax/basic_syntax.md)
- [Examples](EXAMPLES.md)
- [Testing](TESTING.md)
- [GRZ Format Specification](GRZ_FORMAT.md) 