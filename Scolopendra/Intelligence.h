#include "Includes.h"

#ifndef INTELLIGENCE_H

class intelligence {
private:
	SYSTEM_INFO st_SysInfo;
	MEMORYSTATUS st_MemoryInfo;

	struct client {
		std::string OperatingSystem,
			Processor,
			VideoController,
			Architecture,
			Digit,
			RAM,
			UserHWID,
			UserName,
			MachineName;
		bool Access;
	};

public:
	client Client{
		GetOperatingSystemC(),
		GetCPUNameC(),
		GetGPUNameC(),
		GetArchitectureC(),
		GetSysDigit(),
		GetMemoryCountC(),
		GetUserIdC(),
		GetUserNameC(),
		GetMachineNameC(),
		GetClientAccessC()
	};

	std::string GetUserIdC(void) {
		HW_PROFILE_INFO hwProfileInfo;
		GetCurrentHwProfile(&hwProfileInfo);
		return std::string(hwProfileInfo.szHwProfileGuid);
	}

	std::string GetUserNameC(void) {
		char* Buffer = (char*)malloc(sizeof(char*) * MAX_PATH);
		DWORD szBuffer = sizeof(char*) * MAX_PATH;
		GetUserNameA(Buffer, &szBuffer);
		return std::string(Buffer);
	}

	std::string GetMachineNameC(void) {
		char* Buffer = (char*)malloc(sizeof(char*) * MAX_PATH);
		DWORD szBuffer = sizeof(char*) * MAX_PATH;
		GetComputerNameA(Buffer, &szBuffer);
		return std::string(Buffer);
	}
	
	std::string GetOperatingSystemC(void) {
		if (IsWindows10OrGreater) {
			return "Windows 10";
		} else if(IsWindows8OrGreater) {
			return "Windows 8";
		} else if (IsWindows7OrGreater) {
			return "Windows 7";
		} else if (IsWindowsVistaOrGreater) {
			return "Windows Vista";
		} else if (IsWindowsXPOrGreater) {
			return "Windows XP";
		} else {
			return "Unknown";
		}
	}

	std::string GetMemoryCountC(void) {
		GlobalMemoryStatus(&st_MemoryInfo);
		return std::to_string((int)round(st_MemoryInfo.dwTotalPhys / (1e+9)));
	}

	std::string GetArchitectureC(void) {
		switch (st_SysInfo.wProcessorArchitecture) {
			case PROCESSOR_ARCHITECTURE_INTEL:
				return "Intel";
				break;

			case PROCESSOR_ARCHITECTURE_AMD64:
				return "AMD64";
				break;
	
			case PROCESSOR_ARCHITECTURE_ARM:
				return "ARM32";
				break;

			case PROCESSOR_ARCHITECTURE_ARM64:
				return "ARM64";
				break;

			case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64:
				return "ARM32&Win64";
				break;

			default:
				return "";
				break;
		};
	}

	bool GetClientAccessC(void) {
		BOOL fRet = FALSE;
		HANDLE hToken = NULL;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
			TOKEN_ELEVATION Elevation;
			DWORD cbSize = sizeof(TOKEN_ELEVATION);
			if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
				fRet = Elevation.TokenIsElevated;
			}
		}

		if (hToken) {
			CloseHandle(hToken);
		}
		return fRet;
	}

	std::string GetProcessListA(void) {
		std::string Result;
		PROCESSENTRY32 peProcessEntry;
		TCHAR szBuff[1024];
		DWORD dwTemp;
		HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapshot) {
			return "Permission denied";
		}

		peProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnapshot, &peProcessEntry);
		do {
			Result = Result + std::string(peProcessEntry.szExeFile) + ", ";
		} while (Process32Next(hSnapshot, &peProcessEntry));

		CloseHandle(hSnapshot);
		Result = Result.substr(0, Result.length() - 2) + ".";
		return Result;
	}

	std::string GetCPUNameC(void) {
		return WMIRead("Win32_Processor");
	}

	std::string GetGPUNameC() {
		return WMIRead("Win32_VideoController");
	}

	std::string GetSysDigit(void) {
		bool is_64_bit = true;
		if (FALSE == GetSystemWow64DirectoryW(nullptr, 0u)) {
			auto const last_error{ ::GetLastError() };
			if (ERROR_CALL_NOT_IMPLEMENTED == last_error) {
				is_64_bit = false;
			}
		}

		if (is_64_bit)
			return "x64";
		else
			return "x86";
	}

	
};

intelligence Intelligence;

#endif