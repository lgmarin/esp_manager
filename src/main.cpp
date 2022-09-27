#include "ConfigManager.h"
#include "WifiManager.h"
#include "WebServer.h"

void setup()
{
  Serial.begin(115200);
  delay(500);

  configManager.begin(DEBUG);
  wifiManager.begin();
}

void loop()
{
  wifiManager.loop();
}