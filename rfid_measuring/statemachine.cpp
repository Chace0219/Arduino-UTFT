
#include <Arduino.h>
#include "Statistic.h"

#include "global.h"
#include "FiniteStateMachine.h"
#include "RTClib.h"
#include "setting.h"
#include "flash.h"
#include "EM4205.h"
#include "loadcell.h"
#include "sd_manager.h"

#include "uploaddata.h"

#define LOGINUSER 0
#define LOGINPASS 1
#define NEWUSER 2
#define NEWPASS 3
#define CONFIRMPASS 4
#define APPASSWORD 5

void initAPTable();
void scanAP();
void showAPListPage();

typedef struct
{
	char ssid[32];
}APItem;

static APItem apItems[20];

uint8_t keyboardfocus = LOGINUSER;

/* */
struct
{
	uint16_t selectIndex;
	uint16_t itemCount;
	uint16_t pageNumber;
	uint16_t pageMax;

} TableInfo;

typedef struct
{
	uint16_t topX;
	uint16_t topY;
	uint16_t bottomX;
	uint16_t bottomY;

}touchregion;

char* msg;

#define ROWCOUNT 5
const touchregion TABLEROWS[ROWCOUNT] = {
	{ 100, 90, 380, 120 },
	{ 100, 120, 380, 150 },
	{ 100, 150, 380, 180 },
	{ 100, 180, 380, 210 },
	{ 100, 210, 380, 240 },
};

// USER SETTING
const touchregion NEWUSEREDIT = { 130, 80, 350, 100 };
const touchregion NEWPASSEDIT = { 130, 130, 350, 150 };
const touchregion CONFIRMPASSEDIT = { 130, 180, 350, 200 };
const touchregion SAVEUSERBUTTON = { 160, 220, 320, 245 };


const touchregion MEMORYSTATUS = { 130, 80, 350, 105 };
const touchregion DATETIMEBUTTON = { 130, 140, 350, 165 };
const touchregion USERSETTING = { 130, 180, 350, 205 };
const touchregion RESETBUTTON = { 130, 220, 350, 245 };

const touchregion GRIDFIELD = { 100, 90, 380, 240 };
const touchregion TABLETITLE = { 100, 60, 380, 90 };

const touchregion PREVBUTTON = { 100, 245, 190, 270 };
const touchregion NEXTBUTTON = { 195, 245, 285, 270 };
const touchregion SAVEBUTTON = { 290, 245, 380, 270 };

const touchregion BACKBUTTON = { 5, 245, 95, 270 };
const touchregion HOMEBUTTON = { 385, 245, 475, 270 };

const touchregion TAREBUTTON = { 39, 112, 95, 150 };
const touchregion APSETTINGBUTTON = { 39, 160, 95, 198 };
const touchregion CLOUDBUTTON = { 39, 205, 95, 248 };
const touchregion SAVETOUSBBUTTON = { 144, 215, 239, 253 };
const touchregion SETTINGBUTOON = { 254, 215, 351, 253 };

const touchregion USERNAMEEDIT = { 230, 175, 325, 195 };
const touchregion PASSWORDEDIT = { 230, 205, 325, 225 };
const touchregion LOGINBUTTON = { 165, 235, 315, 260 };

const touchregion NOTIFYREGION = { 150, 180, 325, 225 };


const touchregion DATECOLUMN = { 100, 60, 170, 76 };
const touchregion TIMECOLUMN = { 170, 60, 240, 76 };
const touchregion ANIIDCOLUMN = { 240, 60, 325, 76 };
const touchregion WEIGHTCOLUMN = { 325, 60, 380, 76 };


static uint32_t tarems = 0;
static Statistic weightStatistic;

static bool tareing = false;

static String currLogName = "";
static uint8_t nPage = 0;

String usernameEdit = "";
String passwordEdit = "";

String newuserEdit = "";
String newpassEdit = "";
String confirmEdit = "";
String apPassword = "";


void displayWifiStatus(bool status)
{
	if (status)
	{
		//myFiles.loadBitmap(395, 55, 40, 40, "wifion.raw");
		myFiles.pan(395, 55, 40, 40, 0, 0, 40, 40, "wifion.raw");
	}
	else
	{
		//myFiles.loadBitmap(395, 55, 40, 40, "wifioff.raw");
		myFiles.pan(395, 55, 40, 40, 0, 0, 40, 40, "wifioff.raw");
	}
}

void displaySmallLogo()
{

	// logo image
	myGLCD.setBackColor(0, 0, 0);
	myGLCD.setFont(Retro8x16);
	myGLCD.setColor(VGA_BLACK);
	//myFiles.loadBitmap(10, 3, 60, 60, "TEST_S.raw");
	myFiles.pan(10, 3, 60, 60, 0, 0, 60, 60, "TEST_S.raw");

	// Logo text
	myGLCD.setBackColor(BACKGROUNDCOLOR);
	myGLCD.print("EazyScale", 7, 70);
}

void standbyEnter()
{

}

void standbyUpdate()
{

}

void StandbyExit()
{

}

