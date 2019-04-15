#include <Windows.h>
#include <WinInet.h>
#include <Urlmon.h>

#include <thread>

#include <tlhelp32.h>
#include <shellapi.h>
#include <intrin.h>
#include <filesystem>
#include <stdlib.h>
#include <fstream>

#include <versionhelpers.h>
#include <comdef.h>
#include <Wbemidl.h>

#include <iostream>
#include <tchar.h>
#include <string>

#define _WIN32_DCOM

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "wininet")
#pragma comment(lib, "urlmon.lib")