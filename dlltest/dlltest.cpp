// dlltest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
/*
#include "TYAuth.h"
#include "ZTEAuth.h"
#include "http.h"
*/
#include "process.h"
#include "Logger.h"
#include "http.h"
#include <atlstr.h>
#include <time.h>

#include"iostream"
#include"winsock2.h"
#pragma comment(lib,"ws2_32.lib")

using namespace std;

BOOL fetchIPAddress(char* ipaddr)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	{
		return NULL;
	}
	char host_name[225]; BOOL tag = FALSE;
	if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR)
	{
		cout << "Error " << WSAGetLastError() << " when getting local host name." << endl;
	}

	struct hostent *phe = gethostbyname(host_name);
	if (phe == 0)
	{
		cout << "Yow!Bad host lookup.";
	}
	int num = phe->h_length;
	struct in_addr* addr = new struct in_addr[num];
	for (int i = 0; i < num &&phe->h_addr_list[i]!=NULL; i++) {
		memcpy(&addr[i], phe->h_addr_list[i], sizeof(struct in_addr));
		char *ip = inet_ntoa(addr[i]);
		if (strstr(ip, "10.10") != NULL) {
			strcpy(ipaddr, inet_ntoa(addr[i]));
			tag = TRUE;
			break;
		}
	}
	delete[] addr;
	WSACleanup();
	return tag;
}
// MacAddress.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <wincon.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <Nb30.h>
#pragma comment(lib,"netapi32.lib")  

int GetMac(char * mac)
{
	NCB ncb;
	typedef struct _ASTAT_
	{
		ADAPTER_STATUS   adapt;
		NAME_BUFFER   NameBuff[30];
	}ASTAT, *PASTAT;

	ASTAT Adapter;

	typedef struct _LANA_ENUM
	{
		UCHAR   length;
		UCHAR   lana[MAX_LANA];
	}LANA_ENUM;

	LANA_ENUM lana_enum;
	UCHAR uRetCode;
	memset(&ncb, 0, sizeof(ncb));
	memset(&lana_enum, 0, sizeof(lana_enum));
	ncb.ncb_command = NCBENUM;
	ncb.ncb_buffer = (unsigned char *)&lana_enum;
	ncb.ncb_length = sizeof(LANA_ENUM);
	uRetCode = Netbios(&ncb);

	if (uRetCode != NRC_GOODRET)
		return uRetCode;

	for (int lana = 0; lana<lana_enum.length; lana++)
	{
		ncb.ncb_command = NCBRESET;
		ncb.ncb_lana_num = lana_enum.lana[lana];
		uRetCode = Netbios(&ncb);
		if (uRetCode == NRC_GOODRET)
		{
			memset(&ncb, 0, sizeof(ncb));
			ncb.ncb_command = NCBASTAT;
			ncb.ncb_lana_num = lana_enum.lana[lana];
			strcpy((char*)ncb.ncb_callname, "*");
			ncb.ncb_buffer = (unsigned char *)&Adapter;
			ncb.ncb_length = sizeof(Adapter);
			uRetCode = Netbios(&ncb);

			if (uRetCode != NRC_GOODRET)
				return uRetCode;

			sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]);
		}
	}
	if (uRetCode != NRC_GOODRET)
		return uRetCode;
	return 0;
}
int main() {
	char ipaddr[19] = {};
	int ret = fetchIPAddress(ipaddr);
	printf(ipaddr);
	char  mac[200];
	GetMac(mac);
	printf("The Mac Address is : %s   \n", mac);
	system("pause");
	return 0;
}
