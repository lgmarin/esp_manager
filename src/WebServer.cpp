#include "WebServer.h"
#include "ConfigManager.h"
#include <LittleFS.h>

void WebServer::_bindServerHandlers()
{
    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse * response = request->beginResponse(LittleFS, "/index.html", "text/html", false);
        request->send(response);
    });

    server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest * request){
        request->redirect("/index.html");
    }).setFilter(ON_AP_FILTER);

    server.on("/fwlink", HTTP_GET, [](AsyncWebServerRequest * request){
        request->redirect("/index.html");
    }).setFilter(ON_AP_FILTER);

    // server.on("/read-sensors", HTTP_GET, [](AsyncWebServerRequest *request){
    //     request->send(200, "application/json", readSensorsJSON());
    // });
}


void WebServer::begin()
{
    _bindServerHandlers();

    Serial.print(F("\n[INFO]: Initializing WebServer..."));
    _server.begin();

    _server.serveStatic("/", LittleFS, "/");
}