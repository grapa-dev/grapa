// grapa_grep_unicode.hpp
/*
Copyright 2025 Chris Ernest Matichuk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
////////////////////////////////////////////////////////////////////////////////

#pragma once

// Define UTF8PROC_STATIC for static linking (only if not already defined)
#ifndef UTF8PROC_STATIC
#define UTF8PROC_STATIC
#endif

// Define PCRE2_STATIC for static linking (only if not already defined)
#ifndef PCRE2_STATIC
#define PCRE2_STATIC
#endif

// #define GRAPA_DEBUG_PRINTF


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
   d - Diacritic-insensitive matching (strip accents/diacritics from both pattern and input)
*/

#include <string>
#include <vector>
#include <regex>
#include <memory>
#include <cstring>
#include <map>
#include <mutex> // Added for thread-safe caching
#include <tuple> // Added for tuple in text_cache_
#include <unordered_map> // Added for Latin-1 diacritic stripping
#include <list> // Added for LRU cache

// Include GrapaThread for threading support
#include "../grapa/GrapaThread.h"
#include "../grapa/GrapaValue.h"


// Forward declaration for GrapaThread
class GrapaThread;

// Forward declarations for GrapaUnicode enums
namespace GrapaUnicode {
    enum class NormalizationForm;
    enum class ProcessingMode;
}

// Forward declarations for helper functions defined in grapa_grep_unicode.cpp
std::string build_json_array(const std::vector<std::string>& items, bool already_json_objects = false);
std::string extract_grapheme_cluster(const std::string& input, size_t offset);
bool is_multiline_pattern(const std::string& pattern);
bool has_lookaround_assertions(const std::string& pattern);
std::vector<std::string> extract_full_segments(const std::string& input, const std::string& delimiter, const std::vector<std::pair<size_t, size_t>>& match_positions);
std::string normalize_newlines(std::string_view input);
std::vector<std::string> extract_matches_with_lookaround(const std::string& input, const std::string& pattern, bool case_insensitive, bool diacritic_insensitive, GrapaUnicode::NormalizationForm norm);
std::vector<std::string> split_input_for_parallel(const std::string& input, size_t num_chunks);
std::vector<std::string> split_by_delimiter(const std::string& input, const std::string& delimiter);
std::string remove_trailing_delimiter(const std::string& str, const std::string& delimiter);
std::string grapa_case_fold_string(const std::string& input);

// Test function
std::vector<std::string> test_function();

// GrapaGrepWorkEvent class for parallel grep processing using WorkQueue pattern
class GrapaGrepWorkEvent : public GrapaWorkEvent
{
public:
    // Input data
    std::string input_chunk;
    std::string pattern;
    std::string options;
    std::string line_delim;
    GrapaUnicode::NormalizationForm normalization;
    GrapaUnicode::ProcessingMode mode;
    
    // Output data
    std::vector<std::string> results;
    
    // Constructor
    GrapaGrepWorkEvent(GrapaWorkQueue* vQueue, 
                       const std::string& chunk,
                       const std::string& pat,
                       const std::string& opts,
                       const std::string& delim,
                       GrapaUnicode::NormalizationForm norm,
                       GrapaUnicode::ProcessingMode proc_mode)
        : input_chunk(chunk), pattern(pat), options(opts), line_delim(delim),
          normalization(norm), mode(proc_mode)
    {
        Set(vQueue);
    }
    
    // Thread lifecycle methods (override from GrapaWorkEvent)
    virtual void Starting() override;
    virtual void Running() override;
    virtual void Stopping() override;
};

// GrapaGrepWorkQueue class for managing grep work events
class GrapaGrepWorkQueue : public GrapaWorkQueue
{
public:
    virtual ~GrapaGrepWorkQueue() {
        GrapaGrepWorkQueue::CLEAR();
    }
    virtual void CLEAR()
    {
        GrapaGrepWorkEvent* e = (GrapaGrepWorkEvent*)PopHead();
        while (e)
        {
            e->CLEAR();
            delete e;
            e = (GrapaGrepWorkEvent*)PopHead();
        }
    }
};

#ifdef _WIN32
#if defined(_M_ARM64)
#elif defined(_M_X64)
#define USE_PCRE
#endif
#endif

#ifdef __APPLE__
#if defined(__aarch64__) || defined(__arm64__) || defined(__x86_64__)
#define USE_PCRE
#endif
#endif

#ifdef __linux__
#if defined(__aarch64__) || defined(__x86_64__)
#define USE_PCRE
#endif
#endif

#ifdef USE_PCRE
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

