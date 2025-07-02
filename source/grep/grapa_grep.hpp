#pragma once

#include <string>
#include <vector>

struct MatchPosition {
    size_t offset;
    size_t length;
    size_t line_number = 0;
};

/*
 Supported grep options:
   a - All-mode (match across full input string)
   b - Output byte offset with matches
   c - Count of matches (deduplicated if 'd' is also set)
   d - Deduplicate results (by line or match depending on mode)
   g - Group results per line
   i - Case-insensitive matching
   l - Line number only output (per matched line)
   n - Prefix matches with line numbers
   o - Match-only (output only matched text)
   v - Invert match (return non-matching lines or spans)
   x - Exact line match (whole line must match pattern)
   line_delim - Custom line delimiter (default: \n)
*/

/**
 * Returns all match positions from the input, with metadata.
 */
std::vector<MatchPosition> grep(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim = ""
);

/**
 * Extracts and formats matches (or matched lines) based on options.
 * Returns strings with match content, offsets, or counts as requested.
 */
std::vector<std::string> grep_extract_matches(
    const std::string& working_input,
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim = ""
);

/**
 * Utility: normalize \r\n to \n across a string view.
 */
std::string normalize_newlines(std::string_view input);
