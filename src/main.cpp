#include <Arduino.h>
#include <definitions.h>
#include <wifi.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

AsyncWebServer server(HTTP_PORT);

// Request Handlers
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Setup LittleFS
  server.serveStatic("/", LittleFS, "/");

  // Open Captive portal and configure Wifi Credentials
  if (openCaptivePortal())
  {
    //Serve configuration page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/wifi.html", "text/html", false);
    }).setFilter(ON_AP_FILTER);

    server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "application/json", scanNetworks());
    });

    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "application/json", "{\"status\": \"" + String(WiFi.status()) + "\", \"network\": \""+ WiFi.SSID() + "\"}");
    });

    server.on("/connect", HTTP_GET, [] (AsyncWebServerRequest *request) {
      bool result = false;

      if (request->hasParam("auto-ip") && request->hasParam("ssid") && request->hasParam("password")) {
        result = connectToWifi(request->getParam("ssid")->value(), request->getParam("password")->value());
      } 
      else if (request->hasParam("ip") && request->hasParam("gateway") && request->hasParam("ssid") && request->hasParam("password"))
      {
        result = connectToWifi(request->getParam("ssid")->value(), request->getParam("password")->value(),\
                  request->getParam("ip")->value(), request->getParam("gateway")->value());
      }

      if (result)
        request->send(200, "application/json", "{\"status\": \"connected\" }");

      request->send(200, "application/json", "{\"status\": \"error\" }");
    });
  }
  
  server.begin();
}

void loop() {

}