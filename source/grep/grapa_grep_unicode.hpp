// grapa_grep_unicode.hpp
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

#pragma once

// Define UTF8PROC_STATIC for static linking (only if not already defined)
#ifndef UTF8PROC_STATIC
#define UTF8PROC_STATIC
#endif

/*
 Supported grep options:
   a - All-mode (match across full input string)
   A<n> - Show n lines after match (context)
   b - Output byte offset with matches
   B<n> - Show n lines before match (context)
   c - Count of matches (deduplicated if 'd' is also set)
   C<n> - Show n lines before and after match (context)
   d - Deduplicate results (by line or match depending on mode)
   g - Group results per line
   i - Case-insensitive matching
   j - JSON output format with named groups, offsets, and line numbers
   l - Line number only output (per matched line)
   n - Prefix matches with line numbers
   N - Normalize input and pattern to NFC before matching
   o - Match-only (output only matched text)
   v - Invert match (return non-matching lines or spans)
   x - Exact line match (whole line must match pattern)
   line_delim - Custom line delimiter (default: \n)
   
   Context options (A, B, C) can be combined with numbers:
   - A1, A2, A3... (show 1, 2, 3... lines after)
   - B1, B2, B3... (show 1, 2, 3... lines before)  
   - C1, C2, C3... (show 1, 2, 3... lines before and after)
*/

#include <string>
#include <vector>
#include <regex>
#include <memory>
#include <cstring>
#include <map>
#include <mutex> // Added for thread-safe caching
#include <tuple> // Added for tuple in text_cache_


#ifdef _WIN32
#if defined(_M_ARM64)
#elif defined(_M_X64)
#define USE_PCRE
#endif
#endif

#ifdef USE_PCRE
#define PCRE2_STATIC
#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2/pcre2.h"
#endif

// Include the original grep header for MatchPosition struct
//#include "grapa_grep.hpp"

// Include utf8proc
#include "utf8proc/utf8proc.h"

struct MatchPosition {
    size_t offset;
    size_t length;
    size_t line_number = 0;
};

struct ContextOptions {
    int after = 0;
    int before = 0;
    int context = 0;
};

namespace GrapaUnicode {

    /**
     * Unicode normalization forms
     */
    enum class NormalizationForm {
        NONE,  // No normalization (default, matches standard grep tools)
        NFC,   // Normalization Form Canonical Composition
        NFD,   // Normalization Form Canonical Decomposition
        NFKC,  // Normalization Form Compatibility Composition
        NFKD   // Normalization Form Compatibility Decomposition
    };

    /**
     * Processing modes for grep functions
     */
    enum class ProcessingMode {
        UNICODE_MODE,    // Full Unicode processing (normalization, case folding)
        BINARY_MODE      // Raw byte processing (no Unicode processing)
    };

    /**
     * Unicode-aware string utilities
     */
    class UnicodeString {
    private:
        std::string data_;

    public:
        UnicodeString() = default;
        UnicodeString(const std::string& str) : data_(str) {}
        UnicodeString(const char* str) : data_(str ? str : "") {}

        // Access underlying data
        const std::string& data() const { return data_; }
        std::string& data() { return data_; }
        const char* c_str() const { return data_.c_str(); }
        size_t size() const { return data_.size(); }
        bool empty() const { return data_.empty(); }

