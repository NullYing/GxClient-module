/*
* This file is part of the EasyLogger Library.
*
* Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* 'Software'), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* Function: Portable interface for each platform.
* Created on: 2015-04-28
*/
#include <elog.h>
#include <stdio.h>
#include <windows.h>
#include <sys/stat.h>
#include <Logger.h>

static char LogFilePath[256];
extern HWND m_hWnd;
extern void (*SendLog)(char *time, char *tag, uint8_t level, char *raw, HWND hWnd);
FILE *fp = NULL;
OutLog slog;
HANDLE m_hMutex;
/**
* EasyLogger port initialize
*
* @return result
*/
ElogErrCode elog_port_init(void) {
	ElogErrCode result = ELOG_NO_ERR;
	//获取日志路径
	GetModuleFileNameA(GetModuleHandleA(NULL), LogFilePath, 256);
	strcpy(strrchr(LogFilePath, '\\') + 1, WRITER_FILE_NAME);
	struct _stat info;
	_stat(LogFilePath, &info);
	int size = info.st_size;
	//大于1M自动清空日志
	if (size >= 1024 * 1024)
		DeleteFileA(LogFilePath);
	//互斥加锁
	m_hMutex = CreateMutex(NULL, FALSE, L"mutex_for_log");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 如果已有互斥量存在则释放句柄并复位互斥量　
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	};
	return result;
}
/**
* output log port interface
*
* @param log output of log
* @param size log size
*/
void elog_port_output(const char *log, size_t size) {
	/* output to terminal */
	//写到文件
	fopen_s(&fp, LogFilePath, "a+");
	if (fp) {
		fwrite(log, size, 1, fp);
		fclose(fp);
	}
	(*SendLog)(slog.time, slog.tag, slog.level, slog.lograw, m_hWnd);
	//输出控制台
	//printf("%.*s", size, log);
}

/**
* output lock
*/
void elog_port_output_lock(void) {
	DWORD dReturn = WaitForSingleObject(m_hMutex, INFINITE);
}

/**
* output unlock
*/
void elog_port_output_unlock(void) {
	ReleaseMutex(m_hMutex);
}


/**
* get current time interface
*
* @return current time
*/
const char *elog_port_get_time(void) {
	static char cur_system_time[24] = { 0 };
	static SYSTEMTIME currTime;

	GetLocalTime(&currTime);
	//snprintf(cur_system_time, 24, "%02d-%02d %02d:%02d:%02d.%03d", currTime.wMonth, currTime.wDay,
	//	currTime.wHour, currTime.wMinute, currTime.wSecond, currTime.wMilliseconds);
	snprintf(cur_system_time, 24, "%02d-%02d %02d:%02d:%02d", currTime.wMonth, currTime.wDay,
		currTime.wHour, currTime.wMinute, currTime.wSecond);
	return cur_system_time;
}

/**
* get current process name interface
*
* @return current process name
*/
const char *elog_port_get_p_info(void) {
	static char cur_process_info[10] = { 0 };

	snprintf(cur_process_info, 10, "pid:%04ld", GetCurrentProcessId());

	return cur_process_info;
}

/**
* get current thread name interface
*
* @return current thread name
*/
const char *elog_port_get_t_info(void) {
	static char cur_thread_info[10] = { 0 };

	snprintf(cur_thread_info, 10, "tid:%04ld", GetCurrentThreadId());

	return cur_thread_info;
}
