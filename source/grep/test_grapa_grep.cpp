// test_grapa_grep.cpp
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

#include "grapa_grep.hpp"
#include <iostream>
#include <cassert>

void grapa_test_grep(const std::string& label, const std::string& input, const std::string& pattern,
              const std::string& options, const std::vector<std::string>& expected,
              const std::string& delimiter = "") {
    std::string working_input = normalize_newlines(input);
    auto result = grep_extract_matches(working_input, input, pattern, options, delimiter);
    if (result != expected) {
        std::cerr << "[FAIL] " << label << "\nExpected:";
        for (const auto& e : expected) std::cerr << " [" << e << "]";
        std::cerr << "\nGot:";
        for (const auto& r : result) std::cerr << " [" << r << "]";
        std::cerr << "\n\n";
    } else {
        std::cout << "[PASS] " << label << "\n";
    }
}

int grapa_test_grep_main() {
    // Match digits (match-only)
    grapa_test_grep("Match digits", "apple 123 pear 456\nbanana 789", "\\d+", "o", { "123", "456", "789" });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "o").str() == ["123", "456", "789"].str();

    // Line mode with line numbers
    grapa_test_grep("Line numbers", "apple 123 pear 456\nbanana 789", "\\d+", "on", { "1:123", "1:456", "2:789" });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "on").str() == ["1:123", "1:456", "2:789"].str();

    // Invert match
    grapa_test_grep("Invert match", "apple 123 pear 456\nbanana\n789", "\\d+", "nvo", { "2:banana" });
    // "apple 123 pear 456\nbanana\n789".grep("\\d+", "nvo").str() == ["2:banana"].str();

    // Count only
    grapa_test_grep("Count matches", "apple 123 pear 456\nbanana 789", "\\d+", "c", { "2" });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "c").str() == ["2"].str();

    // Custom delimiter test (replace \n with |||)
    grapa_test_grep("Custom delimiter", "apple 123|||pear 456|||banana 789", "\\d+", "on", { "1:123", "2:456", "3:789" }, "|||");
    // "apple 123|||pear 456|||banana 789".grep("\\d+", "on", "|||").str() == ["1:123", "2:456", "3:789"].str();

    // Invert with delimiter
    grapa_test_grep("Invert with delimiter", "apple|||123|||pear|||banana 789", "\\d+", "onv", { "1:apple", "3:pear" }, "|||");
    // "apple|||123|||pear|||banana 789".grep("\\d+", "onv", "|||").str() == ["1:apple", "3:pear"].str();

    // All-mode match spans
    grapa_test_grep("All-mode match", "apple 123 pear 456\nbanana 789", "\\d+", "ao", { "123", "456", "789" });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "ao").str() == ["123", "456", "789"].str();

    // All-mode inverted spans
    grapa_test_grep("All-mode inverted spans", "apple 123 pear 456\nbanana 789", "\\d+", "aov", { "apple ", " pear ", "\nbanana " });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "aov").str() == ["apple ", " pear ", "\nbanana "].str();

    // Multiple matches on one line
    grapa_test_grep("Multiple matches on one line", "apple 123 pear 456\nbanana 789", "\\d+", "on", { "1:123", "1:456", "2:789" });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "on").str() == ["1:123", "1:456", "2:789"].str();

    // Multiple matches no line number
    grapa_test_grep("Multiple matches no line number", "apple 123 pear 456\nbanana 789", "\\d+", "o", { "123", "456", "789" });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "o").str() == ["123", "456", "789"].str();

    // Grouped line matches
    grapa_test_grep("Multiple matches no line number", "apple 123 pear 456\nbanana 789", "\\d+", "nog", { "1:123,456","2:789" });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "nog").str() == ["1:123,456","2:789"].str();

    // Grouped line matches
    grapa_test_grep("Multiple matches no line number", "apple 123 pear 456\nbanana 789", "\\d+", "nog", { "1:123\n456","2:789" }, "\n");
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "nog", "\n").str() == ["1:123\n456","2:789"].str();

    // Grouped line matches
    grapa_test_grep("Multiple matches no line number", "apple 123 pear 456\r\nbanana 789", "\\d+", "nog", { "1:123\n456","2:789" }, "\n");
    // "apple 123 pear 456\r\nbanana 789".grep("\\d+", "nog", "\n").str() == ["1:123\n456","2:789"].str();

    // Grouped line matches
    grapa_test_grep("Multiple matches no line number", "apple 123 pear 456|||banana 789", "\\d+", "nog", { "1:123|||456","2:789" }, "|||");
    // "apple 123 pear 456|||banana 789".grep("\\d+", "nog", "|||").str() == ["1:123|||456","2:789"].str();


    // All-mode full block (no match-only, no line numbers)
    grapa_test_grep("All-mode full block", "apple 123 pear 456\nbanana 789", "\d+", "a", { "apple 123 pear 456\nbanana 789" });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "a").str() == ["apple 123 pear 456\nbanana 789"].str();

    // All-mode inverted block
    grapa_test_grep("All-mode inverted block", "no digits here", "\d+", "av", { "no digits here" });
    // "no digits here".grep("\\d+", "av").str() == ["no digits here"].str();

    // All-mode match-only with delimiter
    grapa_test_grep("All-mode match-only with delimiter", "apple 123|||pear 456|||banana 789", "\d+", "ao", { "123", "456", "789" }, "|||");
    // "apple 123|||pear 456|||banana 789".grep("\\d+", "ao", "|||").str() == ["123", "456", "789"].str();

    // Grouped line output with inverted match (should ignore 'g')
    grapa_test_grep("Grouped inverted full lines", "abc\n123\ndef", "\d+", "nvg", { "1:abc", "3:def" });
    // "abc\n123\ndef".grep("\\d+", "nvg").str() == ["1:abc", "3:def"].str();

    // Grouped match output with no line numbers
    grapa_test_grep("Grouped matches no line numbers", "apple 123 pear 456\nbanana 789", "\d+", "g", { "apple 123 pear 456","banana 789" });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "g").str() == ["123,456","789"].str();

    // Exact match (whole line must match exactly)
    grapa_test_grep("Exact match (x)", "match this line\nnot this one\nmatch this line", "^match this line$", "x", { "match this line", "match this line" });
    // "match this line\nnot this one\nmatch this line".grep("^match this line$", "x").str() == ["match this line", "match this line"].str();

    // Line number only (lines that matched)
    grapa_test_grep("Line number only (l)", "apple 123 pear 456\nbanana 789", "\\d+", "l", { "1", "2" });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "l").str() == ["1", "2"].str();

    // Offset prefix (b)
    grapa_test_grep("Match with offsets (b)", "apple 123 pear 456\nbanana 789", "\\d+", "b", { "6:123", "15:456", "26:789" });
    // "apple 123 pear 456\nbanana 789".grep("\\d+", "b").str() == ["6:123", "15:456", "26:789"].str();

    // Deduplicate (d)
    grapa_test_grep("Deduplicated matches (d)", "number 123\nrepeat 123\nnew 456", "\\d+", "od", { "123", "456" });
    // "number 123\nrepeat 123\nnew 456".grep("\\d+", "od").str() == ["123", "456"].str();

    // Count deduplicated matches (cd)
    grapa_test_grep("Count unique matches (cd)", "123\n123\n456", "\\d+", "cd", { "2" });
    // "123\n123\n456".grep("\\d+", "cd").str() == ["2"].str();

    // Count deduplicated matches (cd)
    grapa_test_grep("Count unique matches (cd)", "number 123\nrepeat 123\nnew 456", "\\d+", "cd", { "2" });
    // "number 123\nrepeat 123\nrepeat 123\nnew 456".grep("\\d+", "cd").str() == ["3"].str();
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456".grep("\\d+", "cd").str() == ["3"].str();
    // 
    // "number 123\nrepeat 123\nrepeat 123\nnew 456".grep("\\d+", "d").str() == ["number 123","repeat 123","new 456"].str();
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456".grep("\\d+", "d").str() == ["number 123","repeat 123","new 456"].str();
    //
    // "number 123\nrepeat 123\nrepeat 123\nnew 456".grep("\\d+", "o");
    // "number 123\nrepeat 123\nrepeat 123\nnew 456\n".grep("\\d+", "o");
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456".grep("\\d+", "o");
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456\r\n".grep("\\d+", "o");
    // "number 123|||repeat 123|||repeat 123|||new 456".grep("\\d+", "o", "|||");
    // "number 123|||repeat 123|||repeat 123|||new 456|||".grep("\\d+", "o", "|||");

    // "number 123\nrepeat 123\nrepeat 123\nnew 456".grep("\\d+", "oc");
    // "number 123\nrepeat 123\nrepeat 123\nnew 456\n".grep("\\d+", "oc");
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456".grep("\\d+", "oc");
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456\r\n".grep("\\d+", "oc");
    // "number 123|||repeat 123|||repeat 123|||new 456".grep("\\d+", "oc", "|||");
    // "number 123|||repeat 123|||repeat 123|||new 456|||".grep("\\d+", "oc", "|||");

    // "number 123\nrepeat 123\nrepeat 123\nnew 456".grep("\\d+", "c");
    // "number 123\nrepeat 123\nrepeat 123\nnew 456\n".grep("\\d+", "c");
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456".grep("\\d+", "c");
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456\r\n".grep("\\d+", "c");
    // "number 123|||repeat 123|||repeat 123|||new 456".grep("\\d+", "c", "|||");
    // "number 123|||repeat 123|||repeat 123|||new 456|||".grep("\\d+", "c", "|||");
    
    // "number 123\nrepeat 123\nrepeat 123\nnew 456".grep("\\d+", "d");
    // "number 123\nrepeat 123\nrepeat 123\nnew 456\n".grep("\\d+", "d");
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456".grep("\\d+", "d");
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456\r\n".grep("\\d+", "d");
    // "number 123|||repeat 123|||repeat 123|||new 456".grep("\\d+", "d", "|||");
    // "number 123|||repeat 123|||repeat 123|||new 456|||".grep("\\d+", "d", "|||");

    // "number 123\nrepeat 123\nrepeat 123\nnew 456".grep("\\d+", "dc");
    // "number 123\nrepeat 123\nrepeat 123\nnew 456\n".grep("\\d+", "dc");
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456".grep("\\d+", "dc");
    // "number 123\r\nrepeat 123\r\nrepeat 123\r\nnew 456\r\n".grep("\\d+", "dc");
    // "number 123|||repeat 123|||repeat 123|||new 456".grep("\\d+", "dc", "|||");
    // "number 123|||repeat 123|||repeat 123|||new 456|||".grep("\\d+", "dc", "|||");


    // Count + Offset (bc): should return count of matches, ignoring offset
    grapa_test_grep("Count matches with offset flag (bc)", "one 123 two 456", "\\d+", "bc", { "2" });
    // "one 123 two 456".grep("\\d+", "bc").str() == ["2"].str();

    // Count + Dedupe + Offset (bcd): should return count of unique matches
    grapa_test_grep("Count unique matches with offset (bcd)", "123 x 123 y 456", "\\d+", "bcd", { "2" });
    // "123 x 123 y 456".grep("\\d+", "bcd").str() == ["2"].str();
std::cout << "\nAll tests completed.\n";
    return 0;
}