        /**
         * Normalize string to specified Unicode normalization form
         */
        UnicodeString normalize(NormalizationForm form) const {
            if (data_.empty()) return UnicodeString();

            utf8proc_option_t options = UTF8PROC_STABLE;
            switch (form) {
            case NormalizationForm::NFC:
                options = static_cast<utf8proc_option_t>(options | UTF8PROC_COMPOSE);
                break;
            case NormalizationForm::NFD:
                options = static_cast<utf8proc_option_t>(options | UTF8PROC_DECOMPOSE);
                break;
            case NormalizationForm::NFKC:
                options = static_cast<utf8proc_option_t>(options | UTF8PROC_COMPOSE | UTF8PROC_COMPAT);
                break;
            case NormalizationForm::NFKD:
                options = static_cast<utf8proc_option_t>(options | UTF8PROC_DECOMPOSE | UTF8PROC_COMPAT);
                break;
            }

            utf8proc_uint8_t* result = nullptr;
            utf8proc_ssize_t result_len = utf8proc_map(
                reinterpret_cast<const utf8proc_uint8_t*>(data_.c_str()),
                static_cast<utf8proc_ssize_t>(data_.size()),
                &result,
                options
            );

            if (result_len < 0) {
                // Error occurred, return original string
                return UnicodeString(data_);
            }

            UnicodeString normalized(reinterpret_cast<const char*>(result));
            free(result);
            return normalized;
        }

        /**
         * Case-fold string for case-insensitive comparison
         */
        UnicodeString case_fold() const {
            if (data_.empty()) return UnicodeString();

            utf8proc_uint8_t* result = nullptr;
            utf8proc_ssize_t result_len = utf8proc_map(
                reinterpret_cast<const utf8proc_uint8_t*>(data_.c_str()),
                static_cast<utf8proc_ssize_t>(data_.size()),
                &result,
                static_cast<utf8proc_option_t>(UTF8PROC_STABLE | UTF8PROC_CASEFOLD)
            );

            if (result_len < 0) {
                // Error occurred, return original string
                return UnicodeString(data_);
            }

            UnicodeString folded(reinterpret_cast<const char*>(result));
            free(result);
            return folded;
        }

        /**
         * Check if string contains valid UTF-8
         */
        bool is_valid_utf8() const {
            if (data_.empty()) return true;

            const utf8proc_uint8_t* str = reinterpret_cast<const utf8proc_uint8_t*>(data_.c_str());
            utf8proc_ssize_t len = static_cast<utf8proc_ssize_t>(data_.size());

            while (len > 0) {
                utf8proc_int32_t codepoint;
                utf8proc_ssize_t char_len = utf8proc_iterate(str, len, &codepoint);

                if (char_len < 0 || !utf8proc_codepoint_valid(codepoint)) {
                    return false;
                }

                str += char_len;
                len -= char_len;
            }

            return true;
        }

        /**
         * Get Unicode character count (not byte count)
         */
        size_t char_count() const {
            if (data_.empty()) return 0;

            size_t count = 0;
            const utf8proc_uint8_t* str = reinterpret_cast<const utf8proc_uint8_t*>(data_.c_str());
            utf8proc_ssize_t len = static_cast<utf8proc_ssize_t>(data_.size());

            while (len > 0) {
                utf8proc_int32_t codepoint;
                utf8proc_ssize_t char_len = utf8proc_iterate(str, len, &codepoint);

                if (char_len < 0) break;

                str += char_len;
                len -= char_len;
                count++;
            }

            return count;
        }

        /**
         * Convert to std::string
         */
        std::string to_string() const {
            return data_;
        }

        // Implicit conversion to std::string
        operator std::string() const {
            return data_;
        }
    };

    /**
     * Parse Unicode escapes in regex patterns
     * Converts \uXXXX sequences to actual UTF-8 bytes
     */
    std::string parse_unicode_escapes(const std::string& pattern);

    /**
     * Unicode-aware regex wrapper
     */
    class UnicodeRegex {
    private:
        std::string pattern_;
        bool case_insensitive_;
        NormalizationForm normalization_;
        bool is_ascii_only_;  // Track if pattern is ASCII-only
        mutable std::string cached_normalized_pattern_;
        mutable bool pattern_cached_ = false;
        bool compilation_error_ = false;  // Track if compilation failed

#ifdef USE_PCRE
        pcre2_code* pcre_regex_ = nullptr;
        pcre2_match_data* pcre_match_data_ = nullptr;
#endif

        static bool has_unicode_properties(const std::string& pattern);
        static bool has_named_groups(const std::string& pattern) {
            return pattern.find("(?P<") != std::string::npos;
        }

