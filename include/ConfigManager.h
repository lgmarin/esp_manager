#ifndef ConfigManager_h
#define ConfigManager_h

#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include "definitions.h"

// CONFIGURATIONS STRUCTS
typedef struct
{
  char wifi_ssid[32];
  char wifi_pw  [64];
} WiFi_Credential;

typedef struct
{
  IPAddress ip_addr{};
  IPAddress gw_addr{};
  IPAddress mask{};
} IP_Config;

typedef struct
{
  WiFi_Credential   WiFi_cred{};
  IP_Config         IP_config{};
  bool              dyn_ip = false;
  uint16_t          checksum{};
} Wifi_Config;

typedef struct
{
  char       host_name[24]{};
  bool       ap_mode = false;
  uint16_t   air_value{};
  uint16_t   wat_value{};
  uint16_t   checksum{};
} Device_Config;

/**
 * @brief ConfigManager - Creates a configuration structure
 * 
 * @param debug *optional  - Enables debuging (default = false)
 */
class ConfigManager
{
private:
    bool _debug = false;
    void _listFSFiles(String dir_path);
    bool _initFS(bool listFiles);
    bool _removeFSData(const char* filename);
    bool _loadFSData(void *str_Config, size_t size, const char* filename);
    bool _saveFSData(void *str_Config, size_t size, const char* filename);
    uint16_t _calcChecksum(uint8_t* address, uint16_t sizeToCalc);
    bool _initDeviceConfiguration();

public:
    //Constructor
    ConfigManager();
    ConfigManager(bool debug);

    //Public Methods
    bool loadWifiConfig();
    bool storeWifiConfig(String SSID, String password, bool dyn_ip, IPAddress ip, IPAddress gw, IPAddress mask);
    bool removeWifiConfig();

    bool loadDeviceConfig();
    bool storeDeviceConfig(String host_name, String air_v, String wat_v, bool apmode);
    bool removeDeviceConfig();
};

#endif