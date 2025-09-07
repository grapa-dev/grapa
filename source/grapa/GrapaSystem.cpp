// GrapaSystem.cpp
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

#include "GrapaSystem.h"
#include "GrapaLink.h"
#include "GrapaTime.h"
#include <string>
#include <algorithm>  // for std::max and std::min

#ifdef WIN32
	// Windows-specific includes (already included in GrapaSystem.h)
#else
#include <unistd.h>   // for read()
#include <termios.h>  // for terminal settings (if needed)
#include <errno.h>
#include <sys/select.h>  // for select()
#endif

#include <openssl/rand.h>

// Undefine max and min macros that might be defined by X11 headers
// to prevent conflicts with std::vector<bool> and other C++ standard library components
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

// Remove the max and min macros to avoid conflicts with std::vector<bool>
// Use std::max and std::min from <algorithm> instead

extern GrapaSystem* gSystem;

GrapaDebug::GrapaDebug() 
{
	mDebugMode = false;
	mDebugLevel = 0;
	mDebugComponents.FROM("*");  // Default to all components
	
	// Read environment variables for initial values
	const char* debugMode = getenv("GRAPA_DEBUG_MODE");
	if (debugMode && strcmp(debugMode, "1") == 0) {
		mDebugMode = true;
	}
	
	const char* debugLevel = getenv("GRAPA_DEBUG_LEVEL");
	if (debugLevel) {
		mDebugLevel = atoi(debugLevel);
	}
	
	const char* debugComponents = getenv("GRAPA_DEBUG_COMPONENTS");
	if (debugComponents) {
		mDebugComponents.FROM(debugComponents);
	}
}

GrapaDebug::~GrapaDebug() 
{
}

void GrapaDebug::DebugPrint(const char* pStr)
{
	if (!mDebugMode) return;

	gSystem->mPrintLock.WaitCritical();
	fprintf(stderr, "[DEBUG-0-system] %s\n", pStr);
	gSystem->mPrintLock.LeaveCritical();
}

void GrapaDebug::DebugPrint(const GrapaCHAR& pValue)
{
	if (!mDebugMode) return;

	gSystem->mPrintLock.WaitCritical();
	fprintf(stderr, "[DEBUG-0-system] %.*s\n", (int)pValue.mLength, (char*)pValue.mBytes);
	gSystem->mPrintLock.LeaveCritical();
}

int GrapaDebug::GetComponentDebugLevel(const char* component)
{
	if (!mDebugMode) return 0;
	if (mDebugComponents.Cmp("*") == 0) return mDebugLevel;
	
	// Parse component-specific debug levels
	// Format: "grep:2,vector:1,database:3" or "grep:2,*:1"
	const char* components = (const char*)mDebugComponents.mBytes;
	if (!components || !component) return 0;
	
	// Handle "compiler" shorthand - expand to "lexer,parser"
	if (strcmp(component, "compiler") == 0) {
		// Check if either lexer or parser is enabled
		int lexerLevel = GetComponentDebugLevel("lexer");
		int parserLevel = GetComponentDebugLevel("parser");
		// Return the higher level if either is enabled
		return (lexerLevel > parserLevel) ? lexerLevel : parserLevel;
	}
	
	// Handle "runtime" shorthand - expand to "compiler,executor"
	if (strcmp(component, "runtime") == 0) {
		// Check if either compiler or executor is enabled
		int compilerLevel = GetComponentDebugLevel("compiler");
		int executorLevel = GetComponentDebugLevel("executor");
		// Return the higher level if either is enabled
		return (compilerLevel > executorLevel) ? compilerLevel : executorLevel;
	}
	
	// First, look for exact component match with level
	char searchPattern[256];
	snprintf(searchPattern, sizeof(searchPattern), "%s:", component);
	const char* pos = strstr(components, searchPattern);
	if (pos) {
		// Found component with level specification
		const char* levelStart = pos + strlen(searchPattern);
		const char* levelEnd = strchr(levelStart, ',');
		if (levelEnd) {
			// Extract level number
			char levelStr[16];
			u64 levelLen = levelEnd - levelStart;
			if (levelLen < sizeof(levelStr)) {
				strncpy(levelStr, levelStart, levelLen);
				levelStr[levelLen] = '\0';
				return atoi(levelStr);
			}
		} else {
			// Component is at the end of the list
			return atoi(levelStart);
		}
	}
	
	// Look for wildcard level specification
	pos = strstr(components, "*:");
	if (pos) {
		const char* levelStart = pos + 2; // Skip "*:"
		const char* levelEnd = strchr(levelStart, ',');
		if (levelEnd) {
			char levelStr[16];
			u64 levelLen = levelEnd - levelStart;
			if (levelLen < sizeof(levelStr)) {
				strncpy(levelStr, levelStart, levelLen);
				levelStr[levelLen] = '\0';
				return atoi(levelStr);
			}
		} else {
			return atoi(levelStart);
		}
	}
	
	// Fallback to simple component matching (no level specified)
	pos = strstr(components, component);
	if (pos) {
		u64 componentLen = strlen(component);
		if (pos > components && *(pos-1) != ',') return 0;  // Not start of list
		if (pos + componentLen < components + mDebugComponents.mLength && *(pos + componentLen) != ',') return 0;  // Not end of list
		return mDebugLevel; // Use default level
	}
	
	return 0; // Component not found
}

bool GrapaDebug::ShouldDebug(const char* component, int minLevel)
{
	int componentLevel = GetComponentDebugLevel(component);
	return componentLevel >= minLevel;
}

void GrapaDebug::DebugPrint(const char* component, const char* pStr, int level)
{
	if (!ShouldDebug(component, level)) return;

	gSystem->mPrintLock.WaitCritical();
	fprintf(stderr, "[DEBUG-%s] %s\n", component, pStr);
	gSystem->mPrintLock.LeaveCritical();
}

void GrapaDebug::DebugPrint(const char* component, const GrapaCHAR& pValue, int level)
{
	if (!ShouldDebug(component, level)) return;

	gSystem->mPrintLock.WaitCritical();
	fprintf(stderr, "[DEBUG-%s] %.*s\n", component, (int)pValue.mLength, (char*)pValue.mBytes);
	gSystem->mPrintLock.LeaveCritical();
}

