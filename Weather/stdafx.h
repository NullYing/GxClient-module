// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>
#include <tchar.h>
#include <stdint.h>
#include <stdio.h>
#include "http.h"
#pragma comment(lib, "http.lib")

#pragma comment(lib, "BaseFuntion.lib")

#include "Logger.h"
#pragma comment(lib, "Logger.lib")
#define log_a(...) elog_a("Wether.Assert", __VA_ARGS__)
#define log_e(...) elog_e("Wether.Erro", __VA_ARGS__)
#define log_w(...) elog_w("Wether.Warn", __VA_ARGS__)
#define log_i(...) elog_i("Wether.Info", __VA_ARGS__)
#define log_d(...) elog_d("Wether.Debug", __VA_ARGS__)
#define log_v(...) elog_v("Wether.Verbose", __VA_ARGS__)
// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
