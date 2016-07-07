#include "stdafx.h"
#include "BaseFuntion.h"
#include <lm.h>  
#pragma comment (lib,"Netapi32.lib")
#pragma comment(lib, "Version.lib")


int Md5_File(char *file_md5, char *path, const int md5_len)
{
	FILE *fp = fopen(path, "rb");
	MD5_CTX mdContext;
	u_char digest[17];
	int bytes;
	unsigned char data[1024];

	int i;

	if (fp == NULL) {
		return NULL;
	}

	MD5Init(&mdContext);
	while ((bytes = fread(data, 1, 1024, fp)) != 0)
	{
		MD5Update(&mdContext, data, bytes);
	}
	MD5Final(digest, &mdContext);

	if (file_md5 == NULL)
	{
		return NULL;
	}
	memset(file_md5, 0, (md5_len + 1));

	if (md5_len == 16)
	{
		for (i = 4; i<12; i++)
		{
			sprintf(&file_md5[(i - 4) * 2], "%02x", digest[i]);
		}
	}
	else if (md5_len == 32)
	{
		for (i = 0; i<16; i++)
		{
			sprintf(&file_md5[i * 2], "%02x", digest[i]);
		}
	}
	else
	{
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	return 1;
}

int CompareVersion(char *localver, char *remotever) {
	char LocalVer[20] = {}, RomoteVer[20] = {}, TempVar[20] = {}; int a = 0;
	for (int b = 1; b <= 2; b++)
	{
		if (b == 1)strncpy(TempVar, localver, strlen(localver));
		if (b == 2)strncpy(TempVar, remotever, strlen(remotever));

		for (int i = 0; TempVar[i] != '\0'; i++)
		{
			if (TempVar[i] == '.') {
				a = i;
				for (; TempVar[i] != '\0'; i++) {
					TempVar[i] = TempVar[i + 1];
				}
				i = a;
			}
		}
		if (b == 1)strncpy(LocalVer, TempVar, strlen(localver));
		if (b == 2)strncpy(RomoteVer, TempVar, strlen(remotever));
	}
	if (atoll(LocalVer) < atoll(RomoteVer))
		return 1;
	else if (atoll(LocalVer) > atoll(RomoteVer))
		return -1;
	return 0;
}

CStringA GetFileVersion(LPCTSTR lpszFilePath)
{
	CStringA szFilePath(lpszFilePath);
	CStringA szResult(_T("0.0.0.0"));

	if (szFilePath.GetLength() > 0 && PathFileExistsA(szFilePath))
	{
		VS_FIXEDFILEINFO *pVerInfo = NULL;
		DWORD dwTemp, dwSize;
		BYTE *pData = NULL;
		UINT uLen;

		dwSize = GetFileVersionInfoSize(lpszFilePath, &dwTemp);
		if (dwSize == 0)
		{
			return szResult;
		}

		pData = new BYTE[dwSize + 1];
		if (pData == NULL)
		{
			return szResult;
		}

		if (!GetFileVersionInfo(lpszFilePath, 0, dwSize, pData))
		{
			delete[] pData;
			return szResult;
		}

		if (!VerQueryValue(pData, TEXT("\\"), (void **)&pVerInfo, &uLen))
		{
			delete[] pData;
			return szResult;
		}

		DWORD verMS = pVerInfo->dwFileVersionMS;
		DWORD verLS = pVerInfo->dwFileVersionLS;
		DWORD major = HIWORD(verMS);
		DWORD minor = LOWORD(verMS);
		DWORD build = HIWORD(verLS);
		DWORD revision = LOWORD(verLS);
		delete[] pData;

		szResult.Format("%d.%d.%d.%d", major, minor, build, revision);
	}

	return szResult;
}
BOOL IsWow64()
{
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
			GetModuleHandle(L"kernel32"), "IsWow64Process");
	BOOL bIsWow64 = FALSE;
	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			// handle error
		}
	}
	return bIsWow64;
}