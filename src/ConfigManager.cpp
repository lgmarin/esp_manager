#include "ConfigManager.h"

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
 *  @brief  Initialize LittleFS.
 *  @return Returns true if initialized successfully.
 */
bool ConfigManager::_initFS(bool listFiles) 
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

/*!
 *  @brief  Load data from LittleFS.
 *  @param str_Config Struct pointer destination.
 *  @param size Destination struct size.
 *  @param filename Filename to load.
 *  @return Returns true if successful.
 */
bool ConfigManager::_loadFSData(void *str_Config, size_t size, char* filename)
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
bool ConfigManager::_saveFSData(void *str_Config, size_t size, char* filename)
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
bool ConfigManager::_removeFSData(char* filename)
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

ConfigManager::ConfigManager()
{
  _debug = false;
  _initFS(false);
}

ConfigManager::ConfigManager(bool debug)
{
  _debug = debug;
  _initFS(debug);
}
