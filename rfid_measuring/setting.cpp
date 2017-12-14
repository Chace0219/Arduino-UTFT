/*
	Author: jinzhouyun
	Date:	2017.07
	eamil:	2435575291@qq.com
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "setting.h"

// ID of the settings block
#define CONFIG_VERSION "AR8"

// Tell it where to store your config data in EEPROM
#define CONFIG_START 32

DeviceStatus mystatus;

SystemSetting setting = {
	CONFIG_VERSION,
	8,
	8,

	-4167.0F,
	0.0F,
	4000,

	"2B83J", // SSID setting, in EEPROM
	"8VDRR6Y6DWVVFQNB", // Wifi Password
	"jin.munchiesmonitor.com",
	80,
	"/api/restful_munchies.php",
	"Yogen", // current id
	"1234", // current password
	"/logging/",
	"Yogen", // default id 
	"1234", // default password
};

/**/
void loadConfig() 
{
	if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
		EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
		EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2])
		for (unsigned int t = 0; t<sizeof(setting); t++)
			*((char*)&setting + t) = EEPROM.read(CONFIG_START + t);
}

/**/
void saveConfig() 
{
	for (unsigned int t = 0; t<sizeof(setting); t++)
		EEPROM.write(CONFIG_START + t, *((char*)&setting + t));
}
