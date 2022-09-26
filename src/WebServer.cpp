#include "WebServer.h"
#include "ConfigManager.h"
#include <LittleFS.h>


void WebServer::_bindServerHandlers()
{
    _server.on("/", HTTP_GET, []AsyncWebServerRequest *request)
    {
        AsyncWebServerResponse * response = request->beginResponse(LittleFS, "/index.html", "text/html", false);
        request->send(response);
    }
}


void WebServer::begin()
{
    _bindServerHandlers();

    Serial.print(F("\n[INFO]: Initializing WebServer..."));
    server.begin();

    server.serveStatic("/", LittleFS, "/");
}