        std::regex regex_;  // Standard regex for non-PCRE mode
    public:
        UnicodeRegex(const std::string& pattern, bool case_insensitive = false,
            NormalizationForm norm = NormalizationForm::NFC);
        ~UnicodeRegex();

        bool use_pcre_ = false;

        /* Check if compilation was successful */
        bool is_valid() const { return !compilation_error_; }

        /* Check if PCRE2 is being used and is valid */
        bool is_pcre_valid() const { 
#ifdef USE_PCRE
            return use_pcre_ && pcre_regex_ != nullptr && !compilation_error_;
#else
            return false;
#endif
        }

        /* Get PCRE2 code pointer for JSON output (only if valid) */
#ifdef USE_PCRE
        pcre2_code* get_pcre_code() const { 
            return is_pcre_valid() ? pcre_regex_ : nullptr;
        }
#endif

        /* Check if pattern is ASCII-only */
        static bool is_ascii_string(const std::string& str) {
            for (unsigned char c : str) {
                if (c > 127) return false;
            }
            return true;
        }

        /* Get normalized pattern for caching */
        std::string get_normalized_pattern() const {
            if (!pattern_cached_) {
                if (normalization_ != NormalizationForm::NONE) {
                    UnicodeString norm_pattern(pattern_);
                    cached_normalized_pattern_ = norm_pattern.normalize(normalization_).data();
                } else {
                    cached_normalized_pattern_ = pattern_;
                }
                pattern_cached_ = true;
            }
            return cached_normalized_pattern_;
        }

        void compile();

        /* Pattern optimization for performance */
        static std::string optimize_pattern_for_performance(const std::string& pattern) {
            std::string optimized = pattern;
            
            /* Optimize simple repeated patterns */
            if (pattern.size() >= 2 && pattern.back() == '+') {
                std::string base = pattern.substr(0, pattern.size() - 1);
                if (base.size() == 1 && (base[0] == '.' || base[0] == '\\w' || base[0] == '\\s' || base[0] == '\\d')) {
                    /* Keep as is - these are already optimized */
                }
            }
            
            return optimized;
        }

        /* Text normalization cache */
        static std::map<std::tuple<std::string, bool, NormalizationForm>, std::string> text_cache_;
        static std::mutex text_cache_mutex_;

        static std::string get_normalized_text(const std::string& text, bool case_insensitive, NormalizationForm normalization) {
            if (normalization == NormalizationForm::NONE) {
                return text;
            }

            std::lock_guard<std::mutex> lock(text_cache_mutex_);
            auto key = std::make_tuple(text, case_insensitive, normalization);
            auto it = text_cache_.find(key);
            if (it != text_cache_.end()) {
                return it->second;
            }

            UnicodeString unicode_text(text);
            std::string normalized;
            if (case_insensitive) {
                normalized = unicode_text.case_fold().normalize(normalization).data();
            } else {
                normalized = unicode_text.normalize(normalization).data();
            }
            
            text_cache_[key] = normalized;
            return normalized;
        }

        /* Cache management */
        static void clear_text_cache() {
            std::lock_guard<std::mutex> lock(text_cache_mutex_);
            text_cache_.clear();
        }

        static size_t get_text_cache_size() {
            std::lock_guard<std::mutex> lock(text_cache_mutex_);
            return text_cache_.size();
        }

        /* Core matching methods */
        bool search(const UnicodeString& text) const;
        bool match(const UnicodeString& text) const;
        std::vector<std::pair<size_t, size_t>> find_all(const UnicodeString& text) const;

        /* Performance optimization for simple repeated patterns */
        bool is_simple_repeated_pattern() const {
            if (pattern_.size() < 2) return false;
            if (pattern_.back() != '+') return false;
            
            std::string base = pattern_.substr(0, pattern_.size() - 1);
            if (base.size() != 1) return false;
            
            char c = base[0];
            return c == '.' || c == '\\w' || c == '\\s' || c == '\\d' || c == '\\W' || c == '\\S' || c == '\\D';
        }

