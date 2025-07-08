// test_enum_fix.cpp
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

#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <string>

using namespace GrapaUnicode;

int main() {
    std::cout << "Testing enum fix..." << std::endl;
    
    // Test that we can use the new enum values
    std::string test_data = "Hello World";
    
    // Test Unicode mode
    auto unicode_matches = grep_unicode(test_data, "Hello", "", "", 
                                       ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    std::cout << "Unicode mode matches: " << unicode_matches.size() << std::endl;
    
    // Test Binary mode
    auto binary_matches = grep_unicode(test_data, "Hello", "", "", 
                                      ProcessingMode::BINARY_MODE, NormalizationForm::NFC);
    std::cout << "Binary mode matches: " << binary_matches.size() << std::endl;
    
    // Test default mode (should be UNICODE_MODE)
    auto default_matches = grep_unicode(test_data, "Hello", "", "");
    std::cout << "Default mode matches: " << default_matches.size() << std::endl;
    
    std::cout << "Enum fix test completed successfully!" << std::endl;
    return 0;
} 