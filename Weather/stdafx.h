// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
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
// TODO:  在此处引用程序需要的其他头文件
