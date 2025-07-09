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
#include <fstream> // Added for logging PCRE2 errors

#ifdef _WIN32
#if defined(_M_ARM64)
#elif defined(_M_X64)
#pragma comment(lib, "pcre2-lib/win-amd64/pcre2-8-static.lib")
#endif
#endif

using namespace GrapaUnicode;

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
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization,
    GrapaUnicode::ProcessingMode mode
) {
    // The normalization parameter always takes precedence over the 'N' option in the options string.
    // If normalization is NONE (the default) and options contains 'N', set to NFC.
    NormalizationForm norm = normalization;
    if (norm == NormalizationForm::NONE && options.find('N') != std::string::npos) {
        norm = NormalizationForm::NFC;
    }
    bool ignore_case = (options.find('i') != std::string::npos);
    bool invert_match = (options.find('v') != std::string::npos);
    bool all_mode = (options.find('a') != std::string::npos);
    bool match_only = ((options.find('o') != std::string::npos) ||
        (options.find('g') != std::string::npos) ||
        (options.find('b') != std::string::npos)) && !invert_match;
    bool count_only = (options.find('c') != std::string::npos);
    bool exact_match = (options.find('x') != std::string::npos);
    std::vector<MatchPosition> results;

    // If binary mode is requested, use standard regex without Unicode processing
    if (mode == ProcessingMode::BINARY_MODE) {
        // Use standard std::regex for binary processing
        std::regex::flag_type flags = std::regex::ECMAScript;
        if (ignore_case) flags |= std::regex::icase;

        std::regex rx;
        try {
            rx = std::regex(pattern, flags);
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
            
            if (invert_match) {
                std::vector<MatchPosition> matches;
                for (std::sregex_iterator it(match_input.begin(), match_input.end(), rx), end; it != end; ++it) {
                    matches.push_back({ static_cast<size_t>(it->position()), static_cast<size_t>(it->length()), 1 });
                }
                size_t prev_end = 0;
                for (const auto& m : matches) {
                    if (m.offset > prev_end) {
                        results.push_back({ prev_end, m.offset - prev_end, 1 });
                    }
                    prev_end = m.offset + m.length;
                }
                if (prev_end < match_input.size()) {
                    results.push_back({ prev_end, match_input.size() - prev_end, 1 });
                }
            }
            else {
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

            bool matched = exact_match ? std::regex_match(line_copy, rx) : std::regex_search(line_copy, rx);
            
            if ((!invert_match && matched) || (invert_match && !matched)) {
                if (!match_only || invert_match) {
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
                        for (std::sregex_iterator it(line_copy.begin(), line_copy.end(), rx), end; it != end; ++it) {
                            results.push_back({ offset + static_cast<size_t>(it->position()), static_cast<size_t>(it->length()), line_number });
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
    if (pattern.size() >= 2 && pattern.back() == '+' && input.size() > 10000) {
        std::string base_char = pattern.substr(0, pattern.size() - 1);
        
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
    UnicodeRegex unicode_rx(pattern, ignore_case, false, norm);
    
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
        
        if (invert_match) {
            std::vector<std::pair<size_t, size_t>> matches = unicode_rx.find_all(UnicodeString(match_input));
            size_t prev_end = 0;
            for (const auto& m : matches) {
                if (m.first > prev_end) {
                    // Map normalized span to original
                    auto mapped = UnicodeRegex::map_normalized_span_to_original(match_input, cached_normalized_input.empty() ? match_input : cached_normalized_input, prev_end, m.first - prev_end);
                    results.push_back({ mapped.first, mapped.second, 1 });
                }
                prev_end = m.first + m.second;
            }
            if (prev_end < match_input.size()) {
                auto mapped = UnicodeRegex::map_normalized_span_to_original(match_input, cached_normalized_input.empty() ? match_input : cached_normalized_input, prev_end, match_input.size() - prev_end);
                results.push_back({ mapped.first, mapped.second, 1 });
            }
        }
        else {
            if (match_only) {
                std::vector<std::pair<size_t, size_t>> matches = unicode_rx.find_all(UnicodeString(match_input));
                for (const auto& m : matches) {
                    auto mapped = UnicodeRegex::map_normalized_span_to_original(match_input, cached_normalized_input.empty() ? match_input : cached_normalized_input, m.first, m.second);
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
        std::string norm_pattern = pattern;
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
        // Use norm_line for matching.
        UnicodeString unicode_line(norm_line);
        bool matched = exact_match ? unicode_rx.match(unicode_line) : unicode_rx.search(unicode_line);
        
        if ((!invert_match && matched) || (invert_match && !matched)) {
            if (!match_only || invert_match) {
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
                    // For match-only, group-by-line, and offset options, extract matches from the diacritic-stripped line if 'd' is present
                    std::vector<std::pair<size_t, size_t>> matches;
                    std::string match_extract_line = line_copy;
                    if (norm != GrapaUnicode::NormalizationForm::NONE) {
                        match_extract_line = GrapaUnicode::UnicodeRegex::get_normalized_text(line_copy, ignore_case, norm);
                    }
                    UnicodeString match_extract_unicode_line(match_extract_line);
                    UnicodeRegex match_extract_rx(norm_pattern, ignore_case, diacritic_insensitive, norm);
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
                        auto mapped = UnicodeRegex::map_normalized_span_to_original(line_copy, cached_normalized_line.empty() ? line_copy : cached_normalized_line, m.first, m.second);
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
            if (c == 'A') ctx.after = val;
            else if (c == 'B') ctx.before = val;
            else if (c == 'C') ctx.context = val;
            i = j - 1;
        } else {
            filtered_options += c;
        }
    }
    return ctx;
}

std::vector<std::string> grep_extract_matches_unicode_impl(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization,
    GrapaUnicode::ProcessingMode mode
) {
    
    // The normalization parameter always takes precedence over the 'N' option in the options string.
    // If normalization is NONE (the default) and options contains 'N', set to NFC.
    NormalizationForm norm = normalization;
    if (norm == NormalizationForm::NONE && options.find('N') != std::string::npos) {
        norm = NormalizationForm::NFC;
    }
    
    std::string filtered_options = options;
    // Remove 'N' from options since we've already processed it
    filtered_options.erase(std::remove(filtered_options.begin(), filtered_options.end(), 'N'), filtered_options.end());
    
    // Parse context options
    std::string context_filtered_options;
    ContextOptions ctx = parse_context_options(filtered_options, context_filtered_options);
    filtered_options = context_filtered_options;
    int after = ctx.after;
    int before = ctx.before;
    int context = ctx.context;
    if (context > 0) {
        if (context > after) after = context;
        if (context > before) before = context;
    }
    bool all_mode = (filtered_options.find('a') != std::string::npos);
    bool match_only = (filtered_options.find('o') != std::string::npos);
    
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
    
    // Get matches
    auto matches = grep_unicode_impl(working_input, pattern, filtered_options, line_delim, norm, mode);
    
    // Check for compilation error (indicated by offset -1)
    if (!matches.empty() && matches[0].offset == static_cast<size_t>(-1)) {
        // Return a special error indicator that will be converted to $ERR at the Grapa level
        return {"__COMPILATION_ERROR__"};
    }
    
    // Parse additional options
    bool count_only = (filtered_options.find('c') != std::string::npos);
    bool dedupe = (filtered_options.find('u') != std::string::npos);
    bool include_line_numbers = (filtered_options.find('n') != std::string::npos);
    bool group_by_line = (filtered_options.find('g') != std::string::npos);
    bool output_offset = (filtered_options.find('b') != std::string::npos);
    bool line_only = (filtered_options.find('l') != std::string::npos);
    bool strip_trailing_newline = line_delim.empty() && filtered_options.find('a') == std::string::npos;
    std::string actual_delim = (line_delim == "\\n") ? "\n" : line_delim;
    std::set<std::string> unique;
    std::vector<std::string> out;
    
    bool json_output = filtered_options.find('j') != std::string::npos;
    std::string regex_options = filtered_options;
    bool exact_line = regex_options.find('x') != std::string::npos;
    if (json_output) {
        // Remove 'j' and 'x' from options before using them for matching
        regex_options.erase(std::remove(regex_options.begin(), regex_options.end(), 'j'), regex_options.end());
        regex_options.erase(std::remove(regex_options.begin(), regex_options.end(), 'x'), regex_options.end());
    }
    
    if (json_output) {
        try {
            if (exact_line) {
                // Split input into lines and match each line separately
                size_t pos = 0, start = 0, line_num = 1;
                std::vector<std::string> json_matches;
                while (pos <= input.size()) {
                    size_t next = input.find('\n', pos);
                    if (next == std::string::npos) next = input.size();
                    std::string line = input.substr(pos, next - pos);
                    // Normalize the line and pattern if needed
                    bool diacritic_insensitive = has_diacritic_insensitive(regex_options);
                    std::string norm_line = line;
                    std::string norm_pattern = pattern;
                    if (diacritic_insensitive) {
                        norm_line = GrapaUnicode::UnicodeString(norm_line).normalize(norm).case_fold().data();
                        norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).case_fold().data();
                        norm_line = GrapaUnicode::strip_diacritics(norm_line);
                        norm_pattern = GrapaUnicode::strip_diacritics(norm_pattern);
                    }
                    else if (regex_options.find('i') != std::string::npos) {
                        norm_line = GrapaUnicode::UnicodeString(norm_line).normalize(norm).case_fold().data();
                        norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).case_fold().data();
                    }
                    else if (norm != GrapaUnicode::NormalizationForm::NONE) {
                        norm_line = GrapaUnicode::UnicodeString(norm_line).normalize(norm).data();
                        norm_pattern = GrapaUnicode::UnicodeString(norm_pattern).normalize(norm).data();
                    }
                    GrapaUnicode::UnicodeRegex unicode_rx(norm_pattern, regex_options.find('i') != std::string::npos, diacritic_insensitive, norm);
                    if (unicode_rx.is_pcre_valid()) {
#ifdef USE_PCRE
                        pcre2_code* re = unicode_rx.get_pcre_code();
                        if (re != nullptr) {
                            pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(re, NULL);
                            if (match_data != nullptr) {
                                int rc = pcre2_match(
                                    re,
                                    reinterpret_cast<PCRE2_SPTR>(norm_line.c_str()),
                                    norm_line.length(),
                                    0,
                                    0,
                                    match_data,
                                    NULL
                                );
                                if (rc >= 0) {
                                    std::string json = "{";
                                    json += "\"match\":\"";
                                    for (size_t i = 0; i < norm_line.length(); ++i) {
                                        unsigned char c = static_cast<unsigned char>(norm_line[i]);
                                        if (c == '\\' || c == '"') json += '\\';
                                        if (c == '\n') json += "\\n";
                                        else if (c == '\r') json += "\\r";
                                        else if (c == '\t') json += "\\t";
                                        else if (c == '\b') json += "\\b";
                                        else if (c == '\f') json += "\\f";
                                        else if (c < 0x20) {
                                            char buf[7];
                                            snprintf(buf, sizeof(buf), "\\u%04x", c);
                                            json += buf;
                                        }
                                        else {
                                            json += norm_line[i];
                                        }
                                    }
                                    json += "\"";
                                    // Add offset (start of line in input) and line number
                                    json += ",\"offset\":" + std::to_string(pos);
                                    json += ",\"line\":" + std::to_string(line_num);
                                    json += "}";
                                    json_matches.push_back(json);
                                }
                                pcre2_match_data_free(match_data);
                            }
                        }
#endif
                    }
                    if (next == input.size()) break;
                    pos = next + 1;
                    ++line_num;
                }
                std::string json_array = "[";
                for (size_t i = 0; i < json_matches.size(); ++i) {
                    if (i > 0) json_array += ",";
                    json_array += json_matches[i];
                }
                json_array += "]";
                return { json_array };
            }
        }
        catch (...) {
            /* If JSON output fails, fall back to regular output */
        }

        /* Fallback: output as array of matches (no named groups) */
        std::string search_text = input;
        std::string norm_pattern = pattern;
        if (norm != GrapaUnicode::NormalizationForm::NONE) {
            search_text = GrapaUnicode::UnicodeRegex::get_normalized_text(input, regex_options.find('i') != std::string::npos, norm);
            norm_pattern = GrapaUnicode::UnicodeRegex::get_normalized_text(pattern, regex_options.find('i') != std::string::npos, norm);
        }
        bool diacritic_insensitive = has_diacritic_insensitive(regex_options);
        std::string search_text_for_matching = search_text;
        if (diacritic_insensitive) {
            search_text_for_matching = GrapaUnicode::strip_diacritics(search_text);
            norm_pattern = GrapaUnicode::strip_diacritics(norm_pattern);
        }
        GrapaUnicode::UnicodeRegex unicode_rx(norm_pattern, regex_options.find('i') != std::string::npos, diacritic_insensitive, norm);
        std::vector<MatchPosition> matches;
        if (unicode_rx.is_pcre_valid()) {
#ifdef USE_PCRE
            pcre2_code* re = unicode_rx.get_pcre_code();
            if (re != nullptr) {
                pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(re, NULL);
                if (match_data != nullptr) {
                    PCRE2_SIZE offset = 0;
                    PCRE2_SIZE length = static_cast<PCRE2_SIZE>(search_text_for_matching.length());
                    // Get named group info
                    uint32_t namecount = 0;
                    pcre2_pattern_info(re, PCRE2_INFO_NAMECOUNT, &namecount);
                    PCRE2_SPTR nametable = NULL;
                    uint32_t name_entry_size = 0;
                    if (namecount > 0) {
                        pcre2_pattern_info(re, PCRE2_INFO_NAMETABLE, &nametable);
                        pcre2_pattern_info(re, PCRE2_INFO_NAMEENTRYSIZE, &name_entry_size);
                    }
                    std::vector<std::pair<std::string, uint32_t>> named_groups;
                    for (uint32_t i = 0; i < namecount; ++i) {
                        PCRE2_SPTR entry = nametable + i * name_entry_size;
                        uint32_t group_num = (entry[0] << 8) | entry[1];
                        std::string group_name(reinterpret_cast<const char*>(entry + 2));
                        named_groups.emplace_back(group_name, group_num);
                    }
                    std::vector<std::string> json_matches;
                    while (offset <= length) {
                        int rc = pcre2_match(
                            re,
                            reinterpret_cast<PCRE2_SPTR>(search_text_for_matching.c_str()),
                            length,
                            offset,
                            0,
                            match_data,
                            nullptr
                        );
                        if (rc < 0) break;
                        PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
                        size_t orig_start = ovector[0];
                        size_t orig_len = ovector[1] - ovector[0];
                        // Map normalized span to original input
                        std::string cached_normalized_input = search_text;
                        if (diacritic_insensitive) {
                            cached_normalized_input = GrapaUnicode::strip_diacritics(cached_normalized_input);
                        }
                        auto mapped = GrapaUnicode::UnicodeRegex::map_normalized_span_to_original(search_text, cached_normalized_input, orig_start, orig_len);
                        // Calculate line number for this match
                        size_t match_offset = mapped.first;
                        size_t line_num = 1;
                        for (size_t i = 0; i < match_offset && i < input.size(); ++i) {
                            if (input[i] == '\n') ++line_num;
                        }
                        std::string match = search_text.substr(mapped.first, mapped.second);
                        std::string json = "{";
                        json += "\"match\":\"";
                        for (size_t j = 0; j < match.length(); ++j) {
                            unsigned char c = static_cast<unsigned char>(match[j]);
                            if (c == '\\' || c == '"') json += '\\';
                            if (c == '\n') json += "\\n";
                            else if (c == '\r') json += "\\r";
                            else if (c == '\t') json += "\\t";
                            else if (c == '\b') json += "\\b";
                            else if (c == '\f') json += "\\f";
                            else if (c < 0x20) {
                                char buf[7];
                                snprintf(buf, sizeof(buf), "\\u%04x", c);
                                json += buf;
                            }
                            else {
                                json += match[j];
                            }
                        }
                        json += '"';
                        // Add named groups
                        for (const auto& ng : named_groups) {
                            uint32_t group_num = ng.second;
                            std::string group_name = ng.first;
                            json += ",\"" + group_name + "\":";
                            if (group_num < rc && ovector[2 * group_num] != PCRE2_UNSET && ovector[2 * group_num + 1] != PCRE2_UNSET) {
                                size_t gstart = ovector[2 * group_num];
                                size_t gend = ovector[2 * group_num + 1];
                                std::string group_val = search_text_for_matching.substr(gstart, gend - gstart);
                                json += '"';
                                for (size_t k = 0; k < group_val.length(); ++k) {
                                    unsigned char c = static_cast<unsigned char>(group_val[k]);
                                    if (c == '\\' || c == '"') json += '\\';
                                    if (c == '\n') json += "\\n";
                                    else if (c == '\r') json += "\\r";
                                    else if (c == '\t') json += "\\t";
                                    else if (c == '\b') json += "\\b";
                                    else if (c == '\f') json += "\\f";
                                    else if (c < 0x20) {
                                        char buf[7];
                                        snprintf(buf, sizeof(buf), "\\u%04x", c);
                                        json += buf;
                                    }
                                    else {
                                        json += group_val[k];
                                    }
                                }
                                json += '"';
                            }
                            else {
                                json += "null";
                            }
                        }
                        json += ",\"offset\":" + std::to_string(match_offset);
                        json += ",\"line\":" + std::to_string(line_num);
                        json += "}";
                        json_matches.push_back(json);
                        if (ovector[1] > offset) {
                            offset = ovector[1];
                        }
                        else {
                            ++offset;
                        }
                    }
                    pcre2_match_data_free(match_data);
                    std::string json_array = "[";
                    for (size_t i = 0; i < json_matches.size(); ++i) {
                        if (i > 0) json_array += ",";
                        json_array += json_matches[i];
                    }
                    json_array += "]";
                    return { json_array };
                }
            }
#endif
        }
    }

    // If context options are set, only apply if not match-only mode
    if ((after > 0 || before > 0) && !match_only && !all_mode) {
        // Split input into lines
        std::vector<std::string> lines;
        size_t pos = 0, start = 0;
        while (pos <= working_input.size()) {
            size_t next = working_input.find('\n', pos);
            if (next == std::string::npos) next = working_input.size();
            lines.push_back(working_input.substr(pos, next - pos));
            pos = next + 1;
        }
        std::set<size_t> match_lines;
        for (const auto& m : matches) {
            if (m.line_number > 0) match_lines.insert(m.line_number - 1);
        }
        std::set<size_t> output_lines;
        for (size_t line : match_lines) {
            size_t begin = (line >= (size_t)before) ? line - before : 0;
            size_t end = line + after;
            if (end > lines.size() - 1) end = lines.size() - 1;
            for (size_t i = begin; i <= end; ++i) {
                output_lines.insert(i);
            }
        }
        // Output lines in order
        for (size_t i = 0; i < lines.size(); ++i) {
            if (output_lines.count(i)) {
                std::string line = lines[i];
                if (dedupe && !unique.insert(line).second) continue;
                if (include_line_numbers) {
                    out.emplace_back(std::to_string(i + 1) + ":" + line);
                } else {
                    out.emplace_back(line);
                }
            }
        }
        return out;
    }

    if (count_only) {
        // Determine if dedup should apply to match substrings
        bool dedupe_on_match = dedupe && (
            options.find('o') != std::string::npos ||
            options.find('g') != std::string::npos ||
            options.find('b') != std::string::npos ||
            (options.find('o') == std::string::npos &&
                options.find('g') == std::string::npos &&
                options.find('b') == std::string::npos)
            );

        if (dedupe_on_match) {
            for (const auto& m : matches) {
                if (m.length == 0) continue;
                std::string match = working_input.substr(m.offset, m.length);
                if (strip_trailing_newline) {
                    if (!actual_delim.empty() && match.size() >= actual_delim.size() &&
                        match.compare(match.size() - actual_delim.size(), actual_delim.size(), actual_delim) == 0) {
                        match.erase(match.size() - actual_delim.size());
                    }
                    else if (match.size() >= 2 && match.substr(match.size() - 2) == "\r\n") {
                        match.erase(match.size() - 2);
                    }
                    else if (!match.empty() && match[match.size() - 1] == '\n') {
                        match.pop_back();
                    }
                }
                if (dedupe && !unique.insert(match).second) continue;
            }
            out.push_back(std::to_string(unique.size()));
        }
        else {
            out.push_back(std::to_string(matches.size()));
        }

        return out;
    }

    if (line_only) {
        std::set<size_t> seen_lines;
        for (const auto& m : matches) {
            if (seen_lines.insert(m.line_number).second) {
                out.emplace_back(std::to_string(m.line_number));
            }
        }
        return out;
    }

    std::string group_delim = ",";
    if (line_delim == "\\n") {
        group_delim = "\n";
    }
    else if (!line_delim.empty()) {
        group_delim = line_delim;
    }

    if (group_by_line) {
        std::map<size_t, std::vector<std::string>> grouped;
        for (const auto& m : matches) {
            std::string match = working_input.substr(m.offset, m.length);
            if (strip_trailing_newline) {
                if (!actual_delim.empty() && ends_with(match, actual_delim)) {
                    match.erase(match.size() - actual_delim.size());
                }
                else if (match.size() >= 2 && match.substr(match.size() - 2) == "\r\n") {
                    match.erase(match.size() - 2);
                }
                else if (!match.empty() && match[match.size() - 1] == '\n') {
                    match.pop_back();
                }
            }
            if (dedupe && !unique.insert(match).second) continue;
            grouped[m.line_number].push_back(std::move(match));
        }

        for (const auto& pair : grouped) {
            const size_t line = pair.first;
            const std::vector<std::string>& group = pair.second;
            std::string joined;
            for (size_t i = 0; i < group.size(); ++i) {
                if (i > 0) joined += group_delim;
                joined += group[i];
            }
            if (include_line_numbers) {
                out.emplace_back(std::to_string(line) + ":" + joined);
            }
            else {
                out.emplace_back(std::move(joined));
            }
        }
    }
    else {
        for (const auto& m : matches) {
            std::string match = working_input.substr(m.offset, m.length);
            if (strip_trailing_newline) {
                if (!actual_delim.empty() && ends_with(match, actual_delim)) {
                    match.erase(match.size() - actual_delim.size());
                }
                else if (match.size() >= 2 && match.substr(match.size() - 2) == "\r\n") {
                    match.erase(match.size() - 2);
                }
                else if (!match.empty() && match[match.size() - 1] == '\n') {
                    match.pop_back();
                }
            }
            if (dedupe && !unique.insert(match).second) continue;
            if (output_offset) {
                out.emplace_back(std::to_string(m.offset) + ":" + match);
            }
            else if (include_line_numbers && m.line_number > 0) {
                out.emplace_back(std::to_string(m.line_number) + ":" + match);
            }
            else {
                out.emplace_back(std::move(match));
            }
        }
    }

    return out;
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
        if (normalization_ != NormalizationForm::NONE) {
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
            matches.emplace_back(ovector[0], ovector[1] - ovector[0]);
            if (ovector[1] > offset) {
                offset = ovector[1];
            } else {
                ++offset;
            }
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
    : pattern_(pattern), case_insensitive_(case_insensitive), diacritic_insensitive_(diacritic_insensitive), normalization_(norm) {
    try {
        compile();
    } catch (const std::exception& e) {
        throw;
    }
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

void UnicodeRegex::compile() {
    compilation_error_ = false; /* Reset error state */
    std::string pattern_to_compile = pattern_;
    if (diacritic_insensitive_) {
        pattern_to_compile = GrapaUnicode::strip_diacritics(pattern_to_compile);
    }
#ifdef USE_PCRE
    use_pcre_ = has_unicode_properties(pattern_to_compile) || !is_ascii_only_ || has_named_groups(pattern_to_compile) || has_atomic_groups(pattern_to_compile) || has_lookaround_assertions(pattern_to_compile) || has_grapheme_clusters(pattern_to_compile);
#else
    use_pcre_ = false;
#endif
    if (use_pcre_) {
        std::string pattern_utf8 = parse_unicode_escapes(pattern_to_compile);
        int error_code = 0;
        PCRE2_SIZE error_offset = 0;
        uint32_t compile_flags = PCRE2_UTF | PCRE2_UCP | PCRE2_JIT_COMPLETE;
        if (case_insensitive_) {
            compile_flags |= PCRE2_CASELESS;
        }
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

} // namespace GrapaUnicode

/* Static member definitions */
namespace GrapaUnicode {
    std::map<std::tuple<std::string, bool, NormalizationForm>, std::string> UnicodeRegex::text_cache_;
    std::mutex UnicodeRegex::text_cache_mutex_;
    std::map<std::pair<std::string, std::string>, std::vector<std::pair<size_t, size_t>>> UnicodeRegex::offset_cache_;
    std::mutex UnicodeRegex::offset_cache_mutex_;
    UnicodeRegex::LRUCache UnicodeRegex::text_lru_cache_(1000); /* Initialize with 1000 entry limit */
}

 