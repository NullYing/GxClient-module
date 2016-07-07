// Weather.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <time.h>
#include <string.h>
#include "Weather.h"
#include "WeatherAPI.h"
#include "BaseFuntion.h"
#include "Define.h"
#include "cJson.h"

WEATHER_API int Weather(struct WeatherData *data, char AreaId[], char PM25City[], const char* position_name)
{
	//拼装接口输出
	CWeather Weather;
	Weather.weatherdata = data;
	if (strlen(AreaId) != 0 && strlen(PM25City) != 0 && strlen(position_name) != 0) {
		strncpy(Weather.WeatherArea,AreaId,30);
		strncpy(Weather.PM25City, PM25City, 30);
		strncpy(Weather.position_name, position_name, 30);
	}
	else
		Weather.GetCity();
	int tag = 0;
	if (Weather.GetWeather()) {
		tag = 1;
	}
	else {
		log_w("获取天气失败");
		tag = 0;
	}
	if (Weather.GetPM25()) {
		if (tag == 1)
			tag = 3;
		else
			tag = 2;
	}
	else {
		log_w("获取PM2.5失败");
	}
    return tag;
}

CWeather::CWeather()
{
}
int CWeather::GetCity() {
	char cityid[30] = {};
	char http[HTTPBUFSIZE] = {};
	char url[MAX_STRING] = {};
	//生成时间戳
	time_t timep;
	time(&timep);
	struct tm *ptime;
	ptime = gmtime(&timep);
	timep = mktime(ptime);
	snprintf(url, MAX_STRING, "http://wgeo.weather.com.cn/ip/?_=%d", timep);

	char *p = HttpToBuffer(http, HTTPBUFSIZE, url, NULL, NULL, NULL, NULL, "http://www.weather.com.cn/");
	if (p) {
		log_w("获取本地天气id失败，connect:%s,data:%s", p, http);
		return 0;
	}
	CatchHttp httpdata[] = { "var id",cityid,sizeof(cityid) - 1 };
	GetHTTPMsg(http, HTTPBUFSIZE, 1, httpdata, "=", ";");
	if (strlen(cityid) != NULL)
		strncpy(WeatherArea, cityid, 30);
	return 1;
}
int CWeather::GetWeather(){
	//构造时间戳
	char szTime[MAX_STRING];
	struct tm *t;
	time_t tt;
	time(&tt);
	t = localtime(&tt);
	snprintf(szTime, MAX_STRING, "%4d%02d%02d%02d%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
	
	char http[HTTPBUFSIZE] = {};
	//生成key
	char url[MAX_STRING] = {};
	snprintf(url, MAX_STRING, "http://open.weather.com.cn/data/?areaid=%s&type=forecast_v&date=%s&appid=%s", WeatherArea, szTime, WEATHER_APPID);
	//hmac_sha1取密钥
	unsigned char sec_key[] = WEATHER_PRIVATE_KEY;
	unsigned char *data = (unsigned char *)url;
	unsigned char out[256] = { 0 };
	unsigned int len = sizeof(out);
	hmac_sha1(sec_key, strlen((char *)sec_key), data, strlen((char *)data), out, &len);
	
	//取appid前6位，构造链接
	memset(url, 0, MAX_STRING);
	char appid[] = { WEATHER_APPID }; appid[6] = '\0';
	snprintf(url, MAX_STRING, "http://open.weather.com.cn/data/?areaid=%s&type=forecast_v&date=%s&appid=%s&key=%s", WeatherArea, szTime, appid, URLEncode(CBase64::Encode(out)));

	char *p = HttpToBuffer(http, HTTPBUFSIZE, url, NULL);
	if (p || strstr(http, "data error")) {
		log_w("获取链接失败:%s,data:%s", p, http);
		return 0;
	}
	if (!Interpreting(http)) {
		log_w("读取天气数据失败");
		return 0;
	}
	return 1;
}

int CWeather::Interpreting(const char * http)
{
	int daycode = -1, nightcode = -1;
	int daywind = -1, nightwind = -1;
	cJSON *json = cJSON_Parse(http);
	if (!json) {
		log_w("解析天气失败！");
		return 0;
	}
	cJSON *pcity = cJSON_GetObjectItem(json, "c");
	if (pcity) {
		cJSON *pcityname = cJSON_GetObjectItem(pcity, "c3");
		char *cityname= pcityname->valuestring;
		cJSON *pcityename = cJSON_GetObjectItem(pcity, "c2");
		char *cityename = pcityename->valuestring;

		if (weatherdata->city&&cityname)
			strcpy(weatherdata->city, cityname);
		if (PM25City&&cityename)
			strcpy(PM25City, cityename);
	}
	cJSON *pweather = cJSON_GetObjectItem(json, "f");
	cJSON *ptoday = cJSON_GetObjectItem(pweather, "f1");
	ptoday = ptoday->child;
	cJSON *ptomorrow = ptoday->next;
	DataOneDay *pdata = &weatherdata->today;
	for (int i = 1; i <= 2; i++) {
		if (i == 2) {
			ptoday = ptomorrow;
			pdata = &weatherdata->tomorrow;
		}
		if (ptoday&&pdata) {
			//解析白天状态
			cJSON *pcode = cJSON_GetObjectItem(ptoday, "fa");
			char *day = pcode->valuestring;
			if (strlen(day) != 0) {
				//解析白天温度
				daycode = atoi(day);
				cJSON *ptemperature = cJSON_GetObjectItem(ptoday, "fc");
				pdata->dayTEMP = atoi(ptemperature->valuestring);
				//解析白天风力
				cJSON *pwind = cJSON_GetObjectItem(ptoday, "fg");
				daywind = atoi(pwind->valuestring);
			}
			//解析晚上状态
			pcode = cJSON_GetObjectItem(ptoday, "fb");
			nightcode = atoi(pcode->valuestring);
			//解析晚上温度
			cJSON *ptemperature = cJSON_GetObjectItem(ptoday, "fd");
			pdata->nightTEMP = atoi(ptemperature->valuestring);
			//解析晚上风力
			cJSON *pwind = cJSON_GetObjectItem(ptoday, "fh");
			nightwind = atoi(pwind->valuestring);

			int sizecode = sizeof(WeatherCode) / sizeof(WeatherCode[0]);
			if ((daycode >= sizecode&&daycode < 53 || daycode > 58) || (nightcode >= sizecode&&nightcode < 53 || nightcode > 58)) {
				cJSON_Delete(json);
				log_w("天气数组越界");
				return 0;
			}
			sizecode = sizeof(WindCode) / sizeof(WindCode[0]);
			if (daywind >= sizecode || nightwind >= sizecode) {
				log_w("风向数组越界");
				cJSON_Delete(json);
				return 0;
			}
			if (pdata->day&&daycode != -1)
				if (daycode >= 53)
					strcpy(pdata->day, WeatherCodeExt[daycode - 53]);
				else
					strcpy(pdata->day, WeatherCode[daycode]);
			if (pdata->night&&nightcode != -1)
				if (daycode >= 53)
					strcpy(pdata->night, WeatherCodeExt[daycode - 53]);
				else
					strcpy(pdata->night, WeatherCode[nightcode]);
			if (pdata->daywind&&daywind != -1)
				strcpy(pdata->daywind, WindCode[daywind]);
			if (pdata->nightwind&&nightwind != -1)
				strcpy(pdata->nightwind, WindCode[nightwind]);
			DownPhoto(daycode, nightcode, i);
		}
	}
	cJSON_Delete(json);
	return 1;
}
int CWeather::DownPhoto(const int daycode, const int nightcode,const int day) {
	char url[MAX_STRING];
	char szTempPath[MAX_PATH];
	GetTempPathA(MAX_PATH, szTempPath);
	if (daycode != -1) {
		snprintf(szTempPath, MAX_PATH, "%sweatherday%d.png", szTempPath, day);
		snprintf(url, MAX_STRING, "http://qn.gxgk.cc/day/%02d.png", daycode);
		char *p = DownLoadFile(szTempPath, url, 20, NULL, NULL);
		if (p) {
			log_w("下载白天天气图片失败:%s", p);
			return 0;
		}
	}
	if (nightcode != -1) {
		snprintf(szTempPath, MAX_PATH, "%sweathernight%d.png", szTempPath, day);
		snprintf(url, MAX_STRING, "http://qn.gxgk.cc/night/%02d.png", nightcode);
		char *p = DownLoadFile(szTempPath, url, 20, NULL, NULL);
		if (p) {
			log_w("下载夜间天气图片失败:%s", p);
			return 0;
		}
		return 1;
	}
	return 0;
}
int CWeather::GetPM25() {
	char url[MAX_STRING];
	char http[HTTPBUFSIZE] = {};
	if (strlen(position_name))
		snprintf(url, MAX_STRING, "http://www.pm25.in/api/querys/pm2_5.json?city=%s&avg=false&token=%s&stations=no", PM25City, PM2_5_TOKEN);
	else
		snprintf(url, MAX_STRING, "http://www.pm25.in/api/querys/pm2_5.json?city=%s&avg=true&token=%s&stations=no", PM25City, PM2_5_TOKEN);
	char *p = HttpToBuffer(http, HTTPBUFSIZE, url, NULL);
	if (p) {
		log_w("获取PM2.5链接失败:%s", p);
		return 0;
	}
	cJSON *json = cJSON_Parse(http);
	cJSON *bakjson = json;
	if (!json) {
		log_w("解析天气PM2.5失败！");
		return 0;
	}
	cJSON *pError = cJSON_GetObjectItem(json, "error"); 
	if(pError){
		log_w("获取天气PM2.5失败！错误信息：%s", pError->valuestring);
		cJSON_Delete(json);
		return 0;
	}
	int total = cJSON_GetArraySize(json);
	json = json->child;
	for (int i = 0; i <= total&&json != NULL; i++) {
		cJSON *pPosition_Name = cJSON_GetObjectItem(json, "position_name");
		if (pPosition_Name == NULL || (pPosition_Name != NULL && strcmp(position_name, pPosition_Name->valuestring) == 0) || strlen(position_name) == 0) {
			cJSON *paqi = cJSON_GetObjectItem(json, "aqi");
			int aqi = paqi->valueint;
			weatherdata->AQI = paqi->valueint;
			cJSON *ppm2_5 = cJSON_GetObjectItem(json, "pm2_5");
			weatherdata->PM2_5 = ppm2_5->valueint;
			cJSON *pquality = cJSON_GetObjectItem(json, "quality");
			if (pquality) {
				char *quality = pquality->valuestring;
				strcpy(weatherdata->AirQuality, quality);
				cJSON_Delete(bakjson);
				return 1;
			}
		}
		json = json->next;
	}
	cJSON_Delete(bakjson);
	return 0;
}