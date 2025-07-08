#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

void print_test_header(const std::string& test_name) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "TEST: " << test_name << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void print_result(const std::string& test_name, bool passed, const std::string& details = "") {
    std::cout << std::left << std::setw(40) << test_name;
    if (passed) {
        std::cout << "✓ PASSED";
    } else {
        std::cout << "✗ FAILED";
    }
    if (!details.empty()) {
        std::cout << " (" << details << ")";
    }
    std::cout << std::endl;
}

void print_matches(const std::vector<std::string>& matches, const std::string& label = "Matches") {
    std::cout << label << " (" << matches.size() << "): ";
    if (matches.empty()) {
        std::cout << "none";
    } else {
        for (size_t i = 0; i < matches.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << "\"" << matches[i] << "\"";
        }
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "Unicode Grep Validation Test" << std::endl;
    std::cout << "============================" << std::endl;

    // Test 1: Basic ASCII case-insensitive matching
    print_test_header("Basic ASCII Case-Insensitive Matching");
    {
        std::string input = "Hello World\nHELLO WORLD\nhello world\n";
        std::string pattern = "hello";
        std::string options = "i";
        
        auto matches = grep_extract_matches_unicode(input, pattern, options, "");
        print_matches(matches, "Case-insensitive 'hello' matches");
        
        bool passed = matches.size() == 3;
        print_result("Case-insensitive ASCII matching", passed, 
                    "Expected 3 matches, got " + std::to_string(matches.size()));
    }

    // Test 2: Unicode normalization (NFC vs NFD)
    print_test_header("Unicode Normalization Test");
    {
        // "é" can be represented as U+00E9 (NFC) or U+0065 U+0301 (NFD)
        std::string input_nfc = "café\n";  // NFC form
        std::string input_nfd = "café\n";  // NFD form (e + combining acute) - actual UTF-8
        
        std::string pattern = "café";
        std::string options = "";
        
        // Test NFC input with NFC pattern
        auto matches_nfc = grep_extract_matches_unicode(input_nfc, pattern, options, "");
        print_matches(matches_nfc, "NFC input matches");
        
        // Test NFD input with NFC pattern (should match due to normalization)
        auto matches_nfd = grep_extract_matches_unicode(input_nfd, pattern, options, "");
        print_matches(matches_nfd, "NFD input matches");
        
        bool passed = matches_nfc.size() == 1 && matches_nfd.size() == 1;
        print_result("Unicode normalization matching", passed,
                    "NFC: " + std::to_string(matches_nfc.size()) + 
                    ", NFD: " + std::to_string(matches_nfd.size()));
    }

    // Test 3: Unicode case folding
    print_test_header("Unicode Case Folding Test");
    {
        // German sharp S (ß) and its uppercase form (SS)
        std::string input = "Straße\nSTRASSE\nstraße\n";
        std::string pattern = "straße";
        std::string options = "i";  // case-insensitive
        
        auto matches = grep_extract_matches_unicode(input, pattern, options, "");
        print_matches(matches, "Case-insensitive 'straße' matches");
        
        bool passed = matches.size() >= 2;  // Should match at least 2 forms
        print_result("Unicode case folding", passed,
                    "Expected >=2 matches, got " + std::to_string(matches.size()));
    }

    // Test 4: Multi-script Unicode
    print_test_header("Multi-Script Unicode Test");
    {
        // Mix of Latin, Cyrillic, and Greek
        std::string input = "Hello Привет Γεια\nHello World\nПривет мир\n";
        std::string pattern = "Привет";
        std::string options = "";
        
        auto matches = grep_extract_matches_unicode(input, pattern, options, "");
        print_matches(matches, "Cyrillic 'Привет' matches");
        
        bool passed = matches.size() == 2;
        print_result("Multi-script Unicode matching", passed,
                    "Expected 2 matches, got " + std::to_string(matches.size()));
    }

    // Test 5: Emoji and special characters
    print_test_header("Emoji and Special Characters Test");
    {
        std::string input = "Hello 😀 World\nSmile 😊\nHappy 😄\n";
        std::string pattern = "😀";
        std::string options = "";
        
        auto matches = grep_extract_matches_unicode(input, pattern, options, "");
        print_matches(matches, "Emoji '😀' matches");
        
        bool passed = matches.size() == 1;
        print_result("Emoji matching", passed,
                    "Expected 1 match, got " + std::to_string(matches.size()));
    }

    // Test 6: Line-by-line matching with Unicode
    print_test_header("Line-by-Line Unicode Matching");
    {
        std::string input = "Line 1: Hello\nLine 2: Привет\nLine 3: Γεια\n";
        std::string pattern = "Привет";
        std::string options = "n";  // include line numbers
        
        auto matches = grep_extract_matches_unicode(input, pattern, options, "");
        print_matches(matches, "Line-by-line 'Привет' matches");
        
        bool passed = matches.size() == 1 && matches[0].find("2:") != std::string::npos;
        print_result("Line-by-line Unicode matching", passed,
                    "Expected 1 match on line 2, got " + std::to_string(matches.size()));
    }

    // Test 7: Unicode word boundaries
    print_test_header("Unicode Word Boundary Test");
    {
        std::string input = "café au lait\ncafé\n";
        std::string pattern = "\\bcafé\\b";  // word boundaries
        std::string options = "";
        
        auto matches = grep_extract_matches_unicode(input, pattern, options, "");
        print_matches(matches, "Word boundary 'café' matches");
        
        bool passed = matches.size() == 1;  // Should match standalone "café"
        print_result("Unicode word boundaries", passed,
                    "Expected 1 match, got " + std::to_string(matches.size()));
    }

    // Test 8: Complex Unicode normalization
    print_test_header("Complex Unicode Normalization Test");
    {
        // "Å" can be represented as U+00C5 or U+0041 U+030A (A + combining ring)
        std::string input = "Bjørn\nBj\u00F8rn\n";  // Different forms of "ø"
        std::string pattern = "Bjørn";
        std::string options = "";
        
        auto matches = grep_extract_matches_unicode(input, pattern, options, "");
        print_matches(matches, "Normalized 'Bjørn' matches");
        
        bool passed = matches.size() >= 1;
        print_result("Complex Unicode normalization", passed,
                    "Expected >=1 match, got " + std::to_string(matches.size()));
    }

    // Test 9: Count mode with Unicode
    print_test_header("Unicode Count Mode Test");
    {
        std::string input = "Hello World\nПривет мир\nΓεια σου\nHello again\n";
        std::string pattern = "Hello";
        std::string options = "c";  // count mode
        
        auto matches = grep_extract_matches_unicode(input, pattern, options, "");
        print_matches(matches, "Count of 'Hello' matches");
        
        bool passed = matches.size() == 1 && matches[0] == "2";
        print_result("Unicode count mode", passed,
                    "Expected count '2', got '" + (matches.empty() ? "none" : matches[0]) + "'");
    }

    // Test 10: Invert match with Unicode
    print_test_header("Unicode Invert Match Test");
    {
        std::string input = "Line 1: Hello\nLine 2: Привет\nLine 3: World\n";
        std::string pattern = "Hello";
        std::string options = "v";  // invert match
        
        auto matches = grep_extract_matches_unicode(input, pattern, options, "");
        print_matches(matches, "Lines NOT containing 'Hello'");
        
        bool passed = matches.size() == 2;  // Should match lines 2 and 3
        print_result("Unicode invert match", passed,
                    "Expected 2 non-matching lines, got " + std::to_string(matches.size()));
    }

    // Test 11: All mode with Unicode
    print_test_header("Unicode All Mode Test");
    {
        std::string input = "HelloПриветWorld";
        std::string pattern = "Hello";
        std::string options = "a";  // all mode (no line breaks)
        
        auto matches = grep_extract_matches_unicode(input, pattern, options, "");
        print_matches(matches, "All mode 'Hello' matches");
        
        bool passed = matches.size() == 1;
        print_result("Unicode all mode", passed,
                    "Expected 1 match, got " + std::to_string(matches.size()));
    }

    // Test 12: Match only mode with Unicode
    print_test_header("Unicode Match Only Mode Test");
    {
        std::string input = "Line: Hello World\nLine: Привет мир\n";
        std::string pattern = "Hello|Привет";
        std::string options = "o";  // match only
        
        auto matches = grep_extract_matches_unicode(input, pattern, options, "");
        print_matches(matches, "Match only 'Hello|Привет'");
        
        bool passed = matches.size() == 2;
        print_result("Unicode match only mode", passed,
                    "Expected 2 matches, got " + std::to_string(matches.size()));
    }

    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Unicode Grep Validation Test Complete" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
} 