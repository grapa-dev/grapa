#include "grapa_grep_unicode.hpp"
#include "grep_unicode.hpp"
#include <algorithm>
#include <sstream>
#include <set>

// Forward declarations for parallel processing
std::vector<std::string> grep_extract_matches_unicode_impl_sequential(
    const std::string& input,
    const std::string& effective_pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization,
    GrapaUnicode::ProcessingMode mode
);

std::vector<std::string> grep_extract_matches_unicode_impl_parallel(
    const std::string& input,
    const std::string& effective_pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization,
    GrapaUnicode::ProcessingMode mode,
    size_t num_workers
);

std::vector<std::string> split_input_for_parallel(const std::string& input, size_t num_chunks);

// Helper function to parse context options
ContextOptions parse_context_options(const std::string& options, std::string& filtered_options) {
    ContextOptions ctx;
    filtered_options = options;
    // Parse -A (after context) and A (short form)
    size_t pos = filtered_options.find("-A");
    if (pos == std::string::npos) {
        // Look for 'A' followed by a digit
        for (size_t i = 0; i + 1 < filtered_options.size(); ++i) {
            if (filtered_options[i] == 'A' && std::isdigit(filtered_options[i+1])) {
                pos = i;
                break;
            }
        }
    }
    if (pos != std::string::npos && pos + 2 <= filtered_options.size()) {
        size_t num_start = pos + 1;
        size_t num_end = num_start;
        while (num_end < filtered_options.size() && std::isdigit(filtered_options[num_end])) {
            num_end++;
        }
        if (num_end > num_start) {
            ctx.after = std::stoi(filtered_options.substr(num_start, num_end - num_start));
            filtered_options.erase(pos, num_end - pos);
        }
    }
    // Parse -B (before context) and B (short form)
    pos = filtered_options.find("-B");
    if (pos == std::string::npos) {
        for (size_t i = 0; i + 1 < filtered_options.size(); ++i) {
            if (filtered_options[i] == 'B' && std::isdigit(filtered_options[i+1])) {
                pos = i;
                break;
            }
        }
    }
    if (pos != std::string::npos && pos + 2 <= filtered_options.size()) {
        size_t num_start = pos + 1;
        size_t num_end = num_start;
        while (num_end < filtered_options.size() && std::isdigit(filtered_options[num_end])) {
            num_end++;
        }
        if (num_end > num_start) {
            ctx.before = std::stoi(filtered_options.substr(num_start, num_end - num_start));
            filtered_options.erase(pos, num_end - pos);
        }
    }
    // Parse -C (context) and C (short form)
    pos = filtered_options.find("-C");
    if (pos == std::string::npos) {
        for (size_t i = 0; i + 1 < filtered_options.size(); ++i) {
            if (filtered_options[i] == 'C' && std::isdigit(filtered_options[i+1])) {
                pos = i;
                break;
            }
        }
    }
    if (pos != std::string::npos && pos + 2 <= filtered_options.size()) {
        size_t num_start = pos + 1;
        size_t num_end = num_start;
        while (num_end < filtered_options.size() && std::isdigit(filtered_options[num_end])) {
            num_end++;
        }
        if (num_end > num_start) {
            ctx.context = std::stoi(filtered_options.substr(num_start, num_end - num_start));
            filtered_options.erase(pos, num_end - pos);
        }
    }
    return ctx;
}

