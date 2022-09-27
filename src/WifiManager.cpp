#include "WifiManager.h"
#include "ConfigManager.h"


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
        {
            _staticIP = true;
            Serial.println(PSTR("[INFO] Using static IP."));
            return true;
        }
    }

    _staticIP = false;
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
    
    _staticIP = false;

    if(configManager.Wifi_config.dyn_ip)
        _setStaticIp();

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

        _dnsServer = new DNSServer();

        _dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
        _dnsServer->start(53, "*", deviceIP);

        _isAPMode = true;
        _staticIP = true;

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
    delete _dnsServer;
    _isAPMode = false;
}

/**
 * @brief Conver wifi RSSI to percent value.
 * 
 * @param RSSI 
 * @return String 
 */
String WifiManager::_rssiToPercent(int32_t RSSI)
{
    if (RSSI <= -100)
        return String(0);
    
    if (RSSI >= -50)
        return String(100);

    return String(2 * (RSSI + 100));
}

/**
 * @brief  Retry connection to a stored network.
 * 
 * @param retry_count Ammount of times to retry for connection.
 */
void WifiManager::_retrySTAConnection(uint8_t retry_count)
{
    int i = 0;
    while(i < retry_count)
    {
        if(_startSTA())
            break;
        i++;
    }

    if(i == retry_count)
    {
        Serial.println(PSTR("[WARN] Can't connect back to STA, starting AP."));
        _startAP(configManager.Device_config.host_name);
    }
}

/**
 * @brief Return a list of available wifi stations nearby.
 * 
 * @return String List of networks in JSON format.
 */
String WifiManager::scanNetworks()
{
    String json;
    bool canScan;
    int n = WiFi.scanComplete();

    _currentMillis = millis();

    if (_currentMillis - _lastScanMillis > SCAN_PERIOD)
    {
        canScan = true;
        Serial.print(F("\n[INFO]: Scanning networks... "));
        _lastScanMillis = _currentMillis;
    }
        json += "{\"networks\": [";
        if(n == -2 && canScan){
        // Scan not triggered, and not in the waiting timer
        WiFi.scanNetworks(true);
    }
    else if(n)
    {
        for (int i = 0; i < n; ++i)
        {
            if(i) json += ",";
            json += "{";
            json += "\"SSID\":\""+WiFi.SSID(i)+"\"";
            json += ",\"Quality\":\""+_rssiToPercent(WiFi.RSSI(i))+"%\"";     
            json += "}";
        }

        WiFi.scanDelete();

        if(WiFi.scanComplete() == -2)
        {
            WiFi.scanNetworks(true);
        }
    }
    json += "] }";

    return json;
}

/**
 * @brief Get device MAC Address.
 * 
 * @return String Device MAC address.
 */
String WifiManager::getMACAddress()
{
    if(WiFi.getMode() == 2)
        return WiFi.softAPmacAddress();

    return WiFi.macAddress();
}

/**
 * @brief Get current connection mode (AP, STA, AP+STA).
 * 
 * @return String Connection mode.
 */
String WifiManager::getMode()
{
    switch (WiFi.getMode())
    {
        case WIFI_STA:
            return "Conectado";
            break;
        case WIFI_AP:
            return "Ponto de Acesso";
            break;
        case WIFI_OFF:
            return "Desligado";
            break;
        case WIFI_AP_STA:
            return "AP+STA";
            break;
        default:
            return "Desconhecido";
            break;
    }
}

/**
 * @brief Get connected SSID (for STA mode) or device Host Name (for AP mode).
 * 
 * @return String Device SSID
 */
String WifiManager::getSSID()
{
    if (WiFi.getMode() == 2)
    {
        return String(configManager.Device_config.host_name);
    } 
    return WiFi.SSID(); 
}

/**
 * @brief Display Wifi mode information.
 * 
 * @return String Wifi information.
 */
String wifiInfo()
{
    String infoText;
    if (WiFi.getMode() == 2 || WiFi.getMode() == 3)
    {
        infoText = "AP: " + WiFi.softAPIP().toString();
        return infoText;
    }

    infoText = "STA: " + WiFi.localIP().toString();
    return infoText;
}

/**
 * @brief WifiManager initialization scritps.
 * 
 */
void WifiManager::begin()
{
    if(!configManager.Device_config.ap_mode)
    {
        if(!_startSTA())
            _startAP(configManager.Device_config.host_name);
    }
    else
    {
        _startAP(configManager.Device_config.host_name);
    }

    _disconnectedEventHandler = WiFi.onStationModeDisconnected([this](const WiFiEventStationModeDisconnected& event)
    {
        Serial.println(PSTR("[WARN] Wifi disconnected from STA."));
        _isDisconnected = true;
    });  
}

/**
 * @brief WifiManager main loop.
 * 
 */
void WifiManager::loop()
{
    if(_isAPMode)
        _dnsServer->processNextRequest();

    if(!_isAPMode && _isDisconnected)
    {
       _retrySTAConnection(RETRY_BEFORE_AP);
    }
}

WifiManager wifiManager;