        /* Fast path for simple repeated patterns */
        std::vector<std::pair<size_t, size_t>> find_all_fast_path(const UnicodeString& text) const {
            std::vector<std::pair<size_t, size_t>> matches;
            if (!is_simple_repeated_pattern()) return matches;
            
            std::string base = pattern_.substr(0, pattern_.size() - 1);
            char c = base[0];
            
            const std::string& str = text.data();
            size_t pos = 0;
            
            while (pos < str.size()) {
                size_t start = pos;
                bool matched = false;
                
                /* Find start of sequence */
                while (pos < str.size()) {
                    bool char_matches = false;
                    if (c == '.') {
                        char_matches = true;
                    } else if (c == '\\w') {
                        char_matches = (str[pos] >= 'a' && str[pos] <= 'z') ||
                                      (str[pos] >= 'A' && str[pos] <= 'Z') ||
                                      (str[pos] >= '0' && str[pos] <= '9') ||
                                      str[pos] == '_';
                    } else if (c == '\\s') {
                        char_matches = str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\n' || str[pos] == '\r';
                    } else if (c == '\\d') {
                        char_matches = (str[pos] >= '0' && str[pos] <= '9');
                    } else if (c == '\\W') {
                        char_matches = !((str[pos] >= 'a' && str[pos] <= 'z') ||
                                       (str[pos] >= 'A' && str[pos] <= 'Z') ||
                                       (str[pos] >= '0' && str[pos] <= '9') ||
                                       str[pos] == '_');
                    } else if (c == '\\S') {
                        char_matches = str[pos] != ' ' && str[pos] != '\t' && str[pos] != '\n' && str[pos] != '\r';
                    } else if (c == '\\D') {
                        char_matches = (str[pos] < '0' || str[pos] > '9');
                    }
                    
                    if (char_matches) {
                        matched = true;
                        ++pos;
                    } else {
                        break;
                    }
                }
                
                if (matched && pos > start) {
                    matches.emplace_back(start, pos - start);
                }
                
                if (pos == start) ++pos; /* Avoid infinite loop */
            }
            
            return matches;
        }

        /* Check if using ASCII mode */
        bool is_ascii_mode() const {
            return is_ascii_only_ && !use_pcre_;
        }

        /* Offset mapping cache for normalized text */
        static std::map<std::pair<std::string, std::string>, std::vector<std::pair<size_t, size_t>>> offset_cache_;
        static std::mutex offset_cache_mutex_;

