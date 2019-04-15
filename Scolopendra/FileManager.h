#include "Includes.h"

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

class fileManager {
public:
	std::string List(std::string Dir) {
		std::string Result;

		WIN32_FIND_DATA FindFileData;
		HANDLE hf;
		hf = FindFirstFile(Dir.c_str(), &FindFileData);
		if (hf != INVALID_HANDLE_VALUE) {
			do {
				Result += std::string(FindFileData.cFileName) + ", ";
			} while (FindNextFile(hf, &FindFileData) != 0);
			FindClose(hf);
		}

		Result = Result.substr(0, Result.length() - 2) + ".";
		return Result;
	}

	void Open(std::string Path) {
		ShellExecuteA(NULL, "open", Path.c_str(), NULL, NULL, SW_SHOW);
	}
};

fileManager FileManager;

#endif