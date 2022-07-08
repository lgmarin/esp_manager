#ifndef fileUtils_h
#define fileUtils_h

#include <Arduino.h>
#include <LittleFS.h>

void initFS();
bool loadConfigData();
void saveConfigData();

#endif