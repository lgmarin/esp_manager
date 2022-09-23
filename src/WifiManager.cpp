#include "WifiManager.h"
#include "ConfigManager.h"

#include <ESP8266WiFi.h>

/**
 * @brief Set static IP according to data stored in the flash memory.
 * 
 * @return true Static IP set successfully.
 */
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

/**
 * @brief Connect to Wifi in station mode
 * 
 * @return true if the connection is successfull.
 */
bool WifiManager::_startSTA()
{
    WiFi.mode(WIFI_STA);
    WiFi.persistent(true);
    
    if(configManager.Wifi_config.dyn_ip)
    {
        if(_setStaticIp())
            Serial.println(F("[INFO] Using static IP."));
    }

    if (WiFi.SSID() == "")
        return false;

    ETS_UART_INTR_DISABLE();
    wifi_station_disconnect();
    ETS_UART_INTR_ENABLE();
    
    WiFi.begin();

    if(WiFi.waitForConnectResult(WIFI_RETRY_TIMEOUT) != WL_CONNECTED) 
    {
        Serial.print(F("\n[ERROR]: Failed to connect."));
        return false;
    }
    
    Serial.print(F("\n[SUCCESS]: CONNECTED: Mode: STA, SSID: ")); Serial.print(WiFi.SSID());
    Serial.print(F(" IP: ")); Serial.print(WiFi.localIP());
    return true;
}

void WifiManager::begin()
{
    if(!configManager.Device_config.ap_mode)
    {
        _startSTA();
    }


}