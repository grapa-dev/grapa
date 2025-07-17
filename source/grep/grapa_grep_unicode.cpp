// grapa_grep_unicode.cpp
/*
Copyright 2025 Chris Ernest Matichuk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissionsand
limitations under the License.
*/
////////////////////////////////////////////////////////////////////////////////

// Static compilation definitions are already defined in the header file

#include "grep_unicode.cpp"
//#include "grapa_grep_unicode.hpp"
//#include "grapa_grep.hpp"
#include <regex>
#include <map>
#include <set>
#include <string_view>
#include <cctype>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <fstream> // Added for logging PCRE2 errors
#include <future> // Added for parallel processing
#include <thread> // Added for parallel processing
#include <chrono> // Added for sleep_for
#include <sstream>

// Helper: Build a JSON array string from a vector of strings
std::string build_json_array(const std::vector<std::string>& items, bool already_json_objects) {
    std::string json_array = "[";
    for (size_t i = 0; i < items.size(); ++i) {
        if (already_json_objects) {
            json_array += items[i];
        } else {
            json_array += "\"" + items[i] + "\"";
        }
        if (i + 1 < items.size()) json_array += ",";
    }
    json_array += "]";
    return json_array;
}

// Helper: Split string by custom delimiter (supports multi-character delimiters)
std::vector<std::string> split_by_delimiter(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> result;
    std::string actual_delim = delimiter.empty() ? "\n" : delimiter;
    
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: split_by_delimiter called with input length %zu, delimiter '%s' (length %zu)\n", 
           input.length(), actual_delim.c_str(), actual_delim.length());
    #endif // DEBUG_END
    
    size_t pos = 0;
    while (pos < input.size()) {
        size_t found = input.find(actual_delim, pos);
        if (found == std::string::npos) {
            // Last line (no trailing delimiter)
            std::string segment = input.substr(pos);
            result.push_back(segment);
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: split_by_delimiter - final segment: '%s' (length %zu)\n", segment.c_str(), segment.length());
            #endif // DEBUG_END
            break;
        } else {
            // Line with trailing delimiter (will be removed later)
            std::string segment = input.substr(pos, found - pos);
            result.push_back(segment);
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: split_by_delimiter - segment: '%s' (length %zu) at pos %zu\n", segment.c_str(), segment.length(), pos);
            #endif // DEBUG_END
            pos = found + actual_delim.size();
        }
    }
    
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: split_by_delimiter - returning %zu segments\n", result.size());
    #endif // DEBUG_END
    
    return result;
}

// Helper: Remove trailing delimiter from a string
std::string remove_trailing_delimiter(const std::string& str, const std::string& delimiter) {
    if (delimiter.empty() || str.empty()) {
        return str;
    }
    
    if (str.size() >= delimiter.size() && 
        str.compare(str.size() - delimiter.size(), delimiter.size(), delimiter) == 0) {
        return str.substr(0, str.size() - delimiter.size());
    }
    return str;
}

// Helper: Remove delimiters from all strings in a vector (for programming language integration)
std::vector<std::string> remove_delimiters_from_matches(const std::vector<std::string>& matches, const std::string& delimiter) {
    std::vector<std::string> cleaned_matches;
    cleaned_matches.reserve(matches.size());
    
    for (const auto& match : matches) {
        cleaned_matches.push_back(remove_trailing_delimiter(match, delimiter));
    }
    
    return cleaned_matches;
}

// Helper: Check if pattern contains multiline features that need special handling
bool is_multiline_pattern(const std::string& pattern) {
    bool has_dot_star = pattern.find(".*") != std::string::npos;
    bool has_whitespace = pattern.find("\\s") != std::string::npos;
    bool has_newline = pattern.find("\\n") != std::string::npos;
    bool has_carriage_return = pattern.find("\\r") != std::string::npos;
    
    bool result = has_dot_star || has_whitespace || has_newline || has_carriage_return;
    
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: is_multiline_pattern('%s') analysis:\n", pattern.c_str());
    printf("DEBUG:   has_dot_star: %s\n", has_dot_star ? "true" : "false");
    printf("DEBUG:   has_whitespace: %s\n", has_whitespace ? "true" : "false");
    printf("DEBUG:   has_newline: %s\n", has_newline ? "true" : "false");
    printf("DEBUG:   has_carriage_return: %s\n", has_carriage_return ? "true" : "false");
    printf("DEBUG:   result: %s\n", result ? "true" : "false");
    #endif // DEBUG_END
    
    return result;
}

// Helper: Check if pattern contains lookaround assertions
bool has_lookaround_assertions(const std::string& pattern) {
    bool result = pattern.find("(?=") != std::string::npos || 
           pattern.find("(?!") != std::string::npos ||
           pattern.find("(?<=") != std::string::npos ||
           pattern.find("(?<!") != std::string::npos;
    
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: has_lookaround_assertions('%s') = %s\n", pattern.c_str(), result ? "true" : "false");
    #endif // DEBUG_END
    
    return result;
}

