
#ifndef SETTING_H

#define SETTING_H

#include <Arduino.h>

void loadConfig();
void saveConfig();

typedef struct
{
	byte wifiStatus : 1;
	byte unknown : 7;
	uint8_t memoryPerc;
	uint8_t currscr;
	uint8_t prevscr;
	char animolID[20];
	char logFileName[32];
	double weight;
	char macaddr[18];
} DeviceStatus;

typedef struct 
{
	char version[4];
	uint8_t contrast;
	uint8_t brightness;

	double cali_factor;
	double scaleOffset;

	uint16_t weightinginterval;

	char apSSID[32];
	char apPASS[32];
	char cloudHost[32];
	uint16_t cloudPort;
	char cloudPath[32];
	char id[32];
	char pass[32];
	char loggingDir[32];
	
	char defaultid[32];
	char defaultpass[32];
} SystemSetting;

extern SystemSetting setting;
extern DeviceStatus mystatus;

#endif // !SETTING_H
