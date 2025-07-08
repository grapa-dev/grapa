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
    ProcessingMode mode,
    NormalizationForm normalization
) {
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
    UnicodeRegex unicode_rx(pattern, ignore_case, normalization);

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
            cached_normalized_input = UnicodeRegex::get_normalized_text(match_input, ignore_case, normalization);
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

        UnicodeString unicode_line(line_copy);
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
                    std::vector<std::pair<size_t, size_t>> matches = unicode_rx.find_all(unicode_line);
                    // Cache normalized line to avoid repeated normalization
                    std::string cached_normalized_line;
                    if (!unicode_rx.is_ascii_mode()) {
                        cached_normalized_line = UnicodeRegex::get_normalized_text(line_copy, ignore_case, normalization);
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
    ProcessingMode mode,
    NormalizationForm normalization
) {
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
            return {input};
        }
    }

    std::string filtered_options;
    ContextOptions ctx = parse_context_options(options, filtered_options);
    int after = ctx.after;
    int before = ctx.before;
    int context = ctx.context;
    if (context > 0) {
        if (context > after) after = context;
        if (context > before) before = context;
    }
    bool all_mode = (filtered_options.find('a') != std::string::npos);
    bool delimiter_provided = !line_delim.empty();
    std::string working_input;
    if (all_mode && delimiter_provided) {
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
    auto matches = grep_unicode(working_input, pattern, filtered_options, line_delim, mode, normalization);
    std::vector<std::string> out;

    bool count_only = filtered_options.find('c') != std::string::npos;
    bool dedupe = filtered_options.find('d') != std::string::npos;
    bool include_line_numbers = filtered_options.find('n') != std::string::npos;
    bool group_by_line = filtered_options.find('g') != std::string::npos;
    bool output_offset = filtered_options.find('b') != std::string::npos;
    bool line_only = filtered_options.find('l') != std::string::npos;
    bool match_only = filtered_options.find('o') != std::string::npos;
    bool strip_trailing_newline = line_delim.empty() && filtered_options.find('a') == std::string::npos;
    std::string actual_delim = (line_delim == "\\n") ? "\n" : line_delim;
    std::set<std::string> unique;

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
                unique.insert(match);
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
std::string parse_unicode_escapes(const std::string& pattern) {
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

 