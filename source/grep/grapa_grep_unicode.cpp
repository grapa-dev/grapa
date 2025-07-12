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

#include "grapa_grep_unicode.hpp"
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

#ifdef _WIN32
#if defined(_M_ARM64)
#elif defined(_M_X64)
#pragma comment(lib, "pcre2-lib/win-amd64/pcre2-8-static.lib")
#endif
#endif

using namespace GrapaUnicode;

// Forward declaration for grapheme cluster extraction helper
static std::string extract_grapheme_cluster(const std::string& input, size_t offset);
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
    NormalizationForm norm = normalization;
    if (norm == NormalizationForm::NONE) {
        norm = NormalizationForm::NFC;
    }
    bool ignore_case = (options.find('i') != std::string::npos);
    bool all_mode = (options.find('a') != std::string::npos);
    bool match_only = ((options.find('o') != std::string::npos) ||
        (options.find('g') != std::string::npos) ||
        (options.find('b') != std::string::npos));
    bool count_only = (options.find('c') != std::string::npos);
    bool exact_match = (options.find('x') != std::string::npos);
    std::vector<MatchPosition> results;

    // If binary mode is requested, use standard regex without Unicode processing
    if (mode == ProcessingMode::BINARY_MODE) {
        // Use standard std::regex for binary processing
        std::regex::flag_type flags = std::regex::ECMAScript;
        if (ignore_case) flags |= std::regex::icase;
        // Enable multiline mode if available (for patterns with \n)
        bool multiline = (effective_pattern.find("\n") != std::string::npos);
        std::regex rx;
        try {
            rx = std::regex(effective_pattern, flags);
        }
        catch (const std::regex_error&) {
            return {};
        }

        if (all_mode) {
            // If a delimiter is provided, remove all instances of the delimiter for matching
            const std::string* match_input_ptr = &input;
            std::string normalized_input;
            if (!line_delim.empty()) {
                normalized_input.reserve(input.size());
                size_t pos = 0;
                while (pos < input.size()) {
                    size_t found = input.find(line_delim, pos);
                    if (found == std::string::npos) {
                        normalized_input.append(input, pos, input.size() - pos);
                        break;
                    }
                    else {
                        normalized_input.append(input, pos, found - pos);
                        pos = found + line_delim.size();
                    }
                }
                match_input_ptr = &normalized_input;
            }
            const std::string& match_input = *match_input_ptr;
            
            if (match_only) {
                for (std::sregex_iterator it(match_input.begin(), match_input.end(), rx), end; it != end; ++it) {
                    results.push_back({ static_cast<size_t>(it->position()), static_cast<size_t>(it->length()), 1 });
                }
            }
            else {
                if (std::regex_search(match_input, rx)) {
                    results.push_back({ 0, match_input.size(), 1 });
                }
            }
            return results;
        }

        std::string working_input = (line_delim.empty() && !all_mode) ? normalize_newlines(input) : input;
        size_t offset = 0;
        size_t line_number = 1;
        
        while (offset < working_input.size()) {
            size_t next = line_delim.empty()
                ? working_input.find('\n', offset)
                : working_input.find(line_delim, offset);
            if (next == std::string::npos) next = working_input.size();
            else next += line_delim.empty() ? 1 : line_delim.size();

            std::string_view line(working_input.data() + offset, next - offset);
            std::string line_copy(line);

            // Remove trailing newline for exact match
            if (exact_match && !line_copy.empty() && line_copy.back() == '\n') {
                line_copy.pop_back();
            }

            // In the standard output path (grep_unicode_impl and related), before matching:
            bool diacritic_insensitive = has_diacritic_insensitive(options);
            std::string norm_pattern = effective_pattern;
            std::string norm_line = line_copy;
            if (diacritic_insensitive) {
                // Always normalize and case fold before stripping diacritics
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
            unicode_rx.compile(norm_line); // Compile with the actual input to enable PCRE2
            // Use norm_line for matching.
            UnicodeString unicode_line(norm_line);
            
            // Debug: print normalized pattern and line
            bool matched = exact_match ? unicode_rx.match(unicode_line) : unicode_rx.search(unicode_line);
            
            if (matched) {
                if (!match_only) {
                    size_t line_len = line.size();
                    if (line.size() >= line_delim.size() &&
                        line.compare(line.size() - line_delim.size(), line_delim.size(), line_delim) == 0)
                    {
                        line_len -= line_delim.size();
                    }
                    results.push_back({ offset, line_len, line_number });
                }
                else {
                    if (exact_match) {
                        results.push_back({ offset, line.size(), line_number });
                    }
                    else {
                        // For match-only, group-by-line, and offset options, extract matches from the diacritic-stripped line if 'd' is present
                        std::vector<std::pair<size_t, size_t>> matches;
                        std::string match_extract_line = line_copy;
                        if (norm != GrapaUnicode::NormalizationForm::NONE) {
                            match_extract_line = GrapaUnicode::UnicodeRegex::get_normalized_text(line_copy, ignore_case, norm);
                        }
                        UnicodeString match_extract_unicode_line(match_extract_line);
                        UnicodeRegex match_extract_rx(norm_pattern, ignore_case, diacritic_insensitive, norm);
                        match_extract_rx.compile(match_extract_line); // Compile with the actual input to enable PCRE2
                        matches = match_extract_rx.find_all(match_extract_unicode_line);
                        // Map normalized match offsets back to original line
                        std::string cached_normalized_line;
                        if (!unicode_rx.is_ascii_mode()) {
                            if (diacritic_insensitive) {
                                cached_normalized_line = GrapaUnicode::UnicodeString(line_copy).normalize(norm).case_fold().data();
                                cached_normalized_line = GrapaUnicode::strip_diacritics(cached_normalized_line);
                            } else if (ignore_case) {
                                cached_normalized_line = GrapaUnicode::UnicodeString(line_copy).normalize(norm).case_fold().data();
                            } else if (norm != GrapaUnicode::NormalizationForm::NONE) {
                                cached_normalized_line = GrapaUnicode::UnicodeString(line_copy).normalize(norm).data();
                            }
                        }
                        for (const auto& m : matches) {
                            auto mapped = UnicodeRegex::map_normalized_span_to_original(line_copy, match_extract_line, m.first, m.second);
                            results.push_back({ offset + mapped.first, mapped.second, line_number });
                        }
                    }
                }
            }
            offset = next;
            ++line_number;
        }

        return results;
    }

    // Fast path for catastrophic backtracking cases: single character with + quantifier on large inputs
    if (effective_pattern.size() >= 2 && effective_pattern.back() == '+' && input.size() > 10000) {
        std::string base_char = effective_pattern.substr(0, effective_pattern.size() - 1);
        
        // Check if the entire input consists of the base character
        bool all_same = true;
        for (size_t i = 0; i < input.size(); i += base_char.size()) {
            if (i + base_char.size() > input.size() || 
                input.compare(i, base_char.size(), base_char) != 0) {
                all_same = false;
                break;
            }
        }
        
        if (all_same) {
            // Return the entire input as a match
            results.push_back({0, input.size(), 1});
            return results;
        }
    }

    // Create Unicode-aware regex for Unicode mode
    UnicodeRegex unicode_rx(effective_pattern, ignore_case, false, norm);
    unicode_rx.compile(input); // Compile with the actual input to enable PCRE2
    
    /* Check if regex compilation was successful */
    if (!unicode_rx.is_valid()) {
        // Return a special result to indicate compilation failure
        // We'll use a single match with offset -1 to indicate error
        return {{static_cast<size_t>(-1), 0, 0}};
    }

    if (all_mode) {
        // If a delimiter is provided, remove all instances of the delimiter for matching
        const std::string* match_input_ptr = &input;
        std::string normalized_input;
        if (!line_delim.empty()) {
            normalized_input.reserve(input.size());
            size_t pos = 0;
            while (pos < input.size()) {
                size_t found = input.find(line_delim, pos);
                if (found == std::string::npos) {
                    normalized_input.append(input, pos, input.size() - pos);
                    break;
                }
                else {
                    normalized_input.append(input, pos, found - pos);
                    pos = found + line_delim.size();
                }
            }
            match_input_ptr = &normalized_input;
        }
        const std::string& match_input = *match_input_ptr;
        
        // Cache normalized input to avoid repeated normalization
        std::string cached_normalized_input;
        if (!unicode_rx.is_ascii_mode()) {
            cached_normalized_input = UnicodeRegex::get_normalized_text(match_input, ignore_case, norm);
        }
        
        if (match_only) {
            // For all-mode with match-only, use the processed pattern and input
            bool diacritic_insensitive = has_diacritic_insensitive(options);
            std::string norm_pattern = effective_pattern;
            std::string norm_input = match_input;
            if (diacritic_insensitive) {
                norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).case_fold().data();
                norm_input = GrapaUnicode::UnicodeString(norm_input).normalize(norm).case_fold().data();
                norm_pattern = GrapaUnicode::strip_diacritics(norm_pattern);
                norm_input = GrapaUnicode::strip_diacritics(norm_input);
            } else if (ignore_case) {
                norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).case_fold().data();
                norm_input = GrapaUnicode::UnicodeString(norm_input).normalize(norm).case_fold().data();
            } else if (norm != GrapaUnicode::NormalizationForm::NONE) {
                norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).data();
                norm_input = GrapaUnicode::UnicodeString(norm_input).normalize(norm).data();
            }
            
            UnicodeRegex match_extract_rx(norm_pattern, ignore_case, diacritic_insensitive, norm);
            match_extract_rx.compile(norm_input);
            std::vector<std::pair<size_t, size_t>> matches = match_extract_rx.find_all(UnicodeString(norm_input));
            
            // Map normalized match offsets back to original input
            for (const auto& m : matches) {
                auto mapped = UnicodeRegex::map_normalized_span_to_original(match_input, norm_input, m.first, m.second);
                results.push_back({ mapped.first, mapped.second, 1 });
            }
        }
        else {
            if (unicode_rx.search(UnicodeString(match_input))) {
                // Whole input matches: map full span
                auto mapped = UnicodeRegex::map_normalized_span_to_original(match_input, cached_normalized_input.empty() ? match_input : cached_normalized_input, 0, match_input.size());
                results.push_back({ mapped.first, mapped.second, 1 });
            }
        }
        return results;
    }

    std::string working_input = (line_delim.empty() && !all_mode) ? normalize_newlines(input) : input;
    size_t offset = 0;
    size_t line_number = 1;
    
    while (offset < working_input.size()) {
        size_t next = line_delim.empty()
            ? working_input.find('\n', offset)
            : working_input.find(line_delim, offset);
        if (next == std::string::npos) next = working_input.size();
        else next += line_delim.empty() ? 1 : line_delim.size();

        std::string_view line(working_input.data() + offset, next - offset);
        std::string line_copy(line);

        // Remove trailing newline for exact match
        if (exact_match && !line_copy.empty() && line_copy.back() == '\n') {
            line_copy.pop_back();
        }

        // In the standard output path (grep_unicode_impl and related), before matching:
        bool diacritic_insensitive = has_diacritic_insensitive(options);
        std::string norm_pattern = effective_pattern;
        std::string norm_line = line_copy;
        if (diacritic_insensitive) {
            // Always normalize and case fold before stripping diacritics
            norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).case_fold().data();
            norm_line = GrapaUnicode::UnicodeString(norm_line).normalize(norm).case_fold().data();
            // Strip diacritics from both pattern and line for diacritic-insensitive matching
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
        unicode_rx.compile(norm_line); // Compile with the actual input to enable PCRE2
        // Use norm_line for matching.
        UnicodeString unicode_line(norm_line);
        bool matched = exact_match ? unicode_rx.match(unicode_line) : unicode_rx.search(unicode_line);
        
        if (matched) {
            if (!match_only) {
                size_t line_len = line.size();
                if (line.size() >= line_delim.size() &&
                    line.compare(line.size() - line_delim.size(), line_delim.size(), line_delim) == 0)
                {
                    line_len -= line_delim.size();
                }
                results.push_back({ offset, line_len, line_number });
            }
            else {
                if (exact_match) {
                    if (matched) {
                        results.push_back({ offset, line.size(), line_number });
                    }
                }
                else {
                    // For match-only, group-by-line, and offset options, extract matches from the processed line
                    std::vector<std::pair<size_t, size_t>> matches;
                    UnicodeRegex match_extract_rx(norm_pattern, ignore_case, diacritic_insensitive, norm);
                    match_extract_rx.compile(norm_line); // Compile with the processed input
                    matches = match_extract_rx.find_all(UnicodeString(norm_line));
                    
                    // Map normalized match offsets back to original line
                    std::string cached_normalized_line;
                    if (!unicode_rx.is_ascii_mode()) {
                        if (diacritic_insensitive) {
                            cached_normalized_line = GrapaUnicode::UnicodeString(line_copy).normalize(norm).case_fold().data();
                            cached_normalized_line = GrapaUnicode::strip_diacritics(cached_normalized_line);
                        } else if (ignore_case) {
                            cached_normalized_line = GrapaUnicode::UnicodeString(line_copy).normalize(norm).case_fold().data();
                        } else if (norm != GrapaUnicode::NormalizationForm::NONE) {
                            cached_normalized_line = GrapaUnicode::UnicodeString(line_copy).normalize(norm).data();
                        }
                    }
                    
                    for (const auto& m : matches) {
                        auto mapped = UnicodeRegex::map_normalized_span_to_original(line_copy, norm_line, m.first, m.second);
                        results.push_back({ offset + mapped.first, mapped.second, line_number });
                    }
                }
            }
        }
        offset = next;
        ++line_number;
    }

    return results;
}