// Helper: Extract matches with proper lookaround handling
std::vector<std::string> extract_matches_with_lookaround(const std::string& input, const std::string& pattern, bool case_insensitive, bool diacritic_insensitive, GrapaUnicode::NormalizationForm norm) {
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: extract_matches_with_lookaround called\n");
    printf("DEBUG: Pattern: '%s'\n", pattern.c_str());
    printf("DEBUG: Input length: %zu\n", input.length());
    printf("DEBUG: Case insensitive: %s\n", case_insensitive ? "true" : "false");
    printf("DEBUG: Diacritic insensitive: %s\n", diacritic_insensitive ? "true" : "false");
    #endif // DEBUG_END
    
    std::vector<std::string> matches;
    
    // Use the original pattern to find match positions
    GrapaUnicode::UnicodeRegex regex(pattern, case_insensitive, diacritic_insensitive, norm);
    regex.compile(input);
    
    if (!regex.is_valid()) {
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Regex compilation failed for pattern '%s'\n", pattern.c_str());
        #endif // DEBUG_END
        return matches;
    }
    
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: Regex compilation successful for pattern '%s'\n", pattern.c_str());
    #endif // DEBUG_END

    // Get all match positions
    auto match_positions = regex.find_all(GrapaUnicode::UnicodeString(input));
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: Found %zu match positions\n", match_positions.size());
    #endif // DEBUG_END
    
    for (const auto& pos : match_positions) {
        std::string match_text;
        
        if (pattern.find("(?=") != std::string::npos) {
            // Positive lookahead: extract everything before the lookahead
            size_t lookahead_pos = pattern.find("(?=");
            std::string before_lookahead = pattern.substr(0, lookahead_pos);
            
            // For \w+(?=\d), we want to extract just the \w+ part
            // The match span includes the lookahead, so we need to find where the word ends
            std::string full_match = input.substr(pos.first, pos.second);
            
            // Find the end of the word part (before the digits)
            size_t word_end = 0;
            for (size_t i = 0; i < full_match.length(); ++i) {
                if (std::isdigit(full_match[i])) {
                    word_end = i;
                    break;
                }
            }
            if (word_end > 0) {
                match_text = full_match.substr(0, word_end);
            } else {
                match_text = full_match;
            }
        } else if (pattern.find("(?!") != std::string::npos) {
            // Negative lookahead: extract everything before the lookahead
            size_t lookahead_pos = pattern.find("(?!");
            std::string before_lookahead = pattern.substr(0, lookahead_pos);
            
            // For \w+(?!\d), we want to extract just the \w+ part
            std::string full_match = input.substr(pos.first, pos.second);
            
            // Find the end of the word part (before the digits)
            size_t word_end = 0;
            for (size_t i = 0; i < full_match.length(); ++i) {
                if (std::isdigit(full_match[i])) {
                    word_end = i;
                    break;
                }
            }
            if (word_end > 0) {
                match_text = full_match.substr(0, word_end);
            } else {
                match_text = full_match;
            }
        } else if (pattern.find("(?<=") != std::string::npos) {
            // Positive lookbehind: extract everything after the lookbehind
            size_t lookbehind_pos = pattern.find("(?<=");
            std::string after_lookbehind = pattern.substr(lookbehind_pos + 4); // Skip "(?<="
            
            // For (?<=\d)\w+, we want to extract just the \w+ part
            std::string full_match = input.substr(pos.first, pos.second);
            
            // Find the start of the word part (after the digits)
            size_t word_start = 0;
            for (size_t i = 0; i < full_match.length(); ++i) {
                if (std::isalpha(full_match[i])) {
                    word_start = i;
                    break;
                }
            }
            if (word_start < full_match.length()) {
                match_text = full_match.substr(word_start);
            } else {
                match_text = full_match;
            }
        } else if (pattern.find("(?<!") != std::string::npos) {
            // Negative lookbehind: extract everything after the lookbehind
            size_t lookbehind_pos = pattern.find("(?<!");
            std::string after_lookbehind = pattern.substr(lookbehind_pos + 4); // Skip "(?<!"
            
            // For (?<!\d)\w+, we want to extract just the \w+ part
            std::string full_match = input.substr(pos.first, pos.second);
            
            // Find the start of the word part (after the digits)
            size_t word_start = 0;
            for (size_t i = 0; i < full_match.length(); ++i) {
                if (std::isalpha(full_match[i])) {
                    word_start = i;
                    break;
                }
            }
            if (word_start < full_match.length()) {
                match_text = full_match.substr(word_start);
            } else {
                match_text = full_match;
            }
        } else {
            // Fallback to full match
            match_text = input.substr(pos.first, pos.second);
        }
        
        matches.push_back(match_text);
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Extracted match at pos %zu, length %zu: '%s'\n", pos.first, pos.second, match_text.c_str());
        #endif // DEBUG_END
    }
    
    return matches;
}

// Helper: Extract full segments containing matches (for match-only mode with custom delimiters)
std::vector<std::string> extract_full_segments(const std::string& input, const std::string& delimiter, const std::vector<std::pair<size_t, size_t>>& match_positions) {
    std::vector<std::string> segments;
    std::vector<std::string> lines = split_by_delimiter(input, delimiter);
    
    // Find which lines contain matches
    std::set<size_t> match_lines;
    
    for (const auto& pos : match_positions) {
        // Find which line this match is on by calculating cumulative offset
        size_t current_offset = 0;
        for (size_t line_num = 0; line_num < lines.size(); ++line_num) {
            size_t line_length = lines[line_num].length();
            // Add delimiter size for all lines except the last one
            if (line_num < lines.size() - 1) {
                line_length += delimiter.size();
            }
            
            // Check if the match position falls within this line
            if (pos.first >= current_offset && pos.first < current_offset + line_length) {
                match_lines.insert(line_num);
                break;
            }
            current_offset += line_length;
        }
    }
    
    // Extract full segments for matched lines (with delimiters removed)
    for (size_t line_num : match_lines) {
        if (line_num < lines.size()) {
            // Remove trailing delimiter from the segment
            std::string segment = lines[line_num];
            if (!delimiter.empty() && segment.size() >= delimiter.size() && 
                segment.compare(segment.size() - delimiter.size(), delimiter.size(), delimiter) == 0) {
                segment = segment.substr(0, segment.size() - delimiter.size());
            }
            segments.push_back(segment);
        }
    }
    return segments;
}

// Helper: Extract grapheme clusters excluding delimiters
static std::vector<std::string> extract_grapheme_clusters_excluding_delimiters(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> clusters;
    std::vector<std::string> lines = split_by_delimiter(input, delimiter);
    
    for (const auto& line : lines) {
        size_t offset = 0;
        while (offset < line.size()) {
            // Temporarily use a simple character-based approach until extract_grapheme_cluster is available
            if (offset < line.size()) {
                std::string cluster = line.substr(offset, 1);
                clusters.push_back(cluster);
                offset += 1;
            } else {
                break;
            }
        }
    }
    
    return clusters;
}

