#ifndef WEBSERV_H
#define WEBSERV_H

#include "definitions.h"
#include <ESPAsyncWebServer.h>

class WebServer
{
private:
    AsyncWebServer _server = AsyncWebServer(80);
    static String _indexPageProcessor(const String& var);
    static String _configPageProcessor(const String& var);
    static String _wifiPageProcessor(const String& var);
    static void _notFoundHandler(AsyncWebServerRequest *request);
    void _bindServerHandlers();
public:
    void begin();
    void loop();
};

#endif