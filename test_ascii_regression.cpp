// test_ascii_regression.cpp
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

#include "source/grep/grapa_grep.hpp"
#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

using namespace GrapaUnicode;

struct TestCase {
    std::string input;
    std::string pattern;
    std::string options;
    std::string line_delim;
    std::string description;
};

bool compare_matches(const std::vector<MatchPosition>& a, const std::vector<MatchPosition>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i].offset != b[i].offset || a[i].length != b[i].length || a[i].line_number != b[i].line_number)
            return false;
    }
    return true;
}

int main() {
    std::vector<TestCase> tests = {
        {"Hello World", "Hello", "", "", "Simple match"},
        {"Hello World\nHello Again", "Hello", "", "", "Multiple lines"},
        {"Hello World\nhello again", "hello", "i", "", "Case-insensitive"},
        {"abc def ghi", "def", "", "", "Middle word"},
        {"abc def ghi", "^abc", "", "", "Start anchor"},
        {"abc def ghi", "ghi$", "", "", "End anchor"},
        {"abc\ndef\nghi", "def", "", "", "Line match"},
        {"abc\ndef\nghi", "^def$", "", "", "Exact line match"},
        {"abc def ghi", "[a-z]+", "g", "", "Global match"},
        {"abc def ghi", "[A-Z]+", "i", "", "Case-insensitive class"},
        {"abc def ghi", "[0-9]+", "", "", "No match"},
        {"abc def ghi", "abc|ghi", "g", "", "Alternation"},
        {"abc def ghi", "abc", "v", "", "Invert match"},
        {"abc def ghi", "abc", "c", "", "Count only"},
        {"abc def ghi", "abc", "x", "", "Exact match (should fail)"},
        {"abc", "abc", "x", "", "Exact match (should succeed)"},
        {"abc\nabc\nabc", "abc", "c", "", "Count only multiple"},
        {"abc def ghi", "def", "o", "", "Match only"},
        {"abc def ghi", "def", "b", "", "Byte mode"},
        {"abc def ghi", "def", "n", "", "Line numbers"},
        {"abc def ghi", "def", "A1", "", "After context"},
        {"abc def ghi", "def", "B1", "", "Before context"},
        {"abc def ghi", "def", "C1", "", "Context"},
    };

    int failures = 0;
    for (const auto& test : tests) {
        auto old_matches = grep(test.input, test.pattern, test.options, test.line_delim);
        auto new_matches = grep_unicode(test.input, test.pattern, test.options, test.line_delim, ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
        if (!compare_matches(old_matches, new_matches)) {
            std::cout << "FAILED: " << test.description << std::endl;
            std::cout << "  Input:    '" << test.input << "'" << std::endl;
            std::cout << "  Pattern:  '" << test.pattern << "'" << std::endl;
            std::cout << "  Options:  '" << test.options << "'" << std::endl;
            std::cout << "  Old matches: " << old_matches.size() << std::endl;
            for (const auto& m : old_matches) {
                std::cout << "    [offset=" << m.offset << ", len=" << m.length << ", line=" << m.line_number << "]" << std::endl;
            }
            std::cout << "  New matches: " << new_matches.size() << std::endl;
            for (const auto& m : new_matches) {
                std::cout << "    [offset=" << m.offset << ", len=" << m.length << ", line=" << m.line_number << "]" << std::endl;
            }
            ++failures;
        }
    }
    if (failures == 0) {
        std::cout << "All ASCII regression tests passed!" << std::endl;
    } else {
        std::cout << failures << " test(s) failed." << std::endl;
    }
    return failures;
} 