// Implementation of the sequential function
std::vector<std::string> grep_extract_matches_unicode_impl_sequential(
    const std::string& input,
    const std::string& effective_pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization,
    GrapaUnicode::ProcessingMode mode
) {
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: ===== FUNCTION ENTERED =====\n");
    printf("DEBUG: ENTER grep_extract_matches_unicode_impl_sequential\n");
    printf("DEBUG: input: '%s'\n", input.c_str());
    printf("DEBUG: effective_pattern: '%s'\n", effective_pattern.c_str());
    printf("DEBUG: options: '%s'\n", options.c_str());
    printf("DEBUG: line_delim: '%s'\n", line_delim.c_str());
    #endif // DEBUG_END
    // Validate option combinations (ripgrep-style)
    bool has_o = options.find('o') != std::string::npos;
    bool has_x = options.find('x') != std::string::npos;
    bool has_a = options.find('a') != std::string::npos;
    bool has_v = options.find('v') != std::string::npos;
    bool has_c = options.find('c') != std::string::npos;
    bool has_l = options.find('l') != std::string::npos;
    bool has_n = options.find('n') != std::string::npos;

    // Ripgrep-style precedence rules
    if (has_o && has_x) {
        std::string filtered_options = options;
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'x'), filtered_options.end());
        return grep_extract_matches_unicode_impl_sequential(input, effective_pattern, filtered_options, line_delim, normalization, mode);
    }
    if (has_c && (has_o || has_l || has_n)) {
        std::string filtered_options = options;
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'o'), filtered_options.end());
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'l'), filtered_options.end());
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'n'), filtered_options.end());
        return grep_extract_matches_unicode_impl_sequential(input, effective_pattern, filtered_options, line_delim, normalization, mode);
    }
    if (has_l && (has_o || has_n)) {
        std::string filtered_options = options;
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'o'), filtered_options.end());
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'n'), filtered_options.end());
        return grep_extract_matches_unicode_impl_sequential(input, effective_pattern, filtered_options, line_delim, normalization, mode);
    }
    
    std::string filtered_options;
    ContextOptions ctx = parse_context_options(options, filtered_options);
    GrapaUnicode::NormalizationForm norm = normalization;
    if (options.find('N') != std::string::npos) {
        norm = GrapaUnicode::NormalizationForm::NFC;
    } else if (norm == GrapaUnicode::NormalizationForm::NONE) {
        norm = GrapaUnicode::NormalizationForm::NFC;
    }
    filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'N'), filtered_options.end());
    int after = ctx.after;
    int before = ctx.before;
    int context = ctx.context;
    if (context > 0) {
        before = context;
        after = context;
    }
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: Context option values: before=%d, after=%d, context=%d\n", before, after, context);
    #endif // DEBUG_END
    bool all_mode = (filtered_options.find('a') != std::string::npos);
    bool match_only = (filtered_options.find('o') != std::string::npos);
    bool count_only = (filtered_options.find('c') != std::string::npos);
    if (all_mode) {
        before = 0;
        after = 0;
    }
    bool null_data = (filtered_options.find('z') != std::string::npos);
    std::string working_input;
    
    // Store the original input with delimiters for multiline patterns
    std::string original_input_with_delimiters = input;
    
    if (null_data) {
        working_input = input;
    } else if (all_mode && !line_delim.empty()) {
        working_input.reserve(input.size());
        size_t pos = 0;
        while (pos < input.size()) {
            size_t found = input.find(line_delim, pos);
            if (found == std::string::npos) {
                working_input.append(input, pos, input.size() - pos);
                break;
            } else {
                working_input.append(input, pos, found - pos);
                pos = found + line_delim.size();
            }
        }
    } else {
        // UNIFIED DESIGN: Always use a delimiter, with "\n" as default
        std::string effective_delimiter = line_delim.empty() ? "\n" : line_delim;
        working_input = input;
    }
    
    // Handle invalid/null delimiter: if delimiter is a single null character, treat as invalid
    if (line_delim.size() == 1 && line_delim[0] == '\0') {
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Invalid delimiter (\\0) detected, returning empty array\n");
        #endif // DEBUG_END
        if (filtered_options.find('j') != std::string::npos) {
            return {"[]"};
        }
        return {};
    }
    
    // Define json_output early for all special cases
    bool json_output = (filtered_options.find('j') != std::string::npos);
    bool color_output = (filtered_options.find('L') != std::string::npos);
    bool column_output = (filtered_options.find('T') != std::string::npos);
    bool invert_match = (filtered_options.find('v') != std::string::npos);
    
    // Initialize the main result vector
    std::vector<std::string> extracted_matches;
    
    // Declare match_positions at function scope so it's available for all logic
    std::vector<MatchPosition> match_positions;
    
    // Special case: if pattern is '^' and match_only, return one empty string per line
    if (match_only && effective_pattern == "^") {
        std::vector<std::string> starts;
        for (size_t i = 0; i < working_input.size(); ++i) {
            starts.push_back("");
        }
        if (json_output) {
            return {build_json_array(starts)};
        }
        return starts;
    }
    
    // Special case: if pattern is exactly '\\X' and match_only ('o') is set, segment input into grapheme clusters
    if (match_only && effective_pattern == "\\X") {
        std::vector<std::string> graphemes;
        
        // Split input by delimiter and process each line separately to exclude delimiters
        std::vector<std::string> lines = split_by_delimiter(working_input, line_delim);
        
        for (const auto& line : lines) {
            size_t offset = 0;
            while (offset < line.size()) {
                std::string cluster = extract_grapheme_cluster(line, offset);
                if (cluster.empty()) break;
                graphemes.push_back(GrapaUnicode::UnicodeRegex::get_normalized_text(cluster, false, GrapaUnicode::NormalizationForm::NFC));
                offset += cluster.size();
            }
        }
        
        if (json_output) {
            return {build_json_array(graphemes)};
        }
        return graphemes;
    }
    
    // Apply word boundary logic if 'w' option is present
    std::string working_pattern = effective_pattern;
    if (filtered_options.find('w') != std::string::npos) {
        if (working_pattern.substr(0,2) != "\\b") working_pattern = "\\b" + working_pattern;
        if (working_pattern.size() < 2 || working_pattern.substr(working_pattern.size()-2) != "\\b") working_pattern += "\\b";
    }
    
    // Special handling for multiline patterns with custom delimiters
    bool is_multiline = is_multiline_pattern(effective_pattern);
    bool has_lookaround = has_lookaround_assertions(effective_pattern);
    bool has_s_flag = (filtered_options.find('s') != std::string::npos);
    bool multiline_handled = false; // Flag to track if multiline patterns were handled
    
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: Pattern: '%s'\n", effective_pattern.c_str());
    printf("DEBUG: Options: '%s'\n", filtered_options.c_str());
    printf("DEBUG: Line delimiter: '%s'\n", line_delim.c_str());
    printf("DEBUG: Is multiline pattern: %s\n", is_multiline ? "true" : "false");
    printf("DEBUG: Has lookaround: %s\n", has_lookaround ? "true" : "false");
    printf("DEBUG: Has s flag: %s\n", has_s_flag ? "true" : "false");
    printf("DEBUG: Match only mode: %s\n", match_only ? "true" : "false");
    printf("DEBUG: Working input length: %zu\n", working_input.length());
    printf("DEBUG: Line delimiter empty: %s\n", line_delim.empty() ? "true" : "false");
    #endif // DEBUG_END
    
    // After option parsing and before main logic
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: match_only: %s, all_mode: %s, null_data: %s\n", match_only ? "true" : "false", all_mode ? "true" : "false", null_data ? "true" : "false");
    #endif // DEBUG_END
    
    // For multiline patterns with custom delimiters, treat the entire input as one string
    // FIX: If a custom delimiter (not '\n') is present, treat as single string for multiline patterns, even without s flag
    bool custom_delimiter = (!line_delim.empty() && line_delim != "\n");
    if ((is_multiline || has_s_flag) && custom_delimiter) {
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Multiline pattern handling block entered\n");
        printf("DEBUG: is_multiline: %s, has_s_flag: %s, line_delim: '%s'\n", is_multiline ? "true" : "false", has_s_flag ? "true" : "false", line_delim.c_str());
        #endif // DEBUG_END
        
        // Set flag to indicate multiline patterns were handled
        multiline_handled = true;
        
        // Store the original input for result mapping
        std::string original_input = original_input_with_delimiters;
        
        // UNIFIED DESIGN: Use effective delimiter consistently
        std::string effective_delimiter = line_delim.empty() ? "\n" : line_delim;
        
        // For multiline patterns with s flag, we need to ensure the pattern works correctly
        if (has_s_flag) {
            // The s flag means dot should match newlines, which PCRE2 handles automatically
            // when DOTALL flag is set (which it is in our UnicodeRegex constructor)
            working_pattern = effective_pattern;
            
            // For multiline patterns with s flag, use the original input with delimiters
            // The regex engine should handle the delimiters as part of the pattern matching
            working_input = original_input_with_delimiters;
        } else if (is_multiline) {
            // FIX: For multiline patterns with a custom delimiter, treat as single string
            // This allows patterns like 'start.*end' to match across custom delimiters (even multi-char)
            // This is a Grapa extension beyond traditional grep/ripgrep
            working_pattern = effective_pattern;
            working_input = original_input_with_delimiters;
            // multiline_handled remains true
        }
        
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: After delimiter replacement, multiline_input length: %zu\n", working_input.length());
        printf("DEBUG: First 100 chars of multiline_input: '%.100s'\n", working_input.c_str());
        #endif // DEBUG_END
        
        // For multiline patterns with a flag, we need to ensure we return the full original input
        if (all_mode) {
            // The a flag means we should return the full original input with delimiters
            // when any match is found, regardless of whether it's a full match
            working_pattern = effective_pattern;
            working_input = original_input_with_delimiters;
            
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Using original input for a flag: '%s'\n", working_input.c_str());
            #endif // DEBUG_END
            
            // For the a flag, we need to check if there are any matches and return the full input if so
            bool case_insensitive = (filtered_options.find('i') != std::string::npos);
            bool diacritic_insensitive = (filtered_options.find('d') != std::string::npos);
            
            // Use UnicodeRegex to check for matches
            GrapaUnicode::UnicodeRegex regex(working_pattern, case_insensitive, diacritic_insensitive, norm);
            regex.compile(working_input);
            
            if (regex.is_valid()) {
                // Get all matches in the input
                auto a_flag_matches = regex.find_all(GrapaUnicode::UnicodeString(working_input));
                
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: a flag regex matching found %zu matches\n", a_flag_matches.size());
                #endif // DEBUG_END
                
                // If we have any matches, return the full original input
                if (!a_flag_matches.empty()) {
                    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                    printf("DEBUG: a flag matches found, returning full original input\n");
                    #endif // DEBUG_END
                    std::vector<std::string> full_result = {original_input};
                    if (json_output) {
                        return {build_json_array(full_result)};
                    }
                    return full_result;
                }
            }
            
            // If no matches found or regex compilation failed, return empty result
            if (json_output) {
                return {"[]"};
            }
            return {};
        }
        
        // Only call the main regex matching function if we're still handling multiline patterns
        if (multiline_handled && !all_mode) {
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: multiline_handled is true, using direct regex matching\n");
            #endif // DEBUG_END
            
            // For multiline patterns, use direct regex matching instead of line-by-line processing
            bool case_insensitive = (filtered_options.find('i') != std::string::npos);
            bool diacritic_insensitive = (filtered_options.find('d') != std::string::npos);
            
            // Use UnicodeRegex for direct multiline matching
            GrapaUnicode::UnicodeRegex regex(working_pattern, case_insensitive, diacritic_insensitive, norm);
            regex.compile(working_input);
            
            if (!regex.is_valid()) {
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Regex compilation failed for multiline pattern\n");
                #endif // DEBUG_END
                if (json_output) {
                    return {"[]"};
                }
                return {};
            }
            
            // Get all matches in the multiline input
            auto multiline_matches = regex.find_all(GrapaUnicode::UnicodeString(working_input));
            
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Direct regex matching found %zu matches\n", multiline_matches.size());
            #endif // DEBUG_END
            
            // For multiline patterns, if we have matches, return the full original input with delimiters
            if (!multiline_matches.empty()) {
                // For multiline patterns, always return the full original input with delimiters
                // This is the expected behavior for multiline patterns with s and a flags
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Multiline match found, returning full original input\n");
                #endif // DEBUG_END
                std::vector<std::string> full_result = {original_input};
                if (json_output) {
                    return {build_json_array(full_result)};
                }
                return full_result;
            } else {
                // If no matches found, return empty result
                if (json_output) {
                    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                    printf("DEBUG: Multiline pattern - no matches found, returning empty JSON\n");
                    #endif // DEBUG_END
                    return {"[]"};
                }
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Multiline pattern - no matches found, returning empty array\n");
                #endif // DEBUG_END
                return {};
            }
        }
    }
    
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: After all_mode block, multiline_handled: %s\n", multiline_handled ? "true" : "false");
    #endif // DEBUG_END
    
    // Special handling for oa combination (match_only + all_mode)
    if (match_only && all_mode) {
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Special handling for oa combination (match_only + all_mode)\n");
        #endif // DEBUG_END
        
        // For oa combination, we need to check if there are any matches and return the full input if so
        bool case_insensitive = (filtered_options.find('i') != std::string::npos);
        bool diacritic_insensitive = (filtered_options.find('d') != std::string::npos);
        
        // Use UnicodeRegex to check for matches
        GrapaUnicode::UnicodeRegex regex(working_pattern, case_insensitive, diacritic_insensitive, norm);
        regex.compile(working_input);
        
        if (regex.is_valid()) {
            // Get all matches in the input
            auto oa_matches = regex.find_all(GrapaUnicode::UnicodeString(working_input));
            
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: oa combination regex matching found %zu matches\n", oa_matches.size());
            #endif // DEBUG_END
            
            // If we have any matches, return the full original input
            if (!oa_matches.empty()) {
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: oa combination matches found, returning full original input\n");
                #endif // DEBUG_END
                std::vector<std::string> full_result = {original_input_with_delimiters};
                if (json_output) {
                    return {build_json_array(full_result)};
                }
                return full_result;
            }
        }
        
        // If no matches found or regex compilation failed, return empty result
        if (json_output) {
            return {"[]"};
        }
        return {};
    }
    
    // Special handling for lookaround assertions - handle them before calling the main regex function
    if (has_lookaround) {
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Lookaround handling block entered\n");
        printf("DEBUG: Processing lookaround assertions\n");
        printf("DEBUG: Working pattern: '%s'\n", working_pattern.c_str());
        printf("DEBUG: Working input length: %zu\n", working_input.length());
        #endif // DEBUG_END
        
        bool diacritic_insensitive = (filtered_options.find('d') != std::string::npos);
        std::vector<std::string> lookaround_matches = extract_matches_with_lookaround(working_input, working_pattern, 
            filtered_options.find('i') != std::string::npos, diacritic_insensitive, norm);
        
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Found %zu lookaround matches\n", lookaround_matches.size());
        for (size_t i = 0; i < lookaround_matches.size() && i < 3; ++i) {
            printf("DEBUG: Lookaround match %zu: '%s'\n", i, lookaround_matches[i].c_str());
        }
        #endif // DEBUG_END
        
        if (match_only) {
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Lookaround match-only mode, extracted_matches set\n");
            #endif // DEBUG_END
            // For match-only mode, return the lookaround matches directly
            extracted_matches = lookaround_matches;
            // Set flag to skip main regex processing
            multiline_handled = true;
        } else {
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Lookaround non-match-only mode, returning full lines\n");
            #endif // DEBUG_END
            // For non-match-only mode, we need to return the full lines containing matches
            if (!lookaround_matches.empty()) {
                std::vector<std::string> full_lines;
                std::vector<std::string> lines = split_by_delimiter(working_input, line_delim);
                
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Split into %zu lines for full line extraction\n", lines.size());
                #endif // DEBUG_END
                
                for (const auto& match : lookaround_matches) {
                    // Find which line contains this match
                    for (const auto& line : lines) {
                        if (line.find(match) != std::string::npos) {
                            full_lines.push_back(line);
                            break;
                        }
                    }
                }
                
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Extracted %zu full lines from lookaround matches\n", full_lines.size());
                #endif // DEBUG_END
                
                if (json_output) {
                    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                    printf("DEBUG: Lookaround assertions - returning full lines JSON\n");
                    #endif // DEBUG_END
                    return {build_json_array(full_lines)};
                }
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Lookaround assertions - returning full lines array\n");
                #endif // DEBUG_END
                return full_lines;
            } else {
                // No lookaround matches found, return empty for non-match-only mode
                if (json_output) {
                    return {"[]"};
                }
                return {};
            }
        }
    }
    
    // Call the main regex matching function (only if multiline patterns weren't already handled)
    if (!multiline_handled) {
        // UNIFIED DESIGN: Use effective delimiter consistently
        std::string effective_delimiter = line_delim.empty() ? "\n" : line_delim;
        match_positions = grep_unicode_impl(working_input, working_pattern, filtered_options, effective_delimiter, norm, mode);
        
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Main regex matching returned %zu match positions\n", match_positions.size());
        #endif // DEBUG_END
        
        // Extract matches from the positions
        for (const auto& pos : match_positions) {
            if (pos.offset != static_cast<size_t>(-1)) { // Skip compilation errors
                std::string match_text = working_input.substr(pos.offset, pos.length);
                
                // Apply color wrapping if requested
                if (color_output) {
                    match_text = "\x1b[1;31m" + match_text + "\x1b[0m";
                }
                
                // Apply column number prefix if requested
                if (column_output) {
                    match_text = std::to_string(pos.offset + 1) + ":" + match_text;
                }
                
                extracted_matches.push_back(match_text);
            } else {
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Compilation error - returning early\n");
                #endif // DEBUG_END
                return {"__COMPILATION_ERROR__"};
            }
        }
        
        // Handle count-only option early (before invert and context processing)
        if (count_only) {
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Count-only mode - returning count: %zu\n", extracted_matches.size());
            #endif // DEBUG_END
            // Grapa always returns arrays, even for count-only mode, for consistency with all other grep outputs
            return {std::to_string(extracted_matches.size())};
        }
        
        if (invert_match) {
            std::vector<std::string> non_matches;
            
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Invert match logic - line_delim: '%s'\n", line_delim.c_str());
            printf("DEBUG: match_positions.size(): %zu\n", match_positions.size());
            #endif // DEBUG_END
            
            // For custom delimiters, we need to process by segments
            if (!line_delim.empty()) {
                // Split input by delimiter to get segments
                std::vector<std::string> segments = split_by_delimiter(working_input, line_delim);
                
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Split into %zu segments\n", segments.size());
                for (size_t i = 0; i < segments.size(); ++i) {
                    printf("DEBUG: segment[%zu]: '%s'\n", i, segments[i].c_str());
                }
                #endif // DEBUG_END
                
                // Find which segments contain matches
                std::set<size_t> matching_segments;
                for (const auto& pos : match_positions) {
                    if (pos.offset != static_cast<size_t>(-1)) {
                        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                        printf("DEBUG: Processing match at offset %zu, length %zu\n", pos.offset, pos.length);
                        #endif // DEBUG_END
                        
                        // Find which segment this match is in
                        size_t current_offset = 0;
                        for (size_t seg_num = 0; seg_num < segments.size(); ++seg_num) {
                            size_t segment_length = segments[seg_num].length() + line_delim.length();
                            
                            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                            printf("DEBUG: segment[%zu] spans offset %zu to %zu\n", seg_num, current_offset, current_offset + segment_length);
                            #endif // DEBUG_END
                            
                            if (pos.offset >= current_offset && pos.offset < current_offset + segment_length) {
                                matching_segments.insert(seg_num);
                                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                                printf("DEBUG: Match found in segment %zu\n", seg_num);
                                #endif // DEBUG_END
                                break;
                            }
                            current_offset += segment_length;
                        }
                    }
                }
                
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Matching segments: ");
                for (size_t seg : matching_segments) {
                    printf("%zu ", seg);
                }
                printf("\n");
                #endif // DEBUG_END
                
                // Return segments that don't contain matches
                for (size_t i = 0; i < segments.size(); ++i) {
                    if (matching_segments.find(i) == matching_segments.end()) {
                        non_matches.push_back(segments[i]);
                        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                        printf("DEBUG: Adding non-match segment %zu: '%s'\n", i, segments[i].c_str());
                        #endif // DEBUG_END
                    }
                }
            } else {
                // Original logic for newline delimiters
                size_t last_end = 0;
                for (const auto& pos : match_positions) {
                    if (pos.offset != static_cast<size_t>(-1)) {
                        if (pos.offset > last_end) {
                            std::string non_match = working_input.substr(last_end, pos.offset - last_end);
                            non_matches.push_back(non_match);
                        }
                        last_end = (pos.offset + pos.length > last_end) ? (pos.offset + pos.length) : last_end;
                    }
                }
                if (last_end < working_input.size()) {
                    std::string non_match = working_input.substr(last_end);
                    non_matches.push_back(non_match);
                }
            }
            
            // Remove empty strings (for adjacent matches)
            non_matches.erase(std::remove_if(non_matches.begin(), non_matches.end(), [](const std::string& s){ return s.empty(); }), non_matches.end());
            if (json_output) {
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: Invert match - returning non-matches JSON\n");
                #endif // DEBUG_END
                return {build_json_array(non_matches)};
            }
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Invert match - returning non-matches array\n");
            #endif // DEBUG_END
            return non_matches;
        }
        
        // Handle context lines if context options are present
        if (before > 0 || after > 0) {
            // Split input into segments for context processing
            std::vector<std::string> segments;
            if (!line_delim.empty()) {
                segments = split_by_delimiter(working_input, line_delim);
            } else {
                // Use newlines for default delimiter
                size_t start = 0, end;
                while ((end = working_input.find('\n', start)) != std::string::npos) {
                    segments.push_back(working_input.substr(start, end - start));
                    start = end + 1;
                }
                if (start < working_input.size()) {
                    segments.push_back(working_input.substr(start));
                }
            }
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Context mode - split into %zu segments\n", segments.size());
            for (size_t i = 0; i < segments.size(); ++i) {
                printf("DEBUG: segment[%zu]: '%s'\n", i, segments[i].c_str());
            }
            #endif // DEBUG_END
            // Find which segments contain matches
            std::set<size_t> match_segments;
            for (const auto& pos : match_positions) {
                if (pos.offset != static_cast<size_t>(-1)) {
                    // Find which segment this match is in
                    size_t current_offset = 0;
                    for (size_t seg_num = 0; seg_num < segments.size(); ++seg_num) {
                        size_t segment_length = segments[seg_num].length() + (line_delim.empty() ? 1 : line_delim.length());
                        if (pos.offset >= current_offset && pos.offset < current_offset + segment_length) {
                            match_segments.insert(seg_num);
                            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                            printf("DEBUG: Match at offset %zu in segment %zu\n", pos.offset, seg_num);
                            #endif // DEBUG_END
                            break;
                        }
                        current_offset += segment_length;
                    }
                }
            }
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Match segments: ");
            for (size_t seg : match_segments) printf("%zu ", seg);
            printf("\n");
            #endif // DEBUG_END
            // Collect context segments
            std::set<size_t> context_segments;
            for (size_t match_segment : match_segments) {
                // Add segments before
                for (int i = 1; i <= before && match_segment - i >= 0; ++i) {
                    context_segments.insert(match_segment - i);
                }
                // Add the match segment itself
                context_segments.insert(match_segment);
                // Add segments after
                for (int i = 1; i <= after && match_segment + i < segments.size(); ++i) {
                    context_segments.insert(match_segment + i);
                }
            }
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Context segments: ");
            for (size_t seg : context_segments) printf("%zu ", seg);
            printf("\n");
            #endif // DEBUG_END
            // Convert to sorted vector and extract segments with proper merging and separators
            std::vector<size_t> sorted_context_segments(context_segments.begin(), context_segments.end());
            std::sort(sorted_context_segments.begin(), sorted_context_segments.end());
            extracted_matches.clear();
            if (!sorted_context_segments.empty()) {
                // Add first segment
                extracted_matches.push_back(segments[sorted_context_segments[0]]);
                // Add remaining segments with separators for gaps
                for (size_t i = 1; i < sorted_context_segments.size(); ++i) {
                    size_t prev_segment = sorted_context_segments[i-1];
                    size_t curr_segment = sorted_context_segments[i];
                    // If there's a gap of more than 1 segment, add separator
                    if (curr_segment > prev_segment + 1) {
                        if (json_output) {
                            extracted_matches.push_back("---");
                        } else {
                            extracted_matches.push_back("--");
                        }
                    }
                    extracted_matches.push_back(segments[curr_segment]);
                }
            }
        }
    }
    
    // Handle empty pattern case - let regex engine find all empty string matches
    if (effective_pattern.empty()) {
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Empty pattern case - returning early\n");
        #endif // DEBUG_END
        std::vector<std::string> empty_matches;
        empty_matches.push_back("");
        if (json_output) {
            return {build_json_array(empty_matches)};
        }
        return empty_matches;
    }
    
    // Handle edge cases for empty input and delimiter-only input
    if (working_input.empty() || (working_input.find_first_not_of(line_delim) == std::string::npos && !line_delim.empty())) {
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Empty or delimiter-only input case - returning empty array\n");
        #endif // DEBUG_END
        if (json_output) {
            return {"[]"};
        }
        return {};
    }
    
    // Handle match-only mode with custom delimiters
    if (match_only && !line_delim.empty()) {
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: ENTERED MATCH-ONLY BLOCK\n");
        printf("DEBUG: extracted_matches.size(): %zu\n", extracted_matches.size());
        #endif // DEBUG_END
        
        // For match-only mode, we need to determine whether to return full segments or matched portions
        // based on the pattern type and options specified
        bool has_lookaround = has_lookaround_assertions(effective_pattern);
        bool has_unicode_script = (effective_pattern.find("\\p{") != std::string::npos);
        bool has_conditional = (effective_pattern.find("(?(") != std::string::npos);
        bool full_segments_mode = (filtered_options.find('f') != std::string::npos);
        
        // Pattern-based behavior control:
        // - Regular patterns (e.g., 'line', 'word'): Return full segments containing matches
        // - Lookaround assertions (e.g., '\w+(?=\d)'): Return only the matched portions
        // - Unicode script properties (e.g., '\p{sc=Latin}+'): Return only the matched portions
        // - Conditional patterns (e.g., '(a)?(?(1)b|c)'): Return only the matched portions
        // - Option 'f' overrides: forces full segments for all pattern types
        if (full_segments_mode) {
            // Return full segments when 'f' flag is used (overrides pattern type)
            // Convert MatchPosition vector to std::pair vector for extract_full_segments
            std::vector<std::pair<size_t, size_t>> pair_positions;
            for (const auto& pos : match_positions) {
                pair_positions.push_back({pos.offset, pos.length});
            }
            std::vector<std::string> full_segments = extract_full_segments(working_input, line_delim, pair_positions);
            
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Returning full segments for match-only mode with 'f' flag: %zu\n", full_segments.size());
            for (size_t i = 0; i < full_segments.size() && i < 3; ++i) {
                printf("DEBUG: full_segment[%zu]: '%s'\n", i, full_segments[i].c_str());
            }
            #endif // DEBUG_END
            
            if (json_output) {
                return {build_json_array(full_segments)};
            }
            return full_segments;
        } else if (has_lookaround || has_unicode_script || has_conditional) {
            // Return matched portions for lookaround assertions, Unicode script properties, and conditional patterns
            std::vector<std::string> matched_portions;
            
            // Use the extracted_matches which contain the actual pattern matches
            for (const auto& match : extracted_matches) {
                matched_portions.push_back(match);
            }
            
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Returning matched portions for match-only mode: %zu\n", matched_portions.size());
            for (size_t i = 0; i < matched_portions.size() && i < 3; ++i) {
                printf("DEBUG: matched_portion[%zu]: '%s'\n", i, matched_portions[i].c_str());
            }
            #endif // DEBUG_END
            
            if (json_output) {
                return {build_json_array(matched_portions)};
            }
            return matched_portions;
        } else {
            // Return full segments for regular patterns (default behavior)
            // Convert MatchPosition vector to std::pair vector for extract_full_segments
            std::vector<std::pair<size_t, size_t>> pair_positions;
            for (const auto& pos : match_positions) {
                pair_positions.push_back({pos.offset, pos.length});
            }
            std::vector<std::string> full_segments = extract_full_segments(working_input, line_delim, pair_positions);
            
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: Returning full segments for regular patterns: %zu\n", full_segments.size());
            for (size_t i = 0; i < full_segments.size() && i < 3; ++i) {
                printf("DEBUG: full_segment[%zu]: '%s'\n", i, full_segments[i].c_str());
            }
            #endif // DEBUG_END
            
            if (json_output) {
                return {build_json_array(full_segments)};
            }
            return full_segments;
        }
    }
    
    // Final return for non-match-only mode
    if (json_output) {
        if (extracted_matches.empty()) {
            return {"[]"};
        }
        bool already_json_objects = false;
        if (!extracted_matches.empty() && extracted_matches[0].find("\"match\"") != std::string::npos) {
            already_json_objects = true;
        }
        #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
        printf("DEBUG: Final return, extracted_matches.size(): %zu\n", extracted_matches.size());
        printf("DEBUG: ===== FUNCTION EXITING =====\n");
        #endif // DEBUG_END
        return {build_json_array(extracted_matches, already_json_objects)};
    }
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: ===== FUNCTION EXITING =====\n");
    #endif // DEBUG_END
    return extracted_matches;
}

// Test function to verify compilation
std::vector<std::string> test_function() {
    return {"test"};
} 