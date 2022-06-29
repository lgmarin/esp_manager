#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

AsyncWebServer server(80);
DNSServer dnsServer;

const char* device = "esp-manager";
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Request Handlers
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void openCaptivePortal(){
  WiFi.softAP(device);
  dnsServer.start(53, "*", WiFi.softAPIP());
}

void setup() {
  Serial.begin(115200);

  //
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm");

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}