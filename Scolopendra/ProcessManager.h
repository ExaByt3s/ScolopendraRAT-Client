#include "Includes.h"

#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

class processManager {
public:
	bool Kill(std::string Process) {
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, FindProcessId(Process));
		return TerminateProcess(hProcess, 1);
	}

	bool New(std::string Path) {
		STARTUPINFO startinfo = { 0 };
		PROCESS_INFORMATION processInfo = { 0 };
		if (CreateProcess(Path.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startinfo, &processInfo)) 
			return true;
		else
			return false;
	}

private:
	int FindProcessId(std::string processName) {
		PROCESSENTRY32 processInfo;
		processInfo.dwSize = sizeof(processInfo);

		HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (processesSnapshot == INVALID_HANDLE_VALUE)
			return 0;

		Process32First(processesSnapshot, &processInfo);
		if (!processName.compare(processInfo.szExeFile)) {
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}

		while (Process32Next(processesSnapshot, &processInfo)) {
			if (!processName.compare(processInfo.szExeFile)) {
				CloseHandle(processesSnapshot);
				return processInfo.th32ProcessID;
			}
		}

		CloseHandle(processesSnapshot);
		return 0;
	}
};

processManager ProcessManager;

#endif