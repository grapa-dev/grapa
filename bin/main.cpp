// main.cpp - Simple Grapa CLI Tool
#include "grapa/GrapaLink.h"
#include "grapa/GrapaValue.h"
#include "grapa/GrapaSystem.h"
#include "grapa/GrapaCompress.h"
#include "grapa/GrapaLibRule.h"
#include "grapa/GrapaConsole.h"
#include <iostream>
#include <string>

// Global Grapa system - required by the libraries
GrapaSystem* gSystem = NULL;

int main(int argc, char* argv[]) {
    std::cout << "DEBUG: main() entered" << std::endl;
    
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <command>" << std::endl;
        return 1;
    }
    
    std::cout << "DEBUG: About to get Grapa system" << std::endl;
    
    // Try to get the Grapa system
    gSystem = GrapaLink::GetGrapaSystem();
    if (!gSystem) {
        std::cerr << "Failed to get Grapa system" << std::endl;
        return 1;
    }
    
    std::cout << "DEBUG: Got Grapa system successfully" << std::endl;
    std::cout << "DEBUG: Exiting successfully" << std::endl;
    
    return 0;
}
