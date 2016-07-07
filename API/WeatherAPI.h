#pragma once
// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� WEATHER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// WEATHER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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
WEATHER_API int Weather(struct WeatherData *data, char AreaId[] = "101281601", char PM25City[] = "dongguan", const char* position_name = "�ϳ���ƽ");