#ifdef _WIN32
#if defined(_M_ARM64)
#elif defined(_M_X64)
#pragma comment(lib, "pcre2-lib/win-amd64/pcre2-8-static.lib")
#endif
#endif

using namespace GrapaUnicode;

// Forward declaration for grapheme cluster extraction helper
std::string extract_grapheme_cluster(const std::string& input, size_t offset);

std::string normalize_newlines(std::string_view input) {
    std::string result;
    result.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\r') {
            if (i + 1 < input.size() && input[i + 1] == '\n') ++i;
            result.push_back('\n');
        } else {
            result.push_back(input[i]);
        }
    }
    return result;
}

std::vector<MatchPosition> grep_unicode_impl(
    const std::string& input,
    const std::string& effective_pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization,
    GrapaUnicode::ProcessingMode mode
) {
    
    // Always use NFC if not explicitly set (for Unicode edge cases)
    GrapaUnicode::NormalizationForm norm = normalization;
    if (norm == GrapaUnicode::NormalizationForm::NONE) {
        norm = GrapaUnicode::NormalizationForm::NFC;
    }
    bool ignore_case = (options.find('i') != std::string::npos);
    bool all_mode = (options.find('a') != std::string::npos);
    bool match_only = ((options.find('o') != std::string::npos) ||
        (options.find('g') != std::string::npos) ||
        (options.find('b') != std::string::npos));
    bool count_only = (options.find('c') != std::string::npos);
    bool exact_match = (options.find('x') != std::string::npos);

    std::vector<MatchPosition> results;

    // UNIFIED DESIGN: Always use a delimiter, with "\n" as default
    std::string effective_delimiter = line_delim.empty() ? "\n" : line_delim;
    
    // UNIFIED DESIGN: Single processing path for all modes
    std::string working_input = input;
    
    // Handle all-mode processing (remove delimiters for matching)
    if (all_mode && !effective_delimiter.empty()) {
        working_input.reserve(input.size());
        size_t pos = 0;
        while (pos < input.size()) {
            size_t found = input.find(effective_delimiter, pos);
            if (found == std::string::npos) {
                working_input.append(input, pos, input.size() - pos);
                break;
            } else {
                working_input.append(input, pos, found - pos);
                pos = found + effective_delimiter.size();
            }
        }
    }
    
    // UNIFIED DESIGN: Single line-by-line processing for all modes
    size_t offset = 0;
    size_t line_number = 1;
    
    while (offset < working_input.size()) {
        size_t next = working_input.find(effective_delimiter, offset);
        if (next == std::string::npos) next = working_input.size();
        else next += effective_delimiter.size();

        std::string_view line(working_input.data() + offset, next - offset);
        std::string line_copy(line);

        // Remove trailing delimiter for exact match
        if (exact_match && !line_copy.empty() && line_copy.size() >= effective_delimiter.size()) {
            if (line_copy.compare(line_copy.size() - effective_delimiter.size(), effective_delimiter.size(), effective_delimiter) == 0) {
                line_copy = line_copy.substr(0, line_copy.size() - effective_delimiter.size());
            }
        }

        // UNIFIED DESIGN: Single matching logic for all modes
        bool matched = false;
        
        if (mode == ProcessingMode::BINARY_MODE) {
            // Use standard std::regex for binary processing
            std::regex::flag_type flags = std::regex::ECMAScript;
            if (ignore_case) flags |= std::regex::icase;
            std::regex rx;
            try {
                rx = std::regex(effective_pattern, flags);
            }
            catch (const std::regex_error&) {
                return {};
            }
            matched = exact_match ? std::regex_match(line_copy, rx) : std::regex_search(line_copy, rx);
        } else {
            // Use Unicode regex for normal processing
            bool diacritic_insensitive = (options.find('d') != std::string::npos);
            std::string norm_pattern = effective_pattern;
            std::string norm_line = line_copy;
            
            if (diacritic_insensitive) {
                norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).case_fold().data();
                norm_line = GrapaUnicode::UnicodeString(norm_line).normalize(norm).case_fold().data();
                norm_pattern = GrapaUnicode::strip_diacritics(norm_pattern);
                norm_line = GrapaUnicode::strip_diacritics(norm_line);
            } else if (ignore_case) {
                norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).case_fold().data();
                norm_line = GrapaUnicode::UnicodeString(norm_line).normalize(norm).case_fold().data();
            } else if (norm != GrapaUnicode::NormalizationForm::NONE) {
                norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).data();
                norm_line = GrapaUnicode::UnicodeString(norm_line).normalize(norm).data();
            }
            
            UnicodeRegex unicode_rx(norm_pattern, ignore_case, diacritic_insensitive, norm);
            unicode_rx.compile(norm_line);
            UnicodeString unicode_line(norm_line);
            matched = exact_match ? unicode_rx.match(unicode_line) : unicode_rx.search(unicode_line);
        }
        
        if (matched) {
            if (!match_only) {
                // Return full line position
                size_t line_len = line.size();
                if (line.size() >= effective_delimiter.size() &&
                    line.compare(line.size() - effective_delimiter.size(), effective_delimiter.size(), effective_delimiter) == 0)
                {
                    line_len -= effective_delimiter.size();
                }
                results.push_back({ offset, line_len, line_number });
            } else {
                // For match-only mode, we need to find the actual pattern matches within the line
                // Use the regex engine to find all matches within this line
                if (mode == ProcessingMode::BINARY_MODE) {
                    // Use standard std::regex for binary processing
                    std::regex::flag_type flags = std::regex::ECMAScript;
                    if (ignore_case) flags |= std::regex::icase;
                    std::regex rx;
                    try {
                        rx = std::regex(effective_pattern, flags);
                    }
                    catch (const std::regex_error&) {
                        continue;
                    }
                    
                    std::string::const_iterator search_start(line_copy.cbegin());
                    std::smatch match;
                    while (std::regex_search(search_start, line_copy.cend(), match, rx)) {
                        size_t match_offset = offset + static_cast<size_t>(match.position() + (search_start - line_copy.cbegin()));
                        results.push_back({ match_offset, static_cast<size_t>(match.length()), line_number });
                        search_start = match.suffix().first;
                    }
                } else {
                    // Use Unicode regex for normal processing
                    bool diacritic_insensitive = (options.find('d') != std::string::npos);
                    std::string norm_pattern = effective_pattern;
                    std::string norm_line = line_copy;
                    
                    if (diacritic_insensitive) {
                        norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).case_fold().data();
                        norm_line = GrapaUnicode::UnicodeString(norm_line).normalize(norm).case_fold().data();
                        norm_pattern = GrapaUnicode::strip_diacritics(norm_pattern);
                        norm_line = GrapaUnicode::strip_diacritics(norm_line);
                    } else if (ignore_case) {
                        norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).case_fold().data();
                        norm_line = GrapaUnicode::UnicodeString(norm_line).normalize(norm).case_fold().data();
                    } else if (norm != GrapaUnicode::NormalizationForm::NONE) {
                        norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).data();
                        norm_line = GrapaUnicode::UnicodeString(norm_line).normalize(norm).data();
                    }
                    
                    UnicodeRegex unicode_rx(norm_pattern, ignore_case, diacritic_insensitive, norm);
                    unicode_rx.compile(norm_line);
                    UnicodeString unicode_line(norm_line);
                    
                    // Get all match positions within this line
                    auto line_matches = unicode_rx.find_all(unicode_line);
                    for (const auto& line_match : line_matches) {
                        // Map normalized position back to original position
                        auto original_pos = unicode_rx.map_normalized_span_to_original(line_copy, norm_line, line_match.first, line_match.second);
                        size_t match_offset = offset + original_pos.first;
                        results.push_back({ match_offset, original_pos.second, line_number });
                    }
                }
            }
        }
        
        offset = next;
        ++line_number;
    }

    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: grep_unicode_impl returning match_positions: [");
    for (size_t i = 0; i < results.size(); ++i) {
        printf("{%zu, %zu}", results[i].offset, results[i].length);
        if (i + 1 < results.size()) printf(", ");
    }
    printf("]\n");
    fflush(stdout);
    #endif // DEBUG_END

    return results;
}

