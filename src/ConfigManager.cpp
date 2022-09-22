#include "ConfigManager.h"

const char* wifi_config_file PROGMEM = "/wifi.cfg";
const char* device_config_file PROGMEM = "/device.cfg";

/*!
 *  @brief  Return the content of dir in the FS on the Serial Console.
 *  @param  string Directory path.
 */
void ConfigManager::_listFSFiles(String dir_path)
{
	Dir dir = LittleFS.openDir(dir_path);
	while(dir.next()) {
		if (dir.isFile()) {
			Serial.print(F("File: "));
			Serial.println(dir_path + dir.fileName());
		}
		if (dir.isDirectory()) {
			Serial.print(F("Dir: "));
			Serial.println(dir_path + dir.fileName() + "/");
			_listFSFiles(dir_path + dir.fileName() + "/");
		}
	}
}

/*!
 *  @brief  Load data from LittleFS.
 *  @param str_Config Struct pointer destination.
 *  @param size Destination struct size.
 *  @param filename Filename to load.
 *  @return Returns true if successful.
 */
bool ConfigManager::_loadFSData(void *str_Config, size_t size, const char* filename)
{
  File file = LittleFS.open(filename, "r");
  memset(str_Config, 0, size);

  if (file)
  {
    file.readBytes((char *) str_Config, size);
    file.close();
    return true;
  }
  else
  {
    return false;
  }
}

/*!
 *  @brief  Save data to LittleFS.
 *  @param str_Config Struct pointer to save.
 *  @param size Struct size.
 *  @param filename Filename to save.
 *  @return Returns true if successful.
 */
bool ConfigManager::_saveFSData(void *str_Config, size_t size, const char* filename)
{
  File file = LittleFS.open(filename, "w");

  if (file)
  {
    file.write((uint8_t*) str_Config, size);
    file.close();
    return true;
  }
  else
  {
    return false;
  }
}

/*!
 *  @brief  Remove data from LittleFS.
 *  @param filename Filename to remove.
 *  @return Returns true if successful.
 */
bool ConfigManager::_removeFSData(const char* filename)
{
  if (LittleFS.exists(filename))
  {
    if (LittleFS.remove(filename))
    {
      Serial.print(F("\n[INFO]: File removed!"));
      return true;
    }
    Serial.print(F("[ERROR]: File couldn't be removed! Removal error."));
    return false;
  }
  Serial.print(F("[ERROR]: File couldn't be removed! Not found."));
  return false;
}

/*!
 *  @brief  Calculate configuration struct checksum.
 *  @param address Struct pointer to calculate.
 *  @param sizeToCalc Struct size.
 *  @return Returns checksum data.
 */
uint16_t ConfigManager::_calcChecksum(uint8_t* address, uint16_t sizeToCalc)
{
  uint16_t checkSum = 0;

  for (uint16_t index = 0; index < sizeToCalc; index++)
  {
    checkSum += * ( ( (byte*) address ) + index);
  }
  return checkSum;
}

/*    PUBLIC FUNCTIONS

/*!
 *  @brief  Initialize LittleFS.
 *  @return Returns true if initialized successfully.
 */
bool ConfigManager::begin(bool listFiles) 
{
  if (!LittleFS.begin()) {
    Serial.print(F("\n[ERROR]: An error has occurred while mounting LittleFS"));
    return false;
  }
  else
  {
    Serial.print(F("\n[INFO]: LittleFS mounted successfully"));
    
    if (listFiles)
    {
      Serial.print(F("\n[INFO]: Listing files...\n"));
      _listFSFiles("/");
    }

    return true;
  }
}

/* ConfigManager PUBLIC METHODS */
/*!
 *  @brief  Load WifiConfiguration saved in LitteFS.
 *  @return Returns true if configuration loaded successfully.
 */
bool ConfigManager::loadWifiConfig()
{
  if(_loadFSData(&Wifi_config, sizeof(Wifi_config), (char*) wifi_config_file))
  {
    if ( Wifi_config.checksum != _calcChecksum( (uint8_t*) &Wifi_config, sizeof(Wifi_config) - sizeof(Wifi_config.checksum) ) )
    {
      Serial.print(F("\n[ERROR]: Wifi Credentials checksum wrong!"));
      return false;
    }

    // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
    if ((String(Wifi_config.WiFi_cred.wifi_ssid) == "") || (String(Wifi_config.WiFi_cred.wifi_pw) == ""))
    {
      Serial.print(F("\n[ERROR]: Wifi SSID or Password is empty!"));
      return false;
    }
    Serial.print(F("\n[INFO]: Wifi Config File Read. Checksum ok."));
    return true; 
  }
  else
  {
    Serial.print(F("\n[ERROR]: Could not read Wifi Config File."));
    return false;
  }
}

/*!
 *  @brief  Store WifiConfiguration into LitteFS.
 *  @return Returns true if configuration saved successfully.
 */
