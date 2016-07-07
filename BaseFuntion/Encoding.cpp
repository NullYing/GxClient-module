// BaseFuntion.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "BaseFuntion.h"

CStringA GBToUTF8(const char* str)
{
	CStringA result;
	WCHAR *strSrc;
	TCHAR *szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new TCHAR[i + 1];
	int j = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, (LPSTR)(LPCTSTR)szRes, i, NULL, NULL);

	result = (char*)szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}
BASEFUNTION_API int UTF82GBK(char *szUtf8, char *szGbk, int Len)
{
	int n = MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, NULL, 0);
	WCHAR * wszGBK = (WCHAR *)malloc(sizeof(WCHAR) * n);
	memset(wszGBK, 0, sizeof(WCHAR) * n);
	MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, wszGBK, n);

	n = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	if (n > Len)
	{
		free(wszGBK);
		return -1;
	}

	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGbk, n, NULL, NULL);

	free(wszGBK);
	wszGBK = NULL;

	return 0;
}
inline BYTE toHex(const BYTE &x)
{
	return x > 9 ? x + 55 : x + 48;
}
BASEFUNTION_API CStringA URLEncode(CStringA sIn)
{
	CStringA sOut;
	const int nLen = sIn.GetLength() + 1;
	register LPBYTE pOutTmp = NULL;
	LPBYTE pOutBuf = NULL;
	register LPBYTE pInTmp = NULL;
	LPBYTE pInBuf = (LPBYTE)sIn.GetBuffer(nLen);
	BYTE b = 0;

	//alloc out buffer
	pOutBuf = (LPBYTE)sOut.GetBuffer(nLen * 3 - 2);//new BYTE [nLen * 3];

	if (pOutBuf)
	{
		pInTmp = pInBuf;
		pOutTmp = pOutBuf;

		// do encoding
		while (*pInTmp)
		{
			if (isalnum(*pInTmp))
				*pOutTmp++ = *pInTmp;
			else
				if (isspace(*pInTmp))
					*pOutTmp++ = '+';
				else
				{
					*pOutTmp++ = '%';
					*pOutTmp++ = toHex(*pInTmp >> 4);
					*pOutTmp++ = toHex(*pInTmp % 16);
				}
			pInTmp++;
		}
		*pOutTmp = '\0';
		//sOut=pOutBuf;
		//delete [] pOutBuf;
		sOut.ReleaseBuffer();
	}
	sIn.ReleaseBuffer();
	return sOut;
}

CStringA Utf8ToStringT(LPSTR str)
{
	_ASSERT(str);
	USES_CONVERSION;
	WCHAR *buf;
	int length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	buf = new WCHAR[length + 1];
	ZeroMemory(buf, (length + 1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, buf, length);

	return (CStringA(W2T(buf)));
}
#define IsHexNum(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
BASEFUNTION_API CStringA UrlDecode(LPCTSTR url)
{
	_ASSERT(url);
	USES_CONVERSION;
	LPSTR _url = T2A(const_cast<LPTSTR>(url));
	int i = 0;
	int length = (int)strlen(_url);
	CHAR *buf = new CHAR[length];
	ZeroMemory(buf, length);
	LPSTR p = buf;
	char tmp[4];
	while (i < length)
	{
		if (i <= length - 3 && _url[i] == '%' && IsHexNum(_url[i + 1]) && IsHexNum(_url[i + 2]))
		{
			memset(tmp, 0, sizeof(tmp));
			memcpy(tmp, _url + i + 1, 2);
			sscanf(tmp, "%x", p++);
			i += 3;
		}
		else
		{
			*(p++) = _url[i++];
		}
	}
	return Utf8ToStringT(buf);
}
BASEFUNTION_API CString ANSIToUnicode(const char* str)
{
	int len = 0;
	len = strlen(str);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP,
		0,
		str,
		-1,
		NULL,
		0);
	wchar_t * pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP,
		0,
		str,
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	CString rt;
	rt = (wchar_t*)pUnicode;
	delete pUnicode;
	return rt;
}
BASEFUNTION_API CStringA UnicodeToANSI(const LPCWCH str)
{
	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_ACP,
		0,
		str,
		-1,
		NULL,
		0,
		NULL,
		NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
	::WideCharToMultiByte(CP_ACP,
		0,
		str,
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL);
	CStringA strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}
int GbkToUtf8(char *szGbk, char *szUtf8, int Len)
{
	WCHAR *strSrc;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_ACP, 0, szGbk, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_ACP, 0, szGbk, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
	if (i > Len)
	{
		free(strSrc);
		return 0;
	}
	int j = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szUtf8, i, NULL, NULL);
	delete[]strSrc;
	return 1;
}