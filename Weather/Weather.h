#pragma once
class CWeather {
public:
	CWeather(void);
	int GetCity();
	int GetWeather();
	int GetPM25();

	//��ݸ��������
	char WeatherArea[30] = "101281601";
	char PM25City[30] = "dongguan";
	//�ϳ���ƽ����
	char position_name[30] = "";
	struct WeatherData *weatherdata;
private:
	char WeatherCode[32][20] = { "��","����","��","����","������","��������б���",
		"���ѩ","С��","����","����","����","����",
		"�ش���","��ѩ","Сѩ","��ѩ","��ѩ","��ѩ","��",
		"����","ɳ����","С������","�е�����","�󵽱���",
		"���굽����","���굽�ش���","С����ѩ","�е���ѩ",
		"�󵽱�ѩ","����","��ɳ","ǿɳ����" };

	char WeatherCodeExt[4][20] = { "��","�ж���","�ض���","������" };
	char WindCode[10][20] = { "΢��", "3-4��", "4-5��", "5-6��", "6-7��", "7-8��",
		"8-9��", "9-10��", "10-11��", "11-12��" };

	int Interpreting(const char * http);
	int DownPhoto(const int daycode, const int nightcode,const int day);
};