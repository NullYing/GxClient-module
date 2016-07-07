#pragma once
class CWeather {
public:
	CWeather(void);
	int GetCity();
	int GetWeather();
	int GetPM25();

	//东莞天气代码
	char WeatherArea[30] = "101281601";
	char PM25City[30] = "dongguan";
	//南城西平检测点
	char position_name[30] = "";
	struct WeatherData *weatherdata;
private:
	char WeatherCode[32][20] = { "晴","多云","阴","阵雨","雷阵雨","雷阵雨伴有冰雹",
		"雨夹雪","小雨","中雨","大雨","暴雨","大暴雨",
		"特大暴雨","阵雪","小雪","中雪","大雪","暴雪","雾",
		"冻雨","沙尘暴","小到中雨","中到大雨","大到暴雨",
		"暴雨到大暴雨","大暴雨到特大暴雨","小到中雪","中到大雪",
		"大到暴雪","浮尘","扬沙","强沙尘暴" };

	char WeatherCodeExt[4][20] = { "霾","中度霾","重度霾","严重霾" };
	char WindCode[10][20] = { "微风", "3-4级", "4-5级", "5-6级", "6-7级", "7-8级",
		"8-9级", "9-10级", "10-11级", "11-12级" };

	int Interpreting(const char * http);
	int DownPhoto(const int daycode, const int nightcode,const int day);
};