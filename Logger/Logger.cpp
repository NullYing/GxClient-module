// Logger.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Logger.h"
#include "elog.h"
void(*SendLog)(char *time, char *tag, uint8_t level, char *raw, HWND hWnd);
HWND m_hWnd;

LOGGER_API int Logger_Init(void(*Log)(char *time, char *tag, uint8_t level, char *raw, HWND m_hWnd), HWND &hWnd) {
	SendLog = Log;
 	m_hWnd = hWnd;
	//日志初始化
	elog_init();
	/* set EasyLogger log format */
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_FUNC | ELOG_FMT_LINE);
	//elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	/* start EasyLogger */
	elog_start();
	return 0;
}