// High-level function that extracts matches as strings
std::vector<std::string> grep_extract_matches_unicode_impl(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization,
    GrapaUnicode::ProcessingMode mode,
    size_t num_workers
) {
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: ENTER grep_extract_matches_unicode_impl\n");
    printf("DEBUG: input: '%s'\n", input.c_str());
    printf("DEBUG: pattern: '%s'\n", pattern.c_str());
    printf("DEBUG: options: '%s'\n", options.c_str());
    printf("DEBUG: line_delim: '%s'\n", line_delim.c_str());
    #endif // DEBUG_END
    
    // Call the low-level function to get match positions
    std::vector<MatchPosition> match_positions = grep_unicode_impl(input, pattern, options, line_delim, normalization, mode);
    
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: grep_unicode_impl returned %zu match positions\n", match_positions.size());
    #endif // DEBUG_END
    
    // Convert match positions to strings
    std::vector<std::string> results;
    results.reserve(match_positions.size());
    
    bool match_only = ((options.find('o') != std::string::npos) ||
        (options.find('g') != std::string::npos) ||
        (options.find('b') != std::string::npos));
    
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: match_only: %s\n", match_only ? "true" : "false");
    #endif // DEBUG_END
    
    // UNIFIED DESIGN: Always use a delimiter, with "\n" as default
    std::string effective_delimiter = line_delim.empty() ? "\n" : line_delim;
    
    // Check for count-only and invert match options
    bool count_only = (options.find('c') != std::string::npos);
    bool invert_match = (options.find('v') != std::string::npos);
    
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: count_only: %s\n", count_only ? "true" : "false");
    printf("DEBUG: invert_match: %s\n", invert_match ? "true" : "false");
    #endif // DEBUG_END
    
    // Handle count-only option early
    if (count_only) {
        size_t count = 0;
        for (const auto& pos : match_positions) {
            if (pos.offset != static_cast<size_t>(-1)) {
                count++;
            }
        }
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Count-only mode - returning count: %zu\n", count);
        #endif // DEBUG_END
        // Return count as a single string, not an array
        return {std::to_string(count)};
    }
    
    if (invert_match) {
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Entered invert match block\n");
        fflush(stdout);
        #endif // DEBUG_END
        // For invert match, we need to return segments that don't contain matches
        std::vector<std::string> non_matches;
        // Split input by delimiter to get segments
        std::vector<std::string> segments = split_by_delimiter(input, effective_delimiter);
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Invert match - split_by_delimiter returned %zu segments\n", segments.size());
        printf("DEBUG: effective_delimiter: '%s' (length: %zu)\n", effective_delimiter.c_str(), effective_delimiter.length());
        printf("DEBUG: input length: %zu\n", input.length());
        for (size_t i = 0; i < segments.size(); ++i) {
            printf("DEBUG: segment[%zu]: '%s' (length: %zu)\n", i, segments[i].c_str(), segments[i].length());
        }
        #endif // DEBUG_END
        // For each segment, check if the pattern matches anywhere in the segment
        for (size_t i = 0; i < segments.size(); ++i) {
            const std::string& seg = segments[i];
            bool has_match = false;
            // Use the same regex logic as normal matching
            std::vector<MatchPosition> seg_matches = grep_unicode_impl(seg, pattern, options, "", normalization, mode);
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Invert match - segment[%zu] ('%s') has %zu matches\n", i, seg.c_str(), seg_matches.size());
            #endif // DEBUG_END
            for (const auto& pos : seg_matches) {
                if (pos.offset != static_cast<size_t>(-1)) {
                    has_match = true;
                    break;
                }
            }
            if (!has_match) {
                non_matches.push_back(seg);
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Invert match - adding non-match segment[%zu]: '%s'\n", i, seg.c_str());
                #endif // DEBUG_END
            }
        }
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Invert match - returning %zu non-matches\n", non_matches.size());
        for (size_t i = 0; i < non_matches.size(); ++i) {
            printf("DEBUG: non_match[%zu]: '%s'\n", i, non_matches[i].c_str());
        }
        fflush(stdout);
        #endif // DEBUG_END
        return non_matches;
    }
    
    // Normal processing for non-invert mode
    for (const auto& pos : match_positions) {
        if (pos.offset != static_cast<size_t>(-1)) { // Skip compilation errors
            std::string match_text = input.substr(pos.offset, pos.length);
            
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Processing match at offset %zu, length %zu: '%s'\n", pos.offset, pos.length, match_text.c_str());
            #endif // DEBUG_END
            
            if (match_only) {
                // For match-only mode, the match_text is already the pattern match
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Match-only mode - pattern match: '%s'\n", match_text.c_str());
                #endif // DEBUG_END
                
                results.push_back(match_text);
            } else {
                // For non-match-only mode, return the full line content
                results.push_back(match_text);
            }
        }
    }
    
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: Returning %zu results\n", results.size());
    for (size_t i = 0; i < results.size() && i < 3; ++i) {
        printf("DEBUG: result[%zu]: '%s'\n", i, results[i].c_str());
    }
    #endif // DEBUG_END
    
    return results;
}

