#include <stdio.h>
#include "string.h"
#include "malloc.h"
#include "atlstr.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 宏定义
#define STR_VERMAJOR "VER_Major"
#define STR_VERMINOR "VER_Minor"
#define STR_VERBUILD "VER_Build"
#define STR_VERRELEASE "VER_Release"

#define STR_RC_FILEVER _T("FILEVERSION ")
#define STR_RC_FILEVER2 _T("\"FileVersion\", \"")

#define STR_RC_PRODUCTVER _T("PRODUCTVERSION ")
#define STR_RC_PRODUCTVER2 _T("\"ProductVersion\", \"")

#define IsNumberic(c) ((c >= '0') && (c <= '9'))
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//版本
CStringA VersionDot;
CStringA VersionComma;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long GetLength(FILE *fp)
{
	long cur_pos;
	long len;

	//取得当前文件流的读取位置
	cur_pos = ftell(fp);
	//将文件流的读取位置设为文件末尾
	fseek(fp, 0, SEEK_END);
	//获取文件末尾的读取位置,即文件大小
	len = ftell(fp);
	//将文件流的读取位置还原为原先的值
	fseek(fp, cur_pos, SEEK_SET);
	return len;
}
void DelChart(char **p) {
	for (; IsNumberic(**p) == FALSE; (*p)++) {
		;
	}
	char *end = NULL;
	for (end = *p; IsNumberic(*end) == TRUE; end++) {
		;
	}
	*end = '\0';
}
CString ANSIToUnicode(const char* str)
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
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
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
int GetDefineVersion(char *VerAddr) {
	//寻找打开define文件
	FILE *fp = NULL;
	fp = fopen(VerAddr, "r");
	if (!fp) {
		printf("打开版本文件失败\n");
		return 0;
	}
	//读取define文件
	int length = GetLength(fp) + 1;
	char *p = (char *)malloc(length*(sizeof(char)));
	fread_s(p, length, length, 1, fp);
	if (strlen(p) == 0) {
		printf("读取文件失败\n");
		return 0;
	}
	char * VerMajor = strstr(p, STR_VERMAJOR) + strlen(STR_VERMAJOR);
	char * VerMinor = strstr(p, STR_VERMINOR) + strlen(STR_VERMINOR);
	char * VerBuild = strstr(p, STR_VERBUILD) + strlen(STR_VERBUILD);
	char * VerRelease = strstr(p, STR_VERRELEASE) + strlen(STR_VERRELEASE);
	DelChart(&VerMajor);
	DelChart(&VerMinor);
	DelChart(&VerBuild);
	DelChart(&VerRelease);
	if (!VerMajor || !VerMinor || !VerBuild || !VerRelease) {
		printf("读取版本失败\n");
		return 0;
	}
	printf("读取版本号成功：%s.%s.%s.%s\n", VerMajor, VerMinor, VerRelease, VerBuild);
	VersionDot.Format("%s.%s.%s.%s", VerMajor, VerMinor, VerRelease, VerBuild);
	VersionComma.Format("%s,%s,%s,%s", VerMajor, VerMinor, VerRelease, VerBuild);
	//关闭并清理内存
	fclose(fp);
	free(p);
	return 1;
}
BOOL ReWriteBuf(wchar_t *buf, wchar_t *str, wchar_t *EndSign, wchar_t *ver) {
	wchar_t *p = buf;
	wchar_t *VerInfo = wcsstr(buf, str);
	if (!VerInfo) {
		printf("寻找版本号失败\n");
		return 0;
	}
	VerInfo += wcslen(str);
	//修改RC
	int offset = (VerInfo - p) * sizeof(char);
	wchar_t *p_ver = p + offset;
	p = wcsstr(p_ver, EndSign);
	int buf2_len = wcslen(buf) - wcslen(VerInfo) + 100;
	wchar_t *Remaining = new wchar_t[buf2_len];
	memset(Remaining, 0, buf2_len);
	//复制内容
	wcscpy(Remaining, p);
	wcscpy(p_ver, ver);
	wcscpy(p_ver + wcslen(ver), Remaining);
	delete(Remaining);
	return TRUE;
}
BOOL WriteRC(char *RcAddr, BOOL bProduct = FALSE) {
	FILE *fp = NULL;
	//打开RC
	fp = _wfopen(ANSIToUnicode(RcAddr), _T("r, ccs=UTF-16LE"));
	if (!fp) {
		printf("打开版本文件失败\n");
		return 0;
	}
	int length = GetLength(fp) + 500;
	wchar_t *buf = new wchar_t[length];
	memset(buf, 0, length);
	fread_s(buf, length, length, 1, fp);
	if (!buf) {
		printf("读取RC文件失败%s\n", ferror(fp));
		return 0;
	}
	fclose(fp);
	//写入第一个版本
	CString Version = ANSIToUnicode(VersionComma);
	ReWriteBuf(buf, STR_RC_FILEVER, _T("\n"), Version.GetBuffer());
	//写入产品版本
	if (bProduct) {
		ReWriteBuf(buf, STR_RC_PRODUCTVER, _T("\n"), Version.GetBuffer());
	}
	printf("修改第一个版本位置成功\n");
	//写入第二个版本
	Version = ANSIToUnicode(VersionDot);
	ReWriteBuf(buf, STR_RC_FILEVER2, _T("\""), Version.GetBuffer());
	//写入产品版本
	if (bProduct) {
		ReWriteBuf(buf, STR_RC_PRODUCTVER2, _T("\""), Version.GetBuffer());
	}
	printf("修改第二个版本位置成功\n");
	//重新写入
	fp = _wfopen(ANSIToUnicode(RcAddr), _T("w, ccs=UTF-16LE"));
	fwrite(buf, wcslen(buf)*(sizeof(wchar_t)), 1, fp);
	printf("重写数据成功\n");
	fclose(fp);
	delete(buf);
	return TRUE;
}

int main(int argc, char *argv[]) {
	BOOL bProduct = FALSE;
	char VerAddr[MAX_PATH] = "";
	char RcAddr[MAX_PATH] = "";
	if (argc < 5) {
		printf("缺少参数\n");
		return 0;
	}
	if (strcmp(argv[1], "-d") == 0) {
		strcpy(VerAddr, argv[2]);
	}
	if (strcmp(argv[3], "-rc") == 0) {
		strcpy(RcAddr, argv[4]);
	}
	if (argc == 6 && strcmp(argv[5], "-p") == 0) {
		bProduct = TRUE;
	}
	if (GetDefineVersion(VerAddr)) {
		WriteRC(RcAddr, bProduct);
	}
	return 0;
}
