#include "WifiManager.h"
#include "ConfigManager.h"

#include <ESP8266WiFi.h>

bool WifiManager::_setStaticIp()
{
    IPAddress ip;
    IPAddress gw;
    IPAddress ms;

    ip = IPAddress(configManager.Wifi_config.IP_config.ip_addr);
    gw = IPAddress(configManager.Wifi_config.IP_config.gw_addr);
    ms = IPAddress(configManager.Wifi_config.IP_config.mask);

    if(ip.isSet() && gw.isSet() && ms.isSet())
    {
        if(WiFi.config(ip, gw, ms))
            return true;
    }

    return false;
}

void WifiManager::begin()
{
    WiFi.mode(WIFI_STA);
    WiFi.persistent(true);
    
    if(configManager.Wifi_config.dyn_ip)
    {
        if(_setStaticIp())
            Serial.println(F("[INFO] Using static IP."));
    }





}