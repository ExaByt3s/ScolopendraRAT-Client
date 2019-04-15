#include "Includes.h"

#ifndef AUXILIARY_H
#define AUXILIARY_H

//-<Default>-//
bool FileExist(std::string Path) {
	if (GetFileAttributesA(Path.c_str()) == 0xFFFFFFFF)
		return false;
	else
		return true;
}

std::string GetCurrentPathC() {
	char* szPath = (char*)malloc(sizeof(char) * MAX_PATH);
	DWORD dwPath = sizeof(char) * MAX_PATH;
	GetModuleFileName(NULL, szPath, dwPath);
	return std::string(szPath);
}

std::string GetTempPathC() {
	char Path[MAX_PATH];
	GetTempPath(MAX_PATH, Path);
	return std::string(Path);
}

//-<Regedit>-//
bool CreateKey(HKEY hKey, std::string Path) {
	HKEY Buffer;
	if (RegCreateKey(hKey, Path.c_str(), &Buffer) == ERROR_SUCCESS) {
		RegCloseKey(Buffer);
		return true;
	}
	return false;
}

bool RegWriteKey(HKEY hKey, std::string Path, std::string Key, std::string szData) {
	bool Result = false;
	if (RegOpenKey(hKey, Path.c_str(), &hKey) == ERROR_SUCCESS) {
		if (RegSetValueEx(hKey, Key.c_str(), 0, REG_SZ, (BYTE*)szData.c_str(), szData.length()) == ERROR_SUCCESS)
			Result = true;

		RegCloseKey(hKey);
	}
	return Result;
}

std::string RegReadKey(HKEY hKey, std::string Path, std::string Key) {
	char szData[MAX_PATH];
	DWORD dwBufLen = MAX_PATH;
	if (RegGetValue(hKey, Path.c_str(), Key.c_str(), RRF_RT_REG_SZ, NULL, (BYTE*)szData, &dwBufLen) == ERROR_SUCCESS) 
		return std::string(szData);
	return "";
}

bool RegExistsKey(HKEY hKey, std::string Path, std::string Key) {
	if (RegGetValue(hKey, Path.c_str(), Key.c_str(), RRF_RT_REG_SZ, NULL, NULL, NULL) == ERROR_SUCCESS)
		return true;

	return false;
}

std::string WMIRead(std::string Object) {
	std::string Result;
	HRESULT hres;
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		return "";
	}

	hres = CoInitializeSecurity(
		NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE,
		NULL
	);

	if (FAILED(hres)) {
		CoUninitialize();
		return "";
	}

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		CoUninitialize();
		return "";
	}


	IWbemServices *pSvc = NULL;

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc
	);

	if (FAILED(hres)) {
		pLoc->Release();
		CoUninitialize();
		return "";
	}

	hres = CoSetProxyBlanket(
		pSvc,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE
	);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return "";
	}

	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t(std::string(std::string("SELECT * FROM ") + Object).c_str()),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;
		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		std::wstring s(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
		Result = std::string(s.begin(), s.end());
		VariantClear(&vtProp);
		pclsObj->Release();
	}

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();
	return Result;
}
#endif