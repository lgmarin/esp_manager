#ifndef WIFIMNG_H
#define WIFIMNG_H

#include "definitions.h"
#include <DNSServer.h>
#include <ESP8266WiFi.h>

class WifiManager
{
private:
    DNSServer *_dnsServer;
    WiFiEventHandler _disconnectedEventHandler;
    bool _setStaticIp();
    bool _startAP(const char* ap_name);
    bool _startSTA();
    void _finishAP();
    String _rssiToPercent(int32_t RSSI);
    void _retrySTAConnection(uint8_t retry_count);
    bool _staticIP;
    bool _isAPMode;
    bool _isDisconnected;
    unsigned long _currentMillis;
    unsigned long _lastScanMillis;

public:
    IPAddress deviceIP;
    void begin();
    void loop();
    String scanNetworks();
    String getMACAddress();
    String getMode();
    String getSSID();
};

extern WifiManager wifiManager;

#endif