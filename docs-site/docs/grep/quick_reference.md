
> **Note:** Grapa grep returns results as a structured array (list of strings), not a concatenated string. This allows for more precise, programmatic handling and affects edge cases (like empty pattern matches and invert matches). This is a deliberate design choice for language integration and should be considered when writing tests or integrating with other tools.

## Quick Reference: Unicode Case Folding

### Basic Case-Insensitive Matching
```grapa
"Hello WORLD".grep("world", "i")  // Case-insensitive matching
```

### Turkish I Characters
```grapa
"İstanbul ıstanbul".grep("istanbul", "i")  // İ, ı match 'i'
"İstanbul ıstanbul".grep("İSTANBUL", "i")  // İ, ı match 'I'
"İ".grep("i", "i")  // İ matches 'i'
"ı".grep("i", "i")  // ı matches 'i'
"I".grep("ı", "i")  // I matches 'ı'
"i".grep("ı", "i")  // i matches 'ı'
```

### German Sharp S (ß)
```grapa
"Straße".grep("strasse", "i")  // ß matches 'ss'
"STRASSE".grep("straße", "i")  // 'ss' matches 'ß'
```

### Greek Final Sigma (ς)
```grapa
"γράμμα".grep("ΓΡΑΜΜΑ", "i")  // ς matches Σ
"ΓΡΑΜΜΑ".grep("γράμμα", "i")  // Σ matches ς
```

### Common Accented Characters
```grapa
"café résumé naïve".grep("CAFE RESUME NAIVE", "i")  // é, é, ï fold to e, e, i
"CAFÉ RÉSUMÉ NAÏVE".grep("cafe resume naive", "i")  // E, E, I fold to e, e, i
```

### Case Folding with Other Options
```grapa
"İstanbul".grep("istanbul", "wi")   // With word boundaries
"İstanbul".grep("istanbul", "oi")   // Match-only output
"İstanbul".grep("istanbul", "iN")   // With normalization
"İstanbul".grep("istanbul", "id")   // With diacritic-insensitive
```

### Implementation Notes
- **Composed Forms**: Produces composed forms (e.g., `i` + combining dot above) for better regex compatibility
- **Locale Independent**: Follows Unicode standards, not locale-specific rules
- **Optimized**: Uses lookup table for common Unicode characters
- **Bidirectional**: Applied to both input text and search pattern 