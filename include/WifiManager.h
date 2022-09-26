#ifndef WIFIMNG_H
#define WIFIMNG_H

#include "definitions.h"
#include <DNSServer.h>

class WifiManager
{
private:
    DNSServer *_dnsServer;
    bool _setStaticIp();
    bool _startAP(const char* ap_name);
    bool _startSTA();
    void _finishAP();
    String _rssiToPercent(int32_t RSSI);
    void _retrySTAConnection();
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
};

extern WifiManager wifiManager;

#endif