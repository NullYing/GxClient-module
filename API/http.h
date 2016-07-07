// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 HTTP_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// HTTP_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once
#ifndef    __HTTP_H__
#define  __HTTP_H__
#include <stdint.h>
#ifdef HTTP_EXPORTS
#define HTTP_API __declspec(dllexport)
#else
#define HTTP_API __declspec(dllimport)
#endif
#define HTTPBUFSIZE 10240
#define MAX_STRING 2048
#define HTTPTIMEOUT 5
#define DOWNTIMEOUT 120

struct CatchHttp {
	char sign[MAX_STRING];
	char *data;
	int datalen;
};
HTTP_API void hmac_sha1(const uint8_t *k,   /* secret key */
	size_t lk,  /* length of the key in bytes */
	const uint8_t *d,   /* data */
	size_t ld,  /* length of data in bytes */
	uint8_t *out, /* output buffer, at least "t" bytes */
	size_t *t);

HTTP_API char *Http302Test(const char *url, char *jumpurl, const int jumpurlsize);
HTTP_API char *HttpToBuffer(char *http, int bufsize, char url[], const char *data = NULL, char *cookies = NULL, char *cookiefile = NULL, char *cookiejar = NULL, char *referer = NULL, const int timeout = HTTPTIMEOUT);
HTTP_API char *POSTFile(char *http, int bufsize, const char * file, const char *savename, const char *url, const int timeout = DOWNTIMEOUT, const char *cookiefile = NULL, const char *cookiejar = NULL);
HTTP_API char *DownLoadFile(const char *savename, const char *url, const int timeout = DOWNTIMEOUT, const char *cookiefile = NULL, const char *cookiejar = NULL);
HTTP_API int GetHTTPMsg(const char  *http, const int size, const int num, struct CatchHttp *data, const char *begainsign, const char *endsign);
#endif