
#include <SPI.h>


#include <Wire.h>
#include "global.h"

#include "loadcell.h"
#include "setting.h"
#include "flash.h"
#include "sd_manager.h"

#include "uploaddata.h"

#include "FiniteStateMachine.h"
const uint8_t chipSelect = SS;

uint32_t nTimeRefms = 0;
const uint32_t TIMEREFINTERVAL = 1000;

// 
RTC_DS3231 rtc;

// standard arduino mega/due shield            : <display model>,38,39,40,41
UTFT myGLCD(SSD1963_480, 38, 39, 40, 41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch myTouch(43, 42, 44, 45, 46);  //byte tclk, byte tcs, byte din, byte dout, byte irq
TFT_Extension myTFT(&myGLCD, &myTouch);
//UTFT_tinyFAT myFiles(&myGLCD);
UTFT_SdRaw myFiles(&myGLCD);


void setup()
{
	// serial port initiallize
	Serial.begin(115200);
	delay(1000);
	
	Serial.println(F("Mousework is started!"));

	// RTC init part
	if (!rtc.begin()) {
		Serial.println(F("Couldn't find RTC"));
		while (1);
	}

	if (rtc.lostPower()) {
		Serial.println(F("RTC lost power, lets set the time!"));
		// following line sets the RTC to the date & time this sketch was compiled
		rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	}
	
	// init SD card
	while (1)
	{
		if (!initSDCard())
		{
			Serial.println(F("SD card is not present!"));
		}
		else
		{
			Serial.println(F("SD init is finished!"));
			break;
		}
		delay(1000);
	}

	// load config from EEPROM
	loadConfig();

	// 
	myGLCD.InitLCD();
	myGLCD.clrScr();

	// -------------------------------------------------------------
	pinMode(8, OUTPUT);  //backlight 
	digitalWrite(8, HIGH);//on
	// -------------------------------------------------------------

	myTouch.InitTouch();
	myTouch.setPrecision(PREC_MEDIUM);
	myGLCD.setFont(BigFont);

	// Flash Drive module
	initFlash();

	//
	usernameEdit = String(setting.id);
	passwordEdit = "";

	Serial.println(setting.apSSID);
	Serial.println(setting.apPASS);

	if (connectToDefaultAP())
		mystatus.wifiStatus = true;
	else
		mystatus.wifiStatus = false;

	systemFSM.transitionTo(Login);
	nTimeRefms = millis();

	scale.set_scale(setting.cali_factor);
	scale.tare();

}

void loop()
{
	if (millis() - nTimeRefms >= TIMEREFINTERVAL)
	{
		nTimeRefms = millis();
		if(!systemFSM.isInState(Keyboard))
			displayTime();
	}
	statemanage();
} // end loop 