GrapaSystem::GrapaSystem()
{
#ifdef WIN32
	mStdinRef = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(mStdinRef, &mStdinMode);
	
	// Enable UTF-8 mode for Windows console
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	
			// Enable virtual terminal processing for better Unicode support
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut != INVALID_HANDLE_VALUE) {
			DWORD mode;
			if (GetConsoleMode(hOut, &mode)) {
				mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				SetConsoleMode(hOut, mode);
			}
			
			// Also set input handle for better Unicode input
			HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
			if (hIn != INVALID_HANDLE_VALUE) {
				DWORD inMode;
				if (GetConsoleMode(hIn, &inMode)) {
					inMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
					SetConsoleMode(hIn, inMode);
				}
			}
		
		// Set console font to support Unicode characters
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = 0;
		cfi.dwFontSize.Y = 16;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		
		// Try Consolas first (best Unicode support), then Cascadia Code, then fallback
		wcscpy_s(cfi.FaceName, L"Consolas");
		if (!SetCurrentConsoleFontEx(hOut, FALSE, &cfi)) {
			wcscpy_s(cfi.FaceName, L"Cascadia Code");
			if (!SetCurrentConsoleFontEx(hOut, FALSE, &cfi)) {
				wcscpy_s(cfi.FaceName, L"Lucida Console");
				SetCurrentConsoleFontEx(hOut, FALSE, &cfi);
			}
		}
	}
#else
	mStdinRef = STDIN_FILENO;
	tcgetattr(mStdinRef, &mStdinMode);
#endif
	mLib.mValue.FROM("$SYS");
	mPath = NULL;
	mVersion.FROM(grapa_version);
	mStaticLib = NULL;
	mArgv = new GrapaRuleQueue();
	mCliArgv = new GrapaRuleQueue();  // Initialize CLI arguments queue
	mLinkInitialized = false;
	mNextSessionId = 1;  // Start session IDs at 1
	mSuppressPrompt = false;  // Initialize prompt suppression flag
}

GrapaSystem::~GrapaSystem()
{
#ifdef WIN32
	SetConsoleMode(mStdinRef, mStdinMode);
#else
	(void)tcsetattr(mStdinRef, TCSANOW, &mStdinMode);
#endif
	if (mPath)
	{
		mPath->CLEAR();
		delete mPath;
		mPath = NULL;
	}
	if (mStaticLib)
	{
		mStaticLib->CLEAR();
		delete mStaticLib;
		mStaticLib = NULL;
	}
	if (mArgv)
	{
		mArgv->CLEAR();
		delete mArgv;
		mArgv = NULL;
	}
	if (mCliArgv)
	{
		mCliArgv->CLEAR();
		delete mCliArgv;
		mCliArgv = NULL;
	}
}

//void GrapaSystem::Print(const char* pStr, bool flush)
//{
//	mPrintLock.WaitCritical();
//	if (mUseGfx)
//	{
//		mGfxConsole.disp->buffer()->append((char*)pStr);
//	}
//	else
//	{
//		if (pStr) std::cout << (char*)pStr;
//		if (flush) std::cout.flush();
//	}
//	mPrintLock.LeaveCritical();
//}
//
//void GrapaSystem::Print(const char pChar, bool flush)
//{
//	mPrintLock.WaitCritical();
//	if (mUseGfx)
//	{
//		char v[2];
//		v[0] = pChar;
//		v[1] = 0;
//		mGfxConsole.disp->buffer()->append((char*)v);
//	}
//	else
//	{
//		std::cout << (char)pChar;
//		if (flush) std::cout.flush();
//	}
//	mPrintLock.LeaveCritical();
//}
//
//void GrapaSystem::Print(const GrapaCHAR& pValue, bool flush)
//{
//	GrapaCHAR s((char*)pValue.mBytes, pValue.mLength);
//	mPrintLock.WaitCritical();
//	if (mUseGfx)
//	{
//		mGfxConsole.disp->buffer()->append((char*)pValue.mBytes);
//	}
//	else
//	{
//		if (s.mBytes) std::cout << (char*)s.mBytes;
//		if (flush) std::cout.flush();
//	}
//	mPrintLock.LeaveCritical();
//}
//
//void GrapaSystem::PrintLine(const GrapaCHAR& pValue, bool flush)
//{
//	mPrintLock.WaitCritical();
//	if (mUseGfx)
//	{
//		mGfxConsole.disp->buffer()->append((char*)pValue.mBytes);
//	}
//	else
//	{
//		if (pValue.mBytes) std::cout << (char*)pValue.mBytes;
//		std::cout << (char*)"\r\n";
//		if (flush) std::cout.flush();
//	}
//	mPrintLock.LeaveCritical();
//}
//
//void GrapaSystem::PrintTimeLine(const char* pChar, bool flush)
//{
//	GrapaTime t;
//	t.SetNow();
//	GrapaCHAR s(t.getString());
//	for (s64 i = s.mLength; i < 27;i++)
//		s.Append(' ');
//	s.Append(pChar);
//	PrintLine(s,flush);
//}



//
//void GrapaSystem::Print(const GrapaSU64& pValue, bool flush)
//{
//	GrapaCHAR value(pValue.mNum);
//	Print(value, flush);
//}
//
//void GrapaSystem::Print(u64 pValue, bool flush)
//{
//	GrapaCHAR value(pValue);
//	Print(value, flush);
//}

