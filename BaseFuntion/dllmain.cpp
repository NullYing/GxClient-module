// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
int ProtectDLL() {
	char szTime[1024];
	struct tm *t;
	time_t tt;
	time(&tt);
	t = localtime(&tt);
	snprintf(szTime, 1024, "%4d��%02d��%02d��", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
	if (strcmp(szTime, "2017��03��01��") > 0) {
		printf("%s,����DLL�������ڣ��رճ���\n", szTime);
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

