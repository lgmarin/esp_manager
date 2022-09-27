#include "WebServer.h"
#include "ConfigManager.h"
#include "WifiManager.h"

#include <LittleFS.h>

/*
*     HTML PRE-PROCESSORS
*/

String WebServer::_indexPageProcessor(const String& var)
{
    if(var == "HOST_NAME"){
        return wifiManager.getSSID();
    }
    else if(var == "DEVICE_IP"){
        return wifiManager.deviceIP.toString();
    }
    // else if(var == "DHT_T"){
    //     return String(airTemp());
    // }
    // else if(var == "DHT_H"){
    //     return String(airHumidity());
    // }
    // else if(var == "SOIL_H"){
    //     return String(soilHumidity());
    // }
    else if(var == "CONN_STATUS"){
        if (wifiManager.getMode() == "Conectado")
        {
            return "fc1505";
        }
        return "000000";
    }
    return String();
}

String WebServer::_configPageProcessor(const String& var)
{
    if(var == "MODE"){
        return wifiManager.getMode();
    }
    else if(var == "SSID"){
        return wifiManager.getSSID();
    }
        else if(var == "HOST_NAME"){
        return wifiManager.getSSID();
    }
    else if(var == "DEVICE_IP"){
        return wifiManager.deviceIP.toString();
    }
    else if (var == "MAC")
    {
        return wifiManager.getMACAddress();
    }
    else if (var == "AP_MODE")
    {
        if(configManager.Device_config.ap_mode)
        {
            return "checked";
        } 
        else
        {
            return "";
        }
    } 
    return String();
}

String WebServer::_wifiPageProcessor(const String& var)
{
    if(var == "HOST_NAME"){
        return wifiManager.getSSID();
    }
    else if(var == "DEVICE_IP"){
        return wifiManager.deviceIP.toString();
    }
    return String();
}

/*
*    HTTP ROUTES HANDLERS
*/

void WebServer::_notFoundHandler(AsyncWebServerRequest *request)
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += request->url();
    message += "\nMethod: ";
    message += (request->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += request->args();
    message += "\n";

    for (uint8_t i = 0; i < request->args(); i++)
    {
        message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
    }
    request->send(404, "text/plain", message);
}

void WebServer::_bindServerHandlers()
{
    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse * response = request->beginResponse(LittleFS, "/index.html", "text/html", false, _indexPageProcessor);
        request->send(response);
    });

    _server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest * request){
        request->redirect("/index.html");
    }).setFilter(ON_AP_FILTER);

    _server.on("/fwlink", HTTP_GET, [](AsyncWebServerRequest * request){
        request->redirect("/index.html");
    }).setFilter(ON_AP_FILTER);

    // server.on("/read-sensors", HTTP_GET, [](AsyncWebServerRequest *request){
    //     request->send(200, "application/json", readSensorsJSON());
    // });

    _server.onNotFound(_notFoundHandler);
}

/**
 * @brief WebServer initialization.
 */
void WebServer::begin()
{
    _bindServerHandlers();

    Serial.print(F("\n[INFO]: Initializing WebServer..."));
    _server.begin();

    _server.serveStatic("/", LittleFS, "/");
}

WebServer webServer;