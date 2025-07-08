#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

// Helper function to print hex representation
std::string to_hex(const std::string& str) {
    std::stringstream ss;
    for (unsigned char c : str) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return ss.str();
}

// Helper function to print escaped string
std::string to_escaped(const std::string& str) {
    std::string result;
    for (unsigned char c : str) {
        if (c >= 32 && c <= 126) {
            result += c;
        } else {
            std::stringstream ss;
            ss << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
            result += ss.str();
        }
    }
    return result;
}

void test_unicode_length_debug() {
    std::cout << "Unicode Length Debug Test (C++)" << std::endl;
    std::cout << "===============================" << std::endl;
    
    // Test strings: "café" in NFC and NFD forms
    std::string cafe1 = "café";  // NFC form: 'c' + 'a' + 'f' + 'é' (U+00E9)
    std::string cafe2 = "café";  // NFD form: 'c' + 'a' + 'f' + 'e' + combining acute accent (U+0301)
    
    std::cout << "cafe1 (NFC): '" << cafe1 << "'" << std::endl;
    std::cout << "cafe1 length: " << cafe1.length() << std::endl;
    std::cout << "cafe1 bytes: " << to_hex(cafe1) << std::endl;
    std::cout << "cafe1 escaped: " << to_escaped(cafe1) << std::endl;
    std::cout << std::endl;
    
    std::cout << "cafe2 (NFD): '" << cafe2 << "'" << std::endl;
    std::cout << "cafe2 length: " << cafe2.length() << std::endl;
    std::cout << "cafe2 bytes: " << to_hex(cafe2) << std::endl;
    std::cout << "cafe2 escaped: " << to_escaped(cafe2) << std::endl;
    std::cout << std::endl;
    
    // Create test input
    std::string test_input = cafe1 + "\n" + cafe2 + "\n" + "coffee";
    std::cout << "Full input:" << std::endl;
    std::cout << test_input << std::endl;
    std::cout << "Input length: " << test_input.length() << std::endl;
    std::cout << "Input bytes: " << to_hex(test_input) << std::endl;
    std::cout << std::endl;
    
    // Test 1: Search for "café" with match-only option
    std::cout << "Test 1: Searching for 'café' with match-only option" << std::endl;
    auto matches1 = grep_extract_matches_unicode(test_input, "café", "o");
    std::cout << "Number of matches: " << matches1.size() << std::endl;
    
    for (size_t i = 0; i < matches1.size(); ++i) {
        const auto& match = matches1[i];
        std::cout << "Match " << (i+1) << ":" << std::endl;
        std::cout << "  Text: '" << match << "'" << std::endl;
        std::cout << "  Length: " << match.length() << std::endl;
        std::cout << "  Bytes: " << to_hex(match) << std::endl;
        std::cout << "  Escaped: " << to_escaped(match) << std::endl;
        std::cout << std::endl;
    }
    
    // Test 2: Get raw match positions
    std::cout << "Test 2: Raw match positions for 'café'" << std::endl;
    auto positions1 = grep_unicode(test_input, "café", "o");
    std::cout << "Number of positions: " << positions1.size() << std::endl;
    
    for (size_t i = 0; i < positions1.size(); ++i) {
        const auto& pos = positions1[i];
        std::cout << "Position " << (i+1) << ":" << std::endl;
        std::cout << "  Offset: " << pos.offset << std::endl;
        std::cout << "  Length: " << pos.length << std::endl;
        std::cout << "  Line: " << pos.line_number << std::endl;
        
        // Extract the substring manually
        if (pos.offset + pos.length <= test_input.length()) {
            std::string extracted = test_input.substr(pos.offset, pos.length);
            std::cout << "  Extracted: '" << extracted << "'" << std::endl;
            std::cout << "  Extracted bytes: " << to_hex(extracted) << std::endl;
            std::cout << "  Extracted escaped: " << to_escaped(extracted) << std::endl;
        } else {
            std::cout << "  ERROR: Position out of bounds!" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Test 3: Search for "cafe" (without accent)
    std::cout << "Test 3: Searching for 'cafe' (no accent)" << std::endl;
    auto matches2 = grep_extract_matches_unicode(test_input, "cafe", "o");
    std::cout << "Number of matches: " << matches2.size() << std::endl;
    
    for (size_t i = 0; i < matches2.size(); ++i) {
        const auto& match = matches2[i];
        std::cout << "Match " << (i+1) << ":" << std::endl;
        std::cout << "  Text: '" << match << "'" << std::endl;
        std::cout << "  Length: " << match.length() << std::endl;
        std::cout << "  Bytes: " << to_hex(match) << std::endl;
        std::cout << std::endl;
    }
    
    // Test 4: Test UnicodeRegex directly
    std::cout << "Test 4: Testing UnicodeRegex directly" << std::endl;
    GrapaUnicode::UnicodeRegex rx("café", false, GrapaUnicode::NormalizationForm::NFC);
    GrapaUnicode::UnicodeString unicode_input(test_input);
    
    auto regex_matches = rx.find_all(unicode_input);
    std::cout << "UnicodeRegex found " << regex_matches.size() << " matches" << std::endl;
    
    for (size_t i = 0; i < regex_matches.size(); ++i) {
        const auto& match = regex_matches[i];
        std::cout << "Regex Match " << (i+1) << ":" << std::endl;
        std::cout << "  Position: " << match.first << std::endl;
        std::cout << "  Length: " << match.second << std::endl;
        
        // Extract from normalized text
        GrapaUnicode::UnicodeString normalized = unicode_input.normalize(GrapaUnicode::NormalizationForm::NFC);
        if (match.first + match.second <= normalized.data().length()) {
            std::string extracted = normalized.data().substr(match.first, match.second);
            std::cout << "  Extracted from normalized: '" << extracted << "'" << std::endl;
            std::cout << "  Extracted bytes: " << to_hex(extracted) << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Test 5: Test normalization directly
    std::cout << "Test 5: Testing normalization directly" << std::endl;
    GrapaUnicode::UnicodeString str1(cafe1);
    GrapaUnicode::UnicodeString str2(cafe2);
    
    auto norm1 = str1.normalize(GrapaUnicode::NormalizationForm::NFC);
    auto norm2 = str2.normalize(GrapaUnicode::NormalizationForm::NFC);
    
    std::cout << "cafe1 normalized (NFC): '" << norm1.data() << "'" << std::endl;
    std::cout << "cafe1 normalized bytes: " << to_hex(norm1.data()) << std::endl;
    std::cout << "cafe2 normalized (NFC): '" << norm2.data() << "'" << std::endl;
    std::cout << "cafe2 normalized bytes: " << to_hex(norm2.data()) << std::endl;
    std::cout << "Equal after normalization: " << (norm1.data() == norm2.data() ? "YES" : "NO") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Analysis:" << std::endl;
    std::cout << "- Check if match lengths match expected byte counts" << std::endl;
    std::cout << "- Look for truncation in byte representations" << std::endl;
    std::cout << "- Verify Unicode normalization is working" << std::endl;
    std::cout << "- Compare raw positions vs extracted strings" << std::endl;
}

int main() {
    test_unicode_length_debug();
    return 0;
} 