// Helper: encode a single code point as UTF-8
static inline std::string utf8_encode(uint32_t codepoint) {
	std::string out;
	if (codepoint <= 0x7F) {
		out += static_cast<char>(codepoint);
	}
	else if (codepoint <= 0x7FF) {
		out += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
		out += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	else if (codepoint <= 0xFFFF) {
		out += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
		out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	else if (codepoint <= 0x10FFFF) {
		out += static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07));
		out += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
		out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	return out;
}

int GrapaSystem::GetCharOLD()
{
	int ch=0;
#ifdef WIN32
	// Use Unicode console input for better character support
	DWORD read;
	WCHAR wch;
	BOOL x = ReadConsoleW(mStdinRef, &wch, 1, &read, NULL);
	if (x && read > 0) {
		// Convert wide char to UTF-8
		char utf8[5];
		int utf8len = WideCharToMultiByte(CP_UTF8, 0, &wch, 1, utf8, sizeof(utf8), NULL, NULL);
		if (utf8len > 0) {
			// Return the first byte for backward compatibility
			ch = (unsigned char)utf8[0];
		}
	}
#else
	ch = getchar();
#endif
	return ch;
}

std::string GrapaSystem::GetUtf8Char()
{
    std::string result;
#ifdef WIN32
    DWORD read;
    WCHAR wch[2];
    
    // Windows: Use Windows API UTF-8 conversion for reliability
    if (gSystem->mStop) return result;
    
    // Block until we get input (this is the Windows limitation)
    BOOL x = ReadConsoleW(mStdinRef, &wch[0], 1, &read, NULL);
    if (x && read > 0) {
        // Check for surrogate pair
        if (wch[0] >= 0xD800 && wch[0] <= 0xDBFF) {
            // High surrogate, read the next WCHAR
            DWORD read2;
            BOOL x2 = ReadConsoleW(mStdinRef, &wch[1], 1, &read2, NULL);
            if (x2 && read2 > 0 && wch[1] >= 0xDC00 && wch[1] <= 0xDFFF) {
                // Combine surrogate pair and convert to UTF-8
                WCHAR surrogate_pair[2] = {wch[0], wch[1]};
                char utf8[5];
                int utf8len = WideCharToMultiByte(CP_UTF8, 0, surrogate_pair, 2, utf8, sizeof(utf8), NULL, NULL);
                if (utf8len > 0) {
                    result = std::string(utf8, utf8len);
                }
            } else {
                // Invalid surrogate, just convert the first
                char utf8[5];
                int utf8len = WideCharToMultiByte(CP_UTF8, 0, &wch[0], 1, utf8, sizeof(utf8), NULL, NULL);
                if (utf8len > 0) {
                    result = std::string(utf8, utf8len);
                }
            }
        } else {
            // Not a surrogate, convert to UTF-8
            char utf8[5];
            int utf8len = WideCharToMultiByte(CP_UTF8, 0, &wch[0], 1, utf8, sizeof(utf8), NULL, NULL);
            if (utf8len > 0) {
                result = std::string(utf8, utf8len);
            }
        }
    }
#else
    // POSIX: Use select() with timeout and loop until input or exit
    fd_set readfds;
    struct timeval timeout;
    
    // Loop until we get input or need to exit
    while (!gSystem->mStop) {
        // Set up select() with 100ms timeout
        FD_ZERO(&readfds);
        FD_SET(mStdinRef, &readfds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100ms timeout
        
        int select_result = select(mStdinRef + 1, &readfds, NULL, NULL, &timeout);
        
        if (select_result > 0 && FD_ISSET(mStdinRef, &readfds)) {
            // Data is available, read the first byte
            unsigned char first;
            ssize_t n = read(mStdinRef, &first, 1);
            if (n == 1) {
                result += first;
                // Determine how many more bytes to read for this UTF-8 character
                int expected = 0;
                if ((first & 0x80) == 0x00) {
                    expected = 0; // ASCII, single byte
                }
                else if ((first & 0xE0) == 0xC0) {
                    expected = 1; // 2-byte sequence
                }
                else if ((first & 0xF0) == 0xE0) {
                    expected = 2; // 3-byte sequence
                }
                else if ((first & 0xF8) == 0xF0) {
                    expected = 3; // 4-byte sequence
                }
                
                // Read remaining bytes for multi-byte UTF-8 sequences
                for (int i = 0; i < expected; ++i) {
                    // Check for exit condition before reading each additional byte
                    if (gSystem->mStop) break;
                    
                    // Use select() with shorter timeout for additional bytes
                    FD_ZERO(&readfds);
                    FD_SET(mStdinRef, &readfds);
                    timeout.tv_sec = 0;
                    timeout.tv_usec = 50000; // 50ms timeout for additional bytes
                    
                    select_result = select(mStdinRef + 1, &readfds, NULL, NULL, &timeout);
                    if (select_result > 0 && FD_ISSET(mStdinRef, &readfds)) {
                        unsigned char next;
                        if (read(mStdinRef, &next, 1) == 1) {
                            result += next;
                        } else {
                            break; // error or EOF
                        }
                    } else {
                        break; // timeout or no data available
                    }
                }
                break; // We got a character, exit the loop
            }
        }
        // If select_result == 0, it's a timeout - continue the loop to check mStop again
        // If select_result < 0, it's an error - continue the loop (could add error handling)
    }
#endif
    return result;
}

void GrapaSystem::SetEcho(bool enable)
{
#ifdef WIN32
	DWORD mode;
	GetConsoleMode(mStdinRef, &mode);
	if (!enable)
		mode &= ~ENABLE_ECHO_INPUT;
	else
		mode |= ENABLE_ECHO_INPUT;
	SetConsoleMode(mStdinRef, mode);

#else
	struct termios tty;
	tcgetattr(mStdinRef, &tty);
	if (!enable)
	{
		tty.c_lflag &= ~ICANON;
		tty.c_lflag &= ~ECHO;
	}
	else
	{
		tty.c_lflag |= ICANON;
		tty.c_lflag |= ECHO;
	}
	(void)tcsetattr(mStdinRef, TCSANOW, &tty);
#endif
}

void GrapaSystem::SetLineInput(bool enable)
{
#ifdef WIN32
	DWORD mode;
	GetConsoleMode(mStdinRef, &mode);
	if (!enable)
	{
		mode &= ~ENABLE_LINE_INPUT;
		mode &= ~ENABLE_EXTENDED_FLAGS;
		mode &= ~ENABLE_INSERT_MODE;
		mode &= ~ENABLE_MOUSE_INPUT;
		mode &= ~ENABLE_PROCESSED_INPUT;
		mode &= ~ENABLE_QUICK_EDIT_MODE;
		mode &= ~ENABLE_WINDOW_INPUT;
	}
	else
	{
		mode |= ENABLE_LINE_INPUT;
		mode |= ENABLE_EXTENDED_FLAGS;
		mode |= ENABLE_INSERT_MODE;
		mode |= ENABLE_MOUSE_INPUT;
		mode |= ENABLE_PROCESSED_INPUT;
		mode |= ENABLE_QUICK_EDIT_MODE;
		mode |= ENABLE_WINDOW_INPUT;
	}
	SetConsoleMode(mStdinRef, mode);

#else
	struct termios tty;
	tcgetattr(mStdinRef, &tty);
	if (!enable)
		tty.c_lflag &= ~ICANON;
	else
		tty.c_lflag |= ICANON;
	(void)tcsetattr(mStdinRef, TCSANOW, &tty);
#endif
}

void GrapaSystem::RandSeed(u64 pSeed)
{
	u32 seed = Random32();
	if (pSeed)
	{
		for (size_t i = 0; i < sizeof(u64); i++)
			seed = seed * (0xff + 2u) + ((u8*)&pSeed)[i];
	}
	GrapaTime t;
	t.SetNow();
	u64 timeval = t.mSec.LongValue();
	unsigned char *ptr = (unsigned char *)&timeval;
	for (size_t i = 0; i < sizeof(u64); i++)
		seed = seed * (0xff + 2u) + ptr[i];
	srand(seed);
	u32 rd = Random32();
	RAND_seed((void*)&rd, 32);
}

u64 GrapaSystem::GetNextSessionId()
{
	mSessionIdLock.WaitCritical();
	u64 sessionId = mNextSessionId++;
	mSessionIdLock.LeaveCritical();
	return sessionId;
}

GrapaCHAR GrapaSystem::exec(GrapaCHAR& command)
{
	char buffer[128];
	GrapaCHAR result;
#ifdef WIN32
	FILE* pipe = _popen((char*)command.mBytes, "r");
#else
	FILE* pipe = popen((char*)command.mBytes, "r");
#endif
	if (pipe)
	{
		while (!feof(pipe))
		{
			if (fgets(buffer, 128, pipe) != NULL)
				result.Append(buffer);
		}
#ifdef WIN32
		_pclose(pipe);
#else
		pclose(pipe);
#endif
	}
	else
	{ 
	}
	return result;
}

void GrapaConsole2Response::SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
{
    // printf("[DEBUG] GrapaConsole2Response::SendCommand called, sendbuflen=%llu\n", (unsigned long long)sendbuflen);
    // if (sendbuflen) printf("[DEBUG] sendbuf='%.*s'\n", (int)sendbuflen, (char*)sendbuf);
#ifdef WIN32
	// Use standard output for proper stream redirection
	if (sendbuflen) {
		std::cout << (char*)sendbuf;
	}
	std::cout.flush();
#else
	if (sendbuflen) std::cout << (char*)sendbuf;
	std::cout.flush();
#endif
}

void GrapaConsole2Response::SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
{
    // printf("[DEBUG] GrapaConsole2Response::SendPrompt called, sendbuf.mLength=%llu\n", (unsigned long long)sendbuf.mLength);
    // if (sendbuf.mLength) printf("[DEBUG] sendbuf='%.*s'\n", (int)sendbuf.mLength, (char*)sendbuf.mBytes);
#ifdef WIN32
	// Use standard output for proper stream redirection
	if (sendbuf.mLength) {
		std::cout << (char*)sendbuf.mBytes;
	}
	std::cout.flush();
#else
	if (sendbuf.mLength) std::cout << (char*)sendbuf.mBytes;
	std::cout.flush();
#endif
}

void GrapaConsole2Response::SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
    // printf("[DEBUG] GrapaConsole2Response::SendEnd called\n");
	if (pValue)
	{
		if (pValue->vQueue) vScriptExec->EchoList(this, pValue, false, false, false);
		else vScriptExec->EchoValue(this, pValue, false, false, false);
	}
#ifdef WIN32
	// Use standard output for proper stream redirection
	std::cout << (char*)"\n";
	std::cout.flush();
#else
	std::cout << (char*)"\n";
	std::cout.flush();
#endif
}

void GrapaConsole2Response::SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen)
{
    // printf("[DEBUG] GrapaConsole2Response::SendError called, sendbuflen=%llu\n", (unsigned long long)sendbuflen);
    // if (sendbuflen) printf("[DEBUG] sendbuf='%.*s'\n", (int)sendbuflen, (char*)sendbuf);
#ifdef WIN32
	// Use standard error for proper stream redirection
	if (sendbuflen) {
		fprintf(stderr, "%.*s", (int)sendbuflen, (char*)sendbuf);
	}
	fflush(stderr);
#else
	if (sendbuflen) fprintf(stderr, "%.*s", (int)sendbuflen, (char*)sendbuf);
	fflush(stderr);
#endif
}

