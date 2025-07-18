
## Structured Array Output vs. Text Output

Unlike traditional tools like grep or ripgrep, which output a stream of matching lines as plain text, **Grapa grep** returns results as a structured array (list of strings). This design allows for:

- **Precise programmatic handling:** Each match is a distinct element in the array, making it easy to manipulate, filter, or process results in scripts.
- **Accurate edge case representation:** For example, an empty pattern will return an array of empty strings (one per line), and an invert match will return an empty array if no lines qualify. This is more explicit than the plain text output of grep/ripgrep.
- **Test and integration clarity:** When writing tests or integrating Grapa grep with other tools, remember that the result is an array, not a concatenated string. This affects how you check for 'no matches' (empty array) or multiple empty matches (array of empty strings).

This is a deliberate design choice to make Grapa grep more useful and predictable in a programming language context, while still matching the inclusion/exclusion logic of grep/ripgrep for which lines are included in the result. 