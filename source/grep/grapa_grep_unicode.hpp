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

#include <string>
#include <vector>
#include <regex>
#include <memory>
#include <cstring>

// Include the original grep header for MatchPosition struct
#include "grapa_grep.hpp"

// Include utf8proc
#include "utf8proc/utf8proc.h"

namespace GrapaUnicode {

/**
 * Unicode normalization forms
 */
enum class NormalizationForm {
    NFC,   // Normalization Form Canonical Composition
    NFD,   // Normalization Form Canonical Decomposition
    NFKC,  // Normalization Form Compatibility Composition
    NFKD   // Normalization Form Compatibility Decomposition
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
 * Unicode-aware regex wrapper
 */
class UnicodeRegex {
private:
    std::string pattern_;
    std::regex regex_;
    bool case_insensitive_;
    NormalizationForm normalization_;
    
public:
    UnicodeRegex(const std::string& pattern, bool case_insensitive = false, 
                 NormalizationForm norm = NormalizationForm::NFC)
        : pattern_(pattern), case_insensitive_(case_insensitive), normalization_(norm) {
        compile();
    }
    
    /**
     * Compile the regex pattern with Unicode support
     */
    void compile() {
        // Normalize the pattern
        UnicodeString norm_pattern(pattern_);
        UnicodeString normalized_pattern = norm_pattern.normalize(normalization_);
        
        // Apply case folding if case-insensitive
        if (case_insensitive_) {
            normalized_pattern = normalized_pattern.case_fold();
        }
        
        std::regex::flag_type flags = std::regex::ECMAScript;
        if (case_insensitive_) {
            flags |= std::regex::icase;
        }
        
        try {
            regex_ = std::regex(normalized_pattern.data(), flags);
        } catch (const std::regex_error&) {
            // Fallback to original pattern if Unicode processing fails
            regex_ = std::regex(pattern_, flags);
        }
    }
    
    /**
     * Search for matches in text
     */
    bool search(const UnicodeString& text) const {
        UnicodeString normalized_text = text.normalize(normalization_);
        if (case_insensitive_) {
            normalized_text = normalized_text.case_fold();
        }
        
        return std::regex_search(normalized_text.data(), regex_);
    }
    
    /**
     * Match entire text
     */
    bool match(const UnicodeString& text) const {
        UnicodeString normalized_text = text.normalize(normalization_);
        if (case_insensitive_) {
            normalized_text = normalized_text.case_fold();
        }
        
        return std::regex_match(normalized_text.data(), regex_);
    }
    
    /**
     * Get all matches in text
     */
    std::vector<std::pair<size_t, size_t>> find_all(const UnicodeString& text) const {
        std::vector<std::pair<size_t, size_t>> matches;
        
        UnicodeString normalized_text = text.normalize(normalization_);
        if (case_insensitive_) {
            normalized_text = normalized_text.case_fold();
        }
        
        try {
            for (std::sregex_iterator it(normalized_text.data().begin(), 
                                        normalized_text.data().end(), regex_), end; 
                 it != end; ++it) {
                matches.emplace_back(it->position(), it->length());
            }
        } catch (const std::regex_error&) {
            // Return empty result on regex error
        }
        
        return matches;
    }
    
    /**
     * Get the underlying std::regex
     */
    const std::regex& get_regex() const {
        return regex_;
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

} // namespace GrapaUnicode

// Unicode-aware grep functions (base implementations)
std::vector<MatchPosition> grep_unicode_impl(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization
);

std::vector<std::string> grep_extract_matches_unicode_impl(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization
);

// Main Unicode-aware grep functions with default parameters
inline std::vector<MatchPosition> grep_unicode(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim = "",
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NFC
) {
    return grep_unicode_impl(input, pattern, options, line_delim, normalization);
}

inline std::vector<std::string> grep_extract_matches_unicode(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim = "",
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NFC
) {
    return grep_extract_matches_unicode_impl(input, pattern, options, line_delim, normalization);
} 