bool ends_with(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
        str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Helper to parse options string for context options
ContextOptions parse_context_options(const std::string& options, std::string& filtered_options) {
    ContextOptions ctx;
    filtered_options.clear();
    // Only the last context option (A, B, or C) should be applied, matching ripgrep
    int last_A = -1, last_B = -1, last_C = -1;
    int val_A = 0, val_B = 0, val_C = 0;
    for (size_t i = 0; i < options.size(); ++i) {
        char c = options[i];
        if (c == 'A' || c == 'B' || c == 'C') {
            int val = 0;
            size_t j = i + 1;
            while (j < options.size() && std::isdigit(options[j])) {
                val = val * 10 + (options[j] - '0');
                ++j;
            }
            if (j == i + 1) val = 1; // default to 1 if no number
            if (c == 'A') { last_A = i; val_A = val; }
            if (c == 'B') { last_B = i; val_B = val; }
            if (c == 'C') { last_C = i; val_C = val; }
            i = j - 1;
        } else {
            filtered_options += c;
        }
    }
    // Precedence: C > last(B) > last(A)
    ctx.after = 0;
    ctx.before = 0;
    ctx.context = 0;
    if (last_C != -1) {
        ctx.context = val_C;
        ctx.after = val_C;
        ctx.before = val_C;
    } else if (last_B > last_A) {
        ctx.before = val_B;
    } else if (last_A > last_B) {
        ctx.after = val_A;
    } else if (last_A != -1) {
        ctx.after = val_A;
    } else if (last_B != -1) {
        ctx.before = val_B;
    }
    // Cap large context numbers to prevent excessive memory usage
    const int MAX_CONTEXT = 1000;
    if (ctx.after > MAX_CONTEXT) ctx.after = MAX_CONTEXT;
    if (ctx.before > MAX_CONTEXT) ctx.before = MAX_CONTEXT;
    if (ctx.context > MAX_CONTEXT) ctx.context = MAX_CONTEXT;
    return ctx;
}

std::vector<std::string> grep_extract_matches_unicode_impl(
    const std::string& input,
    const std::string& effective_pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization,
    GrapaUnicode::ProcessingMode mode,
    size_t num_workers
) {
    
    // Determine optimal number of workers if not specified
    if (num_workers == 0) {
        // Auto-detect based on input size and available cores
        size_t input_size = input.size();
        size_t available_cores = static_cast<size_t>(std::thread::hardware_concurrency());
        
        // For small inputs, use single-threaded processing
        if (input_size < 1024 * 1024) { // 1MB
            num_workers = 1;
        }
        // For medium inputs, use 2-4 threads
        else if (input_size < 10 * 1024 * 1024) { // 10MB
            num_workers = (4 < available_cores) ? 4 : available_cores;
        }
        // For large inputs, use more threads but cap at available cores
        else {
            num_workers = (16 < available_cores) ? 16 : available_cores;
        }
    }
    
    // For single worker or small input, use sequential processing
    if (num_workers <= 1) {
        return grep_extract_matches_unicode_impl_sequential(input, effective_pattern, options, line_delim, normalization, mode);
    }
    
    // Parallel processing implementation
    return grep_extract_matches_unicode_impl_parallel(input, effective_pattern, options, line_delim, normalization, mode, num_workers);
}

// Sequential implementation (original logic)
std::vector<std::string> grep_extract_matches_unicode_impl_sequential(
    const std::string& input,
    const std::string& effective_pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization,
    GrapaUnicode::ProcessingMode mode
) {
    
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
        // o and x are mutually exclusive - o takes precedence (like ripgrep)
        std::string filtered_options = options;
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'x'), filtered_options.end());
        return grep_extract_matches_unicode_impl_sequential(input, effective_pattern, filtered_options, line_delim, normalization, mode);
    }
    
    // Ripgrep behavior: c (count) overrides other output options
    if (has_c && (has_o || has_l || has_n)) {
        std::string filtered_options = options;
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'o'), filtered_options.end());
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'l'), filtered_options.end());
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'n'), filtered_options.end());
        return grep_extract_matches_unicode_impl_sequential(input, effective_pattern, filtered_options, line_delim, normalization, mode);
    }
    
    // Ripgrep behavior: l (files with matches) overrides other output options
    if (has_l && (has_o || has_n)) {
        std::string filtered_options = options;
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'o'), filtered_options.end());
        filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'n'), filtered_options.end());
        return grep_extract_matches_unicode_impl_sequential(input, effective_pattern, filtered_options, line_delim, normalization, mode);
    }
    
    // Parse context options and filter them out
    std::string filtered_options;
    ContextOptions ctx = parse_context_options(options, filtered_options);
    
    // For all-mode, ignore context options
    if (filtered_options.find('a') != std::string::npos) {
        ctx.after = 0;
        ctx.before = 0;
        ctx.context = 0;
    }
    
    // Always use NFC if not explicitly set (for Unicode edge cases)
    NormalizationForm norm = normalization;
    if (norm == NormalizationForm::NONE) {
        norm = NormalizationForm::NFC;
    }
    
    // Remove 'N' from options since we've already processed it
    filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'N'), filtered_options.end());
    
    // Use the context options from the first parsing
    int after = ctx.after;
    int before = ctx.before;
    int context = ctx.context;
    
    // C<n> takes precedence and sets both before and after to n (ripgrep behavior)
    if (context > 0) {
        before = context;
        after = context;
    }
    bool all_mode = (filtered_options.find('a') != std::string::npos);
    bool match_only = (filtered_options.find('o') != std::string::npos);
    
    // Ripgrep behavior: ignore context options in all-mode
    if (all_mode) {
        before = 0;
        after = 0;
    }
    
    // Prepare working input
    std::string working_input;
    if (all_mode && !line_delim.empty()) {
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
        working_input = line_delim.empty() ? normalize_newlines(input) : input;
    }
    
    // Multiline mode: if pattern contains \n, apply regex to the entire input
    if (effective_pattern.find("\n") != std::string::npos) {
        // Normalize both input and pattern using the correct static method
        std::string norm_input = GrapaUnicode::UnicodeRegex::get_normalized_text(working_input, false, norm);
        std::string norm_pattern = GrapaUnicode::UnicodeRegex::get_normalized_text(effective_pattern, false, norm);
        // Use PCRE2 with UTF and DOTALL|MULTILINE
        uint32_t pcre2_options = PCRE2_UTF | PCRE2_DOTALL | PCRE2_MULTILINE;
        int pcre2_errorcode = 0;
        PCRE2_SIZE pcre2_erroroffset = 0;
        pcre2_code* re = pcre2_compile(
            reinterpret_cast<PCRE2_SPTR>(norm_pattern.c_str()),
            norm_pattern.length(),
            pcre2_options,
            &pcre2_errorcode, &pcre2_erroroffset, nullptr);
        if (!re) {
            throw std::runtime_error("Invalid regex pattern");
        }
        pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(re, nullptr);
        std::vector<std::string> matches;
        size_t offset = 0;
        while (true) {
            int rc = pcre2_match(
                re,
                reinterpret_cast<PCRE2_SPTR>(norm_input.c_str()),
                norm_input.length(),
                offset,
                0,
                match_data,
                nullptr);
            if (rc < 0) break;
            PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
            size_t start = ovector[0];
            size_t end = ovector[1];
            matches.push_back(norm_input.substr(start, end - start));
            if (end == offset) break; // Prevent infinite loop on zero-length match
            offset = end;
        }
        pcre2_match_data_free(match_data);
        pcre2_code_free(re);
        return matches;
    }

    // Normalize both pattern and input before matching
    std::string norm_pattern = GrapaUnicode::UnicodeRegex::get_normalized_text(effective_pattern, false, norm);
    std::string norm_input = GrapaUnicode::UnicodeRegex::get_normalized_text(working_input, false, norm);
    // Use norm_pattern and norm_input for all regex operations
    std::string effective_pattern_for_impl = effective_pattern;
    if (filtered_options.find('w') != std::string::npos) {
        if (effective_pattern_for_impl.substr(0,2) != "\\b") effective_pattern_for_impl = "\\b" + effective_pattern_for_impl;
        if (effective_pattern_for_impl.size() < 2 || effective_pattern_for_impl.substr(effective_pattern_for_impl.size()-2) != "\\b") effective_pattern_for_impl += "\\b";
    }
    auto matches = grep_unicode_impl(working_input, effective_pattern_for_impl, filtered_options, line_delim, norm, mode);
    
    // Check for compilation error (indicated by offset -1)
    if (!matches.empty() && matches[0].offset == static_cast<size_t>(-1)) {
        /* Return empty result instead of throwing exception for invalid patterns */
        return {};
    }
    
    // Handle empty pattern case
    if (effective_pattern.empty()) {
        /* Return empty result instead of throwing exception for empty patterns */
        return {};
    }
    
    // Extract matches from positions
    std::vector<std::string> extracted_matches;
    bool json_output = (filtered_options.find('j') != std::string::npos);
    bool invert_match = (filtered_options.find('v') != std::string::npos);
    
    // 1. Parse new options
    bool show_column = filtered_options.find('T') != std::string::npos;
    bool word_boundary = filtered_options.find('w') != std::string::npos;
    bool null_data = filtered_options.find('z') != std::string::npos;
    bool color_output = filtered_options.find('L') != std::string::npos;
    
    // 2. If word_boundary, wrap pattern with \b if not already present
    std::string effective_pattern_for_output = effective_pattern;
    if (word_boundary) {
        if (effective_pattern_for_output.substr(0,2) != "\\b") effective_pattern_for_output = "\\b" + effective_pattern_for_output;
        if (effective_pattern_for_output.size() < 2 || effective_pattern_for_output.substr(effective_pattern_for_output.size()-2) != "\\b") effective_pattern_for_output += "\\b";
    }
    
    // 3. If null_data, split input on '\0' instead of '\n'
    std::vector<std::string> lines;
    std::vector<size_t> line_offsets; // Declare once, before splitting
    if (null_data) {
        size_t start = 0, end;
        while ((end = working_input.find('\0', start)) != std::string::npos) {
            lines.push_back(working_input.substr(start, end - start));
            line_offsets.push_back(start); // Track offset for each record
            start = end + 1;
        }
        if (start < working_input.size()) {
            lines.push_back(working_input.substr(start));
            line_offsets.push_back(start);
        }
    } else {
        size_t pos = 0;
        while (pos < working_input.size()) {
            size_t next = line_delim.empty() 
                ? working_input.find('\n', pos)
                : working_input.find(line_delim, pos);
            if (next == std::string::npos) next = working_input.size();
            else next += line_delim.empty() ? 1 : line_delim.size();
            std::string line = working_input.substr(pos, next - pos);
            // Remove delimiter from the line content for output
            if (!line_delim.empty() && line.length() >= line_delim.length() && 
                line.substr(line.length() - line_delim.length()) == line_delim) {
                line = line.substr(0, line.length() - line_delim.length());
            }
            lines.push_back(line);
            line_offsets.push_back(pos);
            pos = next;
        }
        // If input ends with a newline, add an extra empty line (ripgrep/grep behavior)
        if (!working_input.empty() && ((line_delim.empty() && working_input.back() == '\n') ||
            (!line_delim.empty() && working_input.size() >= line_delim.size() && working_input.substr(working_input.size() - line_delim.size()) == line_delim))) {
            lines.push_back("");
            line_offsets.push_back(working_input.size());
        }
    }
    // Special case: if pattern is '^' and match_only, return one empty string per line
    if (match_only && effective_pattern == "^") {
        std::vector<std::string> starts;
        for (size_t i = 0; i < lines.size(); ++i) {
            starts.push_back("");
        }
        return starts;
    }
    
    // Special case: if pattern is exactly '\X' and match_only ('o') is set, segment input into grapheme clusters
    if (match_only && effective_pattern == "\\X") {
        std::vector<std::string> graphemes;
        size_t offset = 0;
        while (offset < working_input.size()) {
            std::string cluster = extract_grapheme_cluster(working_input, offset);
            if (cluster.empty()) break;
            graphemes.push_back(GrapaUnicode::UnicodeRegex::get_normalized_text(cluster, false, GrapaUnicode::NormalizationForm::NFC));
            offset += cluster.size(); /* Always advance by the size of the cluster */
        }
        return graphemes;
    }
    
    // Find which lines contain matches
    std::set<size_t> matching_lines;
    for (const auto& match : matches) {
        if (match.offset < working_input.size()) {
            // Find which line this match belongs to
            for (size_t i = 0; i < line_offsets.size(); ++i) {
                size_t line_start = line_offsets[i];
                size_t line_end = (i + 1 < line_offsets.size()) ? line_offsets[i + 1] : working_input.size();
                if (match.offset >= line_start && match.offset < line_end) {
                    matching_lines.insert(i);
                    break;
                }
            }
        }
    }
    
    // Find which lines do not contain matches (for invert match)
    std::set<size_t> non_matching_lines;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (matching_lines.find(i) == matching_lines.end()) {
            non_matching_lines.insert(i);
        }
    }

    // Compute context lines if needed
    bool has_context = (before > 0) || (after > 0);
    if (has_context) {
        // For each (non-)matching line, collect its context window
        const std::set<size_t>& target_lines = invert_match ? non_matching_lines : matching_lines;
        std::vector<std::pair<size_t, size_t>> context_windows;
        for (size_t line_idx : target_lines) {
            size_t start = (line_idx >= static_cast<size_t>(before)) ? line_idx - before : 0;
            size_t end = std::min(line_idx + after, lines.size() - 1);
            context_windows.push_back({start, end});
        }
        // Sort and merge overlapping/adjacent windows
        std::sort(context_windows.begin(), context_windows.end());
        std::vector<std::pair<size_t, size_t>> merged_windows;
        for (const auto& win : context_windows) {
            if (merged_windows.empty() || win.first > merged_windows.back().second + 1) {
                merged_windows.push_back(win);
            } else {
                merged_windows.back().second = std::max(merged_windows.back().second, win.second);
            }
        }
        // Output lines with --\n between non-overlapping blocks
        for (size_t w = 0; w < merged_windows.size(); ++w) {
            for (size_t i = merged_windows[w].first; i <= merged_windows[w].second; ++i) {
                // For invert match, skip lines that are matches
                if (invert_match && matching_lines.find(i) != matching_lines.end()) continue;
                extracted_matches.push_back(lines[i]);
            }
            if (w + 1 < merged_windows.size()) {
                extracted_matches.push_back("--\n");
            }
        }
        return extracted_matches;
    }
    
    // Output lines in order (like ripgrep)
    // for (const auto& [line_idx, is_match] : final_lines_with_context) {
    //     extracted_matches.push_back(lines[line_idx]);
    // }
    
    // Output logic for matches
    for (const auto& match : matches) {
        // Bounds check: ensure match.offset and match.length are valid
        if ((match.offset <= working_input.size()) && (match.length <= working_input.size()) && (match.offset + match.length <= working_input.size())) {
            std::string matched_text;
            // Use grapheme cluster extraction for \X pattern
            if (effective_pattern == "\\X") {
                matched_text = extract_grapheme_cluster(working_input, match.offset);
            } else {
                size_t end_offset = match.offset + match.length;
                matched_text = working_input.substr(match.offset, end_offset - match.offset);
            }
            // Normalize output to NFC for display consistency
            matched_text = GrapaUnicode::UnicodeRegex::get_normalized_text(matched_text, false, GrapaUnicode::NormalizationForm::NFC);
            // If color_output, wrap only the matched substring in ANSI color codes
            if (color_output) {
                matched_text = "\x1b[1;31m" + matched_text + "\x1b[0m";
            }
            // Handle zero-length matches: always add empty string, never null
            if (match.length == 0) {
                extracted_matches.push_back("");
            } else {
                // Ensure we never add null values - use empty string as fallback
                if (matched_text.empty()) {
                    extracted_matches.push_back("");
                } else {
                    extracted_matches.push_back(matched_text);
                }
            }
        } else {
            // If mapping is invalid, push empty string (never crash)
            extracted_matches.push_back("");
        }
    }
    // Final safety check: replace any remaining null-like values with empty strings
    for (auto& match : extracted_matches) {
        if (match.empty() && match != "") {
            match = "";
        }
    }
    
    // Add context separator if context options are used
    if (has_context) {
        if (json_output) {
            extracted_matches.push_back("---");
        } else {
            extracted_matches.push_back("---\n");
        }
    }
    
    return extracted_matches;
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
                return grep_extract_matches_unicode_impl_sequential(
                    chunks[i], effective_pattern, options, line_delim, normalization, mode);
            }));
    }
    
    // Collect results from all async tasks
    std::vector<std::string> all_matches;
    for (size_t i = 0; i < futures.size(); i++) {
        auto result = futures[i].get();
        all_matches.insert(all_matches.end(), result.begin(), result.end());
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
        if (normalization_ != NormalizationForm::NONE) {
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
    } else if (normalization_ != NormalizationForm::NONE) {
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
        std::string match_text = text.data();
        if (normalization_ != NormalizationForm::NONE) {
            match_text = get_normalized_text(text.data(), case_insensitive_, normalization_);
        }
        int result = pcre2_match(
            pcre_regex_,
            reinterpret_cast<PCRE2_SPTR>(match_text.c_str()),
            static_cast<PCRE2_SIZE>(match_text.length()),
            0, PCRE2_ANCHORED, /* PCRE2_ANCHORED for full match */
            pcre_match_data_,
            nullptr
        );
        return result >= 0;
    }
#endif
    if (is_ascii_only_ && is_ascii_string(text.data())) {
        return std::regex_match(text.data(), regex_);
    } else if (normalization_ != NormalizationForm::NONE) {
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
        bool needs_normalization = (normalization_ != NormalizationForm::NONE);
        
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
    } else if (normalization_ != NormalizationForm::NONE) {
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

UnicodeRegex::UnicodeRegex(const std::string& pattern, bool case_insensitive, bool diacritic_insensitive, NormalizationForm norm)
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
    // If pattern or input is non-ASCII, always use PCRE2
    if (!is_ascii_string(pattern_to_compile) || !is_ascii_string(input)) {
        use_pcre_ = true;
    }
#endif
    if (use_pcre_) {
        std::string pattern_utf8 = parse_unicode_escapes(pattern_to_compile);
        int error_code = 0;
        PCRE2_SIZE error_offset = 0;
        uint32_t compile_flags = PCRE2_UTF | PCRE2_UCP | PCRE2_JIT_COMPLETE;
        if (case_insensitive_) {
            compile_flags |= PCRE2_CASELESS;
        }
        // Always enable multiline and dotall for ripgrep parity
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

bool UnicodeRegex::has_unicode_properties(const std::string& pattern) {
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
    std::map<std::tuple<std::string, bool, NormalizationForm>, std::string> UnicodeRegex::text_cache_;
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
    results = grep_extract_matches_unicode_impl(
        input_chunk, effective_pattern, options, line_delim, normalization, mode);
}

void GrapaGrepWorkEvent::Stopping() {
    // Clean up - no special cleanup needed for grep
}

// Helper: Extract a single grapheme cluster at a given byte offset from a UTF-8 string
static std::string extract_grapheme_cluster(const std::string& input, size_t offset) {
    size_t input_len = input.size();
    if (offset >= input_len) return "";
    size_t start = offset;
    size_t end = offset;
    utf8proc_int32_t cp;
    utf8proc_ssize_t char_len = utf8proc_iterate(reinterpret_cast<const utf8proc_uint8_t*>(input.c_str()) + start, input_len - start, &cp);
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
    return input.substr(start, end - start);
}

 
 