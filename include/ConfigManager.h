#ifndef ConfigManager_h
#define ConfigManager_h

#include "definitions.h"

// CONFIGURATIONS STRUCTS
typedef struct
{
  char wifi_ssid[32];
  char wifi_pw  [64];
} WiFiCredential;

typedef struct
{
  uint32_t ip_addr{};
  uint32_t gw_addr{};
  uint32_t mask{};
} IPConfig;

typedef struct
{
  WiFiCredential   WiFi_cred{};
  IPConfig         IP_config{};
  bool             dyn_ip = false;
  uint16_t         checksum{};
} WifiConfig;

typedef struct
{
  char       host_name[24]{};
  bool       ap_mode = false;
  uint16_t   checksum{};
} DeviceConfig;

/**
 * @brief ConfigManager - Manages ESP configuration using LittleFS.
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

    bool _loadWifiConfig();
    bool _loadDeviceConfig();

public:
    bool request_save = false;
    //Config Structs
    WifiConfig Wifi_config;
    DeviceConfig Device_config;

    //Public Methods
    bool begin(bool listFiles);
    void storeCharString(char *charDestination, const char *charString);

    
    //bool storeWifiConfig(String SSID, String password, bool dyn_ip, IPAddress ip, IPAddress gw, IPAddress mask);
    bool removeWifiConfig();

    //bool storeDeviceConfig(const char* host_name, bool apmode);
    bool removeDeviceConfig();

    void saveConfig();
};

extern ConfigManager configManager;

#endif