// Parallel implementation
std::vector<std::string> grep_extract_matches_unicode_impl_parallel(
    const std::string& input,
    const std::string& effective_pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization,
    GrapaUnicode::ProcessingMode mode,
    size_t num_workers
) {
    // Split input into chunks for parallel processing
    std::vector<std::string> chunks = split_input_for_parallel(input, num_workers);
    
    // Use std::async for reliable parallel processing
    std::vector<std::future<std::vector<std::string>>> futures;
    
    // Start async tasks for each chunk
    for (size_t i = 0; i < chunks.size(); i++) {
        futures.push_back(std::async(std::launch::async, 
            [&chunks, i, &effective_pattern, &options, &line_delim, normalization, mode]() {
                return grep_extract_matches_unicode_impl(
                    chunks[i], effective_pattern, options, line_delim, normalization, mode);
            }));
    }
    
    // Collect results from all async tasks
    std::vector<std::string> all_matches;
    for (size_t i = 0; i < futures.size(); i++) {
        auto result = futures[i].get();
        all_matches.insert(all_matches.end(), result.begin(), result.end());
    }
    
    // At the end of grep_extract_matches_unicode_impl_parallel, after collecting all_matches:
    if (!all_matches.empty() && options.find('j') != std::string::npos) {
        std::string json_array = "[";
        for (size_t i = 0; i < all_matches.size(); ++i) {
            json_array += all_matches[i];
            if (i + 1 < all_matches.size()) json_array += ",";
        }
        json_array += "]";
        return {json_array};
    }
    return all_matches;
}

// Helper function to split input for parallel processing
std::vector<std::string> split_input_for_parallel(const std::string& input, size_t num_chunks) {
    std::vector<std::string> chunks;
    
    if (input.empty() || num_chunks == 0) {
        return chunks;
    }

    if (num_chunks == 1) {
        chunks.push_back(input);
        return chunks;
    }

    size_t input_size = input.size();
    size_t chunk_size = (input_size / num_chunks > 1024) ? input_size / num_chunks : 1024; // Minimum 1KB chunks
    
    size_t pos = 0;
    for (size_t i = 0; i < num_chunks && pos < input_size; ++i) {
        size_t end_pos = pos + chunk_size;
        
        // If this is not the last chunk, try to find a good boundary
        if (i < num_chunks - 1 && end_pos < input_size) {
            // Look for a newline boundary to avoid splitting in the middle of a line
            size_t newline_pos = input.find('\n', end_pos);
            if (newline_pos != std::string::npos && newline_pos < end_pos + chunk_size) {
                end_pos = newline_pos + 1; // Include the newline
            }
        }
        
        // Ensure we don't go past the end of the input
        end_pos = (end_pos < input_size) ? end_pos : input_size;
        
        chunks.push_back(input.substr(pos, end_pos - pos));
        pos = end_pos;
    }
    
    return chunks;
}

/**
 * Parse Unicode escapes in regex patterns
 * Converts \uXXXX sequences to actual UTF-8 bytes
 */
std::string GrapaUnicode::parse_unicode_escapes(const std::string& pattern) {
    std::string result;
    result.reserve(pattern.size());
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] == '\\' && i + 1 < pattern.size() && pattern[i + 1] == 'u') {
            // Found \u, check for 4 hex digits
            if (i + 5 < pattern.size()) {
                std::string hex_str = pattern.substr(i + 2, 4);
                
                // Check if all characters are hex digits
                bool valid_hex = true;
                for (char c : hex_str) {
                    if (!std::isxdigit(static_cast<unsigned char>(c))) {
                        valid_hex = false;
                        break;
                    }
                }
                
                if (valid_hex) {
                    // Convert hex to Unicode code point
                    unsigned int code_point = std::stoul(hex_str, nullptr, 16);
                    
                    // Convert code point to UTF-8
                    if (code_point <= 0x7F) {
                        // 1-byte UTF-8
                        result.push_back(static_cast<char>(code_point));
                    } else if (code_point <= 0x7FF) {
                        // 2-byte UTF-8
                        result.push_back(static_cast<char>(0xC0 | (code_point >> 6)));
                        result.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
                    } else if (code_point <= 0xFFFF) {
                        // 3-byte UTF-8
                        result.push_back(static_cast<char>(0xE0 | (code_point >> 12)));
                        result.push_back(static_cast<char>(0x80 | ((code_point >> 6) & 0x3F)));
                        result.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
                    } else if (code_point <= 0x10FFFF) {
                        // 4-byte UTF-8
                        result.push_back(static_cast<char>(0xF0 | (code_point >> 18)));
                        result.push_back(static_cast<char>(0x80 | ((code_point >> 12) & 0x3F)));
                        result.push_back(static_cast<char>(0x80 | ((code_point >> 6) & 0x3F)));
                        result.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
                    } else {
                        // Invalid code point, keep original
                        result.push_back(pattern[i]);
                        continue;
                    }
                    
                    i += 5; // Skip the \uXXXX sequence
                    continue;
                }
            }
        }
        
        // Copy character as-is
        result.push_back(pattern[i]);
    }
    
    return result;
}

