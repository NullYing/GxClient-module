// http.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "http.h"
#include "BaseFuntion.h"

typedef struct _buffer {
	char *buffer;
	size_t size;
	int offset;
} Buffer;

size_t write_buffer_data(void *ptr, size_t size, size_t nmemb, void *buffer)
{
	Buffer *buf = (Buffer*)buffer;
	if (buf->buffer == NULL) return 0;
	if (buf->offset + size >= buf->size) 
		return 0;
	memcpy(buf->buffer + buf->offset, ptr, size * nmemb);
	buf->offset += size * nmemb;
	return size * nmemb;
}
static size_t write_file_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	if (stream == NULL) return 0;
	int written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}
int AutoCharset(Buffer *head, Buffer *http) {
	char charset[10] = {};
	CatchHttp getchar[1] = { "charset",charset,sizeof(charset) - 1 };
	if (GetHTTPMsg(head->buffer, head->size, 1, getchar, "=", "\n")) {
		if (strstr(charset, "utf-8") != NULL) {
			UTF82GBK(http->buffer, http->buffer, http->size);
			return 1;
		}
		if (strstr(charset, "GBK") != NULL) {
			return 1;
		}
		if (strstr(charset, "GBK") == NULL&&strstr(charset, "utf-8") == NULL) {
			UTF82GBK(http->buffer, http->buffer, http->size);
			return 1;
		}
	}
	else
		UTF82GBK(http->buffer, http->buffer, http->size);
	return 0;
}
char *CURLErrMsg(const int res) {
	switch (res) {
	case CURLE_COULDNT_RESOLVE_HOST: {//无法解析主机。给定的远程主机没有得到解决。
		return "DNS无法解析";
	}
	case CURLE_REMOTE_ACCESS_DENIED: {//不能连接到remote 主机或者代理
		return "远程服务器拒绝访问";
	}
	case CURLE_WRITE_ERROR: {
		return "写入缓冲区错误";
	}
	case CURLE_OPERATION_TIMEDOUT: {
		return "链接超时";
	}
	default: {
		return "链接错误";
	}
	}
}
HTTP_API char *Http302Test(const char *url,char *jumpurl,const int jumpurlsize)
{
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url); //url地址
		//url_easy_setopt(curl, CURLOPT_VERBOSE, 1); //打印调试信息
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTPTIMEOUT);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2486.0 Safari/537.36");
		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "GXClient Send By GXGK Team");
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L); //进度条
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			curl_easy_cleanup(curl);
			return CURLErrMsg(res);
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (200 <= retcode && retcode < 300) {
			curl_easy_cleanup(curl);
			return "网络已连接";
		}
		if (300 <= retcode && retcode < 400) {
			char *r = NULL;
			res = curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &r);
			strncpy(jumpurl, r, jumpurlsize);
			curl_easy_cleanup(curl);
			return NULL;
		}
		static char msg[MAX_STRING];
		memset(msg, 0, MAX_STRING);
		snprintf(msg, MAX_STRING, "网页无跳转，HTTP状态码：%d", retcode);
		curl_easy_cleanup(curl);
		return msg;
	}
	return "建立连接失败";
}

