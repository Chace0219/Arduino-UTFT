#ifndef EM4205_H
#define EM4205_H

#include <Arduino.h>

#define RFIDSerial Serial2

String getResponse();

bool initRFID();
bool checkTag();
#endif