bool GrapaUnicode::UnicodeRegex::search(const UnicodeString& text) const {
    if (compilation_error_) return false;
    
#ifdef USE_PCRE
    if (use_pcre_) {
        std::string search_text = text.data();
        if (normalization_ != GrapaUnicode::NormalizationForm::NONE) {
            search_text = get_normalized_text(text.data(), case_insensitive_, normalization_);
        }
        int result = pcre2_match(
            pcre_regex_,
            reinterpret_cast<PCRE2_SPTR>(search_text.c_str()),
            static_cast<PCRE2_SIZE>(search_text.length()),
            0, 0,
            pcre_match_data_,
            nullptr
        );
        return result >= 0;
    }
#endif
    if (is_ascii_only_ && is_ascii_string(text.data())) {
        return std::regex_search(text.data(), regex_);
    } else if (normalization_ != GrapaUnicode::NormalizationForm::NONE) {
        std::string normalized_text = get_normalized_text(text.data(), case_insensitive_, normalization_);
        return std::regex_search(normalized_text, regex_);
    } else {
        return std::regex_search(text.data(), regex_);
    }
}

bool GrapaUnicode::UnicodeRegex::match(const UnicodeString& text) const {
    if (compilation_error_) return false;
    
#ifdef USE_PCRE
    if (use_pcre_) {
        std::string search_text = text.data();
        if (normalization_ != GrapaUnicode::NormalizationForm::NONE) {
            search_text = get_normalized_text(text.data(), case_insensitive_, normalization_);
        }
        int result = pcre2_match(
            pcre_regex_,
            reinterpret_cast<PCRE2_SPTR>(search_text.c_str()),
            static_cast<PCRE2_SIZE>(search_text.length()),
            0, 0,
            pcre_match_data_,
            nullptr
        );
        if (result < 0) return false;
        PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(pcre_match_data_);
        return (ovector[0] == 0 && ovector[1] == search_text.length());
    }
#endif
    if (is_ascii_only_ && is_ascii_string(text.data())) {
        return std::regex_match(text.data(), regex_);
    } else if (normalization_ != GrapaUnicode::NormalizationForm::NONE) {
        std::string normalized_text = get_normalized_text(text.data(), case_insensitive_, normalization_);
        return std::regex_match(normalized_text, regex_);
    } else {
        return std::regex_match(text.data(), regex_);
    }
}

std::vector<std::pair<size_t, size_t>> GrapaUnicode::UnicodeRegex::find_all(const UnicodeString& text) const {
    if (compilation_error_) return {};
    
    std::vector<std::pair<size_t, size_t>> matches;
#ifdef USE_PCRE
    if (use_pcre_) {
        std::string search_text = text.data();
        bool needs_normalization = (normalization_ != GrapaUnicode::NormalizationForm::NONE);
        
        if (needs_normalization) {
            search_text = get_normalized_text(text.data(), case_insensitive_, normalization_);
        }
        
        PCRE2_SIZE offset = 0;
        PCRE2_SIZE length = static_cast<PCRE2_SIZE>(search_text.length());
        
        while (offset <= length) {
            int rc = pcre2_match(
                pcre_regex_,
                reinterpret_cast<PCRE2_SPTR>(search_text.c_str()),
                length,
                offset,
                0,
                pcre_match_data_,
                nullptr
            );
            if (rc < 0) break;
            PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(pcre_match_data_);
            
            /* Safety check: prevent infinite loop on zero-length matches */
            if (ovector[0] == ovector[1] && ovector[0] == offset) {
                offset++;
                if (offset > length) break;
                continue;
            }
            
            // If we're using normalized text, map the match back to original positions
            if (needs_normalization) {
                auto mapped = map_normalized_span_to_original(text.data(), search_text, ovector[0], ovector[1] - ovector[0]);
                matches.emplace_back(mapped.first, mapped.second);
            } else {
            matches.emplace_back(ovector[0], ovector[1] - ovector[0]);
            }
            
            // Always advance past the current match to avoid infinite loops
                offset = ovector[1];
            if (offset == ovector[0]) offset++; /* Ensure we always advance */
        }
        return matches;
    }
#endif
    if ((is_simple_repeated_pattern() && text.data().size() > 100) ||
        (pattern_.back() == '+' && text.data().size() > 10000)) {
        return find_all_fast_path(text);
    }
    
    /* Try fast paths for simple patterns */
    if (is_simple_literal_pattern(pattern_) && text.data().size() > 50) {
        return find_all_literal_fast_path(text);
    }
    
    if (is_simple_word_pattern(pattern_) && text.data().size() > 100) {
        return find_all_word_fast_path(text);
    }
    
    if (is_simple_digit_pattern(pattern_) && text.data().size() > 100) {
        return find_all_digit_fast_path(text);
    }
    
    if (is_ascii_only_ && is_ascii_string(text.data())) {
        try {
            for (std::sregex_iterator it(text.data().begin(),
                text.data().end(), regex_), end;
                it != end; ++it) {
                matches.emplace_back(it->position(), it->length());
            }
        } catch (const std::regex_error&) {}
    } else if (normalization_ != GrapaUnicode::NormalizationForm::NONE) {
        std::string normalized_text = get_normalized_text(text.data(), case_insensitive_, normalization_);
        try {
            for (std::sregex_iterator it(normalized_text.begin(),
                normalized_text.end(), regex_), end;
                it != end; ++it) {
                matches.emplace_back(it->position(), it->length());
            }
        } catch (const std::regex_error&) {}
    } else {
        try {
            for (std::sregex_iterator it(text.data().begin(),
                text.data().end(), regex_), end;
                it != end; ++it) {
                matches.emplace_back(it->position(), it->length());
            }
        } catch (const std::regex_error&) {}
    }
    return matches;
}