        static std::pair<size_t, size_t> map_normalized_span_to_original(const std::string& original, const std::string& normalized, size_t norm_offset, size_t norm_length) {
            if (original == normalized) {
                return {norm_offset, norm_length};
            }

            std::lock_guard<std::mutex> lock(offset_cache_mutex_);
            auto key = std::make_pair(original, normalized);
            auto it = offset_cache_.find(key);
            if (it != offset_cache_.end()) {
                /* Use cached mapping */
                if (norm_offset < it->second.size()) {
                    size_t orig_offset = it->second[norm_offset].first;
                    size_t orig_length = 0;
                    
                    /* Calculate length by finding the end offset */
                    size_t end_norm_offset = norm_offset + norm_length;
                    if (end_norm_offset < it->second.size()) {
                        orig_length = it->second[end_norm_offset].first - orig_offset;
                    } else {
                        orig_length = original.size() - orig_offset;
                    }
                    
                    return {orig_offset, orig_length};
                }
            }

            /* Build mapping cache */
            std::vector<std::pair<size_t, size_t>> mapping;
            const utf8proc_uint8_t* orig_str = reinterpret_cast<const utf8proc_uint8_t*>(original.c_str());
            const utf8proc_uint8_t* norm_str = reinterpret_cast<const utf8proc_uint8_t*>(normalized.c_str());
            
            utf8proc_ssize_t orig_len = static_cast<utf8proc_ssize_t>(original.size());
            utf8proc_ssize_t norm_len = static_cast<utf8proc_ssize_t>(normalized.size());
            
            size_t orig_pos = 0;
            size_t norm_pos = 0;
            
            while (orig_pos < static_cast<size_t>(orig_len) && norm_pos < static_cast<size_t>(norm_len)) {
                mapping.emplace_back(orig_pos, norm_pos);
                
                /* Advance in original string */
                utf8proc_int32_t orig_cp;
                utf8proc_ssize_t orig_char_len = utf8proc_iterate(orig_str + orig_pos, orig_len - orig_pos, &orig_cp);
                if (orig_char_len <= 0) break;
                orig_pos += orig_char_len;
                
                /* Advance in normalized string */
                utf8proc_int32_t norm_cp;
                utf8proc_ssize_t norm_char_len = utf8proc_iterate(norm_str + norm_pos, norm_len - norm_pos, &norm_cp);
                if (norm_char_len <= 0) break;
                norm_pos += norm_char_len;
            }
            
            /* Add final positions */
            mapping.emplace_back(orig_pos, norm_pos);
            offset_cache_[key] = mapping;
            
            /* Use the mapping */
            if (norm_offset < mapping.size()) {
                size_t orig_offset = mapping[norm_offset].first;
                size_t orig_length = 0;
                
                size_t end_norm_offset = norm_offset + norm_length;
                if (end_norm_offset < mapping.size()) {
                    orig_length = mapping[end_norm_offset].first - orig_offset;
                } else {
                    orig_length = original.size() - orig_offset;
                }
                
                return {orig_offset, orig_length};
            }
            
            return {norm_offset, norm_length}; /* Fallback */
        }

        /* Cache management */
        static void clear_offset_cache() {
            std::lock_guard<std::mutex> lock(offset_cache_mutex_);
            offset_cache_.clear();
        }

        static size_t get_offset_cache_size() {
            std::lock_guard<std::mutex> lock(offset_cache_mutex_);
            return offset_cache_.size();
        }

        static void clear_all_caches() {
            clear_text_cache();
            clear_offset_cache();
        }
    };

    /**
     * Unicode-aware string comparison
     */
    inline bool unicode_equals(const UnicodeString& a, const UnicodeString& b,
        bool case_insensitive = false,
        NormalizationForm norm = NormalizationForm::NFC) {
        UnicodeString norm_a = a.normalize(norm);
        UnicodeString norm_b = b.normalize(norm);

        if (case_insensitive) {
            norm_a = norm_a.case_fold();
            norm_b = norm_b.case_fold();
        }

        return norm_a.data() == norm_b.data();
    }

    /**
     * Unicode-aware string contains
     */
    inline bool unicode_contains(const UnicodeString& haystack, const UnicodeString& needle,
        bool case_insensitive = false,
        NormalizationForm norm = NormalizationForm::NFC) {
        UnicodeString norm_haystack = haystack.normalize(norm);
        UnicodeString norm_needle = needle.normalize(norm);

        if (case_insensitive) {
            norm_haystack = norm_haystack.case_fold();
            norm_needle = norm_needle.case_fold();
        }

        return norm_haystack.data().find(norm_needle.data()) != std::string::npos;
    }

    // Declaration inside namespace
    std::string parse_unicode_escapes(const std::string& pattern);

} // namespace GrapaUnicode

// Unicode-aware grep functions (base implementations)
std::vector<MatchPosition> grep_unicode_impl(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NONE,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE
);

std::vector<std::string> grep_extract_matches_unicode_impl(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NONE,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE
);

// Main Unicode-aware grep functions with default parameters
inline std::vector<MatchPosition> grep_unicode(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim = "",
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NONE,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE
) {
    return grep_unicode_impl(input, pattern, options, line_delim, normalization, mode);
}

inline std::vector<std::string> grep_extract_matches_unicode(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim = "",
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NONE,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE
) {
    return grep_extract_matches_unicode_impl(input, pattern, options, line_delim, normalization, mode);
} 