// Forward declaration for build_json_objects_array (after MatchPosition is defined)
std::string build_json_objects_array(const std::vector<std::string>& matches, const std::vector<MatchPosition>& positions, const std::string& input, const std::string& line_delim);

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

    // Forward declarations for helper functions
    char32_t case_fold_codepoint(char32_t c);
    char32_t strip_latin_diacritic(char32_t c);
    bool is_combining_mark(char32_t cp);

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

            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: normalize() called with input: '%s' (length: %zu), form: %d\n", data_.c_str(), data_.size(), static_cast<int>(form));
            #endif // DEBUG_END

            utf8proc_option_t options = UTF8PROC_STABLE;
            switch (form) {
            case NormalizationForm::NONE:
                // No normalization, use default options
                break;
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

            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: normalize() utf8proc_map returned result_len: %zd\n", result_len);
            #endif // DEBUG_END

            if (result_len < 0) {
                // Error occurred, return original string
                #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
                printf("DEBUG: normalize() utf8proc_map error, returning original string\n");
                #endif // DEBUG_END
                return UnicodeString(data_);
            }

            UnicodeString normalized(reinterpret_cast<const char*>(result));
            
            #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
            printf("DEBUG: normalize() result: '%s' (length: %zu)\n", normalized.data_.c_str(), normalized.data_.size());
            #endif // DEBUG_END
            
            free(result);
            return normalized;
        }

        /**
         * Case-fold string for case-insensitive comparison
         */
        UnicodeString case_fold() const {
            if (data_.empty()) return UnicodeString();

            // Use lookup table for reliable case folding, especially for Turkish I
            std::string result;
            size_t i = 0;
            while (i < data_.size()) {
                unsigned char c = data_[i];
                char32_t cp = 0;
                size_t len = 0;
                
                // Decode UTF-8
                if (c < 0x80) { cp = c; len = 1; }
                else if ((c & 0xE0) == 0xC0) { cp = ((c & 0x1F) << 6) | (data_[i+1] & 0x3F); len = 2; }
                else if ((c & 0xF0) == 0xE0) { cp = ((c & 0x0F) << 12) | ((data_[i+1] & 0x3F) << 6) | (data_[i+2] & 0x3F); len = 3; }
                else if ((c & 0xF8) == 0xF0) { cp = ((c & 0x07) << 18) | ((data_[i+1] & 0x3F) << 12) | ((data_[i+2] & 0x3F) << 6) | (data_[i+3] & 0x3F); len = 4; }
                else { cp = c; len = 1; }
                
                // Apply case folding using lookup table
                char32_t folded = case_fold_codepoint(cp);
                
                // Re-encode as UTF-8
                if (folded < 0x80) result += static_cast<char>(folded);
                else if (folded < 0x800) {
                    result += static_cast<char>(0xC0 | (folded >> 6));
                    result += static_cast<char>(0x80 | (folded & 0x3F));
                } else if (folded < 0x10000) {
                    result += static_cast<char>(0xE0 | (folded >> 12));
                    result += static_cast<char>(0x80 | ((folded >> 6) & 0x3F));
                    result += static_cast<char>(0x80 | (folded & 0x3F));
                } else {
                    result += static_cast<char>(0xF0 | (folded >> 18));
                    result += static_cast<char>(0x80 | ((folded >> 12) & 0x3F));
                    result += static_cast<char>(0x80 | ((folded >> 6) & 0x3F));
                    result += static_cast<char>(0x80 | (folded & 0x3F));
                }
                
                i += len;
            }
            
            return UnicodeString(result);
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
        bool diacritic_insensitive_; // Track if diacritic-insensitive matching is enabled
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

        static bool has_atomic_groups(const std::string& pattern) {
            return pattern.find("(?>") != std::string::npos;
        }

        static bool has_lookaround_assertions(const std::string& pattern) {
            // Check for positive lookahead (?=...)
            if (pattern.find("(?=") != std::string::npos) return true;
            // Check for negative lookahead (?!...)
            if (pattern.find("(?!") != std::string::npos) return true;
            // Check for positive lookbehind (?<=...)
            if (pattern.find("(?<=") != std::string::npos) return true;
            // Check for negative lookbehind (?<!...)
            if (pattern.find("(?<!") != std::string::npos) return true;
            return false;
        }

        static bool has_grapheme_clusters(const std::string& pattern) {
            // Check for Unicode grapheme cluster \X
            return pattern.find("\\X") != std::string::npos;
        }

        static bool has_possessive_quantifiers(const std::string& pattern) {
            // Check for possessive quantifiers (*+, ++, ?+, {n,m}+)
            return pattern.find("*+") != std::string::npos ||
                   pattern.find("++") != std::string::npos ||
                   pattern.find("?+") != std::string::npos ||
                   pattern.find("}+") != std::string::npos;
        }

        static bool has_conditional_patterns(const std::string& pattern) {
            // Check for conditional patterns (?(condition)...)
            return pattern.find("(?(") != std::string::npos;
        }

        /* Fast path detection for common patterns */
        static bool is_simple_literal_pattern(const std::string& pattern) {
            // Check for literal strings without special regex characters
            return pattern.find_first_of(".*+?{}()[]\\|^$") == std::string::npos;
        }

        static bool is_simple_word_pattern(const std::string& pattern) {
            // Check for simple word patterns like \w+, \b\w+\b
            return pattern == "\\w+" || pattern == "\\b\\w+\\b" || pattern == "\\w*";
        }

        static bool is_simple_digit_pattern(const std::string& pattern) {
            // Check for simple digit patterns like \d+, \b\d+\b
            return pattern == "\\d+" || pattern == "\\b\\d+\\b" || pattern == "\\d*";
        }

        std::regex regex_;  // Standard regex for non-PCRE mode
    public:
        UnicodeRegex(const std::string& pattern, bool case_insensitive = false, bool diacritic_insensitive = false, NormalizationForm norm = NormalizationForm::NFC);
        ~UnicodeRegex();

        bool use_pcre_ = false;
        bool use_jit_matching_ = false;  /* Track if JIT compilation is available */

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

        /* Check if JIT matching is available */
        bool is_jit_available() const {
#ifdef USE_PCRE
            return use_pcre_ && use_jit_matching_;
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

        void compile(const std::string& input);

        /* Pattern optimization for performance */
        static std::string optimize_pattern_for_performance(const std::string& pattern) {
            std::string optimized = pattern;
            
            /* Optimize simple repeated patterns */
            if (pattern.size() >= 2 && pattern.back() == '+') {
                std::string base = pattern.substr(0, pattern.size() - 1);
                if (base.size() == 1 && (base[0] == '.' || base == "\\w" || base == "\\s" || base == "\\d")) {
                    /* Keep as is - these are already optimized */
                }
            }
            
            return optimized;
        }

        /* Text normalization cache */
        static std::map<std::tuple<std::string, bool, NormalizationForm>, std::string> text_cache_;
        static std::mutex text_cache_mutex_;

        /* LRU Cache for better memory management */
        class LRUCache {
        private:
            size_t max_size_;
            std::list<std::pair<std::string, std::string>> cache_;
            std::unordered_map<std::string, std::list<std::pair<std::string, std::string>>::iterator> map_;
            mutable std::mutex cache_mutex_; /* Make mutable for const methods */

        public:
            LRUCache(size_t max_size = 1000) : max_size_(max_size) {}

            std::string get(const std::string& key) {
                std::lock_guard<std::mutex> lock(cache_mutex_);
                auto it = map_.find(key);
                if (it == map_.end()) {
                    return ""; // Not found
                }
                
                // Move to front (most recently used)
                cache_.splice(cache_.begin(), cache_, it->second);
                return it->second->second;
            }

            void put(const std::string& key, const std::string& value) {
                std::lock_guard<std::mutex> lock(cache_mutex_);
                auto it = map_.find(key);
                if (it != map_.end()) {
                    // Update existing entry
                    it->second->second = value;
                    cache_.splice(cache_.begin(), cache_, it->second);
                } else {
                    // Add new entry
                    if (cache_.size() >= max_size_) {
                        // Remove least recently used
                        auto lru = cache_.back();
                        map_.erase(lru.first);
                        cache_.pop_back();
                    }
                    
                    cache_.push_front({key, value});
                    map_[key] = cache_.begin();
                }
            }

            size_t size() const {
                std::lock_guard<std::mutex> lock(cache_mutex_);
                return cache_.size();
            }

            void clear() {
                std::lock_guard<std::mutex> lock(cache_mutex_);
                cache_.clear();
                map_.clear();
            }
        };

        static LRUCache text_lru_cache_;

        static std::string get_normalized_text(const std::string& text, bool case_insensitive, NormalizationForm normalization) {
            if (normalization == NormalizationForm::NONE) {
                return text;
            }

            /* Create cache key */
            std::string key = text + "|" + (case_insensitive ? "1" : "0") + "|" + std::to_string(static_cast<int>(normalization));
            
            /* Try LRU cache first */
            std::string cached = text_lru_cache_.get(key);
            if (!cached.empty()) {
                return cached;
            }

            /* Fall back to original cache for backward compatibility */
            std::lock_guard<std::mutex> lock(text_cache_mutex_);
            auto cache_key = std::make_tuple(text, case_insensitive, normalization);
            auto it = text_cache_.find(cache_key);
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
            
            /* Store in both caches */
            text_cache_[cache_key] = normalized;
            text_lru_cache_.put(key, normalized);
            
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

        /* LRU Cache management */
        static void clear_lru_cache() {
            text_lru_cache_.clear();
        }

        static size_t get_lru_cache_size() {
            return text_lru_cache_.size();
        }

        static void set_lru_cache_size(size_t max_size) {
            /* Clear and recreate with new size */
            text_lru_cache_.clear();
            /* Note: LRUCache doesn't support dynamic resizing, so we clear it */
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
            return c == '.' || base == "\\w" || base == "\\s" || base == "\\d" || base == "\\W" || base == "\\S" || base == "\\D";
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
                    } else if (base == "\\w") {
                        char_matches = (str[pos] >= 'a' && str[pos] <= 'z') ||
                                      (str[pos] >= 'A' && str[pos] <= 'Z') ||
                                      (str[pos] >= '0' && str[pos] <= '9') ||
                                      str[pos] == '_';
                    } else if (base == "\\s") {
                        char_matches = str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\n' || str[pos] == '\r';
                    } else if (base == "\\d") {
                        char_matches = (str[pos] >= '0' && str[pos] <= '9');
                    } else if (base == "\\W") {
                        char_matches = !((str[pos] >= 'a' && str[pos] <= 'z') ||
                                       (str[pos] >= 'A' && str[pos] <= 'Z') ||
                                       (str[pos] >= '0' && str[pos] <= '9') ||
                                       str[pos] == '_');
                    } else if (base == "\\S") {
                        char_matches = str[pos] != ' ' && str[pos] != '\t' && str[pos] != '\n' && str[pos] != '\r';
                    } else if (base == "\\D") {
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

        /* Fast path for simple literal patterns */
        std::vector<std::pair<size_t, size_t>> find_all_literal_fast_path(const UnicodeString& text) const {
            std::vector<std::pair<size_t, size_t>> matches;
            if (!is_simple_literal_pattern(pattern_)) return matches;
            
            const std::string& str = text.data();
            const std::string& pattern = pattern_;
            size_t pos = 0;
            
            while ((pos = str.find(pattern, pos)) != std::string::npos) {
                matches.emplace_back(pos, pattern.size());
                pos += pattern.size();
            }
            
            return matches;
        }

        /* Fast path for simple word patterns */
        std::vector<std::pair<size_t, size_t>> find_all_word_fast_path(const UnicodeString& text) const {
            std::vector<std::pair<size_t, size_t>> matches;
            if (!is_simple_word_pattern(pattern_)) return matches;
            
            const std::string& str = text.data();
            size_t pos = 0;
            
            while (pos < str.size()) {
                /* Skip non-word characters */
                while (pos < str.size() && !is_word_char(str[pos])) ++pos;
                if (pos >= str.size()) break;
                
                size_t start = pos;
                while (pos < str.size() && is_word_char(str[pos])) ++pos;
                
                if (pos > start) {
                    matches.emplace_back(start, pos - start);
                }
            }
            
            return matches;
        }

        /* Fast path for simple digit patterns */
        std::vector<std::pair<size_t, size_t>> find_all_digit_fast_path(const UnicodeString& text) const {
            std::vector<std::pair<size_t, size_t>> matches;
            if (!is_simple_digit_pattern(pattern_)) return matches;
            
            const std::string& str = text.data();
            size_t pos = 0;
            
            while (pos < str.size()) {
                /* Skip non-digit characters */
                while (pos < str.size() && !is_digit_char(str[pos])) ++pos;
                if (pos >= str.size()) break;
                
                size_t start = pos;
                while (pos < str.size() && is_digit_char(str[pos])) ++pos;
                
                if (pos > start) {
                    matches.emplace_back(start, pos - start);
                }
            }
            
            return matches;
        }

        /* Helper functions for fast paths */
        static bool is_word_char(char c) {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
                   (c >= '0' && c <= '9') || c == '_';
        }

        static bool is_digit_char(char c) {
            return c >= '0' && c <= '9';
        }

        /* Check if using ASCII mode */
        bool is_ascii_mode() const {
            return is_ascii_only_ && !use_pcre_;
        }

        /* Offset mapping cache for normalized text */
        static std::map<std::pair<std::string, std::string>, std::vector<std::pair<size_t, size_t>>> offset_cache_;
        static std::mutex offset_cache_mutex_;

        // Hybrid mapping strategy with multiple fallbacks for robust Unicode boundary handling
        static std::pair<size_t, size_t> map_normalized_span_to_original(const std::string& original, const std::string& normalized, size_t norm_offset, size_t norm_length);

    private:
        // Strategy 1: Grapheme cluster boundary alignment
        static std::pair<size_t, size_t> try_grapheme_cluster_mapping(const std::string& original, const std::string& normalized, size_t norm_offset, size_t norm_length) {
            std::vector<size_t> orig_grapheme_starts, norm_grapheme_starts;
            size_t orig_pos = 0, norm_pos = 0;
            utf8proc_ssize_t orig_len = static_cast<utf8proc_ssize_t>(original.size());
            utf8proc_ssize_t norm_len = static_cast<utf8proc_ssize_t>(normalized.size());
            
            // Build grapheme cluster boundaries for original string
            while (orig_pos < static_cast<size_t>(orig_len)) {
                orig_grapheme_starts.push_back(orig_pos);
                utf8proc_int32_t cp;
                utf8proc_ssize_t char_len = utf8proc_iterate(reinterpret_cast<const utf8proc_uint8_t*>(original.c_str()) + orig_pos, orig_len - orig_pos, &cp);
                if (char_len <= 0) break;
                size_t next = orig_pos + char_len;
                while (next < orig_len) {
                    utf8proc_int32_t next_cp;
                    utf8proc_ssize_t next_len = utf8proc_iterate(reinterpret_cast<const utf8proc_uint8_t*>(original.c_str()) + next, orig_len - next, &next_cp);
                    if (next_len <= 0) break;
                    if (utf8proc_grapheme_break(cp, next_cp) != 0) break;
                    next += next_len;
                    cp = next_cp;
                }
                orig_pos = next;
            }
            orig_grapheme_starts.push_back(orig_len);
            
            // Build grapheme cluster boundaries for normalized string
            while (norm_pos < static_cast<size_t>(norm_len)) {
                norm_grapheme_starts.push_back(norm_pos);
                utf8proc_int32_t cp;
                utf8proc_ssize_t char_len = utf8proc_iterate(reinterpret_cast<const utf8proc_uint8_t*>(normalized.c_str()) + norm_pos, norm_len - norm_pos, &cp);
                if (char_len <= 0) break;
                size_t next = norm_pos + char_len;
                while (next < norm_len) {
                    utf8proc_int32_t next_cp;
                    utf8proc_ssize_t next_len = utf8proc_iterate(reinterpret_cast<const utf8proc_uint8_t*>(normalized.c_str()) + next, norm_len - next, &next_cp);
                    if (next_len <= 0) break;
                    if (utf8proc_grapheme_break(cp, next_cp) != 0) break;
                    next += next_len;
                    cp = next_cp;
                }
                norm_pos = next;
            }
            norm_grapheme_starts.push_back(norm_len);
            
            // Map normalized grapheme cluster offset to original
            if (norm_offset < norm_grapheme_starts.size() - 1 && (norm_offset + norm_length) <= norm_grapheme_starts.size() - 1) {
                size_t norm_start_byte = norm_grapheme_starts[norm_offset];
                size_t norm_end_byte = norm_grapheme_starts[norm_offset + norm_length];
                
                // Find corresponding original boundaries
                size_t orig_start = 0, orig_end = 0;
                if (norm_offset < orig_grapheme_starts.size()) {
                    size_t idx = (norm_offset < orig_grapheme_starts.size() - 1) ? norm_offset : orig_grapheme_starts.size() - 1;
                    orig_start = orig_grapheme_starts[idx];
                }
                if (norm_offset + norm_length < orig_grapheme_starts.size()) {
                    size_t idx = (norm_offset + norm_length < orig_grapheme_starts.size() - 1) ? norm_offset + norm_length : orig_grapheme_starts.size() - 1;
                    orig_end = orig_grapheme_starts[idx];
                } else {
                    orig_end = original.size();
                }
                
                if (orig_end >= orig_start) {
                    return {orig_start, orig_end - orig_start};
                }
            }
            
            return {0, 0}; // Invalid mapping
        }

        // Strategy 2: Character-by-character alignment
        static std::pair<size_t, size_t> try_character_alignment(const std::string& original, const std::string& normalized, size_t norm_offset, size_t norm_length) {
            size_t orig_pos = 0, norm_pos = 0;
            utf8proc_ssize_t orig_len = static_cast<utf8proc_ssize_t>(original.size());
            utf8proc_ssize_t norm_len = static_cast<utf8proc_ssize_t>(normalized.size());
            
            // Find the character position in normalized string
            size_t target_norm_pos = 0;
            while (norm_pos < norm_offset && norm_pos < static_cast<size_t>(norm_len)) {
                utf8proc_int32_t cp;
                utf8proc_ssize_t char_len = utf8proc_iterate(reinterpret_cast<const utf8proc_uint8_t*>(normalized.c_str()) + norm_pos, norm_len - norm_pos, &cp);
                if (char_len <= 0) break;
                norm_pos += char_len;
                target_norm_pos++;
            }
            
            // Find corresponding position in original string
            size_t orig_start = 0;
            size_t char_count = 0;
            while (orig_pos < static_cast<size_t>(orig_len) && char_count < target_norm_pos) {
                utf8proc_int32_t cp;
                utf8proc_ssize_t char_len = utf8proc_iterate(reinterpret_cast<const utf8proc_uint8_t*>(original.c_str()) + orig_pos, orig_len - orig_pos, &cp);
                if (char_len <= 0) break;
                orig_pos += char_len;
                char_count++;
            }
            orig_start = orig_pos;
            
            // Find end position
            size_t orig_end = orig_start;
            char_count = 0;
            while (orig_pos < static_cast<size_t>(orig_len) && char_count < norm_length) {
                utf8proc_int32_t cp;
                utf8proc_ssize_t char_len = utf8proc_iterate(reinterpret_cast<const utf8proc_uint8_t*>(original.c_str()) + orig_pos, orig_len - orig_pos, &cp);
                if (char_len <= 0) break;
                orig_pos += char_len;
                char_count++;
            }
            orig_end = orig_pos;
            
            return {orig_start, orig_end - orig_start};
        }

        // Strategy 3: Fallback to direct substring with bounds checking
        static std::pair<size_t, size_t> try_fallback_substring(const std::string& original, const std::string& normalized, size_t norm_offset, size_t norm_length) {
            // Use the normalized offset directly, but ensure bounds
            size_t orig_start = (norm_offset < original.size()) ? norm_offset : original.size();
            size_t orig_length = (norm_length < original.size() - orig_start) ? norm_length : original.size() - orig_start;
            
            // Ensure we don't split UTF-8 characters
            if (orig_start > 0) {
                // Find the start of the current character
                while (orig_start > 0 && (original[orig_start] & 0xC0) == 0x80) {
                    orig_start--;
                }
            }
            
            if (orig_start + orig_length < original.size()) {
                // Find the end of the current character
                while (orig_start + orig_length < original.size() && (original[orig_start + orig_length] & 0xC0) == 0x80) {
                    orig_length++;
                }
            }
            
            return {orig_start, orig_length};
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

    // Expanded diacritic-stripping table covering:
    // - Latin-1 Supplement (U+00C0–U+00FF)
    // - Latin Extended-A (U+0100–U+017F)
    // - Common Turkish, Romanian, and Vietnamese diacritics
    // - Both uppercase and lowercase
    // Note: This is not exhaustive for all Unicode, but covers most European languages.
    inline char32_t strip_latin_diacritic(char32_t c) {
        switch (c) {
            // --- Latin (existing) ---
            case 0x00C0: case 0x00C1: case 0x00C2: case 0x00C3: case 0x00C4: case 0x00C5: return U'A'; // ÀÁÂÃÄÅ
            case 0x00E0: case 0x00E1: case 0x00E2: case 0x00E3: case 0x00E4: case 0x00E5: return U'a'; // àáâãäå
            case 0x00C8: case 0x00C9: case 0x00CA: case 0x00CB: return U'E'; // ÈÉÊË
            case 0x00E8: case 0x00E9: case 0x00EA: case 0x00EB: return U'e'; // èéêë
            case 0x00CC: case 0x00CD: case 0x00CE: case 0x00CF: return U'I'; // ÌÍÎÏ
            case 0x00EC: case 0x00ED: case 0x00EE: case 0x00EF: return U'i'; // ìíîï
            case 0x00D2: case 0x00D3: case 0x00D4: case 0x00D5: case 0x00D6: return U'O'; // ÒÓÔÕÖ
            case 0x00F2: case 0x00F3: case 0x00F4: case 0x00F5: case 0x00F6: return U'o'; // òóôõö
            case 0x00D9: case 0x00DA: case 0x00DB: case 0x00DC: return U'U'; // ÙÚÛÜ
            case 0x00F9: case 0x00FA: case 0x00FB: case 0x00FC: return U'u'; // ùúûü
            case 0x00C7: return U'C'; // Ç
            case 0x00E7: return U'c'; // ç
            case 0x00D1: return U'N'; // Ñ
            case 0x00F1: return U'n'; // ñ
            case 0x00DD: return U'Y'; // Ý
            case 0x00FD: case 0x00FF: return U'y'; // ýÿ
            // Turkish (also used in Romanian, but only appears once)
            case 0x0130: return U'I'; // İ
            case 0x0131: return U'i'; // ı
            case 0x011E: return U'G'; // Ğ
            case 0x011F: return U'g'; // ğ
            case 0x015E: return U'S'; // Ş (also Romanian)
            case 0x015F: return U's'; // ş (also Romanian)
            // Romanian
            case 0x021A: case 0x0162: return U'T'; // Ț, Ţ
            case 0x021B: case 0x0163: return U't'; // ț, ţ
            case 0x0218: return U'S'; // Ș
            case 0x0219: return U's'; // ș
            // Polish
            case 0x0141: return U'L'; // Ł
            case 0x0142: return U'l'; // ł
            // Czech/Slovak
            case 0x010C: return U'C'; // Č
            case 0x010D: return U'c'; // č
            case 0x010E: return U'D'; // Ď
            case 0x010F: return U'd'; // ď
            case 0x011A: return U'E'; // Ě
            case 0x011B: return U'e'; // ě
            case 0x0147: return U'N'; // Ň
            case 0x0148: return U'n'; // ň
            case 0x0158: return U'R'; // Ř
            case 0x0159: return U'r'; // ř
            case 0x0160: return U'S'; // Š
            case 0x0161: return U's'; // š
            case 0x0164: return U'T'; // Ť
            case 0x0165: return U't'; // ť
            case 0x017D: return U'Z'; // Ž
            case 0x017E: return U'z'; // ž
            // Hungarian
            case 0x0150: return U'O'; // Ő
            case 0x0151: return U'o'; // ő
            case 0x0170: return U'U'; // Ű
            case 0x0171: return U'u'; // ű
            // Croatian/Bosnian/Serbian
            case 0x0110: return U'D'; // Đ
            case 0x0111: return U'd'; // đ
            // Vietnamese (partial)
            case 0x0102: return U'A'; // Ă
            case 0x0103: return U'a'; // ă
            case 0x0128: return U'I'; // Ĩ
            case 0x0129: return U'i'; // ĩ
            case 0x0168: return U'U'; // Ũ
            case 0x0169: return U'u'; // ũ
            // --- Greek ---
            case 0x0386: case 0x0391: return 0x0391; // Ά, Α
            case 0x03AC: case 0x03B1: return 0x03B1; // ά, α
            case 0x0388: case 0x0395: return 0x0395; // Έ, Ε
            case 0x03AD: case 0x03B5: return 0x03B5; // έ, ε
            case 0x0389: case 0x0397: return 0x0397; // Ή, Η
            case 0x03AE: case 0x03B7: return 0x03B7; // ή, η
            case 0x038A: case 0x03AA: case 0x0399: return 0x0399; // Ί, Ϊ, Ι
            case 0x03AF: case 0x03CA: case 0x0390: case 0x03B9: return 0x03B9; // ί, ϊ, ΐ, ι
            case 0x038C: case 0x039F: return 0x039F; // Ό, Ο
            case 0x03CC: case 0x03BF: return 0x03BF; // ό, ο
            case 0x038E: case 0x03AB: case 0x03A5: return 0x03A5; // Ύ, Ϋ, Υ
            case 0x03CD: case 0x03CB: case 0x03B0: case 0x03C5: return 0x03C5; // ύ, ϋ, ΰ, υ
            case 0x038F: case 0x03A9: return 0x03A9; // Ώ, Ω
            case 0x03CE: case 0x03C9: return 0x03C9; // ώ, ω
            // --- Cyrillic ---
            case 0x0401: return 0x0415; // Ё -> Е
            case 0x0451: return 0x0435; // ё -> е
            case 0x0419: return 0x0418; // Й -> И
            case 0x0439: return 0x0438; // й -> и
            case 0x0407: return 0x0418; // Ї -> И
            case 0x0457: return 0x0438; // ї -> и
            case 0x040E: return 0x0423; // Ў -> У
            case 0x045E: return 0x0443; // ў -> у
            case 0x0490: return 0x0413; // Ґ -> Г
            case 0x0491: return 0x0433; // ґ -> г
            default: return c;
        }
    }
    // Helper: returns true if codepoint is a combining diacritical mark (U+0300–U+036F)
    inline bool is_combining_mark(char32_t cp) {
        return (cp >= 0x0300 && cp <= 0x036F);
    }
    // Case folding lookup table for reliable Unicode case folding
    // Handles special cases like Turkish I (İ↔i, I↔ı) and German sharp S (ß↔ss)
    inline char32_t case_fold_codepoint(char32_t c) {
        switch (c) {
            // Basic Latin uppercase to lowercase
            case 0x0041: return 0x0061; // A -> a
            case 0x0042: return 0x0062; // B -> b
            case 0x0043: return 0x0063; // C -> c
            case 0x0044: return 0x0064; // D -> d
            case 0x0045: return 0x0065; // E -> e
            case 0x0046: return 0x0066; // F -> f
            case 0x0047: return 0x0067; // G -> g
            case 0x0048: return 0x0068; // H -> h
            case 0x0049: return 0x0069; // I -> i
            case 0x004A: return 0x006A; // J -> j
            case 0x004B: return 0x006B; // K -> k
            case 0x004C: return 0x006C; // L -> l
            case 0x004D: return 0x006D; // M -> m
            case 0x004E: return 0x006E; // N -> n
            case 0x004F: return 0x006F; // O -> o
            case 0x0050: return 0x0070; // P -> p
            case 0x0051: return 0x0071; // Q -> q
            case 0x0052: return 0x0072; // R -> r
            case 0x0053: return 0x0073; // S -> s
            case 0x0054: return 0x0074; // T -> t
            case 0x0055: return 0x0075; // U -> u
            case 0x0056: return 0x0076; // V -> v
            case 0x0057: return 0x0077; // W -> w
            case 0x0058: return 0x0078; // X -> x
            case 0x0059: return 0x0079; // Y -> y
            case 0x005A: return 0x007A; // Z -> z
            
            // Basic Latin lowercase to lowercase (no change)
            case 0x0061: case 0x0062: case 0x0063: case 0x0064: case 0x0065: case 0x0066: case 0x0067: case 0x0068: case 0x0069: case 0x006A: case 0x006B: case 0x006C: case 0x006D: case 0x006E: case 0x006F: case 0x0070: case 0x0071: case 0x0072: case 0x0073: case 0x0074: case 0x0075: case 0x0076: case 0x0077: case 0x0078: case 0x0079: case 0x007A: return c;
            
            // Turkish I - special case folding
            case 0x0130: return 0x0069; // İ -> i (Turkish dotted I to lowercase i)
            case 0x0131: return 0x0069; // ı -> i (Turkish dotless I to lowercase i)
            
            // German sharp S - special case folding
            case 0x00DF: return 0x0073; // ß -> s (sharp S to lowercase s)
            
            // Greek final sigma
            case 0x03C2: return 0x03C3; // ς -> σ (final sigma to lowercase sigma)
            
            // Lithuanian dot above
            case 0x012F: return 0x0069; // į -> i (i with ogonek to lowercase i)
            
            // Common accented characters
            case 0x00C0: return 0x00E0; // À -> à
            case 0x00C1: return 0x00E1; // Á -> á
            case 0x00C2: return 0x00E2; // Â -> â
            case 0x00C3: return 0x00E3; // Ã -> ã
            case 0x00C4: return 0x00E4; // Ä -> ä
            case 0x00C5: return 0x00E5; // Å -> å
            case 0x00C8: return 0x00E8; // È -> è
            case 0x00C9: return 0x00E9; // É -> é
            case 0x00CA: return 0x00EA; // Ê -> ê
            case 0x00CB: return 0x00EB; // Ë -> ë
            case 0x00CC: return 0x00EC; // Ì -> ì
            case 0x00CD: return 0x00ED; // Í -> í
            case 0x00CE: return 0x00EE; // Î -> î
            case 0x00CF: return 0x00EF; // Ï -> ï
            case 0x00D2: return 0x00F2; // Ò -> ò
            case 0x00D3: return 0x00F3; // Ó -> ó
            case 0x00D4: return 0x00F4; // Ô -> ô
            case 0x00D5: return 0x00F5; // Õ -> õ
            case 0x00D6: return 0x00F6; // Ö -> ö
            case 0x00D9: return 0x00F9; // Ù -> ù
            case 0x00DA: return 0x00FA; // Ú -> ú
            case 0x00DB: return 0x00FB; // Û -> û
            case 0x00DC: return 0x00FC; // Ü -> ü
            case 0x00C7: return 0x00E7; // Ç -> ç
            case 0x00D1: return 0x00F1; // Ñ -> ñ
            case 0x00DD: return 0x00FD; // Ý -> ý
            case 0x00FF: return 0x00FF; // ÿ -> ÿ (no change)
            
            // Default: no change
            default: return c;
        }
    }

    // Helper: convert UTF-8 string to codepoints, strip diacritics (table and combining marks), and re-encode
    inline std::string strip_diacritics(const std::string& input) {
        std::string out;
        size_t i = 0;
        while (i < input.size()) {
            unsigned char c = input[i];
            char32_t cp = 0;
            size_t len = 0;
            if (c < 0x80) { cp = c; len = 1; }
            else if ((c & 0xE0) == 0xC0) { cp = ((c & 0x1F) << 6) | (input[i+1] & 0x3F); len = 2; }
            else if ((c & 0xF0) == 0xE0) { cp = ((c & 0x0F) << 12) | ((input[i+1] & 0x3F) << 6) | (input[i+2] & 0x3F); len = 3; }
            else if ((c & 0xF8) == 0xF0) { cp = ((c & 0x07) << 18) | ((input[i+1] & 0x3F) << 12) | ((input[i+2] & 0x3F) << 6) | (input[i+3] & 0x3F); len = 4; }
            else { cp = c; len = 1; }
            char32_t base = strip_latin_diacritic(cp);
            // Skip combining marks (U+0300–U+036F)
            if (!is_combining_mark(base)) {
                // Re-encode as UTF-8
                if (base < 0x80) out += static_cast<char>(base);
                else if (base < 0x800) {
                    out += static_cast<char>(0xC0 | (base >> 6));
                    out += static_cast<char>(0x80 | (base & 0x3F));
                } else if (base < 0x10000) {
                    out += static_cast<char>(0xE0 | (base >> 12));
                    out += static_cast<char>(0x80 | ((base >> 6) & 0x3F));
                    out += static_cast<char>(0x80 | (base & 0x3F));
                } else {
                    out += static_cast<char>(0xF0 | (base >> 18));
                    out += static_cast<char>(0x80 | ((base >> 12) & 0x3F));
                    out += static_cast<char>(0x80 | ((base >> 6) & 0x3F));
                    out += static_cast<char>(0x80 | (base & 0x3F));
                }
            }
            i += len;
        }
        return out;
    }

    // Add a helper to check for the 'd' option
    inline bool has_diacritic_insensitive(const std::string& options) {
        return options.find('d') != std::string::npos;
    }

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
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE,
    size_t num_workers = 0
);

std::vector<std::string> grep_extract_matches_unicode_impl_sequential(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NONE,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE
);

std::vector<std::string> grep_extract_matches_unicode_impl_parallel(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim,
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NONE,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE,
    size_t num_workers = 0
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
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE,
    size_t num_workers = 0
) {
    #ifdef GRAPA_DEBUG_PRINTF // DEBUG_START
    printf("DEBUG: grep_extract_matches_unicode\n");
    #endif // DEBUG_END
    return grep_extract_matches_unicode_impl_sequential(input, pattern, options, line_delim, normalization, mode);
}

// Simple parallel processing functions
inline std::vector<MatchPosition> grep_unicode_parallel(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim = "",
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NONE,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE,
    size_t num_workers = 0
) {
    // For now, just use the sequential implementation
    // Parallel processing will be implemented later
    return grep_unicode_impl(input, pattern, options, line_delim, normalization, mode);
}

inline std::vector<std::string> grep_extract_matches_unicode_parallel(
    const std::string& input,
    const std::string& pattern,
    const std::string& options,
    const std::string& line_delim = "",
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NONE,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE,
    size_t num_workers = 0
) {
    // For now, just use the sequential implementation
    // Parallel processing will be implemented later
    return grep_extract_matches_unicode_impl(input, pattern, options, line_delim, normalization, mode, num_workers);
} 