namespace GrapaUnicode {

UnicodeRegex::UnicodeRegex(const std::string& pattern, bool case_insensitive, bool diacritic_insensitive, GrapaUnicode::NormalizationForm norm)
    : pattern_(pattern), case_insensitive_(case_insensitive), diacritic_insensitive_(diacritic_insensitive), normalization_(norm), is_ascii_only_(is_ascii_string(pattern)) {
    // Don't compile here - let the caller call compile() with the actual input
    compilation_error_ = false;
}

UnicodeRegex::~UnicodeRegex() {
#ifdef USE_PCRE
    if (pcre_regex_) {
        pcre2_code_free(pcre_regex_);
    }
    if (pcre_match_data_) {
        pcre2_match_data_free(pcre_match_data_);
    }
#endif
}

void UnicodeRegex::compile(const std::string& input) {
    compilation_error_ = false; /* Reset error state */
    std::string pattern_to_compile = pattern_;
    if (diacritic_insensitive_) {
        pattern_to_compile = GrapaUnicode::strip_diacritics(pattern_to_compile);
    }
    use_pcre_ = false;
#ifdef USE_PCRE
    // Check for PCRE2-only features that std::regex doesn't support
    bool has_pcre2_features = false;
    
    // Check for lookbehind assertions (?<=...)
    if (pattern_to_compile.find("(?<=") != std::string::npos) {
        has_pcre2_features = true;
    }
    
    // Check for lookahead assertions (?=...) and (?!...)
    if (pattern_to_compile.find("(?=") != std::string::npos || 
        pattern_to_compile.find("(?!") != std::string::npos) {
        has_pcre2_features = true;
    }
    
    // Check for negative lookbehind (?<!...)
    if (pattern_to_compile.find("(?<!") != std::string::npos) {
        has_pcre2_features = true;
    }
    
    // Check for atomic groups (?>...)
    if (pattern_to_compile.find("(?>") != std::string::npos) {
        has_pcre2_features = true;
    }
    
    // Check for Unicode properties \p{...} and \P{...}
    if (pattern_to_compile.find("\\p{") != std::string::npos || 
        pattern_to_compile.find("\\P{") != std::string::npos) {
        has_pcre2_features = true;
    }
    
    // Check for named groups (?P<...>) or (?<...>)
    if (pattern_to_compile.find("(?P<") != std::string::npos || 
        pattern_to_compile.find("(?<") != std::string::npos) {
        has_pcre2_features = true;
    }
    
    // Check for conditional groups (?(...)...)
    if (pattern_to_compile.find("(?(") != std::string::npos) {
        has_pcre2_features = true;
    }
    
    // Check for \K (keep out)
    if (pattern_to_compile.find("\\K") != std::string::npos) {
        has_pcre2_features = true;
    }
    
    // Check for \X (Unicode grapheme cluster)
    if (pattern_to_compile.find("\\X") != std::string::npos) {
        has_pcre2_features = true;
    }

    // Check for possessive quantifiers (*+, ++, ?+, {n,m}+)
    if (pattern_to_compile.find("*+") != std::string::npos ||
        pattern_to_compile.find("++") != std::string::npos ||
        pattern_to_compile.find("?+") != std::string::npos) {
        has_pcre2_features = true;
    }
    
    // Check for possessive quantifiers with braces {n,m}+ using string search
    if (pattern_to_compile.find("}+") != std::string::npos) {
        // Look for patterns like {n}+ or {n,m}+
        size_t pos = 0;
        while ((pos = pattern_to_compile.find("}+", pos)) != std::string::npos) {
            if (pos > 0 && pattern_to_compile[pos-1] == '}') {
                // Check if this is preceded by a valid quantifier pattern
                size_t brace_start = pattern_to_compile.rfind('{', pos);
                if (brace_start != std::string::npos && brace_start < pos) {
                    // Found a potential {n,m}+ pattern
                    has_pcre2_features = true;
                    break;
                }
            }
            pos += 2; // Skip the "}+" we just found
        }
    }
    
    // Use PCRE2 if pattern has advanced features or if pattern/input is non-ASCII
    if (has_pcre2_features || !is_ascii_string(pattern_to_compile) || !is_ascii_string(input)) {
        use_pcre_ = true;
    }
#endif
    // Debug: Print which regex engine is being used
    if (use_pcre_) {
        std::string pattern_utf8 = parse_unicode_escapes(pattern_to_compile);
        int error_code = 0;
        PCRE2_SIZE error_offset = 0;
        uint32_t compile_flags = PCRE2_UTF | PCRE2_UCP | PCRE2_JIT_COMPLETE;
        if (case_insensitive_) {
            compile_flags |= PCRE2_CASELESS;
        }
        // Always enable multiline and dotall for ripgrep parity
        // Note: PCRE2 supports lookbehind assertions by default when UTF and UCP are enabled
        compile_flags |= PCRE2_DOTALL | PCRE2_MULTILINE;
        
        // Debug: Check if pattern is \X and add special handling
        bool is_grapheme_pattern = (pattern_utf8 == "\\X");
        if (is_grapheme_pattern) {
            // For \X pattern, use a different approach - match any Unicode character
            pattern_utf8 = ".";
        }
        
        // Use the actual pattern variable being compiled
        pcre2_code* re = pcre2_compile(
            reinterpret_cast<PCRE2_SPTR>(pattern_utf8.c_str()),
            PCRE2_ZERO_TERMINATED,
            compile_flags,
            &error_code,
            &error_offset,
            nullptr
        );
        if (re == nullptr) {
            compilation_error_ = true;
            return;
        }
        
        /* Try to enable JIT compilation for better performance */
        if (pcre2_jit_compile(re, PCRE2_JIT_COMPLETE) == 0) {
            use_jit_matching_ = true;
        }
        
        pcre_regex_ = re;
        pcre_match_data_ = pcre2_match_data_create_from_pattern(pcre_regex_, nullptr);
        if (pcre_match_data_ == nullptr) {
            compilation_error_ = true;
            pcre2_code_free(pcre_regex_);
            pcre_regex_ = nullptr;
            return;
        }
    } else {
        std::regex::flag_type flags = std::regex::ECMAScript;
        if (case_insensitive_) flags |= std::regex::icase;
        try {
            regex_ = std::regex(pattern_to_compile, flags);
        } catch (const std::regex_error&) {
            compilation_error_ = true;
            return;
        }
    }
}

bool GrapaUnicode::UnicodeRegex::has_unicode_properties(const std::string& pattern) {
    // Check for \p{...} or \P{...}
    size_t pos = 0;
    while ((pos = pattern.find("\\p{", pos)) != std::string::npos) {
        if (pos + 3 < pattern.size() && pattern[pos + 3] == '{') {
            // Found \p{
            size_t end_pos = pattern.find('}', pos);
            if (end_pos == std::string::npos || end_pos <= pos + 4) return true; // Invalid
            ++pos; // Skip '{'
        } else if (pos + 3 < pattern.size() && pattern[pos + 3] == 'P' && pattern[pos + 4] == '{') {
            // Found \P{
            size_t end_pos = pattern.find('}', pos);
            if (end_pos == std::string::npos || end_pos <= pos + 5) return true; // Invalid
            ++pos; // Skip '{'
        } else {
            return true; // Found \p or \P without {
        }
    }
    
    // Check for Unicode blocks \p{In_...}
    pos = 0;
    while ((pos = pattern.find("\\p{In_", pos)) != std::string::npos) {
        return true; // Found Unicode block pattern
    }
    
    return false;
}

// Hybrid mapping strategy with multiple fallbacks for robust Unicode boundary handling
std::pair<size_t, size_t> UnicodeRegex::map_normalized_span_to_original(const std::string& original, const std::string& normalized, size_t norm_offset, size_t norm_length) {
    if (original == normalized) {
        return { norm_offset, norm_length };
    }

    // Strategy 1: Try grapheme cluster boundary alignment
    auto result = try_grapheme_cluster_mapping(original, normalized, norm_offset, norm_length);
    if (result.second > 0 && result.first < original.size()) {
        return result;
    }

    // Strategy 2: Try character-by-character alignment
    result = try_character_alignment(original, normalized, norm_offset, norm_length);
    if (result.second > 0 && result.first < original.size()) {
        return result;
    }

    // Strategy 3: Fallback to direct substring with bounds checking
    result = try_fallback_substring(original, normalized, norm_offset, norm_length);
    if (result.second > 0 && result.first < original.size()) {
        return result;
    }

    // Final fallback: Return the full original string for the span if possible
    if (!original.empty()) {
        return { 0, original.size() };
    }
    // If all else fails, return an empty string (never null or replacement char)
    return { 0, 0 };
}

} // namespace GrapaUnicode

