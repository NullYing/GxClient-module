// http.cpp : ���� DLL Ӧ�ó���ĵ���������
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
	case CURLE_COULDNT_RESOLVE_HOST: {//�޷�����������������Զ������û�еõ������
		return "DNS�޷�����";
	}
	case CURLE_REMOTE_ACCESS_DENIED: {//�������ӵ�remote �������ߴ���
		return "Զ�̷������ܾ�����";
	}
	case CURLE_WRITE_ERROR: {
		return "д�뻺��������";
	}
	case CURLE_OPERATION_TIMEDOUT: {
		return "���ӳ�ʱ";
	}
	default: {
		return "���Ӵ���";
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
		curl_easy_setopt(curl, CURLOPT_URL, url); //url��ַ
		//url_easy_setopt(curl, CURLOPT_VERBOSE, 1); //��ӡ������Ϣ
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTPTIMEOUT);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2486.0 Safari/537.36");
		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "GXClient Send By GXGK Team");
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L); //������
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			curl_easy_cleanup(curl);
			return CURLErrMsg(res);
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (200 <= retcode && retcode < 300) {
			curl_easy_cleanup(curl);
			return "����������";
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
		snprintf(msg, MAX_STRING, "��ҳ����ת��HTTP״̬�룺%d", retcode);
		curl_easy_cleanup(curl);
		return msg;
	}
	return "��������ʧ��";
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
		curl_easy_setopt(curl, CURLOPT_URL, url); //url��ַ
		if (data != NULL) {
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data); //post����
			curl_easy_setopt(curl, CURLOPT_POST, 1); //�����ʷ�0��ʾ���β���Ϊpost
		}
		if (referer)curl_easy_setopt(curl, CURLOPT_REFERER, referer);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //��ӡ������Ϣ
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&head);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_buffer_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);//����write_data�ĵ��ĸ�����ֵ
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer_data);//�Է��ص����ݽ��в����ĺ�����ַ
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2486.0 Safari/537.36");
		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "GXClient Send By GXGK Team");
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L); //������
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		if (cookiefile)curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefile);//��ȡcookies
		if (cookiejar)curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiejar);//����cookies
		if (cookies)curl_easy_setopt(curl, CURLOPT_COOKIE, cookies);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);//��֤֮ǰ���Զ�ת��

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			curl_easy_cleanup(curl);
			return CURLErrMsg(res);
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (retcode < 200 || retcode > 300) {
			static char msg[HTTPBUFSIZE];
			memset(msg, 0, MAX_STRING);
			snprintf(msg, HTTPBUFSIZE, "HTTP״̬�룺%d", retcode);
			curl_easy_cleanup(curl);
			return msg;
		}
		if (strlen(http) == 0) {
			curl_easy_cleanup(curl);
			return "���յ�������Ϊ��";
		}
		AutoCharset(&head, &buf);
		curl_easy_cleanup(curl);
		return NULL;
	}
	return "��������ʧ��";
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
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //��ӡ������Ϣ
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);//����write_data�ĵ��ĸ�����ֵ

		headers = curl_slist_append(headers, "content-type: multipart/form-data;");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2486.0 Safari/537.36");
		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "GXClient Send By GXGK Team");
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L); //������
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		if (cookiefile)curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefile);//��ȡcookies
		if (cookiejar)curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiejar);//����cookies

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
			snprintf(msg, HTTPBUFSIZE, "HTTP״̬�룺%d", retcode);
			curl_easy_cleanup(curl);
			return msg;
		}
		curl_easy_cleanup(curl);
		return NULL;
	}
	return "��������ʧ��";
}
HTTP_API char *DownLoadFile(const char *savename, const char *url, const int timeout, const char *cookiefile, const char *cookiejar)
{
	FILE *file = NULL;
	fopen_s(&file, savename, "wb");
	if (file == NULL) 
		return "�½��ļ�����";

	CURL *curl;
	CURLcode res;
	long retcode = 0;
	int ret = 0;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //��ӡ������Ϣ
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		if (timeout > 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		if (cookiefile)curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefile);//��ȡcookies
		if (cookiejar)curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiejar);//����cookies
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//��֤֮ǰ���Զ�ת��
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
			snprintf(msg, HTTPBUFSIZE, "HTTP״̬�룺%d", retcode);
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
	return "��������ʧ��";
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
		strcpy(p, http);//strcpy���ƶ�ָ�룬�������¸�ֵ
		strncpy(Sign, (data + i)->sign, MAX_STRING);
		p = strstr(p, Sign);
		if (p == NULL)
		{
			//printf("ʧ��1");
			SeccessSign = 0;
			continue;
		}
		p += strlen(Sign);
		if (begainsign != NULL) {
			p = strstr(p, begainsign);
			if (p == NULL)
			{
				//printf("ʧ��2");
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
			//printf("ʧ��3");
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