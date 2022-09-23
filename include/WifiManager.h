#ifndef WIFIMNG_H
#define WIFIMNG_H

#include "definitions.h"

#include <DNSServer.h>

class WifiManager
{
private:
    DNSServer *dnsServer;

    bool _setStaticIp();
    bool _startAP();
    bool _startSTA();

public:
    void begin();
};

#endif