void GrapaConsole2Response::SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
{
    // printf("[DEBUG] GrapaConsole2Response::SendError called, sendbuf.mLength=%llu\n", (unsigned long long)sendbuf.mLength);
    // if (sendbuf.mLength) printf("[DEBUG] sendbuf='%.*s'\n", (int)sendbuf.mLength, (char*)sendbuf.mBytes);
#ifdef WIN32
	// Use standard error for proper stream redirection
	if (sendbuf.mLength) {
		fprintf(stderr, "%.*s", (int)sendbuf.mLength, (char*)sendbuf.mBytes);
	}
	fflush(stderr);
#else
	if (sendbuf.mLength) fprintf(stderr, "%.*s", (int)sendbuf.mLength, (char*)sendbuf.mBytes);
	fflush(stderr);
#endif
}

void GrapaConsole2Response::SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
    // printf("[DEBUG] GrapaConsole2Response::SendError called\n");
	if (pValue)
	{
		if (pValue->vQueue) vScriptExec->EchoList(this, pValue, false, false, false);
		else vScriptExec->EchoValue(this, pValue, false, false, false);
	}
#ifdef WIN32
	// Use standard error for proper stream redirection
	fprintf(stderr, "\n");
	fflush(stderr);
#else
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
}

My_Console::My_Console()
{
	mRuleVariables.SetResponse(&mConsole2Response);
	mConsoleSend.mScriptState.SetNameSpace(&mRuleVariables);
}

My_Console::~My_Console()
{
	Stop();
}

void My_Console::Start(GrapaCHAR& in, GrapaCHAR& out, GrapaCHAR& run)
{
    // [DEBUG] Print input arguments
    // printf("[DEBUG] Entering My_Console::Start\n");
    // printf("[DEBUG] in.mLength=%llu: '%.*s'\n", (unsigned long long)in.mLength, (int)in.mLength, (char*)in.mBytes);
    // printf("[DEBUG] out.mLength=%llu: '%.*s'\n", (unsigned long long)out.mLength, (int)out.mLength, (char*)out.mBytes);
    // printf("[DEBUG] run.mLength=%llu: '%.*s'\n", (unsigned long long)run.mLength, (int)run.mLength, (char*)run.mBytes);
    GrapaError err = mConsoleSend.Start();
    GrapaCHAR grresult;
    if (gSystem->mGrammar.mLength) 
		grresult = mConsoleSend.SendSync(gSystem->mGrammar,NULL,0,GrapaCHAR());

    RunConfig();

    if (run.mLength) 
		grresult = mConsoleSend.SendSync(run,NULL,0,GrapaCHAR());

}

