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
} WiFiCredential;

typedef struct
{
  IPAddress ip_addr{};
  IPAddress gw_addr{};
  IPAddress mask{};
} IPConfig;

typedef struct
{
  WiFiCredential   WiFi_cred{};
  IPConfig         IP_config{};
  bool              dyn_ip = false;
  uint16_t          checksum{};
} WifiConfig;

typedef struct
{
  char       host_name[24]{};
  bool       ap_mode = false;
  uint16_t   checksum{};
} DeviceConfig;


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

    bool _removeFSData(const char* filename);
    bool _loadFSData(void *str_Config, size_t size, const char* filename);
    bool _saveFSData(void *str_Config, size_t size, const char* filename);
    uint16_t _calcChecksum(uint8_t* address, uint16_t sizeToCalc);
    bool _initDeviceConfiguration();

public:
    //Config Structs
    WifiConfig Wifi_config;
    DeviceConfig Device_config;

    //Public Methods
    bool begin(bool listFiles);

    bool loadWifiConfig();
    bool storeWifiConfig(String SSID, String password, bool dyn_ip, IPAddress ip, IPAddress gw, IPAddress mask);
    bool removeWifiConfig();

    bool loadDeviceConfig();
    bool storeDeviceConfig(String host_name, String air_v, String wat_v, bool apmode);
    bool removeDeviceConfig();
};

extern ConfigManager configManager;

#endif