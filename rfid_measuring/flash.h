#ifndef FLASH_H

#define FLASH_H
#include <Arduino.h>

#define USB Serial1                           // Digital pin 10 on Arduino (RX) connects to TXD on the CH376S module

uint8_t checkUSB();

void initFlash();

uint8_t getResponseFromUSB();

bool waitForResponse(String errorMsg);
bool continueRead();
bool fileClose(uint8_t closeCmd);
void fileWrite(String data);
void fileRead();
bool setByteRead(byte numBytes);
void fileOpen();
void filePointer(bool fileBeginning);
void setFileName(String fileName);
bool USBdiskMount();
void diskConnectionStatus();
bool fileCreate();
int getFileSize();

void set_USB_Mode(byte value);
void writeFile(String fileName, String data);
bool appendFile(String fileName, String data);
uint8_t checkUSB();
bool checkConnection(byte value);

void resetALL();
#endif