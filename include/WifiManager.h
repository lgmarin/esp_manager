#ifndef WIFIMNG_H
#define WIFIMNG_H

#include "definitions.h"

#include <DNSServer.h>

class WifiManager
{
private:
    DNSServer *dnsServer;

    bool _setStaticIp();
    bool _startAP(const char* ap_name);
    bool _startSTA();
    void _finishAP();
    
public:
    IPAddress deviceIP;
    bool staticIP;
    bool isAPMode;
    void begin();
};

#endif