void My_Console::RunConfig()
{
	GrapaCHAR inStr;
	inStr.FROM(gSystem->mHomeDir);
	inStr.Append("/.grapa/config.grz");
	GrapaFileIO gf;
	GrapaError err = gf.Open((char*)inStr.mBytes);
	if (err)
	{
		inStr.FROM(gSystem->mWorkDir);
		inStr.Append("/.grapa/config.grc");
		err = gf.Open((char*)inStr.mBytes);
	}
	GrapaCHAR setValue;
	GrapaCHAR grresult;
	if (err == 0)
	{
		u64 fsize = 0;
		err = gf.GetSize(fsize);
		setValue.SetLength(fsize);
		err = gf.Read(0, 1, 0, fsize, setValue.mBytes);
		gf.Close();
		setValue.SetLength(fsize);
	}
	if (err == 0 && setValue.mLength)
		grresult = mConsoleSend.SendSync(setValue,NULL,0,GrapaCHAR());

		/*
		GrapaRuleEvent* plan = NULL;

		if (setValue.mLength > 4 && setValue.mBytes[0] == 'G' && setValue.mBytes[1] == 'R' && setValue.mBytes[2] == 'Z' && (setValue.mBytes[3] & 0x80) == 0)
		{
			GrapaBYTE expanded;
			GrapaCompress::Expand(setValue, expanded);
			plan = new GrapaRuleEvent();
			plan->vQueue = new GrapaRuleQueue();

			if (expanded.mToken == GrapaTokenType::LIST || expanded.mToken == GrapaTokenType::ARRAY || expanded.mToken == GrapaTokenType::TUPLE)
			{
				GrapaRuleQueue* tq = new GrapaRuleQueue();
				((GrapaRuleQueue*)tq)->FROM(mScriptExec.vScriptState, &mRuleVariables, expanded);
				GrapaRuleEvent* tx = NULL;
				s64 idx = 0;
				if (tq->mCount > 1 && expanded.mToken == GrapaTokenType::LIST)
					tx = tq->Search("op", idx);
				if (tx == NULL)
					tx = tq->Head();
				if (tx)
				{
					tq->PopEvent(tx);
					plan->mValue.mToken = tx->mValue.mToken;
					plan->vQueue = tx->vQueue;
					tx->vQueue = NULL;
					plan->vClass = tx->vClass;
					if (plan->vClass == NULL && tx->mName.mLength)
					{
						plan->vClass = mScriptExec.vScriptState->GetClass(&mRuleVariables, tx->mName);
					}
					tx->CLEAR();
					delete tx;
					tx = NULL;
				}
				tq->CLEAR();
				delete tq;
				tq = NULL;
			}
			else
			{
				plan->vClass = ((GrapaRuleQueue*)plan->vQueue)->FROM(mScriptExec.vScriptState, &mRuleVariables, expanded);
				plan->mValue.mToken = expanded.mToken;
			}

			plan->mValue.SetLength(0);
			plan->mValue.SetSize(0);
		}
		else if (setValue.mLength)
		{
			plan = mScriptExec.Plan(&mRuleVariables, setValue, NULL, 0, GrapaCHAR());
		}
		if (plan)
		{
			GrapaRuleEvent* r = mScriptExec.ProcessPlan(&mRuleVariables, plan);
			if (r)
			{
				r->CLEAR();
				delete r;
			}
			plan->CLEAR();
			delete plan;
			plan = NULL;
		}
		*/
}

void My_Console::Stop()
{
	mConsoleSend.mScriptState.mSync = false;
	mConsoleSend.Stop();
}

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <vector>
#include <iostream>


// Helper: Remove the last full UTF-8 character from a string
void remove_last_utf8_char(std::string& str) {
	if (str.empty()) return;
	size_t i = str.size() - 1;
	// Move back to the start of the last UTF-8 character
	while (i > 0 && (static_cast<unsigned char>(str[i]) & 0xC0) == 0x80) {
		--i;
	}
	str.erase(i);
}



void My_Console::RunOld(GrapaCB cb, void* data)
{
	mConsoleSend.mScriptState.EnablePrompt(&mRuleVariables);
	while (!gSystem->mStop)
	{
		GrapaCHAR sendBuffer;
		int ch = 0;
		do {
			while (((ch = gSystem->GetCharOLD()) != EOF) && (ch != '\n') && (ch != '\r'))
				sendBuffer.Append((char)ch);
			if (sendBuffer.mLength == 1 && sendBuffer.StrNCmp(".") == 0)
			{
				return;
			}
			if (sendBuffer.mLength == 1 && sendBuffer.StrNCmp(">") == 0)
			{
				Fl::lock();
				sendBuffer.FROM("$editor();");
				mConsoleSend.Send(mConsoleSend.mScriptState.vScriptExec, &mRuleVariables, (char*)sendBuffer.mBytes, sendBuffer.mLength);
				mConsoleSend.Send(mConsoleSend.mScriptState.vScriptExec, &mRuleVariables, (u8*)"$\n", 2);
				sendBuffer.SetLength(0);
				while (!gSystem->mStop)
					Fl::wait(1);
				Fl::unlock();
			}
			if (ch == EOF || ch == '\n')
			{
				mConsoleSend.Send(mConsoleSend.mScriptState.vScriptExec,&mRuleVariables,(char*)sendBuffer.mBytes, sendBuffer.mLength);
				mConsoleSend.Send(mConsoleSend.mScriptState.vScriptExec, &mRuleVariables, (u8*)"$\n", 2);
				sendBuffer.SetLength(0);
			}
		} while (!gSystem->mStop && (sendBuffer.mLength || ch == '\n' || ch == '\r') && ch != EOF);
	}
}

void My_Console::Run(GrapaCB cb, void* data)
{
	mConsoleSend.mScriptState.EnablePrompt(&mRuleVariables);
	while (!gSystem->mStop)
	{
		GrapaCHAR sendBuffer;
		std::string input_line;
		std::string ch;
		do {
			while (!((ch = gSystem->GetUtf8Char()).empty()) && (ch != "\n") && (ch != "\r") && !gSystem->mStop)
				sendBuffer.Append(ch.c_str(), ch.length());
			if (sendBuffer.mLength == 1 && sendBuffer.StrNCmp(".") == 0)
			{
				return;
			}
			if (sendBuffer.mLength == 1 && sendBuffer.StrNCmp(">") == 0)
			{
				Fl::lock();
				sendBuffer.FROM("$editor();");
				sendBuffer.Append("$\n");
				mConsoleSend.Send(mConsoleSend.mScriptState.vScriptExec, &mRuleVariables, (char*)sendBuffer.mBytes, sendBuffer.mLength);
				sendBuffer.SetLength(0);
				while (!gSystem->mStop)
					Fl::wait(1);
				Fl::unlock();
			}
			if ((ch.empty() || ch == "\n") && !gSystem->mStop)
			{
				sendBuffer.Append("$\n");
				mConsoleSend.Send(mConsoleSend.mScriptState.vScriptExec, &mRuleVariables, (char*)sendBuffer.mBytes, sendBuffer.mLength);
				sendBuffer.SetLength(0);
			}
		} while (!gSystem->mStop && (sendBuffer.mLength || ch == "\n" || ch == "\r") && !ch.empty());
	}
}

My_Pack::My_Pack(int x, int y, int w, int h, const char* l)
	: Fl_Pack(x, y, w, h, l)
{
};

