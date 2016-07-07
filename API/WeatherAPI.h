#pragma once
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 WEATHER_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// WEATHER_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef WEATHER_EXPORTS
#define WEATHER_API extern "C" __declspec(dllexport)
#else
#define WEATHER_API extern "C" __declspec(dllimport)
#endif
struct DataOneDay {
	char day[30];
	char night[30];
	int dayTEMP;
	int nightTEMP;
	char daywind[30];
	char nightwind[30];
};
struct WeatherData {
	char city[30];
	DataOneDay today;
	DataOneDay tomorrow;
	int AQI = 0;
	int PM2_5;
	char AirQuality[30];
};
WEATHER_API int Weather(struct WeatherData *data, char AreaId[] = "101281601", char PM25City[] = "dongguan", const char* position_name = "南城西平");