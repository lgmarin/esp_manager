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

/**
 * @brief Start a captive portal (AP mode) and enable DNS server redirection to device IP.
 * 
 * @param ap_name Char array containing the SSID name for the AP.
 * @return true If enabled successfully.
 */
bool WifiManager::_startAP(const char* ap_name)
{
    WiFi.persistent(false);
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.persistent(true);

    if(WiFi.softAP(ap_name))
    {
        deviceIP = WiFi.softAPIP();
        Serial.print(PSTR("\n[SUCCESS]: Captive Portal Started at IP: %s")); Serial.print(deviceIP);

        dnsServer = new DNSServer();

        dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
        dnsServer->start(53, "*", deviceIP);

        _isAPMode = true;

        return true;
    }

    return false;
    ESP.reset();
}

/**
 * @brief Disconnect from AP mode and change to STA mode.
 * 
 */
void WifiManager::_finishAP()
{
    WiFi.mode(WIFI_STA);
    delete dnsServer;
    _isAPMode = false;
}

void WifiManager::begin()
{

    if(!configManager.Device_config.ap_mode)
    {
        if(!_startSTA())
            _startAP(configManager.Device_config.host_name);
    }

}