My_Text_Editor::My_Text_Editor(int X, int Y, int W, int H, const char* l)
	: Fl_Text_Editor(X, Y, W, H, l) {
	vConsoleSend = NULL;
	vBuff = new Fl_Text_Buffer();
	buffer(vBuff);
	wrap_mode(WRAP_AT_BOUNDS, 0);
	show_cursor();
	//vBuff->add_modify_callback(My_Text_Editor::buffer_modified_cb2, this);
	resize(x(), y(), w(), h());
}

My_Text_Editor::~My_Text_Editor()
{
}

int My_Text_Editor::handle(int event) {
	int handled = 0;
	switch (event) {
	case FL_KEYDOWN:
		if ((Fl::event_key() & 0x7f) == 0x0d && Fl::event_key(0xffe1)) {
			sendtext();
			return 1;
		}
		handled = Fl_Text_Editor::handle(event);
		break;
	case FL_MOUSEWHEEL:
		return 0;
	default:
		handled = Fl_Text_Editor::handle(event);
		break;
	}
	return(handled);
}

void My_Text_Editor::cleartext()
{
	Fl_Text_Display* t = ((My_Group*)parent()->parent())->disp;
	t->buffer()->remove(0, t->buffer()->length());
}

void My_Text_Editor::sendtext()
{
	char* s = buffer()->selected() ? buffer()->selection_text() : buffer()->text();
	GrapaCHAR sendBuffer(s);
	free(s);
	if (sendBuffer.mLength)
	{
		cleartext();
		vConsoleSend->mScriptState.ClearQueue();
		vConsoleSend->SetActive(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, false);
		vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, sendBuffer.mBytes, sendBuffer.mLength);
		vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, (u8*)"$\n", 2);
	}
}

void My_Text_Editor::sendtext(GrapaCHAR sendBuffer)
{
	vConsoleSend->mScriptState.ClearQueue();
	vConsoleSend->SetActive(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, false);
	vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, sendBuffer.mBytes, sendBuffer.mLength);
	vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, (u8*)"$\n", 2);
}

//void My_Text_Editor::buffer_modified_cb2(int pos, int nInserted, int nDeleted, int nRestyled, const char* deletedText, void* cbArg)
//{
//	buffer_modified_cb(pos, nInserted, nDeleted, nRestyled, deletedText, cbArg);
//	My_Text_Editor*v=(My_Text_Editor*)cbArg;
//	if (v->oldB != v->mNBufferLines)
//	{
//		v->parent()->parent()->parent()->parent()->redraw();
//		v->oldB = v->mNBufferLines;
//	}
//}

void My_Text_Editor::resize(int X, int Y, int W, int H)
{
	Fl_Text_Editor::resize(X, Y, W, H);
	if (H != ((mNBufferLines ? mNBufferLines : 1) + 2) * mMaxsize)
	{
		Fl_Text_Editor::resize(X, Y, W, ((mNBufferLines ? mNBufferLines : 1) + 2) * mMaxsize);
		if (parent())
			parent()->parent()->parent()->parent()->redraw();
	}
}

My_Text_Display::My_Text_Display(int X, int Y, int W, int H, const char* l)
	: Fl_Text_Display(X, Y, W, H, l) {
	vBuff = new Fl_Text_Buffer();
	buffer(vBuff);
	wrap_mode(WRAP_AT_BOUNDS, 0);
	hide_cursor();
	resize(x(), y(), w(), h());
}

My_Text_Display::~My_Text_Display()
{
}

int My_Text_Display::handle(int event) {
	int handled = 0;
	switch (event) {
	case FL_MOUSEWHEEL:
		break;
	default:
		handled = Fl_Text_Display::handle(event);
		break;
	}
	return(handled);
}

void My_Text_Display::resize(int X, int Y, int W, int H)
{
	Fl::lock();      // acquire the lock
	Fl_Text_Display::resize(X, Y, W, ((mNBufferLines ? mNBufferLines : 1) + 2) * mMaxsize);
	Fl::unlock();    // release the lock; allow other threads to access FLTK again
	Fl::awake();     // use Fl::awake() to signal main thread to refresh the GUI
}

My_Button::My_Button(int X, int Y, int W, int H, const char* l)
	: Fl_Button(X, Y, W, H, l) {
	// do initialization stuff...
}

void My_Button::add(Fl_Widget* item, void* data)
{
	My_Button* a = (My_Button*)item;
	My_Group* b = (My_Group*)data;
	My_Group* grp2 = new My_Group(10, 30, 640 - 20, 180 - 75, b->mConsoleResponse.mRuleVariables.GetParrent());
	b->parent()->insert(*grp2, b->parent()->find(b) + 1);
	b->parent()->parent()->redraw();
}

void My_Button::del(Fl_Widget* item, void* data)
{
	My_Button* a = (My_Button*)item;
	My_Group* b = (My_Group*)data;

	int n = b->parent()->children();
	int p = b->parent()->find(b)+1;
	if (n > p)
	{
		My_Group* c = (My_Group*)b->parent()->child(p);
		c->mConsoleSend.Stop();
		b->parent()->remove(*c);
		delete c;
		b->parent()->parent()->redraw();
	}
}

My_Prompt_Editor::My_Prompt_Editor(int X, int Y, int W, int H, GrapaNames* pNameSpace, My_Text_Console* pWin)
	: Fl_Text_Editor(X, Y, W, H, 0) {
	vWin = pWin;
	vBuff = new Fl_Text_Buffer();
	buffer(vBuff);
	wrap_mode(WRAP_AT_BOUNDS,0);
	show_cursor();
	vConsoleSend = &mPromptSend;
	mPromptResponse.disp = this;
	mPromptResponse.mRuleVariables.mSkip = true;
	mPromptSend.mScriptState.SetNameSpace(&mPromptResponse.mRuleVariables);
	GrapaNames* vNames = pNameSpace;
	while (vNames->GetParrent()) vNames = vNames->GetParrent();
	mPromptResponse.mRuleVariables.SetParrent(vNames);
	mPromptResponse.mRuleVariables.SetResponse(&mPromptResponse);
	mPromptSend.mScriptState.EnablePrompt(&mPromptResponse.mRuleVariables);
	mPromptSend.Start();
	//vBuff->add_modify_callback(My_Prompt_Editor::buffer_modified_cb2, this);
}

My_Prompt_Editor::~My_Prompt_Editor()
{
	mPromptSend.mScriptState.mSync = false;
	mPromptSend.Stop();
}

int My_Prompt_Editor::handle(int event) {
	int handled = 0;
	switch (event) {
	case FL_KEYDOWN:
		if ((Fl::event_key() & 0x7f) == 0x0d) {
			sendtext();
			return 1;
		}
		handled = Fl_Text_Editor::handle(event);
		break;
	default:
		handled = Fl_Text_Editor::handle(event);
		break;
	}
	return(handled);
}

