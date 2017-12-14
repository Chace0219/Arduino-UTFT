#ifndef SD_MANAGER_H

#define SD_MANAGER_H

#include <SPI.h>
#include <SdFat.h>

extern SdFat sdcard;
extern SdFile sdfile;

uint8_t getSDFreePerc();
bool initSDCard();
bool writeLog(String userID, String animalID, double weight);

uint16_t getLogFileCount();
double getFreeCapacity();
double getTotalCapacity();

#endif