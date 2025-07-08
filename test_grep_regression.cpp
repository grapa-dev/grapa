#include "source/grep/grapa_grep.hpp"
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

bool compare_matches(const std::vector<std::string>& old_matches, const std::vector<std::string>& new_matches) {
    if (old_matches.size() != new_matches.size()) {
        return false;
    }
    
    for (size_t i = 0; i < old_matches.size(); ++i) {
        if (old_matches[i] != new_matches[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    std::cout << "Grep Regression Test (Old vs New for ASCII)" << std::endl;
    std::cout << "============================================" << std::endl;

    int total_tests = 0;
    int passed_tests = 0;

    // Test 1: Basic ASCII matching
    print_test_header("Basic ASCII Matching");
    {
        std::string input = "Hello World\nTest Line\nAnother Line";
        std::string pattern = "Hello";
        std::string options = "";
        
        auto old_matches = grep_extract_matches(input, pattern, options, "");
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, "");
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("Basic ASCII matching", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Test 2: Case-insensitive matching
    print_test_header("Case-Insensitive Matching");
    {
        std::string input = "Hello World\nHELLO WORLD\nhello world";
        std::string pattern = "hello";
        std::string options = "i";
        
        auto old_matches = grep_extract_matches(input, pattern, options, "");
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, "");
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("Case-insensitive matching", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Test 3: Line numbers
    print_test_header("Line Numbers");
    {
        std::string input = "Line 1: Hello\nLine 2: World\nLine 3: Hello";
        std::string pattern = "Hello";
        std::string options = "n";
        
        auto old_matches = grep_extract_matches(input, pattern, options, "");
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, "");
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("Line numbers", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Test 4: Count mode
    print_test_header("Count Mode");
    {
        std::string input = "Hello World\nHello again\nTest Line";
        std::string pattern = "Hello";
        std::string options = "c";
        
        auto old_matches = grep_extract_matches(input, pattern, options, "");
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, "");
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("Count mode", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Test 5: Invert match
    print_test_header("Invert Match");
    {
        std::string input = "Line 1: Hello\nLine 2: World\nLine 3: Test";
        std::string pattern = "Hello";
        std::string options = "v";
        
        auto old_matches = grep_extract_matches(input, pattern, options, "");
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, "");
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("Invert match", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Test 6: Match only mode
    print_test_header("Match Only Mode");
    {
        std::string input = "Line: Hello World\nLine: Test Line";
        std::string pattern = "Hello|Test";
        std::string options = "o";
        
        auto old_matches = grep_extract_matches(input, pattern, options, "");
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, "");
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("Match only mode", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Test 7: All mode
    print_test_header("All Mode");
    {
        std::string input = "HelloWorldTest";
        std::string pattern = "Hello";
        std::string options = "a";
        
        auto old_matches = grep_extract_matches(input, pattern, options, "");
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, "");
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("All mode", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Test 8: Exact match
    print_test_header("Exact Match");
    {
        std::string input = "Hello\nHello World\nWorld";
        std::string pattern = "Hello";
        std::string options = "x";
        
        auto old_matches = grep_extract_matches(input, pattern, options, "");
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, "");
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("Exact match", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Test 9: Custom delimiter
    print_test_header("Custom Delimiter");
    {
        std::string input = "Line 1: Hello|Line 2: World|Line 3: Hello";
        std::string pattern = "Hello";
        std::string options = "";
        std::string delim = "|";
        
        auto old_matches = grep_extract_matches(input, pattern, options, delim);
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, delim);
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("Custom delimiter", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Test 10: Word boundaries
    print_test_header("Word Boundaries");
    {
        std::string input = "Hello World\nHelloWorld\nWorld Hello";
        std::string pattern = "\\bHello\\b";
        std::string options = "";
        
        auto old_matches = grep_extract_matches(input, pattern, options, "");
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, "");
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("Word boundaries", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Test 11: Complex regex
    print_test_header("Complex Regex");
    {
        std::string input = "Test123\nABC456\nXYZ789";
        std::string pattern = "[A-Z]{3}\\d{3}";
        std::string options = "";
        
        auto old_matches = grep_extract_matches(input, pattern, options, "");
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, "");
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("Complex regex", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Test 12: Multiple matches per line
    print_test_header("Multiple Matches Per Line");
    {
        std::string input = "Hello Hello World\nTest Test Line";
        std::string pattern = "Hello|Test";
        std::string options = "o";
        
        auto old_matches = grep_extract_matches(input, pattern, options, "");
        auto new_matches = grep_extract_matches_unicode(input, pattern, options, "");
        
        print_matches(old_matches, "Old implementation");
        print_matches(new_matches, "New implementation");
        
        bool passed = compare_matches(old_matches, new_matches);
        print_result("Multiple matches per line", passed);
        if (passed) passed_tests++;
        total_tests++;
    }

    // Summary
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "REGRESSION TEST SUMMARY" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Total tests: " << total_tests << std::endl;
    std::cout << "Passed: " << passed_tests << std::endl;
    std::cout << "Failed: " << (total_tests - passed_tests) << std::endl;
    std::cout << "Success rate: " << std::fixed << std::setprecision(1) 
              << (100.0 * passed_tests / total_tests) << "%" << std::endl;
    
    if (passed_tests == total_tests) {
        std::cout << "\n✅ NO REGRESSIONS DETECTED!" << std::endl;
        std::cout << "The new Unicode implementation is fully compatible with ASCII text." << std::endl;
    } else {
        std::cout << "\n❌ REGRESSIONS DETECTED!" << std::endl;
        std::cout << "Some tests failed - the new implementation may have introduced bugs." << std::endl;
    }
    
    return (passed_tests == total_tests) ? 0 : 1;
} 