bool ConfigManager::storeWifiConfig(String SSID, String password, bool dyn_ip, IPAddress ip, IPAddress gw, IPAddress mask)
{
  //SAVE SSID
  if (strlen(SSID.c_str()) < sizeof(Wifi_config.WiFi_cred.wifi_ssid) - 1)
    strcpy(Wifi_config.WiFi_cred.wifi_ssid, SSID.c_str());
  else
    strncpy(Wifi_config.WiFi_cred.wifi_ssid, SSID.c_str(), sizeof(Wifi_config.WiFi_cred.wifi_ssid) - 1);

  //SAVE PWD
  if (strlen(password.c_str()) < sizeof(Wifi_config.WiFi_cred.wifi_pw) - 1)
    strcpy(Wifi_config.WiFi_cred.wifi_pw, password.c_str());
  else
    strncpy(Wifi_config.WiFi_cred.wifi_pw, password.c_str(), sizeof(Wifi_config.WiFi_cred.wifi_pw) - 1);  

  if ((String(Wifi_config.WiFi_cred.wifi_ssid) == "") && (String(Wifi_config.WiFi_cred.wifi_pw) == ""))
    Serial.println(F("[WARNING]: Null SSID or Password!"));

  //SAVE IP
  Wifi_config.IP_config.ip_addr = ip;
  Wifi_config.IP_config.gw_addr = gw;
  Wifi_config.IP_config.mask = mask;

  Wifi_config.dyn_ip = dyn_ip;
  
  //Calculate checksum and save credentials
  Wifi_config.checksum = _calcChecksum((uint8_t*) &Wifi_config, sizeof(Wifi_config) - sizeof(Wifi_config.checksum));
  if (_saveFSData(&Wifi_config, sizeof(Wifi_config), (char*) wifi_config_file))
  {
    Serial.print(F("\n[INFO]: Wifi Credentials file saved!"));
    return true;
  }

  Serial.print(F("\n[ERROR]: Could not store Wifi Config File"));
  return false;
}

/*!
 *  @brief  Remove WifiConfiguration from LitteFS.
 *  @return Returns true if configuration removed successfully.
 */
bool ConfigManager::removeWifiConfig()
{
  if(_removeFSData(wifi_config_file))
    return true;

  return false;
}

///
///     DEVICE CONFIGURATION FILE
///

/*!
 *  @brief  Load Device Configuration saved in LitteFS.
 *  @return Returns true if configuration loaded successfully.
 */
bool ConfigManager::loadDeviceConfig()
{
  if(_loadFSData(&Device_config, sizeof(Device_config), (char*) device_config_file))
  {
    if ( Device_config.checksum != _calcChecksum( (uint8_t*) &Device_config, sizeof(Device_config) - sizeof(Device_config.checksum) ) )
    {
      Serial.print(F("\n[ERROR]: Device config checksum wrong!"));
      return false;
    }

    if ((String(Device_config.host_name) == ""))
    {
      Serial.print(F("\n[ERROR]: Hostname is empty, using default!"));
      if (strlen(String(DEFAULT_HOSTNAME).c_str()) < sizeof(Device_config.host_name) - 1)
        strcpy(Device_config.host_name, String(DEFAULT_HOSTNAME).c_str());
      else
        strncpy(Device_config.host_name, String(DEFAULT_HOSTNAME).c_str(), sizeof(Device_config.host_name) - 1);
    }
    Serial.print(F("\n[INFO]: Device Config File Read. Checksum ok."));
    return true; 
  }
  else
  {
    Serial.print(F("\n[ERROR]: Could not read device Config File."));
    return false;
  }
}


/*!
 *  @brief  Store Device Configuration into LitteFS.
 *  @return Returns true if configuration saved successfully.
 */
bool ConfigManager::storeDeviceConfig(const char* host_name, bool apmode)
{
  //SAVE HOSTNAME
  if (strlen(host_name) < sizeof(Device_config.host_name) - 1)
    strcpy(Device_config.host_name, host_name);
  else
    strncpy(Device_config.host_name, host_name, sizeof(Device_config.host_name) - 1);

  if ((String(Device_config.host_name) == ""))
    Serial.println(F("[WARNING]: Null hostname!"));

  //SAVE AP_MODE
  Device_config.ap_mode = apmode;

  //Calculate checksum and save credentials
  Device_config.checksum = _calcChecksum((uint8_t*) &Device_config, sizeof(Device_config) - sizeof(Device_config.checksum));
  if (_saveFSData(&Device_config, sizeof(Device_config), (char*) device_config_file))
  {
    Serial.print(F("\n[INFO]: Device config file saved!"));
    return true;
  }

  Serial.print(F("\n[ERROR]: Could not store Devices Config File"));
  return false;
}


/*!
 *  @brief  Remove Device Configuration from LitteFS.
 *  @return Returns true if configuration removed successfully.
 */
bool ConfigManager::removeDeviceConfig()
{
  if(_removeFSData((char*) device_config_file))
    return true;

  return false;
}

ConfigManager configManager;