void loginEnter()
{
	myGLCD.fillScr(BACKGROUNDCOLOR);
	//myFiles.loadBitmap(170, 5, 120, 120, "TEST.raw");
	myFiles.pan(170, 5, 120, 120, 0, 0, 120, 120, "TEST.raw", 0);
	displayTime();

	myGLCD.setFont(BigFont);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(BACKGROUNDCOLOR);
	myGLCD.print("MOUSE WORKS", 152, 130);
	myGLCD.print("MD,USA", 192, 150);

	myGLCD.setFont(Retro8x16);
	myGLCD.print("USERNAME:", 152, 178);
	myGLCD.print("PASSWORD:", 152, 208);
	myGLCD.drawRect(USERNAMEEDIT.topX - 1, USERNAMEEDIT.topY - 1, USERNAMEEDIT.bottomX + 1, USERNAMEEDIT.bottomY + 1);
	myGLCD.drawRect(PASSWORDEDIT.topX - 1, PASSWORDEDIT.topY - 1, PASSWORDEDIT.bottomX + 1, PASSWORDEDIT.bottomY + 1);

	myGLCD.setColor(EDITBACKCOLOR);
	myGLCD.fillRect(USERNAMEEDIT.topX, USERNAMEEDIT.topY, USERNAMEEDIT.bottomX, USERNAMEEDIT.bottomY);
	myGLCD.setColor(EDITBACKCOLOR);
	myGLCD.fillRect(PASSWORDEDIT.topX, PASSWORDEDIT.topY, PASSWORDEDIT.bottomX, PASSWORDEDIT.bottomY);

	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(EDITBACKCOLOR);
	myGLCD.print(usernameEdit, USERNAMEEDIT.topX + 3, USERNAMEEDIT.topY + 2);

	String password = "";
	Serial.println(passwordEdit.length());

	if (passwordEdit.length())
	{
		uint8_t nLen = passwordEdit.length();
		password = "";
		for (uint8_t idx = 0; idx < nLen; idx++)
			password += "*";
	}

	myGLCD.setFont(SmallFont);
	myGLCD.print(password, PASSWORDEDIT.topX + 3, PASSWORDEDIT.topY + 2);

	Serial.print(F("current password window value is "));
	Serial.println(passwordEdit);

	myGLCD.setColor(LOGINBUTCOLOR);
	myGLCD.fillRect(LOGINBUTTON.topX, LOGINBUTTON.topY, LOGINBUTTON.bottomX, LOGINBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(LOGINBUTTON.topX - 1, LOGINBUTTON.topY - 1, LOGINBUTTON.bottomX + 1, LOGINBUTTON.bottomY + 1);

	myGLCD.setFont(BigFont);

	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(LOGINBUTCOLOR);
	myGLCD.print("LOGIN", LOGINBUTTON.topX + 30, LOGINBUTTON.topY + 5);

	Serial.print(F("current user is "));
	Serial.println(setting.id);

	Serial.print(F("current password is "));
	Serial.println(setting.pass);
}


static uint32_t nLoginErrorTime = millis();
static bool bLoginError = false;
void loginError()
{
	myGLCD.setColor(REDCOLOR);
	myGLCD.fillRect(LOGINBUTTON.bottomX + 10, LOGINBUTTON.topY, LOGINBUTTON.bottomX + 130, LOGINBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(LOGINBUTTON.bottomX + 10 - 1, LOGINBUTTON.topY - 1, LOGINBUTTON.bottomX + 130 + 1, LOGINBUTTON.bottomY + 1);

	myGLCD.setFont(SmallFont);
	myGLCD.setColor(BACKGROUNDCOLOR);
	myGLCD.setBackColor(REDCOLOR);
	myGLCD.print("Login Invalid", LOGINBUTTON.bottomX + 20, LOGINBUTTON.topY + 5);

	nLoginErrorTime = millis();
	bLoginError = true;
	
}

void loginUpdate()
{

	int x, y;

	if ((millis() - nLoginErrorTime) > 1000 && bLoginError)
	{
		nLoginErrorTime = millis();
		bLoginError = false;
		
		myGLCD.setColor(BACKGROUNDCOLOR);
		myGLCD.drawRoundRect(LOGINBUTTON.bottomX + 10 - 1, LOGINBUTTON.topY - 1, LOGINBUTTON.bottomX + 130 + 1, LOGINBUTTON.bottomY + 1);
	}


	if (myTouch.dataAvailable())
	{
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if ((x >= LOGINBUTTON.topX && x <= LOGINBUTTON.bottomX) && (y >= LOGINBUTTON.topY && y <= LOGINBUTTON.bottomY))
		{
			// check password and user name

			if (usernameEdit != String(setting.id))
			{
				Serial.println(F("User name is invalid!"));
				loginError();
				return;
			}

			if (passwordEdit != String(setting.pass))
			{
				Serial.println(F("password is invalid!"));
				loginError();
				return;
			}

			/*
			if (mystatus.wifiStatus)
			{
				systemFSM.transitionTo(Home);
				// systemFSM.transitionTo(Upload);
			}
			else*/
			{
				DateTime now = rtc.now();
				// filename
				String datetime = "";
				datetime += String(now.year() % 100);
				if (now.month() < 10)
					datetime += F("0");
				datetime += String(now.month());
				if (now.day() < 10)
					datetime += F("0");
				datetime += String(now.day());

				uint8_t idx = 1;
				String fileName = "";

				while (true)
				{
					fileName = datetime;
					if (idx < 10)
						fileName += "0";
					fileName += String(idx);
					fileName += ".CSV";
					String path = String(setting.loggingDir) + fileName;
					if (!sdcard.exists(path.c_str()))
						break;

					idx++;
				}
				strcpy(mystatus.logFileName, fileName.c_str());
				Serial.print(F("logging file name is "));
				Serial.println(mystatus.logFileName);
				systemFSM.transitionTo(Home);
			}


		}
		else if ((x > USERNAMEEDIT.topX && x < USERNAMEEDIT.bottomX) && (y > USERNAMEEDIT.topY && y < USERNAMEEDIT.bottomY))
		{
			Serial.println(F("Touched Username"));
			keyboardfocus = LOGINUSER;
			usernameEdit = "";
			systemFSM.transitionTo(Keyboard);
		}
		else if ((x > PASSWORDEDIT.topX && x < PASSWORDEDIT.bottomX) && (y > PASSWORDEDIT.topY && y < PASSWORDEDIT.bottomY))
		{
			Serial.println(F("Touched Password"));
			keyboardfocus = LOGINPASS;
			passwordEdit = "";
			systemFSM.transitionTo(Keyboard);
		}
	}
}

void loginExit()
{

}

uint32_t nCount = 0;
double average = 0.0F;


void tareEnter()
{

	myGLCD.fillScr(BACKGROUNDCOLOR);
	displayTime();

	// Logo display from SD card or bmp data
	displaySmallLogo();

	myGLCD.setFont(BigFont);
	myGLCD.print("HOME", 208, 20);

	// 
	myGLCD.drawRoundRect(140, 50, 340, 80);
	myGLCD.setFont(SmallFont);
	myGLCD.print("WELCOME ", 180, 60);
	myGLCD.print(setting.id, 245, 60);

	myGLCD.setFont(BigFont);
	myGLCD.setColor(MAINCOLOR);
	myGLCD.print("Animal-ID:", 110, 95);

	myGLCD.setFont(Retro8x16);
	myGLCD.setColor(MAINCOLOR);
	myGLCD.print(mystatus.animolID, 270, 95);

	displayWeight(mystatus.weight);


	myGLCD.setFont(Retro8x16);
	myGLCD.setColor(TAREPUSHCOLOR);
	myGLCD.fillRect(TAREBUTTON.topX, TAREBUTTON.topY, TAREBUTTON.bottomX, TAREBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(TAREBUTTON.topX - 1, TAREBUTTON.topY - 1, TAREBUTTON.bottomX + 1, TAREBUTTON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(TAREPUSHCOLOR);
	myGLCD.print("TARE", TAREBUTTON.topX + 10, TAREBUTTON.topY + 15);

	myGLCD.setColor(WIFICOLOR);
	myGLCD.fillRect(APSETTINGBUTTON.topX, APSETTINGBUTTON.topY, APSETTINGBUTTON.bottomX, APSETTINGBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(APSETTINGBUTTON.topX - 1, APSETTINGBUTTON.topY - 1, APSETTINGBUTTON.bottomX + 1, APSETTINGBUTTON.bottomY + 1);
	myGLCD.setBackColor(WIFICOLOR);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.print("WiFi", APSETTINGBUTTON.topX + 5, APSETTINGBUTTON.topY + 15);

	myGLCD.setColor(CLOUDNORCOLOR);
	myGLCD.fillRect(CLOUDBUTTON.topX, CLOUDBUTTON.topY, CLOUDBUTTON.bottomX, CLOUDBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(CLOUDBUTTON.topX - 1, CLOUDBUTTON.topY - 1, CLOUDBUTTON.bottomX + 1, CLOUDBUTTON.bottomY + 1);
	myGLCD.setBackColor(CLOUDNORCOLOR);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.print("Upload", CLOUDBUTTON.topX + 5, CLOUDBUTTON.topY + 12);

	myGLCD.setColor(SAVENORCOLOR);
	myGLCD.fillRect(SAVETOUSBBUTTON.topX, SAVETOUSBBUTTON.topY, SAVETOUSBBUTTON.bottomX, SAVETOUSBBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(SAVETOUSBBUTTON.topX - 1, SAVETOUSBBUTTON.topY - 1, SAVETOUSBBUTTON.bottomX + 1, SAVETOUSBBUTTON.bottomY + 1);
	myGLCD.setBackColor(SAVENORCOLOR);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.print("SAVE TO USB", 148, 226);

	myGLCD.setColor(SETTINGCOLOR);
	myGLCD.fillRect(SETTINGBUTOON.topX, SETTINGBUTOON.topY, SETTINGBUTOON.bottomX, SETTINGBUTOON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(SETTINGBUTOON.topX - 1, SETTINGBUTOON.topY - 1, SETTINGBUTOON.bottomX + 1, SETTINGBUTOON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(SETTINGCOLOR);
	myGLCD.print("Setting", 274, 226);

	Serial.println(F("HX711 weiging started"));

	tarems = 0;
	weightStatistic.clear();

	nCount = 0;
	average = 0.0F;

}

void tareUpdate()
{
	const uint32_t weightInterval = 500;
	if (systemFSM.timeInCurrentState() >= 5000)
	{
		double nWeight = getWeight();
		average += nWeight;
		nCount++;

		if (nCount == 10)
		{
			nCount = 0;
			average /= 10;
			weightStatistic.add(average);
			//displayWeight(average);
			Serial.println(average);
			average = 0;
		}
	}

	if (systemFSM.timeInCurrentState() >= 10000)
	{
		mystatus.weight = weightStatistic.average();
		Serial.print(F("Weight is "));
		Serial.println(mystatus.weight);
		//displayWeight(mystatus.weight);
		writeLog(setting.id, mystatus.animolID, mystatus.weight);
		systemFSM.transitionTo(Home);
	}
}

void tareExit()
{

}

void statemanage()
{
	systemFSM.update();
}

void displayWeight(double weight)
{
	uint16_t m, g;
	myGLCD.setColor(BACKGROUNDCOLOR);
	myGLCD.fillRect(100, 130, 430, 195);
	myGLCD.setColor(MAINCOLOR);
	myGLCD.setFont(SixteenSegment40x60);

	if (weight < 0)
	{
		m = 110;
		g = 405;
	}
	else if (weight == 0.0F)
	{
		m = 155;
		g = 350;
	}
	else if (weight > 0.0F && weight < 10.0F)
	{
		m = 140;
		g = 370;
	}
	else if (weight >= 10.0F && weight < 100.0F)
	{
		m = 140;
		g = 370;
	}

	else if (weight >= 100.0F)
	{
		m = 125;
		g = 370;
	}

	myGLCD.setBackColor(BACKGROUNDCOLOR);
	myGLCD.printNumF(weight, 2, m, 130, '.', 1, 1);
	
	myGLCD.setColor(MAINCOLOR);
	myGLCD.setFont(Ubuntu);
	myGLCD.print("g", g, 160);
}


void homeEnter()
{
	myGLCD.fillScr(BACKGROUNDCOLOR);
	displayTime();

	displaySmallLogo();

	myGLCD.setFont(BigFont);
	myGLCD.print("HOME", 208, 20);

	// 
	myGLCD.drawRoundRect(140, 50, 340, 80);
	myGLCD.setFont(SmallFont);
	myGLCD.print("WELCOME ", 180, 60);
	myGLCD.print(setting.id, 245, 60);

	myGLCD.setFont(BigFont);
	myGLCD.setColor(MAINCOLOR);
	myGLCD.print("Animal-ID:", 110, 95);
	
	myGLCD.setFont(Retro8x16);
	myGLCD.setColor(MAINCOLOR);
	myGLCD.print(mystatus.animolID, 270, 95);
	displayWeight(mystatus.weight);

	
	myGLCD.setFont(Retro8x16);

	myGLCD.setColor(TARENORCOLOR);
	myGLCD.fillRect(TAREBUTTON.topX, TAREBUTTON.topY, TAREBUTTON.bottomX, TAREBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(TAREBUTTON.topX - 1, TAREBUTTON.topY - 1, TAREBUTTON.bottomX + 1, TAREBUTTON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(TARENORCOLOR);
	myGLCD.print("TARE", TAREBUTTON.topX + 10, TAREBUTTON.topY + 15);

	myGLCD.setColor(WIFICOLOR);
	myGLCD.fillRect(APSETTINGBUTTON.topX, APSETTINGBUTTON.topY, APSETTINGBUTTON.bottomX, APSETTINGBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(APSETTINGBUTTON.topX - 1, APSETTINGBUTTON.topY - 1, APSETTINGBUTTON.bottomX + 1, APSETTINGBUTTON.bottomY + 1);
	myGLCD.setBackColor(WIFICOLOR);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.print("WiFi", APSETTINGBUTTON.topX + 10, APSETTINGBUTTON.topY + 15);

	myGLCD.setColor(CLOUDNORCOLOR);
	myGLCD.fillRect(CLOUDBUTTON.topX, CLOUDBUTTON.topY, CLOUDBUTTON.bottomX, CLOUDBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(CLOUDBUTTON.topX - 1, CLOUDBUTTON.topY - 1, CLOUDBUTTON.bottomX + 1, CLOUDBUTTON.bottomY + 1);
	myGLCD.setBackColor(CLOUDNORCOLOR);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.print("Upload", CLOUDBUTTON.topX + 5, CLOUDBUTTON.topY + 12);

	myGLCD.setColor(SAVENORCOLOR);
	myGLCD.fillRect(SAVETOUSBBUTTON.topX, SAVETOUSBBUTTON.topY, SAVETOUSBBUTTON.bottomX, SAVETOUSBBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(SAVETOUSBBUTTON.topX - 1, SAVETOUSBBUTTON.topY - 1, SAVETOUSBBUTTON.bottomX + 1, SAVETOUSBBUTTON.bottomY + 1);
	myGLCD.setBackColor(SAVENORCOLOR);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.print("SAVE TO USB", 148, 226);

	myGLCD.setColor(SETTINGCOLOR);
	myGLCD.fillRect(SETTINGBUTOON.topX, SETTINGBUTOON.topY, SETTINGBUTOON.bottomX, SETTINGBUTOON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(SETTINGBUTOON.topX - 1, SETTINGBUTOON.topY - 1, SETTINGBUTOON.bottomX + 1, SETTINGBUTOON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(SETTINGCOLOR);
	myGLCD.print("Setting", 274, 226);
	
}

void homeUpdate()
{
	int x, y;
	if (myTouch.dataAvailable())
	{
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if ((x >= TAREBUTTON.topX && x < TAREBUTTON.bottomX) && (y > TAREBUTTON.topY && y < TAREBUTTON.bottomY))
		{
			Serial.println(F("CheckTag status"));
			systemFSM.transitionTo(CheckTag);
		}
		else if ((x > CLOUDBUTTON.topX && x < CLOUDBUTTON.bottomX) && (y > CLOUDBUTTON.topY && y < CLOUDBUTTON.bottomY))
		{
			Serial.println(F("cloud status"));
			systemFSM.transitionTo(Upload);
		}
		else if ((x > APSETTINGBUTTON.topX && x < APSETTINGBUTTON.bottomX) && (y > APSETTINGBUTTON.topY && y < APSETTINGBUTTON.bottomY))
		{
			Serial.println(F("cloud AP setting screen"));
			systemFSM.transitionTo(Cloud);
		}
		else if ((x > SAVETOUSBBUTTON.topX && x < SAVETOUSBBUTTON.bottomX) && (y > SAVETOUSBBUTTON.topY && y < SAVETOUSBBUTTON.bottomY))
		{
			Serial.println(F("Save to usb status"));
			systemFSM.transitionTo(SavetoUSB);
		}
		else if ((x > SETTINGBUTOON.topX && x < SETTINGBUTOON.bottomX) && (y > SETTINGBUTOON.topY && y < SETTINGBUTOON.bottomY))
		{
			Serial.println(F("Setting status"));
			systemFSM.transitionTo(Setting);
		}
	} // end if touch

}

void homeExit()
{

}

void settingEnter()
{
	//
	Serial.println(F("Setting screen!"));

	myGLCD.fillScr(BACKGROUNDCOLOR);
	displayTime();

	displaySmallLogo();

	myGLCD.setFont(BigFont);
	myGLCD.print("SETTINGS", 180, 20);

	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(BACKBUTTON.topX, BACKBUTTON.topY, BACKBUTTON.bottomX, BACKBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(BACKBUTTON.topX - 1, BACKBUTTON.topY - 1, BACKBUTTON.bottomX + 1, BACKBUTTON.bottomY + 1);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.print("BACK", BACKBUTTON.topX + 15, BACKBUTTON.topY + 5);

	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(HOMEBUTTON.topX, HOMEBUTTON.topY, HOMEBUTTON.bottomX, HOMEBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(HOMEBUTTON.topX - 1, HOMEBUTTON.topY - 1, HOMEBUTTON.bottomX + 1, HOMEBUTTON.bottomY + 1);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.print("HOME", HOMEBUTTON.topX + 15, HOMEBUTTON.topY + 5);

	double freeMB = getFreeCapacity();
	uint8_t freePerc = getSDFreePerc();

	myGLCD.setColor(GREENCOLOR);
	myGLCD.fillRect(MEMORYSTATUS.topX, MEMORYSTATUS.topY, MEMORYSTATUS.bottomX, MEMORYSTATUS.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(MEMORYSTATUS.topX - 1, MEMORYSTATUS.topY - 1, MEMORYSTATUS.bottomX + 1, MEMORYSTATUS.bottomY + 1);

	myGLCD.setBackColor(GREENCOLOR);
	myGLCD.setColor(BACKGROUNDCOLOR);
	myGLCD.setFont(BigFont);
	myGLCD.print("Memory Status", MEMORYSTATUS.topX + 5, MEMORYSTATUS.topY + 5);

	myGLCD.setBackColor(BACKGROUNDCOLOR);
	myGLCD.setColor(REDCOLOR);
	myGLCD.setFont(Retro8x16);
	myGLCD.print(String(freeMB, 1) + "MB/(" + String(freePerc) + "%) free", MEMORYSTATUS.topX + 25, MEMORYSTATUS.bottomY + 10);

	myGLCD.setColor(GREENCOLOR);
	myGLCD.fillRect(DATETIMEBUTTON.topX, DATETIMEBUTTON.topY, DATETIMEBUTTON.bottomX, DATETIMEBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(DATETIMEBUTTON.topX - 1, DATETIMEBUTTON.topY - 1, DATETIMEBUTTON.bottomX + 1, DATETIMEBUTTON.bottomY + 1);

	myGLCD.setBackColor(GREENCOLOR);
	myGLCD.setColor(BACKGROUNDCOLOR);
	myGLCD.setFont(BigFont);
	myGLCD.print("DATE & TIME", DATETIMEBUTTON.topX + 25, DATETIMEBUTTON.topY + 5);

	myGLCD.setColor(GREENCOLOR);
	myGLCD.fillRect(USERSETTING.topX, USERSETTING.topY, USERSETTING.bottomX, USERSETTING.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(USERSETTING.topX - 1, USERSETTING.topY - 1, USERSETTING.bottomX + 1, USERSETTING.bottomY + 1);

	myGLCD.setBackColor(GREENCOLOR);
	myGLCD.setColor(BACKGROUNDCOLOR);
	myGLCD.setFont(BigFont);
	myGLCD.print("USER SETTING", USERSETTING.topX + 15, USERSETTING.topY + 5);


	myGLCD.setColor(YELLOWCOLOR);
	myGLCD.fillRect(RESETBUTTON.topX, RESETBUTTON.topY, RESETBUTTON.bottomX, RESETBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(RESETBUTTON.topX - 1, RESETBUTTON.topY - 1, RESETBUTTON.bottomX + 1, RESETBUTTON.bottomY + 1);

	myGLCD.setBackColor(YELLOWCOLOR);
	myGLCD.setColor(BACKGROUNDCOLOR);
	myGLCD.setFont(BigFont);
	myGLCD.print("RESET", RESETBUTTON.topX + 70, RESETBUTTON.topY + 5);

}

void settingUpdate()
{
	int x, y;

	if (myTouch.dataAvailable())
	{
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if ((x > HOMEBUTTON.topX && x < HOMEBUTTON.bottomX) && (y > HOMEBUTTON.topY && y < HOMEBUTTON.bottomY))
		{
			Serial.println(F("Home button is touched"));
			systemFSM.transitionTo(Home);
		}
		else if ((x > RESETBUTTON.topX && x < RESETBUTTON.bottomX) && (y > RESETBUTTON.topY && y < RESETBUTTON.bottomY))
		{
			Serial.println(F("Reset button is touched"));

			Serial.println(F("Default user setting"));
			strcpy(setting.id, setting.defaultid);
			strcpy(setting.pass, setting.defaultpass);
			saveConfig();

			sdcard.chdir(setting.loggingDir, true);
			Serial.print(F("logging directory "));
			Serial.println(setting.loggingDir);

			while (getLogFileCount() > 0)
			{
				sdcard.vwd()->rewind();

				if (!sdfile.openNext(sdcard.vwd(), O_READ))
				{
					Serial.println(F("Failed"));
				}
				else
				{
					// 
					char filename[32];
					sdfile.getName(filename, 32);
					sdfile.close();
					String logPath = setting.loggingDir;
					logPath += String(filename);
					sdcard.remove(logPath.c_str());

					Serial.print(F("delete logfile "));
					Serial.println(filename);
				}
			}

			systemFSM.transitionTo(Login);
		}
		else if ((x > BACKBUTTON.topX && x < BACKBUTTON.bottomX) && (y > BACKBUTTON.topY && y < BACKBUTTON.bottomY))
		{
			Serial.println(F("Back button is touched"));
			systemFSM.transitionTo(Home);
		}
		else if ((x >= USERSETTING.topX && x <= USERSETTING.bottomX) && (y >= USERSETTING.topY && y <= USERSETTING.bottomY))
		{
			Serial.println(F("Touched user setting"));
			systemFSM.transitionTo(UserChange);
		}
		else if ((x >= DATETIMEBUTTON.topX && x <= DATETIMEBUTTON.bottomX) && (y >= DATETIMEBUTTON.topY && y <= DATETIMEBUTTON.bottomY))
		{
			Serial.println(F("Touched datetime button"));
			setClock();
			settingEnter();
		}
		else if ((x >= DATETIMEBUTTON.topX && x <= DATETIMEBUTTON.bottomX) && (y >= DATETIMEBUTTON.topY && y <= DATETIMEBUTTON.bottomY))
		{
			Serial.println(F("Touched datetime button"));
			setClock();
			settingEnter();
		}
	}
}

void settingExit()
{

}

void cloudEnter()
{
	//
	Serial.println(F("Cloud Screen"));

	myGLCD.fillScr(BACKGROUNDCOLOR);
	displayTime();

	displaySmallLogo();

	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(BACKBUTTON.topX, BACKBUTTON.topY, BACKBUTTON.bottomX, BACKBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(BACKBUTTON.topX - 1, BACKBUTTON.topY - 1, BACKBUTTON.bottomX + 1, BACKBUTTON.bottomY + 1);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.print("BACK", BACKBUTTON.topX + 5, BACKBUTTON.topY + 5);

	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(HOMEBUTTON.topX, HOMEBUTTON.topY, HOMEBUTTON.bottomX, HOMEBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(HOMEBUTTON.topX - 1, HOMEBUTTON.topY - 1, HOMEBUTTON.bottomX + 1, HOMEBUTTON.bottomY + 1);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.print("SCAN", HOMEBUTTON.topX + 5, HOMEBUTTON.topY + 5);
	//myGLCD.print("HOME", HOMEBUTTON.topX + 5, HOMEBUTTON.topY + 5);

	myGLCD.setFont(BigFont);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(BACKGROUNDCOLOR);
	myGLCD.print("CHOOSE & CONNECT", 110, 30);

	scanAP();
	showAPListPage();

}

void cloudUpdate()
{
	int x, y;
	if (myTouch.dataAvailable())
	{
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if ((x > HOMEBUTTON.topX && x < HOMEBUTTON.bottomX) && (y > HOMEBUTTON.topY && y < HOMEBUTTON.bottomY))
		{
			scanAP();
			showAPListPage();
		}
		else if ((x > BACKBUTTON.topX && x < BACKBUTTON.bottomX) && (y > BACKBUTTON.topY && y < BACKBUTTON.bottomY))
		{
			systemFSM.transitionTo(Home);
		}

		for (uint8_t idx = 0; idx < ROWCOUNT; idx++)
		{
			if ((x > TABLEROWS[idx].topX && x < TABLEROWS[idx].bottomX) && (y > TABLEROWS[idx].topY && y < TABLEROWS[idx].bottomY))
			{
				if ((idx + TableInfo.pageNumber * ROWCOUNT) > TableInfo.itemCount - 1)
					break;
				Serial.print(F("Table touched, row"));
				Serial.print(idx);
				TableInfo.selectIndex = idx;
				Serial.println(F(" touched!"));
				showAPListPage();
				break;
			}
		}

		if ((x >= PREVBUTTON.topX && x <= PREVBUTTON.bottomX) && (y >= PREVBUTTON.topY && y <= PREVBUTTON.bottomY))
		{
			Serial.println(F("prev page button touched!"));
			if (TableInfo.pageNumber > 0)
			{
				TableInfo.pageNumber--;
				showAPListPage();
			}
		}

		if ((x >= NEXTBUTTON.topX && x <= NEXTBUTTON.bottomX) && (y >= NEXTBUTTON.topY && y <= NEXTBUTTON.bottomY))
		{
			Serial.println(F("next page button touched!"));
			if (TableInfo.pageNumber < TableInfo.pageMax)
			{
				TableInfo.pageNumber++;
				showAPListPage();
			}
		}

		if ((x >= SAVEBUTTON.topX && x <= SAVEBUTTON.bottomX) && (y >= SAVEBUTTON.topY && y <= SAVEBUTTON.bottomY))
		{
			Serial.println(F("open button touched!"));
			uint16_t index = TableInfo.pageNumber * ROWCOUNT + TableInfo.selectIndex;
			
			/* Keyboard */
			myGLCD.InitLCD(LANDSCAPE);
			myGLCD.clrScr();
			myGLCD.setFont(SmallFont);

			myTouch.InitTouch(LANDSCAPE);
			myTouch.setPrecision(PREC_LOW);

			myTFT.ExtSetup();
			myGLCD.fillScr(0, 0, 0);
			myGLCD.drawRect(1, 1, 479, 271);
			myTFT.SetupMobileKB(); //NEEDED TO WORK!

			String apPassword = "";
			String apSSID = String(apItems[index].ssid);
			while (1)
			{
				msg = myTFT.Mobile_KeyBoard(BLUE);
				if (msg != NULL)
				{
					apPassword = String(msg);
					break;
				}
			}


			myGLCD.fillScr(BACKGROUNDCOLOR);
			displayTime();

			displaySmallLogo();

			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(BACKBUTTON.topX, BACKBUTTON.topY, BACKBUTTON.bottomX, BACKBUTTON.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(BACKBUTTON.topX - 1, BACKBUTTON.topY - 1, BACKBUTTON.bottomX + 1, BACKBUTTON.bottomY + 1);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.print("BACK", BACKBUTTON.topX + 5, BACKBUTTON.topY + 5);

			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(HOMEBUTTON.topX, HOMEBUTTON.topY, HOMEBUTTON.bottomX, HOMEBUTTON.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(HOMEBUTTON.topX - 1, HOMEBUTTON.topY - 1, HOMEBUTTON.bottomX + 1, HOMEBUTTON.bottomY + 1);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.print("SCAN", HOMEBUTTON.topX + 5, HOMEBUTTON.topY + 5);
			//myGLCD.print("HOME", HOMEBUTTON.topX + 5, HOMEBUTTON.topY + 5);

			myGLCD.setFont(BigFont);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.setBackColor(BACKGROUNDCOLOR);
			myGLCD.print("CHOOSE & CONNECT", 110, 30);

			showAPListPage();

			// All button disabled
			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(SAVEBUTTON.topX - 1, SAVEBUTTON.topY - 1, SAVEBUTTON.bottomX + 1, SAVEBUTTON.bottomY + 1);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(SAVEBUTTON.topX, SAVEBUTTON.topY, SAVEBUTTON.bottomX, SAVEBUTTON.bottomY);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.setColor(BUTTONDISCOLOR);
			myGLCD.print("SAVE", SAVEBUTTON.topX + 2, SAVEBUTTON.topY + 1);

			// Prev button
			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(PREVBUTTON.topX - 1, PREVBUTTON.topY - 1, PREVBUTTON.bottomX + 1, PREVBUTTON.bottomY + 1);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(PREVBUTTON.topX, PREVBUTTON.topY, PREVBUTTON.bottomX, PREVBUTTON.bottomY);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.setColor(BUTTONDISCOLOR);
			myGLCD.print("PREV", PREVBUTTON.topX + 2, PREVBUTTON.topY + 1);

			// Next button
			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(NEXTBUTTON.topX - 1, NEXTBUTTON.topY - 1, NEXTBUTTON.bottomX + 1, NEXTBUTTON.bottomY + 1);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(NEXTBUTTON.topX, NEXTBUTTON.topY, NEXTBUTTON.bottomX, NEXTBUTTON.bottomY);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.setColor(BUTTONDISCOLOR);
			myGLCD.print("NEXT", NEXTBUTTON.topX + 2, NEXTBUTTON.topY + 1);

			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(BACKBUTTON.topX, BACKBUTTON.topY, BACKBUTTON.bottomX, BACKBUTTON.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(BACKBUTTON.topX - 1, BACKBUTTON.topY - 1, BACKBUTTON.bottomX + 1, BACKBUTTON.bottomY + 1);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.setColor(BUTTONDISCOLOR);
			myGLCD.print("BACK", BACKBUTTON.topX + 5, BACKBUTTON.topY + 5);

			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(HOMEBUTTON.topX, HOMEBUTTON.topY, HOMEBUTTON.bottomX, HOMEBUTTON.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(HOMEBUTTON.topX - 1, HOMEBUTTON.topY - 1, HOMEBUTTON.bottomX + 1, HOMEBUTTON.bottomY + 1);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.setColor(BUTTONDISCOLOR);
			myGLCD.print("SCAN", HOMEBUTTON.topX + 5, HOMEBUTTON.topY + 5);

			displaySavingProg("Connecting");

			Serial.print(F("AP ssid is "));
			Serial.print(apSSID);
			Serial.print(F(", password is "));
			Serial.println(apPassword);

			bool result = setNewAP(apSSID, apPassword);
			mystatus.wifiStatus = result;

			showAPListPage();

			if (result)
				displaySavingProg("Connected");
			else
				displaySavingProg("Failed");

			mystatus.wifiStatus = result;

			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(BACKBUTTON.topX, BACKBUTTON.topY, BACKBUTTON.bottomX, BACKBUTTON.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(BACKBUTTON.topX - 1, BACKBUTTON.topY - 1, BACKBUTTON.bottomX + 1, BACKBUTTON.bottomY + 1);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.print("BACK", BACKBUTTON.topX + 5, BACKBUTTON.topY + 5);

			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(HOMEBUTTON.topX, HOMEBUTTON.topY, HOMEBUTTON.bottomX, HOMEBUTTON.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(HOMEBUTTON.topX - 1, HOMEBUTTON.topY - 1, HOMEBUTTON.bottomX + 1, HOMEBUTTON.bottomY + 1);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.print("HOME", HOMEBUTTON.topX + 5, HOMEBUTTON.topY + 5);
		}
	}
}

void cloudExit()
{

}

int freeRam() {
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void saveUSBEnter()
{
	//
	Serial.println(F("Save to USB"));

	myGLCD.fillScr(BACKGROUNDCOLOR);
	displayTime();

	displaySmallLogo();

	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(BACKBUTTON.topX, BACKBUTTON.topY, BACKBUTTON.bottomX, BACKBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(BACKBUTTON.topX - 1, BACKBUTTON.topY - 1, BACKBUTTON.bottomX + 1, BACKBUTTON.bottomY + 1);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.print("BACK", BACKBUTTON.topX + 5, BACKBUTTON.topY + 5);

	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(HOMEBUTTON.topX, HOMEBUTTON.topY, HOMEBUTTON.bottomX, HOMEBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(HOMEBUTTON.topX - 1, HOMEBUTTON.topY - 1, HOMEBUTTON.bottomX + 1, HOMEBUTTON.bottomY + 1);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.print("HOME", HOMEBUTTON.topX + 5, HOMEBUTTON.topY + 5);

	myGLCD.setFont(BigFont);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(BACKGROUNDCOLOR);
	myGLCD.print("DATA SAVED ON", 120, 20);
	myGLCD.print("DEVICE", 180, 40);

	initTable();
}

void showPage()
{
	// 
	myGLCD.setColor(GRIDBACKCOLOR);
	myGLCD.fillRect(GRIDFIELD.topX, GRIDFIELD.topY, GRIDFIELD.bottomX, GRIDFIELD.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRect(GRIDFIELD.topX - 1, GRIDFIELD.topY - 1, GRIDFIELD.bottomX + 1, GRIDFIELD.bottomY + 1);

	TableInfo.itemCount = getLogFileCount();
	if (TableInfo.itemCount > 0)
		TableInfo.pageMax = (TableInfo.itemCount / ROWCOUNT + ((TableInfo.itemCount % ROWCOUNT > 0) ? 1 : 0)) - 1;
	else
		TableInfo.pageMax = 0;
	Serial.print(F("Page max number is "));
	Serial.println(TableInfo.pageMax);
	sdcard.chdir(setting.loggingDir, true);
	myGLCD.setFont(Retro8x16);
	
	for (uint8_t idx = 0; idx < ((TableInfo.itemCount > ROWCOUNT) ? ROWCOUNT : TableInfo.itemCount); idx++)
	{
		uint16_t fileindex = TableInfo.itemCount - (TableInfo.pageNumber * ROWCOUNT + idx);
		if (fileindex == 0)
			break;
		sdcard.vwd()->rewind();
		if (fileindex > 0)
		{
			for (uint16_t idx2 = 0; idx2 < fileindex - 1; idx2++)
			{
				sdfile.openNext(sdcard.vwd(), O_READ);
				sdfile.close();
			}
		}
		sdfile.openNext(sdcard.vwd(), O_READ);

		char name[20];
		sdfile.getName(name, 20);

		if (TableInfo.selectIndex == idx)
		{
			myGLCD.setColor(GRIDSELCOLOR);
			myGLCD.fillRect(TABLEROWS[idx].topX, TABLEROWS[idx].topY, TABLEROWS[idx].bottomX, TABLEROWS[idx].bottomY);
			myGLCD.setBackColor(GRIDSELCOLOR);
		}
		else
			myGLCD.setBackColor(GRIDBACKCOLOR);
		myGLCD.setColor(VGA_BLACK);
		myGLCD.print(name, TABLEROWS[idx].topX + 4, TABLEROWS[idx].topY + 8);

		sdfile.close();
	}

	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(SAVEBUTTON.topX - 1, SAVEBUTTON.topY - 1, SAVEBUTTON.bottomX + 1, SAVEBUTTON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(SAVEBUTTON.topX, SAVEBUTTON.topY, SAVEBUTTON.bottomX, SAVEBUTTON.bottomY);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	
	// Disable / Enable
	if (TableInfo.itemCount > 0)
		myGLCD.setColor(VGA_BLACK);
	else
		myGLCD.setColor(BUTTONDISCOLOR);
	
	myGLCD.print("SAVE", SAVEBUTTON.topX + 2, SAVEBUTTON.topY + 1);

	// Prev button
	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(PREVBUTTON.topX - 1, PREVBUTTON.topY - 1, PREVBUTTON.bottomX + 1, PREVBUTTON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(PREVBUTTON.topX, PREVBUTTON.topY, PREVBUTTON.bottomX, PREVBUTTON.bottomY);
	myGLCD.setBackColor(GRAYBUTCOLOR);

	if ((TableInfo.itemCount <= ROWCOUNT) || (TableInfo.itemCount > ROWCOUNT && TableInfo.pageNumber == 0))
		myGLCD.setColor(BUTTONDISCOLOR);
	else
		myGLCD.setColor(VGA_BLACK);

	myGLCD.print("PREV", PREVBUTTON.topX + 2, PREVBUTTON.topY + 1);

	// Next button
	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(NEXTBUTTON.topX - 1, NEXTBUTTON.topY - 1, NEXTBUTTON.bottomX + 1, NEXTBUTTON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(NEXTBUTTON.topX, NEXTBUTTON.topY, NEXTBUTTON.bottomX, NEXTBUTTON.bottomY);
	myGLCD.setBackColor(GRAYBUTCOLOR);

	if ((TableInfo.itemCount <= ROWCOUNT) || (TableInfo.itemCount > ROWCOUNT && TableInfo.pageNumber == TableInfo.pageMax))
		myGLCD.setColor(BUTTONDISCOLOR);
	else
		myGLCD.setColor(VGA_BLACK);
	myGLCD.print("NEXT", NEXTBUTTON.topX + 2, NEXTBUTTON.topY + 1);

	myGLCD.setFont(Retro8x16);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(TABLETITLE.topX, TABLETITLE.topY, TABLETITLE.bottomX, TABLETITLE.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRect(TABLETITLE.topX - 1, TABLETITLE.topY - 1, TABLETITLE.bottomX + 1, TABLETITLE.bottomY + 1);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.print("Saved files(newer first)", 105, 66);

}

void scanAP()
{
	String strAPList = wifi.getAPList();
	Serial.println(strAPList);

	uint8_t idx = 0;
	int16_t startPoint, endPoint;
	endPoint = 0;
	for (idx = 0; idx < 20; idx++)
	{
		startPoint = strAPList.indexOf(",\"", endPoint);
		if (startPoint == -1)
			break;
		endPoint = strAPList.indexOf("\"", startPoint + 3);
		if (endPoint == -1)
			break;
		String ssid = strAPList.substring(startPoint + 2, endPoint);
		endPoint = strAPList.indexOf(")", endPoint + 1);
		strcpy(apItems[idx].ssid, ssid.c_str());
	}
	
	TableInfo.itemCount = idx;
	TableInfo.pageNumber = 0;
	TableInfo.selectIndex = 0;

	if (TableInfo.itemCount > 0)
	{
		TableInfo.pageMax = (TableInfo.itemCount / ROWCOUNT + ((TableInfo.itemCount % ROWCOUNT > 0) ? 1 : 0)) - 1;
	}
	else
	{
		TableInfo.pageMax = 0;
	}

	Serial.print(F("Page max number is "));
	Serial.println(TableInfo.pageMax);
	Serial.print(F("AP count is "));
	Serial.println(TableInfo.itemCount);
}

void showAPListPage()
{
	// 
	myGLCD.setColor(GRIDBACKCOLOR);
	myGLCD.fillRect(GRIDFIELD.topX, GRIDFIELD.topY, GRIDFIELD.bottomX, GRIDFIELD.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRect(GRIDFIELD.topX - 1, GRIDFIELD.topY - 1, GRIDFIELD.bottomX + 1, GRIDFIELD.bottomY + 1);

	myGLCD.setFont(Retro8x16);
	for (uint8_t idx = 0; idx < ((TableInfo.itemCount > ROWCOUNT) ? ROWCOUNT : TableInfo.itemCount); idx++)
	{
		uint16_t index = idx + TableInfo.pageNumber * ROWCOUNT;
		if (idx == TableInfo.itemCount)
			break;
		if (TableInfo.selectIndex == idx)
		{
			myGLCD.setColor(GRIDSELCOLOR);
			myGLCD.fillRect(TABLEROWS[idx].topX, TABLEROWS[idx].topY, TABLEROWS[idx].bottomX, TABLEROWS[idx].bottomY);
			myGLCD.setBackColor(GRIDSELCOLOR);
		}
		else
			myGLCD.setBackColor(GRIDBACKCOLOR);

		if (mystatus.wifiStatus)
		{
			String currssid = setting.apSSID;
			if (currssid == apItems[index].ssid)
			{
				myGLCD.setFont(Retro8x16);
				myGLCD.setBackColor(GRIDSELCOLOR);
				myGLCD.setColor(VGA_BLACK);
				myGLCD.print("Connected", TABLEROWS[idx].bottomX - 100, TABLEROWS[idx].topY + 8);
			}
		}


		myGLCD.setColor(VGA_BLACK);
		myGLCD.print(apItems[idx + TableInfo.pageNumber * ROWCOUNT].ssid, TABLEROWS[idx].topX + 4, TABLEROWS[idx].topY + 8);
	}

	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(SAVEBUTTON.topX - 1, SAVEBUTTON.topY - 1, SAVEBUTTON.bottomX + 1, SAVEBUTTON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(SAVEBUTTON.topX, SAVEBUTTON.topY, SAVEBUTTON.bottomX, SAVEBUTTON.bottomY);
	myGLCD.setBackColor(GRAYBUTCOLOR);

	// Disable / Enable
	if (TableInfo.itemCount > 0)
		myGLCD.setColor(VGA_BLACK);
	else
		myGLCD.setColor(BUTTONDISCOLOR);

	myGLCD.print("OPEN", SAVEBUTTON.topX + 10, SAVEBUTTON.topY + 4);

	// Prev button
	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(PREVBUTTON.topX - 1, PREVBUTTON.topY - 1, PREVBUTTON.bottomX + 1, PREVBUTTON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(PREVBUTTON.topX, PREVBUTTON.topY, PREVBUTTON.bottomX, PREVBUTTON.bottomY);
	myGLCD.setBackColor(GRAYBUTCOLOR);

	if ((TableInfo.itemCount <= ROWCOUNT) || (TableInfo.itemCount > ROWCOUNT && TableInfo.pageNumber == 0))
		myGLCD.setColor(BUTTONDISCOLOR);
	else
		myGLCD.setColor(VGA_BLACK);

	myGLCD.print("PREV", PREVBUTTON.topX + 10, PREVBUTTON.topY + 4);

	// Next button
	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(NEXTBUTTON.topX - 1, NEXTBUTTON.topY - 1, NEXTBUTTON.bottomX + 1, NEXTBUTTON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(NEXTBUTTON.topX, NEXTBUTTON.topY, NEXTBUTTON.bottomX, NEXTBUTTON.bottomY);
	myGLCD.setBackColor(GRAYBUTCOLOR);

	if ((TableInfo.itemCount <= ROWCOUNT) || (TableInfo.itemCount > ROWCOUNT && TableInfo.pageNumber == TableInfo.pageMax))
		myGLCD.setColor(BUTTONDISCOLOR);
	else
		myGLCD.setColor(VGA_BLACK);
	myGLCD.print("NEXT", NEXTBUTTON.topX + 10, NEXTBUTTON.topY + 4);

	myGLCD.setFont(Retro8x16);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(TABLETITLE.topX, TABLETITLE.topY, TABLETITLE.bottomX, TABLETITLE.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRect(TABLETITLE.topX - 1, TABLETITLE.topY - 1, TABLETITLE.bottomX + 1, TABLETITLE.bottomY + 1);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.print("WiFi AP List", 105, 66);
}



void initTable()
{
	TableInfo.selectIndex = 0;
	TableInfo.pageNumber = 0;
	showPage();
}

void displaySavingProg(String status)
{
	uint8_t selIndex = TableInfo.selectIndex;

	myGLCD.setFont(Retro8x16);
	myGLCD.setBackColor(GRIDSELCOLOR);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.print(status, TABLEROWS[selIndex].bottomX - 100, TABLEROWS[selIndex].topY + 8);
}

void saveUSBUpdate()
{
	int x, y;
	if (myTouch.dataAvailable())
	{
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if ((x > HOMEBUTTON.topX && x < HOMEBUTTON.bottomX) && (y > HOMEBUTTON.topY && y < HOMEBUTTON.bottomY))
		{
			systemFSM.transitionTo(Home);
		}
		else if ((x > BACKBUTTON.topX && x < BACKBUTTON.bottomX) && (y > BACKBUTTON.topY && y < BACKBUTTON.bottomY))
		{
			systemFSM.transitionTo(Home);
		}

		for (uint8_t idx = 0; idx < ROWCOUNT; idx++)
		{
			if ((idx + TableInfo.pageNumber * ROWCOUNT) > TableInfo.itemCount - 1)
				break;

			if ((x > TABLEROWS[idx].topX && x < TABLEROWS[idx].bottomX) && (y > TABLEROWS[idx].topY && y < TABLEROWS[idx].bottomY))
			{
				Serial.print(F("Table touched, row"));
				Serial.print(idx);
				TableInfo.selectIndex = idx;
				Serial.println(F(" touched!"));
				showPage();
				break;
			}
		}

		if ((x >= PREVBUTTON.topX && x <= PREVBUTTON.bottomX) && (y >= PREVBUTTON.topY && y <= PREVBUTTON.bottomY))
		{
			Serial.println(F("prev page button touched!"));
			if (TableInfo.pageNumber > 0)
			{
				TableInfo.pageNumber--;
				showPage();
			}
		}
		
		if ((x >= NEXTBUTTON.topX && x <= NEXTBUTTON.bottomX) && (y >= NEXTBUTTON.topY && y <= NEXTBUTTON.bottomY))
		{
			Serial.println(F("next page button touched!"));
			if (TableInfo.pageNumber < TableInfo.pageMax)
			{
				TableInfo.pageNumber++;
				showPage();
			}
		}

		if ((x >= SAVEBUTTON.topX && x <= SAVEBUTTON.bottomX) && (y >= SAVEBUTTON.topY && y <= SAVEBUTTON.bottomY))
		{
			Serial.println("save button touched!");
			uint16_t fileindex = TableInfo.itemCount - (TableInfo.pageNumber * ROWCOUNT + TableInfo.selectIndex);
			
			// All button disabled

			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(SAVEBUTTON.topX - 1, SAVEBUTTON.topY - 1, SAVEBUTTON.bottomX + 1, SAVEBUTTON.bottomY + 1);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(SAVEBUTTON.topX, SAVEBUTTON.topY, SAVEBUTTON.bottomX, SAVEBUTTON.bottomY);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.setColor(BUTTONDISCOLOR);
			myGLCD.print("SAVE", SAVEBUTTON.topX + 2, SAVEBUTTON.topY + 1);

			// Prev button
			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(PREVBUTTON.topX - 1, PREVBUTTON.topY - 1, PREVBUTTON.bottomX + 1, PREVBUTTON.bottomY + 1);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(PREVBUTTON.topX, PREVBUTTON.topY, PREVBUTTON.bottomX, PREVBUTTON.bottomY);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.setColor(BUTTONDISCOLOR);
			myGLCD.print("PREV", PREVBUTTON.topX + 2, PREVBUTTON.topY + 1);

			// Next button
			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(NEXTBUTTON.topX - 1, NEXTBUTTON.topY - 1, NEXTBUTTON.bottomX + 1, NEXTBUTTON.bottomY + 1);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(NEXTBUTTON.topX, NEXTBUTTON.topY, NEXTBUTTON.bottomX, NEXTBUTTON.bottomY);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.setColor(BUTTONDISCOLOR);
			myGLCD.print("NEXT", NEXTBUTTON.topX + 2, NEXTBUTTON.topY + 1);

			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(BACKBUTTON.topX, BACKBUTTON.topY, BACKBUTTON.bottomX, BACKBUTTON.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(BACKBUTTON.topX - 1, BACKBUTTON.topY - 1, BACKBUTTON.bottomX + 1, BACKBUTTON.bottomY + 1);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.setColor(BUTTONDISCOLOR);
			myGLCD.print("BACK", BACKBUTTON.topX + 5, BACKBUTTON.topY + 5);

			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(HOMEBUTTON.topX, HOMEBUTTON.topY, HOMEBUTTON.bottomX, HOMEBUTTON.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(HOMEBUTTON.topX - 1, HOMEBUTTON.topY - 1, HOMEBUTTON.bottomX + 1, HOMEBUTTON.bottomY + 1);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.setColor(BUTTONDISCOLOR);
			myGLCD.print("HOME", HOMEBUTTON.topX + 5, HOMEBUTTON.topY + 5);

			if (savefiletoUSB(fileindex))
			{
				displaySavingProg("completed!");
				TableInfo.selectIndex = 0;
				TableInfo.pageNumber = 0;
			}
			else
			{
				displaySavingProg("Failed!");
			}
			delay(1000);
			showPage();

			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(BACKBUTTON.topX, BACKBUTTON.topY, BACKBUTTON.bottomX, BACKBUTTON.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(BACKBUTTON.topX - 1, BACKBUTTON.topY - 1, BACKBUTTON.bottomX + 1, BACKBUTTON.bottomY + 1);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.print("BACK", BACKBUTTON.topX + 5, BACKBUTTON.topY + 5);

			myGLCD.setFont(BigFont);
			myGLCD.setColor(GRAYBUTCOLOR);
			myGLCD.fillRect(HOMEBUTTON.topX, HOMEBUTTON.topY, HOMEBUTTON.bottomX, HOMEBUTTON.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(HOMEBUTTON.topX - 1, HOMEBUTTON.topY - 1, HOMEBUTTON.bottomX + 1, HOMEBUTTON.bottomY + 1);
			myGLCD.setBackColor(GRAYBUTCOLOR);
			myGLCD.print("HOME", HOMEBUTTON.topX + 5, HOMEBUTTON.topY + 5);
		}
	}
}

bool savefiletoUSB(uint16_t fileindex)
{
	
	Serial.println(F("Data Processing is started!"));
	sdcard.vwd()->rewind();
	Serial.println(fileindex);
	if (fileindex > 0)
	{
		for (uint16_t idx2 = 0; idx2 < fileindex - 1; idx2++)
		{
			sdfile.openNext(sdcard.vwd(), O_READ);
			sdfile.close();
		}
	}

	if (!sdfile.openNext(sdcard.vwd(), O_READ))
	{
		Serial.println(F("Failed"));
		return false;
	}

	// 
	char filename[32];
	sdfile.getName(filename, 32);
	sdfile.close();

	File logFile;
	String logPath = setting.loggingDir;
	logPath += String(filename);

	logFile = sdcard.open(logPath);
	if (!logFile)
		return false;

	uint32_t fileSize = logFile.fileSize();

	// USB check
	if (checkConnection(0x01))
	{
		if (logFile.available())
		{
			String strRow = logFile.readStringUntil('\n');
			writeFile(filename, strRow);
			delay(3000);
			
			writeFile(filename, strRow);
			uint8_t percent = (logFile.curPosition() * 100) / fileSize;
			Serial.print(F("percent is "));
			Serial.println(percent);
			displaySavingProg(String(percent) + "%");
			delay(500);
		}

		while (1)
		{
			if (!logFile.available())
				break;
			String strRow = logFile.readStringUntil('\n');
			Serial.println(strRow);
			strRow += "\r\n";
			if (!appendFile(filename, strRow))
			{
				logFile.close();
				return false;
			}

			delay(200);
			uint8_t percent = (logFile.curPosition() * 100) / fileSize;
			Serial.print(F("percent is "));
			Serial.println(percent);
			displaySavingProg(String(percent) + "%");
		}
	}

	logFile.close();
	sdcard.remove(logPath.c_str());
	return true;
}

void saveUSBExit()
{

}

void checkTagUpdate()
{
	String response = getResponse();
	if (response.length() > 3)
	{
		/*
		String tagID = response.substring(4, 16);*/
		response = response.substring(response.length() - 4, response.length());
		strcpy(mystatus.animolID, response.c_str());
		systemFSM.transitionTo(Tare);
	}
	
	int x, y;
	if (myTouch.dataAvailable())
	{
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if ((x >= TAREBUTTON.topX && x < TAREBUTTON.bottomX) && (y > TAREBUTTON.topY && y < TAREBUTTON.bottomY))
		{
			systemFSM.transitionTo(Home);
		}
	} 
}

void checkTagEnter()
{
	strcpy(mystatus.animolID, "");
	mystatus.weight = 0;

	myGLCD.fillScr(BACKGROUNDCOLOR);
	displayTime();

	// Logo display from SD card or bmp data
	displaySmallLogo();

	myGLCD.setFont(BigFont);
	myGLCD.print("HOME", 208, 20);

	// 
	myGLCD.drawRoundRect(140, 50, 340, 80);
	myGLCD.setFont(SmallFont);
	myGLCD.print("WELCOME ", 180, 60);
	myGLCD.print(setting.id, 245, 60);

	myGLCD.setFont(Retro8x16);
	myGLCD.setColor(MAINCOLOR);
	myGLCD.print("Animal-ID:", 110, 95);

	myGLCD.setFont(Retro8x16);
	myGLCD.setColor(MAINCOLOR);
	myGLCD.print(mystatus.animolID, 270, 95);

	displayWeight(mystatus.weight);

	myGLCD.setFont(Retro8x16);

	myGLCD.setColor(TAREPUSHCOLOR);
	myGLCD.fillRect(TAREBUTTON.topX, TAREBUTTON.topY, TAREBUTTON.bottomX, TAREBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(TAREBUTTON.topX - 1, TAREBUTTON.topY - 1, TAREBUTTON.bottomX + 1, TAREBUTTON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(TAREPUSHCOLOR);
	//myGLCD.print("TARE", 48, 143);
	myGLCD.print("STOP", TAREBUTTON.topX + 10, TAREBUTTON.topY + 15);

	myGLCD.setColor(WIFICOLOR);
	myGLCD.fillRect(APSETTINGBUTTON.topX, APSETTINGBUTTON.topY, APSETTINGBUTTON.bottomX, APSETTINGBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(APSETTINGBUTTON.topX - 1, APSETTINGBUTTON.topY - 1, APSETTINGBUTTON.bottomX + 1, APSETTINGBUTTON.bottomY + 1);
	myGLCD.setBackColor(WIFICOLOR);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.print("WiFi", APSETTINGBUTTON.topX + 10, APSETTINGBUTTON.topY + 15);

	myGLCD.setColor(CLOUDNORCOLOR);
	myGLCD.fillRect(CLOUDBUTTON.topX, CLOUDBUTTON.topY, CLOUDBUTTON.bottomX, CLOUDBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(CLOUDBUTTON.topX - 1, CLOUDBUTTON.topY - 1, CLOUDBUTTON.bottomX + 1, CLOUDBUTTON.bottomY + 1);
	myGLCD.setBackColor(CLOUDNORCOLOR);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.print("Upload", CLOUDBUTTON.topX + 5, CLOUDBUTTON.topY + 15);

	myGLCD.setColor(SAVENORCOLOR);
	myGLCD.fillRect(SAVETOUSBBUTTON.topX, SAVETOUSBBUTTON.topY, SAVETOUSBBUTTON.bottomX, SAVETOUSBBUTTON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(SAVETOUSBBUTTON.topX - 1, SAVETOUSBBUTTON.topY - 1, SAVETOUSBBUTTON.bottomX + 1, SAVETOUSBBUTTON.bottomY + 1);
	myGLCD.setBackColor(SAVENORCOLOR);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.print("SAVE TO USB", 148, 226);

	myGLCD.setColor(SETTINGCOLOR);
	myGLCD.fillRect(SETTINGBUTOON.topX, SETTINGBUTOON.topY, SETTINGBUTOON.bottomX, SETTINGBUTOON.bottomY);
	myGLCD.setColor(BORDERCOLOR);
	myGLCD.drawRoundRect(SETTINGBUTOON.topX - 1, SETTINGBUTOON.topY - 1, SETTINGBUTOON.bottomX + 1, SETTINGBUTOON.bottomY + 1);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(SETTINGCOLOR);
	myGLCD.print("Setting", 274, 226);

	scale.set_scale(setting.cali_factor);
	scale.tare();

	initRFID();
}

void keyboardEnter()
{

	myGLCD.InitLCD(LANDSCAPE);
	myGLCD.clrScr();
	myGLCD.setFont(SmallFont);

	myTouch.InitTouch(LANDSCAPE);
	myTouch.setPrecision(PREC_LOW);

	myTFT.ExtSetup();
	myGLCD.fillScr(0, 0, 0);
	myGLCD.drawRect(1, 1, 479, 271);
	myTFT.SetupMobileKB(); //NEEDED TO WORK!

}

void keyboardUpdate()
{
	while (1)
	{
		msg = myTFT.Mobile_KeyBoard(BLUE);
		if (msg != NULL)
		{
			String keybuffer = String(msg);
			switch (keyboardfocus)
			{
				case LOGINUSER:
				{
					usernameEdit = String(msg);
					systemFSM.transitionTo(Login);
				}
				break;

				case LOGINPASS:
				{
					passwordEdit = String(msg);
					systemFSM.transitionTo(Login);
				}
				break;

				case NEWUSER:
				{
					newuserEdit = String(msg);
					systemFSM.transitionTo(UserChange);
				}
				break;

				case NEWPASS:
				{
					newpassEdit = String(msg);
					systemFSM.transitionTo(UserChange);
				}
				break;

				case CONFIRMPASS:
				{
					confirmEdit = String(msg);
					systemFSM.transitionTo(UserChange);
				}
				break;

				case APPASSWORD:
				{
					apPassword = String(msg);
					systemFSM.transitionTo(UserChange);
				}
				break;
			}
			break;
		}
	}
}

void keyboardExit()
{

}

void userChangeEnter()
{
	//
	Serial.println("User setting screen!");

	myGLCD.fillScr(BACKGROUNDCOLOR);
	displayTime();
	displaySmallLogo();

	myGLCD.setFont(BigFont);
	myGLCD.print("USER SETTING", 150, 20);

	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(BACKBUTTON.topX, BACKBUTTON.topY, BACKBUTTON.bottomX, BACKBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(BACKBUTTON.topX - 1, BACKBUTTON.topY - 1, BACKBUTTON.bottomX + 1, BACKBUTTON.bottomY + 1);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.print("BACK", BACKBUTTON.topX + 15, BACKBUTTON.topY + 5);

	myGLCD.setFont(BigFont);
	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(HOMEBUTTON.topX, HOMEBUTTON.topY, HOMEBUTTON.bottomX, HOMEBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(HOMEBUTTON.topX - 1, HOMEBUTTON.topY - 1, HOMEBUTTON.bottomX + 1, HOMEBUTTON.bottomY + 1);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.print("HOME", HOMEBUTTON.topX + 15, HOMEBUTTON.topY + 5);

	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(BACKGROUNDCOLOR);
	myGLCD.setFont(Retro8x16);
	myGLCD.print("New username", NEWUSEREDIT.topX + 5, NEWUSEREDIT.topY - 20);

	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(NEWUSEREDIT.topX, NEWUSEREDIT.topY, NEWUSEREDIT.bottomX, NEWUSEREDIT.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(NEWUSEREDIT.topX - 1, NEWUSEREDIT.topY - 1, NEWUSEREDIT.bottomX + 1, NEWUSEREDIT.bottomY + 1);

	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.setFont(Retro8x16);
	myGLCD.print(newuserEdit, NEWUSEREDIT.topX + 5, NEWUSEREDIT.topY + 2);

	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(BACKGROUNDCOLOR);
	myGLCD.setFont(Retro8x16);
	myGLCD.print("New password", NEWPASSEDIT.topX + 5, NEWPASSEDIT.topY - 20);

	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(NEWPASSEDIT.topX, NEWPASSEDIT.topY, NEWPASSEDIT.bottomX, NEWPASSEDIT.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(NEWPASSEDIT.topX - 1, NEWPASSEDIT.topY - 1, NEWPASSEDIT.bottomX + 1, NEWPASSEDIT.bottomY + 1);

	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.setFont(Retro8x16);
	String password;
	
	password = "";
	for (uint8_t idx = 0; idx < newpassEdit.length(); idx++)
		password += "*";
	myGLCD.print(password, NEWPASSEDIT.topX + 5, NEWPASSEDIT.topY + 2);


	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(BACKGROUNDCOLOR);
	myGLCD.setFont(Retro8x16);
	myGLCD.print("Confirm password", CONFIRMPASSEDIT.topX + 5, CONFIRMPASSEDIT.topY - 20);

	myGLCD.setColor(GRAYBUTCOLOR);
	myGLCD.fillRect(CONFIRMPASSEDIT.topX, CONFIRMPASSEDIT.topY, CONFIRMPASSEDIT.bottomX, CONFIRMPASSEDIT.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(CONFIRMPASSEDIT.topX - 1, CONFIRMPASSEDIT.topY - 1, CONFIRMPASSEDIT.bottomX + 1, CONFIRMPASSEDIT.bottomY + 1);

	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(GRAYBUTCOLOR);
	myGLCD.setFont(Retro8x16);

	password = "";
	for (uint8_t idx = 0; idx < confirmEdit.length(); idx++)
		password += "*";
	myGLCD.print(password, CONFIRMPASSEDIT.topX + 5, CONFIRMPASSEDIT.topY + 2);

	myGLCD.setColor(GREENCOLOR);
	myGLCD.fillRect(SAVEUSERBUTTON.topX, SAVEUSERBUTTON.topY, SAVEUSERBUTTON.bottomX, SAVEUSERBUTTON.bottomY);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.drawRoundRect(SAVEUSERBUTTON.topX - 1, SAVEUSERBUTTON.topY - 1, SAVEUSERBUTTON.bottomX + 1, SAVEUSERBUTTON.bottomY + 1);

	myGLCD.setBackColor(GREENCOLOR);
	myGLCD.setColor(BACKGROUNDCOLOR);
	myGLCD.setFont(BigFont);
	myGLCD.print("SAVE", SAVEUSERBUTTON.topX + 45, SAVEUSERBUTTON.topY + 5);

}

void userChangeUpdate()
{
	int x, y;

	if (myTouch.dataAvailable())
	{
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if ((x > HOMEBUTTON.topX && x < HOMEBUTTON.bottomX) && (y > HOMEBUTTON.topY && y < HOMEBUTTON.bottomY))
		{
			systemFSM.transitionTo(Home);
		}
		else if ((x > BACKBUTTON.topX && x < BACKBUTTON.bottomX) && (y > BACKBUTTON.topY && y < BACKBUTTON.bottomY))
		{
			systemFSM.transitionTo(Setting);
		}
		else if ((x >= NEWUSEREDIT.topX && x <= NEWUSEREDIT.bottomX) && (y >= NEWUSEREDIT.topY && y <= NEWUSEREDIT.bottomY))
		{
			keyboardfocus = NEWUSER;
			systemFSM.transitionTo(Keyboard);
		}
		else if ((x >= NEWPASSEDIT.topX && x <= NEWPASSEDIT.bottomX) && (y >= NEWPASSEDIT.topY && y <= NEWPASSEDIT.bottomY))
		{
			keyboardfocus = NEWPASS;
			systemFSM.transitionTo(Keyboard);
		}
		else if ((x >= CONFIRMPASSEDIT.topX && x <= CONFIRMPASSEDIT.bottomX) && (y >= CONFIRMPASSEDIT.topY && y <= CONFIRMPASSEDIT.bottomY))
		{
			keyboardfocus = CONFIRMPASS;
			systemFSM.transitionTo(Keyboard);
		}
		else if ((x >= SAVEUSERBUTTON.topX && x <= SAVEUSERBUTTON.bottomX) && (y >= SAVEUSERBUTTON.topY && y <= SAVEUSERBUTTON.bottomY))
		{
			Serial.println();

			Serial.println(newuserEdit);
			Serial.println(newpassEdit);
			Serial.println(confirmEdit);

			if (newuserEdit.length() == 0)
				return;

			if ((newpassEdit.length() > 0 && confirmEdit.length() > 0) && newpassEdit == confirmEdit)
			{
				strcpy(setting.id, newuserEdit.c_str());
				strcpy(setting.pass, newpassEdit.c_str());
				saveConfig();
				systemFSM.transitionTo(Login);
			}

		}

	}
}

void userChangeExit()
{

}

void uploadEnter()
{
	myGLCD.fillScr(BACKGROUNDCOLOR);
	displayTime();
	displaySmallLogo();

	myGLCD.setFont(BigFont);
	myGLCD.setColor(VGA_BLACK);
	myGLCD.setBackColor(BACKGROUNDCOLOR);
	myGLCD.print("MOUSE WORKS", 152, 130);
	myGLCD.print("MD,USA", 192, 150);

	Serial.println(F("Uploading is strated"));

	if (getLogFileCount() == 0)
	{
		myGLCD.setColor(GREENCOLOR);
		myGLCD.fillRect(NOTIFYREGION.topX, NOTIFYREGION.topY, NOTIFYREGION.bottomX, NOTIFYREGION.bottomY);
		myGLCD.setColor(VGA_BLACK);
		myGLCD.drawRoundRect(NOTIFYREGION.topX - 1, NOTIFYREGION.topY - 1, NOTIFYREGION.bottomX + 1, NOTIFYREGION.bottomY + 1);

		myGLCD.setFont(Retro8x16);
		myGLCD.setColor(BACKGROUNDCOLOR);
		myGLCD.setBackColor(GREENCOLOR);
		myGLCD.print("There is no previous", NOTIFYREGION.topX + 10, NOTIFYREGION.topY + 5);
		myGLCD.print("logging data", NOTIFYREGION.topX + 30, NOTIFYREGION.topY + 25);

		delay(2500);
		systemFSM.immediateTransitionTo(Home);
	}

	if (!mystatus.wifiStatus)
	{
		myGLCD.setColor(REDCOLOR);
		myGLCD.fillRect(NOTIFYREGION.topX, NOTIFYREGION.topY, NOTIFYREGION.bottomX, NOTIFYREGION.bottomY);
		myGLCD.setColor(VGA_BLACK);
		myGLCD.drawRoundRect(NOTIFYREGION.topX - 1, NOTIFYREGION.topY - 1, NOTIFYREGION.bottomX + 1, NOTIFYREGION.bottomY + 1);

		myGLCD.setFont(Retro8x16);
		myGLCD.setColor(BACKGROUNDCOLOR);
		myGLCD.setBackColor(REDCOLOR);
		myGLCD.print("There is no internet", NOTIFYREGION.topX + 10, NOTIFYREGION.topY + 5);
		myGLCD.print("connection", NOTIFYREGION.topX + 30, NOTIFYREGION.topY + 25);

		delay(2500);
		systemFSM.immediateTransitionTo(Home);
	}

}

void uploadUpdate()
{
	Serial.println(getLogFileCount());
	if (getLogFileCount() > 0)
	{
		Serial.println(F("Data Processing is started!"));
		
		sdcard.vwd()->rewind();
		Serial.println(F("Step1"));

		if (!sdfile.openNext(sdcard.vwd(), O_READ))
		{
			myGLCD.setColor(REDCOLOR);
			myGLCD.fillRect(NOTIFYREGION.topX, NOTIFYREGION.topY, NOTIFYREGION.bottomX, NOTIFYREGION.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(NOTIFYREGION.topX - 1, NOTIFYREGION.topY - 1, NOTIFYREGION.bottomX + 1, NOTIFYREGION.bottomY + 1);

			myGLCD.setFont(Retro8x16);
			myGLCD.setColor(BACKGROUNDCOLOR);
			myGLCD.setBackColor(REDCOLOR);
			myGLCD.print("Can't open log file.", NOTIFYREGION.topX + 10, NOTIFYREGION.topY + 5);

			Serial.println(F("Can't open log file."));

			delay(2500);
			systemFSM.transitionTo(Home);
			return;
		}

		Serial.println(F("Step2"));
		// 
		char filename[32];
		sdfile.getName(filename, 32);
		sdfile.close();
		Serial.println(filename);

		File logFile;
		String logPath = setting.loggingDir;
		logPath += String(filename);

		logFile = sdcard.open(logPath);
		if (!logFile)
		{
			myGLCD.setColor(REDCOLOR);
			myGLCD.fillRect(NOTIFYREGION.topX, NOTIFYREGION.topY, NOTIFYREGION.bottomX, NOTIFYREGION.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(NOTIFYREGION.topX - 1, NOTIFYREGION.topY - 1, NOTIFYREGION.bottomX + 1, NOTIFYREGION.bottomY + 1);

			myGLCD.setFont(Retro8x16);
			myGLCD.setColor(BACKGROUNDCOLOR);
			myGLCD.setBackColor(REDCOLOR);
			myGLCD.print("Can't open log file.", NOTIFYREGION.topX + 10, NOTIFYREGION.topY + 5);

			delay(2500);
			systemFSM.transitionTo(Home);
			return;
		}

		uint32_t fileSize = logFile.fileSize();
		
		String arg1 = "";
		String arg2 = "";
		String arg3 = "";
		String arg4 = "";
		String arg5 = "";
		String arg6 = "";

		while (1)
		{
			if (!logFile.available())
				break;
			String strRow = logFile.readStringUntil('\n');
			Serial.println(strRow);

			// 17/07/2017,21:22:37,Yogen,019632938998,302.24
			int endPos = 0;
			int startPos = 0;
			uint8_t idx = 0;
			while (true)
			{
				endPos = strRow.indexOf(',', startPos);
				String temp;
				if (endPos != -1)
					temp = strRow.substring(startPos, endPos);
				else
					temp = strRow.substring(startPos, strRow.length() - 1);

				Serial.print(temp);
				Serial.print(F(","));
				Serial.println(temp.length());

				arg1 = mystatus.macaddr;

				switch (idx)
				{
				case 0:
					arg3 = temp;
					break;
				case 1:
					arg4 = temp;
					break;
				case 2:
					arg2 = temp;
					break;
				case 3:
					arg5 = temp;
					break;
				case 4:
					arg6 = temp;
					break;
				}
				idx++;

				if (endPos == -1)
					break;
				else
					startPos = endPos + 1;

			}

			uint8_t result = uploaddata(arg1, arg2, arg3, arg4,  arg5, arg6);
			if (result < FAILUPLOAD)
			{
				mystatus.wifiStatus = false;
					
				myGLCD.setColor(REDCOLOR);
				myGLCD.fillRect(NOTIFYREGION.topX, NOTIFYREGION.topY, NOTIFYREGION.bottomX, NOTIFYREGION.bottomY);
				myGLCD.setColor(VGA_BLACK);
				myGLCD.drawRoundRect(NOTIFYREGION.topX - 1, NOTIFYREGION.topY - 1, NOTIFYREGION.bottomX + 1, NOTIFYREGION.bottomY + 1);

				myGLCD.setFont(Retro8x16);
				myGLCD.setColor(BACKGROUNDCOLOR);
				myGLCD.setBackColor(REDCOLOR);
				myGLCD.print("You have to check Wifi", NOTIFYREGION.topX + 10, NOTIFYREGION.topY + 5);
				myGLCD.print("module or password.", NOTIFYREGION.topX + 10, NOTIFYREGION.topY + 25);

				delay(2500);
				systemFSM.transitionTo(Home);
				logFile.close();

				Serial.println(F("Wifi is disabled"));
				return;
			}
			else if (result < SUCCESS)
			{
				myGLCD.setColor(REDCOLOR);
				myGLCD.fillRect(NOTIFYREGION.topX, NOTIFYREGION.topY, NOTIFYREGION.bottomX, NOTIFYREGION.bottomY);
				myGLCD.setColor(VGA_BLACK);
				myGLCD.drawRoundRect(NOTIFYREGION.topX - 1, NOTIFYREGION.topY - 1, NOTIFYREGION.bottomX + 1, NOTIFYREGION.bottomY + 1);

				myGLCD.setFont(Retro8x16);
				myGLCD.setColor(BACKGROUNDCOLOR);
				myGLCD.setBackColor(REDCOLOR);
				myGLCD.print("Can't upload data to", NOTIFYREGION.topX + 10, NOTIFYREGION.topY + 5);
				myGLCD.print("cloud server.", NOTIFYREGION.topX + 20, NOTIFYREGION.topY + 25);
					
				Serial.println(F("upload is failed"));

				delay(2500);
				systemFSM.transitionTo(Home);
				logFile.close();
				return;
			}

			uint8_t percent = (logFile.curPosition() * 100) / fileSize;
			Serial.print(F("percent is "));
			Serial.println(percent);
				
			myGLCD.setColor(GREENCOLOR);
			myGLCD.fillRect(NOTIFYREGION.topX, NOTIFYREGION.topY, NOTIFYREGION.bottomX, NOTIFYREGION.bottomY);
			myGLCD.setColor(VGA_BLACK);
			myGLCD.drawRoundRect(NOTIFYREGION.topX - 1, NOTIFYREGION.topY - 1, NOTIFYREGION.bottomX + 1, NOTIFYREGION.bottomY + 1);

			myGLCD.setFont(Retro8x16);
			myGLCD.setColor(BACKGROUNDCOLOR);
			myGLCD.setBackColor(GREENCOLOR);
			myGLCD.print(filename, NOTIFYREGION.topX + 40, NOTIFYREGION.topY + 5);
				
			myGLCD.print("uploaded " + String(percent) + "%", NOTIFYREGION.topX + 20, NOTIFYREGION.topY + 25);

		}
		logFile.close();
		sdcard.remove(logPath.c_str());
	}
	else
	{
		myGLCD.setColor(GREENCOLOR);
		myGLCD.fillRect(NOTIFYREGION.topX, NOTIFYREGION.topY, NOTIFYREGION.bottomX, NOTIFYREGION.bottomY);
		myGLCD.setColor(VGA_BLACK);
		myGLCD.drawRoundRect(NOTIFYREGION.topX - 1, NOTIFYREGION.topY - 1, NOTIFYREGION.bottomX + 1, NOTIFYREGION.bottomY + 1);

		myGLCD.setFont(Retro8x16);
		myGLCD.setColor(BACKGROUNDCOLOR);
		myGLCD.setBackColor(GREENCOLOR);
		myGLCD.print("All logging data has", NOTIFYREGION.topX + 10, NOTIFYREGION.topY + 5);
		myGLCD.print("been uploaded", NOTIFYREGION.topX + 30, NOTIFYREGION.topY + 25);

		delay(2500);
		systemFSM.transitionTo(Home);
	}

}

void uploadExit()
{

}
/* State list */
State Standby = State(standbyEnter, standbyUpdate, StandbyExit);
State Login = State(loginEnter, loginUpdate, loginExit);
State Upload = State(uploadEnter, uploadUpdate, uploadExit);
State Home = State(homeEnter, homeUpdate, homeExit);
State CheckTag = State(checkTagEnter, checkTagUpdate, NULL);

State Tare = State(tareEnter, tareUpdate, tareExit);
State Setting = State(settingEnter, settingUpdate, settingExit);
State Cloud = State(cloudEnter, cloudUpdate, cloudExit);
State SavetoUSB = State(saveUSBEnter, saveUSBUpdate, saveUSBExit);
State Keyboard = State(keyboardEnter, keyboardUpdate, keyboardExit);
State UserChange = State(userChangeEnter, userChangeUpdate, userChangeExit);


// FSM instance
FSM systemFSM = FSM(Standby);

// display current time
void displayTime()
{
	DateTime now = rtc.now();
	// background
	myGLCD.setBackColor(BACKGROUNDCOLOR);
	// forecolor
	myGLCD.setColor(MAINCOLOR);
	// font
	myGLCD.setFont(Retro8x16);

	uint8_t day = now.day();

	if (day > 0 && day < 10)
	{
		myGLCD.printNumI(0, 380, 20, 1, 1); //day
		myGLCD.printNumI(day, 388, 20, 1, 1); //day
	}
	else
	{
		myGLCD.printNumI(day, 380, 20, 1, 1); //day
	}
	myGLCD.print("/", 397, 20);

	uint8_t month = now.month();
	if (month > 0 && month < 10)
	{
		myGLCD.printNumI(0, 405, 20, 1, 1); 
		myGLCD.printNumI(month, 413, 20, 1, 1); 
	}
	else
	{
		myGLCD.printNumI(month, 405, 20, 1, 1);
	}
	myGLCD.print("/", 422, 20);

	// 
	uint16_t year = now.year();
	myGLCD.printNumI(year, 430, 20, 1, 1);

	uint16_t hour = now.hour();
	if (hour < 10)
	{
		myGLCD.printNumI(0, 380, 38, 1, 1);
		myGLCD.printNumI(hour, 388, 38, 1, 1); 
	}
	else
	{
		myGLCD.printNumI(hour, 380, 38, 1, 1);
	}
	myGLCD.print(":", 397, 38);

	uint8_t minute = now.minute();
	if (minute < 10)
	{
		myGLCD.printNumI(0, 405, 38, 1, 1);
		myGLCD.printNumI(minute, 413, 38, 1, 1);
	}
	else
	{
		myGLCD.printNumI(minute, 405, 38, 1, 1);
	}
	myGLCD.print(":", 422, 38);

	uint8_t sec = now.second();
	if (sec < 10)
	{
		myGLCD.printNumI(0, 430, 38, 1, 1); //min
		myGLCD.printNumI(sec, 438, 38, 1, 1); //min
	}
	else
	{
		myGLCD.printNumI(sec, 430, 38, 1, 1);//sec
	}

	displayWifiStatus(mystatus.wifiStatus);
}

