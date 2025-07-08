#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>

int main() {
    std::string input = "Hello World\nTest Line\nAnother Line";
    std::string pattern = "World";
    std::string options = "i";
    std::string delim = "";
    
    auto matches = grep_extract_matches_unicode(input, pattern, options, delim);
    
    std::cout << "Found " << matches.size() << " matches" << std::endl;
    for (const auto& match : matches) {
        std::cout << "Match: " << match << std::endl;
    }
    
    return 0;
} 