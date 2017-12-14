
#include <Arduino.h>
#include "sd_manager.h"
#include "setting.h"

#include "global.h"
//------------------------SD card stuff
//#define SD_CHIP_SELECT  4  // SD chip select pin
#define SD_CHIP_SELECT  53  // SD chip select pin

// File system object.
SdFat sdcard;

// Use for file creation in folders.
SdFile sdfile;

// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

bool initSDCard()
{
	if (!sdcard.begin(SD_CHIP_SELECT, SPI_FULL_SPEED))
	{
		// SD card init Error
		// sd.initErrorHalt();
		
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------
uint8_t getSDFreePerc()
{
	uint32_t volFree = sdcard.vol()->freeClusterCount();
	float fs = 0.000512*volFree*sdcard.vol()->blocksPerCluster();
	uint32_t totalCluster = sdcard.vol()->clusterCount();
	return map(volFree, 0, totalCluster, 0, 100);

	// cout << F("freeSpace: ") << fs << F(" MB (MB = 1,000,000 bytes)\n\n");
}

double getTotalCapacity()
{
	uint32_t totalCluster = sdcard.vol()->clusterCount();
	double totalMB = 0.000512*totalCluster*sdcard.vol()->blocksPerCluster();
	return totalMB;
}

double getFreeCapacity()
{
	uint32_t volFree = sdcard.vol()->freeClusterCount();
	double freeMB = 0.000512*volFree*sdcard.vol()->blocksPerCluster();
	return freeMB;
}


bool writeLog(String userID, String animalID, double weight)
{
	DateTime now = rtc.now();

	// 
	String csvstring = "";

	// Day
	if (now.day() < 10)
		csvstring += F("0");
	csvstring += String(now.day());
	csvstring += F("/");

	// Month
	if (now.month() < 10)
		csvstring += F("0");
	csvstring += String(now.month());
	csvstring += F("/");

	// Year
	csvstring += String(now.year());

	// CSV comma
	csvstring += String(",");

	// time
	if (now.hour() < 10)
		csvstring += F("0");
	csvstring += String(now.hour());
	csvstring += String(":");

	// Minute
	if (now.minute() < 10)
		csvstring += F("0");
	csvstring += String(now.minute());
	csvstring += String(":");

	// Second
	if (now.second() < 10)
		csvstring += F("0");
	csvstring += String(now.second());

	csvstring += String(",");

	// userID
	csvstring += userID;
	csvstring += String(",");

	// animalID
	csvstring += animalID;
	csvstring += String(",");

	// weight value
	csvstring += String(weight, 2);

	// 
	if (!sdcard.exists(setting.loggingDir))
	{
		Serial.println(F("logging directory does not exists!"));
		sdcard.mkdir(setting.loggingDir);
	}

	String fileName = String(setting.loggingDir) + String(mystatus.logFileName);
	File logfile = sdcard.open(fileName, FILE_WRITE);

	Serial.print(F("File name is "));
	Serial.println(fileName);

	if (logfile)
	{
		// success
		Serial.println(csvstring);
		logfile.println(csvstring);
	}
	else
	{
		// 
		Serial.println(F("I can't open file, "));
	}

	logfile.close();
}


uint16_t getLogFileCount()
{
	uint16_t nFileCount = 0;
	
	sdcard.chdir(setting.loggingDir, true);
	sdcard.vwd()->rewind();
	while (sdfile.openNext(sdcard.vwd(), O_READ))
	{
		if (!sdfile.isHidden())
		{
			nFileCount++;
		}
		sdfile.close();
	}

	return nFileCount;
}

