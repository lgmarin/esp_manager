#include "ConfigManager.h"

void setup()
{
  Serial.begin(115200);
  delay(500);

  configManager.begin(DEBUG);

  configManager.Device_config.ap_mode = false;
}

void loop()
{
  
}