HTTP_API char *HttpToBuffer(char *http, int bufsize, char url[], const char *data, char *cookies, char *cookiefile, char *cookiejar,char *referer,int timeout)
{
	CURL *curl;
	CURLcode res;
	Buffer buf;
	buf.buffer = http;
	buf.size = bufsize;
	buf.offset = 0;

	char httpheader[HTTPBUFSIZE] = {};
	Buffer head;
	head.buffer = httpheader;
	head.size = HTTPBUFSIZE;
	head.offset = 0;

	long retcode = 0;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url); //url地址
		if (data != NULL) {
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data); //post参数
			curl_easy_setopt(curl, CURLOPT_POST, 1); //设置问非0表示本次操作为post
		}
		if (referer)curl_easy_setopt(curl, CURLOPT_REFERER, referer);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //打印调试信息
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&head);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_buffer_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);//这是write_data的第四个参数值
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer_data);//对返回的数据进行操作的函数地址
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2486.0 Safari/537.36");
		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "GXClient Send By GXGK Team");
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L); //进度条
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		if (cookiefile)curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefile);//读取cookies
		if (cookiejar)curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiejar);//保存cookies
		if (cookies)curl_easy_setopt(curl, CURLOPT_COOKIE, cookies);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);//认证之前会自动转跳

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			curl_easy_cleanup(curl);
			return CURLErrMsg(res);
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (retcode < 200 || retcode > 300) {
			static char msg[HTTPBUFSIZE];
			memset(msg, 0, MAX_STRING);
			snprintf(msg, HTTPBUFSIZE, "HTTP状态码：%d", retcode);
			curl_easy_cleanup(curl);
			return msg;
		}
		if (strlen(http) == 0) {
			curl_easy_cleanup(curl);
			return "接收到的数据为空";
		}
		AutoCharset(&head, &buf);
		curl_easy_cleanup(curl);
		return NULL;
	}
	return "建立链接失败";
}
HTTP_API char *POSTFile(char *http, int bufsize, const char * file, const char *savename, const char *url, const int timeout, const char *cookiefile, const char *cookiejar) {

	long retcode = 0;
	Buffer buf;
	buf.buffer = http;
	buf.size = bufsize;
	buf.offset = 0;

	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headers = NULL;
	CURL *curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //打印调试信息
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);//这是write_data的第四个参数值

		headers = curl_slist_append(headers, "content-type: multipart/form-data;");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2486.0 Safari/537.36");
		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "GXClient Send By GXGK Team");
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L); //进度条
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		if (cookiefile)curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefile);//读取cookies
		if (cookiejar)curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiejar);//保存cookies

		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "file",
			CURLFORM_FILE, file,
			CURLFORM_FILENAME, savename,
			CURLFORM_END);

		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

		CURLcode ret = curl_easy_perform(curl);
		if (ret != CURLE_OK) {
			curl_easy_cleanup(curl);
			return CURLErrMsg(ret);
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (retcode < 200 || retcode > 300) {
			static char msg[HTTPBUFSIZE];
			memset(msg, 0, MAX_STRING);
			snprintf(msg, HTTPBUFSIZE, "HTTP状态码：%d", retcode);
			curl_easy_cleanup(curl);
			return msg;
		}
		curl_easy_cleanup(curl);
		return NULL;
	}
	return "建立链接失败";
}
HTTP_API char *DownLoadFile(const char *savename, const char *url, const int timeout, const char *cookiefile, const char *cookiejar)
{
	FILE *file = NULL;
	fopen_s(&file, savename, "wb");
	if (file == NULL) 
		return "新建文件错误";

	CURL *curl;
	CURLcode res;
	long retcode = 0;
	int ret = 0;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //打印调试信息
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		if (timeout > 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		if (cookiefile)curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefile);//读取cookies
		if (cookiejar)curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiejar);//保存cookies
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)file);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2486.0 Safari/537.36");
		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "GXClient Send By GXGK Team");
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			curl_easy_cleanup(curl);
			fclose(file);
			file = NULL;
			DeleteFileA(savename);
			return CURLErrMsg(res);
		}
		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (retcode < 200 || retcode > 300) {
			static char msg[HTTPBUFSIZE];
			memset(msg, 0, MAX_STRING);
			snprintf(msg, HTTPBUFSIZE, "HTTP状态码：%d", retcode);
			curl_easy_cleanup(curl);
			fclose(file);
			file = NULL;
			DeleteFileA(savename);
			return msg;
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(file);
		file = NULL;
		return NULL;
	}
	return "建立链接失败";
}
HTTP_API int GetHTTPMsg(const char  *http, const int size, const int num, CatchHttp *data, const char *begainsign, const char *endsign)
{
	char *x = (char *)malloc(size), *q, *p, *w;
	p = x;
	char Sign[MAX_STRING];
	int SeccessSign = 1;
	strncpy(p, http, size);
	for (int i = 0; i < num; i++)
	{
		p = x;
		strcpy(p, http);//strcpy会移动指针，必须重新赋值
		strncpy(Sign, (data + i)->sign, MAX_STRING);
		p = strstr(p, Sign);
		if (p == NULL)
		{
			//printf("失败1");
			SeccessSign = 0;
			continue;
		}
		p += strlen(Sign);
		if (begainsign != NULL) {
			p = strstr(p, begainsign);
			if (p == NULL)
			{
				//printf("失败2");
				SeccessSign = 0;
				continue;
			}
			p += strlen(begainsign);
		}
		if (*p == '\r') {
			p += 2;
			for (; *p == '\t'; p++);
		}

		q = strstr(p, endsign);
		if (q == NULL)
		{
			//printf("失败3");
			SeccessSign = 0;
			continue;
		}
		*q = '\0';
		w = strstr(p, "\r\n");
		if (w != NULL)*w = '\0';
		strncpy((data + i)->data, p, (data + i)->datalen);
	}
	free(x);
	return SeccessSign;
}