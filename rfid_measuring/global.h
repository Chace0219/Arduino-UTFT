#ifndef GLOBAL_H
#define GLOBAL_H
#include <Arduino.h>
#include <UTFT.h>
#include <UTouch.h>
#include <UTFT_tinyFAT.h>
#include "RTClib.h"
#include "FiniteStateMachine.h"
#include <tinyFAT.h>
#include "TFT_Extension.h"
#include <UTFT_SdRaw.h>

/* font definition */

extern const uint8_t BigFont[] PROGMEM;
extern const uint8_t SmallFont[] PROGMEM;
//extern const uint8_t SevenSegNumFont[];
//extern const uint8_t GroteskBold16x32[];
//extern const uint8_t GroteskBold24x48[];
//extern const uint8_t SixteenSegment64x96Num[];
extern const uint8_t SixteenSegment40x60[] PROGMEM;
//extern const uint8_t SixteenSegment48x72Num[];
//extern const uint8_t OCR_A_Extended_M[];
extern const uint8_t Ubuntu[] PROGMEM;//24x32
extern const uint8_t Retro16x32[] PROGMEM;
extern const uint8_t Retro8x16[] PROGMEM;
//extern const uint8_t SixteenSegment24x36[];
//extern const uint8_t hallfetica_normal[];//16x16
//extern const uint8_t arial_normal[];//16X16

extern UTFT myGLCD;
extern UTouch myTouch;
extern RTC_DS3231 rtc;
//extern UTFT_tinyFAT myFiles;
extern UTFT_SdRaw myFiles;
extern TFT_Extension myTFT;


extern String usernameEdit;
extern String passwordEdit;

void statemanage();
void displayTime();
void displayWeight(double weight);
bool savefiletoUSB(uint16_t fileindex);
void displaySavingProg(String status);
void displaySmallLogo();
void displayWifiStatus(bool status);

int freeRam();
void initTable();

void setClock();

extern FSM systemFSM;
extern State Standby;
extern State Login;
extern State Upload;
extern State Home;
extern State CheckTag;
extern State Tare;
extern State Setting;
extern State Cloud;
extern State SavetoUSB;
extern State Keyboard;

extern State UserChange;

#define REDCOLOR 0x00, 0x00, 0xFF
#define GREENCOLOR 0x00, 0xFF, 0x00
#define YELLOWCOLOR 0x00, 0xFF, 0xFF

#define BACKGROUNDCOLOR    0xFF, 0xFF, 0xFF
#define MAINCOLOR     0x00, 0x00, 0x00

#define ANIMALIDCOLOR   0x00, 0x00, 0xFF
#define BORDERCOLOR     0x10, 0x10, 0x10

#define TARENORCOLOR    0x00, 0xFF, 0xFF
#define TAREPUSHCOLOR   0x00, 0x8E, 0x00

#define CLOUDNORCOLOR   0xFF, 0x00, 0x00
#define CLOUDPUSHCOLOR    0xFF, 0x00, 0x00

#define SAVENORCOLOR    0x80, 0x00, 0x80
#define SAVEPUSHCOLOR   0x00, 0x00, 0xFF

#define WIFICOLOR	  0x00, 0xFF, 0x00
#define SETTINGCOLOR  0x00, 0x00, 0xFF

#define EDITBACKCOLOR  0x80, 0x80, 0x80
#define LOGINBUTCOLOR 0x00, 0xFF, 0x00

#define GRAYBUTCOLOR  0x96, 0x96, 0x96
#define GRIDBACKCOLOR 0xC4, 0x9D, 0x85
#define GRIDSELCOLOR  0xBD, 0x81, 0x5A

#define BUTTONDISCOLOR  0xC0, 0xC0, 0xC0

#endif