void My_Prompt_Editor::sendtext()
{
	bool isSelected = buffer()->selected();
	char* s = NULL;
	if (isSelected)
	{
		s = buffer()->selection_text();
	}
	else
	{
		s = buffer()->line_text(buffer()->length());
	}
	GrapaCHAR sendBuffer;
	if (s)
	{
		sendBuffer.FROM(s);
		free(s);
		s = NULL;
	}
	if (!isSelected && sendBuffer.StrNCmp((char*)mLastPrompt.mBytes, mLastPrompt.mLength) == 0)
	{
		sendBuffer.mLength = GrapaMem::MoveLeft(sendBuffer.mBytes, mLastPrompt.mLength, sendBuffer.mLength);
		//memcpy(sendBuffer.mBytes, &sendBuffer.mBytes[mLastPrompt.mLength], sendBuffer.mLength - mLastPrompt.mLength);
		//sendBuffer.SetLength(sendBuffer.mLength - mLastPrompt.mLength);
	}
	if (sendBuffer.mLength == 0 && !isSelected)
	{
		buffer()->append((char*)"\n");
		vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, (u8*)"$\n", 2);
	}
	else if (sendBuffer.mLength)
	{
		vConsoleSend->mScriptState.ClearQueue();
		vConsoleSend->SetActive(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, false);
		if (isSelected) buffer()->append((char*)sendBuffer.mBytes);
		buffer()->append((char*)"\n");
		vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, sendBuffer.mBytes, sendBuffer.mLength);
		vConsoleSend->Send(vConsoleSend->mScriptState.vScriptExec, vConsoleSend->mScriptState.vRuleVariables, (u8*)"$\n", 2);
	}
}

#include <FL/Fl_Menu_Bar.H>
#include <FL/filename.H>
#include <FL/Fl_Button.H>

static void MyMenuCallback(Fl_Widget* w, void* d) {
	Fl_Menu_Bar* bar = (Fl_Menu_Bar*)w;
	My_Text_Console* con = (My_Text_Console*)d;
	const Fl_Menu_Item* item = bar->mvalue();
	char ipath[256]; bar->item_pathname(ipath, sizeof(ipath));
	if (strcmp(item->label(), "&New") == 0)
	{
		My_Text_Console *t = new My_Text_Console();
	}
	else if (strcmp(item->label(), "&Quit") == 0)
	{
		for (My_Text_Console* gfx : gSystem->mConsoleList)
			gfx->hide();
	}
	else if (strcmp(item->label(), "Version") == 0)
	{
		con->ExecInFocus("$sys().getenv('VERSION');");
	}
	else if (strcmp(item->label(), "License") == 0)
	{
		con->ExecInFocus("$sys().getenv('LICENCE');");
	}
	else if (strcmp(item->label(), "Dir") == 0)
	{
		con->ExecInFocus("$file().ls().vector().str(['\\n','\\t']);");
	}
	else if (strcmp(item->label(), "Console") == 0)
	{
		if (con->prompt->visible())
		{
			con->prompt->hide();
			con->remove(con->prompt);
			int H = con->prompt->h();
			con->scr->resize(con->scr->x(), con->scr->y(), con->scr->w(), con->scr->h() + con->prompt->h());
		}
		else
		{
			if (con->h() < 180 + 140)
			{
				con->resize(con->x(), con->y(), con->w(), 180 + 140);
			}
			int H = con->prompt->h();
			con->scr->resize(con->scr->x(), con->scr->y(), con->scr->w(), con->scr->h() - con->prompt->h());
			con->prompt->resize(con->scr->x(), con->scr->y() + con->scr->h(), con->scr->w() + 2, con->prompt->h() + 2);
			con->add(con->prompt);
			con->prompt->show();
			con->scr->parent()->redraw();
		}
	}
}

void GrapaEditorResponse::SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
{
	//Fl::lock();      // acquire the lock
	disp->buffer()->append((char*)sendbuf);
	//disp->parent()->parent()->parent()->parent()->redraw();
	//Fl::unlock();    // release the lock; allow other threads to access FLTK again
}

void GrapaEditorResponse::SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
{

}

void GrapaEditorResponse::SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
	if (pValue)
	{
		if (pNameSpace->vLast->vQueue) vScriptExec->EchoList(this, pValue, false, false, false);
		else vScriptExec->EchoValue(this, pValue, false, false, false);
	}
	//Fl::lock();      // acquire the lock
	disp->parent()->parent()->parent()->parent()->redraw();
	//Fl::unlock();    // release the lock; allow other threads to access FLTK again
}

void GrapaEditorResponse::SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen)
{
	// Output debug/error messages to stderr for GUI editor
	if (sendbuflen) {
		fprintf(stderr, "%.*s", (int)sendbuflen, (char*)sendbuf);
	}
}

void GrapaEditorResponse::SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
{
	// Output debug/error messages to stderr for GUI editor
	if (sendbuf.mLength) {
		fprintf(stderr, "%.*s", (int)sendbuf.mLength, (char*)sendbuf.mBytes);
	}
}

void GrapaEditorResponse::SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
	if (pValue)
	{
		if (pNameSpace->vLast->vQueue) vScriptExec->EchoList(this, pValue, false, false, false);
		else vScriptExec->EchoValue(this, pValue, false, false, false);
	}
	// Output debug/error messages to stderr for GUI editor
	fprintf(stderr, "\n");
}

void GrapaPromptResponse::SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
{
	disp->buffer()->append((char*)sendbuf);
	Fl_Text_Editor::kf_ctrl_move(FL_End, disp);
	//if (disp->vWin) disp->vWin->redraw();
}

void GrapaPromptResponse::SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
{
	disp->buffer()->append((char*)sendbuf.mBytes);
	disp->mLastPrompt.FROM(sendbuf);
	Fl_Text_Editor::kf_ctrl_move(FL_End, disp);
	//if (disp->vWin) disp->vWin->redraw();
}

void GrapaPromptResponse::SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
	if (pValue)
	{
		if (pNameSpace->vLast->vQueue) vScriptExec->EchoList(this, pValue, false, false, false);
		else vScriptExec->EchoValue(this, pValue, false, false, false);
	}
	disp->buffer()->append((char*)"\n");
	Fl_Text_Editor::kf_ctrl_move(FL_End, disp);
	//if (disp->vWin) disp->vWin->redraw();
}

void GrapaPromptResponse::SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen)
{
	// Output debug/error messages to stderr for GUI prompt
	if (sendbuflen) {
		fprintf(stderr, "%.*s", (int)sendbuflen, (char*)sendbuf);
	}
}

