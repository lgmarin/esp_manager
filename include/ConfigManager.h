#ifndef ConfigManager_h
#define ConfigManager_h

#include <LittleFS.h>
#include "definitions.h"

/**
 * @brief ConfigManager - Creates a configuration structure
 * 
 * @param debug *optional  - Enables debuging (default = false)
 */
class ConfigManager
{
protected:
    bool _debug = false;
    void _listFSFiles(String dir_path);
    bool _initFS(bool listFiles);
    bool _removeFSData(char* filename);
    bool _loadFSData(void *str_Config, size_t size, char* filename);
    bool _saveFSData(void *str_Config, size_t size, char* filename);
    uint16_t _calcChecksum(uint8_t* address, uint16_t sizeToCalc);
};

class WifiConfig : ConfigManager
{
public:
    bool isDebug();
    WifiConfig();
    WifiConfig(bool debug);
};

#endif