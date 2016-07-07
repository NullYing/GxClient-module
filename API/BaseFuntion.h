// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 BASEFUNTION_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// BASEFUNTION_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once
#ifndef    __BASEFUNTION_H__
#define  __BASEFUNTION_H__

#ifdef BASEFUNTION_EXPORTS
#define BASEFUNTION_API  __declspec(dllexport)
#else
#define BASEFUNTION_API __declspec(dllimport)
#endif
#include "atlstr.h"
#include <stdint.h>

//编码
BASEFUNTION_API CStringA GBToUTF8(const char* str);
BASEFUNTION_API int UTF82GBK(char *szUtf8, char *szGbk, int Len);
BASEFUNTION_API CStringA URLEncode(CStringA sIn);
BASEFUNTION_API CStringA UrlDecode(LPCTSTR url);
BASEFUNTION_API CStringA UnicodeToANSI(const LPCWCH str);
BASEFUNTION_API CString ANSIToUnicode(const char* str);
//RC4
BASEFUNTION_API void rc4_crypt(unsigned char *data, int data_len, unsigned char *key, int key_len);

//MD5加密
BASEFUNTION_API typedef struct {
	uint32_t state[4];                                   /* state (ABCD) */
	uint32_t count[2];        /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

BASEFUNTION_API void MD5Init(MD5_CTX *);
BASEFUNTION_API void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
BASEFUNTION_API void MD5Final(unsigned char[16], MD5_CTX *);
BASEFUNTION_API void hmac_md5(unsigned char* text, int text_len, unsigned char* key, int key_len, unsigned char* outPut);
BASEFUNTION_API int Md5_File(char *file_md5, char *path, const int md5_len);
//Base64
class BASEFUNTION_API CBase64
{
public:
	static bool Encode(const unsigned char *pIn, unsigned long uInLen, CStringA& strOut);
	static bool Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen);
	static bool Decode(const CStringA& strIn, unsigned char *pOut, unsigned long *uOutLen);

	static CStringA Encode(CStringA data);
	static CStringA Decode(CStringA data);
	static int GetDecodeBufferLength(CStringA data);
};

//AES
BASEFUNTION_API char *aes_decrypt(const char *cipher);
BASEFUNTION_API char* aes_encrypt(const char *plaintext);
BASEFUNTION_API void set_aes_key(const char *key);

//中兴验证码识别
BASEFUNTION_API int ZTECheckCode(const char *authcodepath, char *text);

BASEFUNTION_API CStringA GetFileVersion(LPCTSTR lpszFilePath);
BASEFUNTION_API int CompareVersion(char *localver, char *remotever);
BASEFUNTION_API int RunAsAdmin(TCHAR *Parameters);
BASEFUNTION_API BOOL IsWow64();
#endif