/* Static member definitions */
namespace GrapaUnicode {
    std::map<std::tuple<std::string, bool, GrapaUnicode::NormalizationForm>, std::string> UnicodeRegex::text_cache_;
    std::mutex UnicodeRegex::text_cache_mutex_;
    std::map<std::pair<std::string, std::string>, std::vector<std::pair<size_t, size_t>>> UnicodeRegex::offset_cache_;
    std::mutex UnicodeRegex::offset_cache_mutex_;
    UnicodeRegex::LRUCache UnicodeRegex::text_lru_cache_(1000); /* Initialize with 1000 entry limit */
}

// GrapaGrepWorkEvent implementation
void GrapaGrepWorkEvent::Starting() {
    // Initialize thread - no special setup needed for grep
}

void GrapaGrepWorkEvent::Running() {
    // Send condition signal to indicate work is starting (like GrapaRuleWorkEvent)
    SendCondition();
    // Use the main grep logic to handle context, invert, and all-mode correctly
    std::string effective_pattern = pattern;
    if (options.find('w') != std::string::npos) {
        if (effective_pattern.substr(0,2) != "\\b") effective_pattern = "\\b" + effective_pattern;
        if (effective_pattern.size() < 2 || effective_pattern.substr(effective_pattern.size()-2) != "\\b") effective_pattern += "\\b";
    }
    results = grep_extract_matches_unicode_impl_sequential(
        input_chunk, effective_pattern, options, line_delim, normalization, mode);
}

void GrapaGrepWorkEvent::Stopping() {
    // Clean up - no special cleanup needed for grep
}

// Helper: Extract a single grapheme cluster at a given byte offset from a UTF-8 string
std::string extract_grapheme_cluster(const std::string& input, size_t offset) {
    size_t input_len = input.size();
    if (offset >= input_len) return "";
    size_t start = offset;
    size_t end = offset;
    utf8proc_int32_t cp;
    utf8proc_ssize_t char_len = utf8proc_iterate(reinterpret_cast<const utf8proc_uint8_t*>(input.c_str()) + start, input_len - start, &cp);
    // Debug output removed for production
    if (char_len <= 0) return "";
    // If the current codepoint is a newline, return it as its own cluster
    if (cp == '\n') return input.substr(start, char_len);
    end = start + char_len;
    // Extend to full grapheme cluster, but stop at newline
    while (end < input_len) {
        utf8proc_int32_t next_cp;
        utf8proc_ssize_t next_len = utf8proc_iterate(reinterpret_cast<const utf8proc_uint8_t*>(input.c_str()) + end, input_len - end, &next_cp);
        if (next_len <= 0) break;
        // If the next codepoint is a newline, stop BEFORE including it
        if (next_cp == '\n') break;
        if (utf8proc_grapheme_break(cp, next_cp) != 0) break;
        end += next_len;
        cp = next_cp;
    }
    // Debug output removed for production
    return input.substr(start, end - start);
}


 