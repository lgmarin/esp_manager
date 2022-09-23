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
    bool _staticIP;
    bool _isAPMode;
    String _rssiToPercent(int32_t RSSI);

public:
    IPAddress deviceIP;
    void begin();
    void loop();
    String scanNetworks(); 
};

extern WifiManager wifiManager;

#endif