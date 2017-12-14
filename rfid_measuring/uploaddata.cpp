
#include "uploaddata.h"
#include "setting.h"
ESP8266 wifi(Serial3, 115200);

bool connectToDefaultAP()
{
	wifi.restart();
	delay(2000);

	// 
	String macaddr = wifi.getMAC();
	Serial.print(F("MAC address is "));
	Serial.println(macaddr);
	strcpy(mystatus.macaddr, macaddr.c_str());

	if (!wifi.setOprToStation())
	{
		Serial.println(F("Fail to setting."));
		return false;
	}

	if (wifi.joinAP(setting.apSSID, setting.apPASS))
	{
		Serial.print(F("Join AP success\r\n"));
		Serial.print(F("IP:"));
		Serial.println(wifi.getLocalIP().c_str());
	}
	else
		return false;

	return true;
}

bool setNewAP(String ssid, String password)
{
	wifi.restart();
	delay(2000);

	if (!wifi.setOprToStation())
	{
		Serial.println(F("Fail to setting."));
		return false;
	}
	
	if (wifi.joinAP(ssid.c_str(), password.c_str()))
	{
		Serial.print(F("Join AP success\r\n"));
		Serial.print(F("IP:"));
		Serial.println(wifi.getLocalIP().c_str());

		strcpy(setting.apSSID, ssid.c_str());
		strcpy(setting.apPASS, password.c_str());

		saveConfig();
		Serial.println(F("Saved"));
	}
	else
		return false;

	return true;
}

uint8_t uploaddata(String macAddr, String userID, String date, String time, String aniID, String weight)
{
	uint8_t buffer[512]= { 0 };

	Serial.println(F("upload is started!"));
	if (!wifi.getVersion())
	{
		Serial.print(F("FW Version error:"));
		return NOTPRESENT;
	}
	String status = wifi.getIPStatus();
	Serial.println(status);
	if (status.toInt() == 5)
	{// there is no connection from AP
		if (!wifi.setOprToStation())
		{
			Serial.println(F("Fail to setting."));
			return NOCONNECT;
		}
		if (wifi.joinAP(setting.apSSID, setting.apPASS)) {
			Serial.print("Join AP success\r\n");

			Serial.print(F("IP:"));
			Serial.println(wifi.getLocalIP().c_str());
		}
		else
		{
			Serial.println(F("I can't connect to AP."));
			return NOCONNECT;
		}
	}

	if (!wifi.disableMUX()) 
		Serial.print(F("single err\r\n"));

	if (!wifi.createTCP(setting.cloudHost, setting.cloudPort))
		Serial.print(F("create tcp err\r\n"));

	String request = "";
	request = "GET " + String(setting.cloudPath);
	request += "?var1=";
	request += macAddr;
	request += "&var2=";
	request += userID;
	request += "&var3=";
	request += date;
	request += "&var4=";
	request += time;
	request += "&var5=";
	request += aniID;
	request += "&var6=";
	request += weight;
	request += " HTTP/1.1\r\n";
	request += "Host: ";
	request += setting.cloudHost;
	request += "\r\nConnection: close\r\n\r\n";

	Serial.println(request);

	bool result = wifi.send((const uint8_t*)request.c_str(), request.length());
	uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
		
	if (len > 0) {
		Serial.print(F("Received:["));
		for (uint32_t i = 0; i < len; i++) {
			Serial.print((char)buffer[i]);
		}
		Serial.print(F("]\r\n"));
	}

	wifi.releaseTCP();
	if (result)
		return SUCCESS;
	else
		return FAILUPLOAD;
}
