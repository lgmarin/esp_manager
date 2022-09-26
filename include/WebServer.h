#ifndef WEBSERV_H
#define WEBSERV_H

#include "definitions.h"
#include <ESPAsyncWebServer.h>


class WebServer
{
private:
    AsyncWebServer _server = AsyncWebServer(80);
public:
    void begin();
    void loop();
};

#endif