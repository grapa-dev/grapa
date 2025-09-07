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
extern GrapaSystem* gSystem;

class GrapaMainResponse : public GrapaConsoleResponse
{
public:
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
    {
    };
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
    {
    };
    virtual void Send(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen)
    {
        if (sendbuf && sendbuflen > 0) {
            std::cout.write((char*)sendbuf, sendbuflen);
            std::cout.flush();
        }
    }
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
    {
    };
};

class SimpleGrapaCLI {
private:
    GrapaScriptExec mScriptExec;
    GrapaConsoleSend mConsoleSend;
    GrapaNames mRuleVariables;
    GrapaMainResponse mConsoleResponse;
    
public:
    SimpleGrapaCLI() {
        // Set up the console send chain exactly like mainpy.cpp does
        mConsoleSend.mScriptState.vScriptExec = &mScriptExec;
        mScriptExec.vScriptState = &mConsoleSend.mScriptState;
        mConsoleSend.mScriptState.SetNameSpace(&mRuleVariables);
        mRuleVariables.SetResponse(&mConsoleResponse);
        mConsoleSend.Start();
    }
    
    ~SimpleGrapaCLI() {
        mConsoleSend.Stop();
    }
    
    bool start() {
        try {
            // Initialize the global system
            gSystem = GrapaLink::GetGrapaSystem();
            if (!gSystem) {
                std::cerr << "Failed to get Grapa system" << std::endl;
                return false;
            }
            
            // Start the system first - this initializes the grammar data
            bool needExit = false, showConsole = false;
            GrapaCHAR outStr, runStr;
            GrapaLink::Start(needExit, showConsole, outStr, runStr);
            
            // Initialize grammar like the source code does (without checking result)
            if (gSystem->mGrammar.mLength) {
                GrapaCHAR grresult = mConsoleSend.SendSync(gSystem->mGrammar, NULL, 0, GrapaCHAR());
                // Note: source code doesn't check grresult.mLength, so we don't either
                GrapaCHAR configName(gSystem->mHomeDir);
                configName.Append("/.grapa/config");
                GrapaLink::RunFile(mConsoleSend,configName);
                // Note: source code doesn't check grresult.mLength, so we don't either
            }
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Exception during startup: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "Unknown exception during startup" << std::endl;
            return false;
        }
    }
    
    void stop() {
        try {
            GrapaLink::Stop();
        } catch (...) {
            std::cerr << "Exception during shutdown" << std::endl;
        }
    }
    
    std::string executeCommand(const std::string& input) {
        try {
            GrapaCHAR command(input.c_str());
            
            // Use SendSync - it automatically outputs the result to stdout
            GrapaCHAR result = mConsoleSend.SendSync(command, NULL, 0, GrapaCHAR());
            
            // Don't return the result since SendSync already displayed it
            return "";
        } catch (...) {
            return "Error executing command";
        }
    }
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <command>" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " \"2+3\"" << std::endl;
    std::cout << "  " << programName << " \"[1,2,3]\"" << std::endl;
    std::cout << "  " << programName << " \"x=5\"" << std::endl;
    std::cout << "  " << programName << " \"help\"" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "help") {
        printUsage(argv[0]);
        return 0;
    }
    
    SimpleGrapaCLI cli;
    
    if (!cli.start()) {
        std::cerr << "Failed to start Grapa system" << std::endl;
        return 1;
    }
    
    std::string result = cli.executeCommand(command);
    if (!result.empty()) {
        std::cout << result << std::endl;
    }
    
    cli.stop();
    
    return 0;
}
