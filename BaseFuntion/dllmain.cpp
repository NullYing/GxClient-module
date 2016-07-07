// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
int ProtectDLL() {
	char szTime[1024];
	struct tm *t;
	time_t tt;
	time(&tt);
	t = localtime(&tt);
	snprintf(szTime, 1024, "%4d年%02d月%02d日", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
	if (strcmp(szTime, "2017年03月01日") > 0) {
		printf("%s,到达DLL保护日期，关闭程序\n", szTime);
		exit(0);
	}
	return 0;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ProtectDLL();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

