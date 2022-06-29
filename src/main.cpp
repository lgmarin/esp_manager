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

bool openCaptivePortal(){
  Serial.print("Starting soft-AP ... ");
  if(WiFi.softAP(device)){
    Serial.println("SofAP Started!");
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());
    dnsServer.start(53, "*", WiFi.softAPIP());
    return true;
  }
  return false;
}

String scanNetworks() {
  String json;
  int n = WiFi.scanComplete();

  json +=  "[";
  //json += "{\"networks\": [";
  if(n == -2){
    // Scan not triggered, start scanning
    WiFi.scanNetworks(true);
  }else if(n){
    for (int i = 0; i < n; ++i){
      if(i) json += ",";
      json += "{";
      json += "\"ssid\":"+WiFi.SSID(i);
      json += ",\"quality\":\""+String(WiFi.RSSI(i))+"\"";     
      json += "}";
    }
    WiFi.scanDelete();
    if(WiFi.scanComplete() == -2){
      WiFi.scanNetworks(true);
    }
  }
  json += "]";
  //json += "] }";

  return json;
}

void setup() {
  Serial.begin(115200);

  // Setup LittleFS
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm");

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
  }
  
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}