void GrapaPromptResponse::SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
{
	// Output debug/error messages to stderr for GUI prompt
	if (sendbuf.mLength) {
		fprintf(stderr, "%.*s", (int)sendbuf.mLength, (char*)sendbuf.mBytes);
	}
}

void GrapaPromptResponse::SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
{
	if (pValue)
	{
		if (pNameSpace->vLast->vQueue) vScriptExec->EchoList(this, pValue, false, false, false);
		else vScriptExec->EchoValue(this, pValue, false, false, false);
	}
	// Output debug/error messages to stderr for GUI prompt
	fprintf(stderr, "\n");
}

My_Group::My_Group(int X, int Y, int W, int H, GrapaNames* pNameSpace)
	: My_Pack(X, Y, W, H, 0) {
	edit = NULL;
	disp = NULL;
	end();
	spacing(4);
	My_Pack* top = new My_Pack(X, Y, W - 15, H-30);
	top->end();
	add(top);
	edit = new My_Text_Editor(X, Y, W-15, H/2 -15);
	edit->end();
	top->add(edit);
	disp = new My_Text_Display(X, Y + H / 2 -15, W-15, H / 2 - 15);
	disp->end();
	top->add(disp);
	edit->vConsoleSend = &mConsoleSend;
	mConsoleResponse.disp = disp;
	mConsoleResponse.mRuleVariables.mSkip = true;
	mConsoleSend.mScriptState.SetNameSpace(&mConsoleResponse.mRuleVariables);
	GrapaNames* vNames = pNameSpace;
	while (vNames->GetParrent()) vNames = vNames->GetParrent();
	mConsoleResponse.mRuleVariables.SetParrent(vNames);
	mConsoleResponse.mRuleVariables.SetResponse(&mConsoleResponse);
	disp->color(FL_LIGHT1);
	My_Pack* enc = new My_Pack(X, Y+H-30, W - 15, 30);
	enc->spacing(4);
	enc->type(HORIZONTAL);
	enc->end();
	add(enc);
	Fl_Widget* blank = new Fl_Group(X, Y, 2, 30);
	enc->add(blank);
	My_Button* butAdd = new My_Button(X, Y, 30, 30, (char*)"+");
	enc->add(butAdd);
	butAdd->callback(My_Button::add, this);
	butAdd->when(FL_WHEN_RELEASE);
	My_Button* butDel = new My_Button(X, Y, 30, 30, (char*)"-");
	enc->add(butDel);
	butDel->callback(My_Button::del, this);
	butDel->when(FL_WHEN_RELEASE);
	resizable(top);
	mConsoleSend.Start();
}

My_Group::~My_Group()
{
	mConsoleSend.mScriptState.mSync = false;
	mConsoleSend.Stop();
	delete edit;
	delete disp;
	edit = NULL;
	disp = NULL;
}

My_ResizableScroll::My_ResizableScroll(int X, int Y, int W, int H)
	: Fl_Scroll(X, Y, W, H, 0) {
}
void My_ResizableScroll::resize(int X, int Y, int W, int H)
{
	Fl_Group::resize(X, Y, W, H);
}

My_Text_Console::My_Text_Console()
	: Fl_Double_Window(640, 180, "Grapa Editor")
{
	end();

	scr = NULL;
	prompt = NULL;

	Fl_Menu_Bar* menu = new Fl_Menu_Bar(0, 0, 640, 30);		// Create menubar, items..
	menu->add("&File/&New", "^n", MyMenuCallback, this);
	menu->add("&File/&Open", "^o", MyMenuCallback, this);
	menu->add("&File/&Save", "^s", MyMenuCallback, this, FL_MENU_DIVIDER);
	menu->add("&File/&Quit", "^q", MyMenuCallback, this);
	menu->add("&Edit/&Copy", "^c", MyMenuCallback, this);
	menu->add("&Edit/&Paste", "^v", MyMenuCallback, this, FL_MENU_DIVIDER);
	menu->add("Script/Console", "", MyMenuCallback, this, FL_MENU_DIVIDER);
	menu->add("Script/Version", "", MyMenuCallback, this);
	menu->add("Script/License", "", MyMenuCallback, this);
	menu->add("Script/Dir", "", MyMenuCallback, this);
	add(menu);

	scr = new My_ResizableScroll(0, 30, 640, 180 - 30);
	scr->end();
	scr->type(Fl_Scroll::VERTICAL_ALWAYS);
	add(scr);

	pk = new My_Pack(0, 30, 640 - 18, 180 - 30);
	pk->end();
	pk->spacing(4);
	scr->add(pk);

	grp = new My_Group(0, 30, 640 - 18, 60, &mRuleVariables);
	pk->add(grp);

	prompt = new My_Prompt_Editor(0, 180, 640, 140, &mRuleVariables, this);
	prompt->hide();

	scr->resizable(*pk);
	resizable(*scr);

	gSystem->mConsoleList.push_back(this);

	Fl::visual(FL_DOUBLE | FL_INDEX);
	show();

	GrapaCHAR grresult;
	if (gSystem->mGrammar.mLength) grresult = grp->mConsoleSend.SendSync(gSystem->mGrammar,NULL,0,GrapaCHAR());
	if (grresult.mLength)
	{
		grp->disp->buffer()->append((char*)grresult.mBytes);
	}
}

My_Text_Console::My_Text_Console(GrapaCHAR& in, GrapaCHAR& out, GrapaCHAR& run)
	: My_Text_Console()
{
	Fl::lock();
	GrapaCHAR grresult;
	if (run.mLength) grresult = grp->mConsoleSend.SendSync(run,NULL,0,GrapaCHAR());
	if (grresult.mLength)
	{
		grp->disp->buffer()->append((char*)grresult.mBytes);
	}
}

My_Text_Console::~My_Text_Console()
{
	gSystem->mConsoleList.remove(this);
	scr = NULL;
	prompt = NULL;
}

void My_Text_Console::hide()
{
	delete this;
}

void My_Text_Console::Run(GrapaCB cb, void* data)
{
	if (cb)
	{
		while (visible())
		{
			Fl::wait(1);
			cb(data);
		}
	}
	else
	{
		Fl::run();
	}
}

void My_Text_Console::ExecInFocus(const char* in)
{
	Fl_Widget* v = (Fl_Widget*)Fl::focus();
	if (v)
	{
		if (v == (Fl_Widget*)prompt)
		{
			My_Prompt_Editor* t = (My_Prompt_Editor*)v;
			t->buffer()->append(in);
			t->sendtext();
		}
		else
		{
			My_Text_Editor* t = ((My_Group*)v->parent()->parent())->edit;
			t->buffer()->remove(0, t->buffer()->length());
			t->buffer()->append(in);
			t->sendtext();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
