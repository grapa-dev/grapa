
/*
## Grapa Grep Engine: Parity and Testing Notes

- The Grapa grep engine achieves full ripgrep parity for all supported features, including Unicode, custom delimiters, and advanced PCRE2 options.
- All results are returned as arrays for consistency and integration with programming languages.
- Pathological and rare option matrix tests are critical for catching subtle bugs and must be maintained as new features are added.
- See `docs/TESTING.md` and `docs/GREP.md` for details on test coverage and maintenance best practices.
*/ 