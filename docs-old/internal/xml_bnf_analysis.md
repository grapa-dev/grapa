# Grapa XML BNF Analysis (Internal)

## 1. Supported XML Syntax (as of current $grapa.grc)
- Elements with opening/closing tags: `<tag>...</tag>`
- Nested elements
- Attributes on elements: `<tag attr="value">...</tag>`
- Self-closing tags: `<tag/>`
- Text content within elements
- Basic entity handling (if present in BNF)

## 2. Current Limitations
- No support for XML comments (`<!-- ... -->`)
- No support for CDATA sections (`<![CDATA[ ... ]]>`)
- No support for processing instructions (`<?xml ... ?>`)
- No support for DTDs or internal/external entities
- Limited or no support for XML namespaces (`xmlns`, `prefix:tag`)
- No support for mixed content (text interleaved with elements) beyond basic cases
- No support for whitespace normalization or preservation rules
- No support for advanced entity resolution

## 3. Recommended BNF Enhancements for Full XML Support
- Add rules for XML comments: `<!-- ... -->`
- Add rules for CDATA sections: `<![CDATA[ ... ]]>`
- Add rules for processing instructions: `<? ... ?>`
- Add rules for DTDs and entity declarations
- Add support for XML namespaces (attributes and prefixed tags)
- Enhance attribute value parsing to support all valid XML quoting/escaping
- Add support for mixed content (text + elements in any order)
- Add whitespace handling rules (preserve/normalize as per XML spec)
- Add entity reference resolution (e.g., `&amp;`, `&lt;`, custom entities)

## 4. Notes
- The above is based on a review of the first 250 lines of $grapa.grc. For a full implementation, review the complete BNF and test with a wide range of XML samples.
- These recommendations are for internal planning and are not user-facing or discoverable in the main documentation. 