#ifndef UPLOADDATA_H
#define UPLOADDATA_H

#include "Arduino.h"

#include "ESP8266.h"

#define NOTPRESENT	0
#define NOCONNECT	1
#define FAILUPLOAD  2
#define SUCCESS		3

extern ESP8266 wifi;

uint8_t uploaddata(String macAddr, String userID, String date, String time, String aniID, String weight);
bool setNewAP(String ssid, String password);
bool connectToDefaultAP();

#endif