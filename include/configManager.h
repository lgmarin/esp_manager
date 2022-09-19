#ifndef configManager_h
#define configManager_h

#include <LittleFS.h>
#include "definitions.h"

class configManager
{
private:
    bool debug = false;
public:
    configManager(bool debug = false);
};

#endif