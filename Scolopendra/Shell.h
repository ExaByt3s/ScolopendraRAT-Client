#include "Includes.h"

#ifndef SHELL_H

class shell {
public:
	void Show(std::string uParam) {
		ShellExecuteA(NULL, "open", "cmd.exe", std::string("/k " + uParam).c_str(), NULL, SW_SHOW);
	}

	void Hide(std::string uParam) {
		ShellExecuteA(NULL, "open", "cmd.exe", std::string("/c " + uParam).c_str(), NULL, SW_HIDE);
	}
};

shell Shell;
#endif