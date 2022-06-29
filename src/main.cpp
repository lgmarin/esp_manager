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

#define SCAN_PERIOD 3500
long lastScanMillis;
long currentMillis;

String scanNetworks() {
  String json;
  bool canScan;
  int n = WiFi.scanComplete();

  if (currentMillis - lastScanMillis > SCAN_PERIOD)
  {
    canScan = true;
    Serial.print("\nScan interval ... ");
    lastScanMillis = currentMillis;
  }

  //json +=  "[";
  json += "{\"networks\": [";
  if(n == -2 && canScan){
    // Scan not triggered, and not in the waiting timer
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
  //json += "]";
  json += "] }";

  return json;
}

void connectToWifi(String ssid, String pwd, String ip = "", String gw = "", String mask = "") {
  
  Serial.println("Connecting to station: ");
  Serial.print(ssid);
  if (ip == "" && gw == "")
  {
    IPAddress wifi_ip;
    IPAddress wifi_gw; 
    IPAddress wifi_mask; 
    if (wifi_ip.fromString(ip) && wifi_gw.fromString(gw), wifi_mask.fromString(mask))
    {
      Serial.println("Using dynamic IP...");
      WiFi.config(wifi_ip, wifi_gw, wifi_mask);
    }
  }
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  Serial.println(WiFi.waitForConnectResult());
}

void setup() {
  Serial.begin(115200);

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
        if (request->hasParam("auto-ip") && request->hasParam("wifi-ssid") && request->hasParam("wifi-password")) {
          connectToWifi(request->getParam("wifi-ssid")->value(), request->getParam("wifi-password")->value());
        } else if (request->hasParam("wifi-ip") && request->hasParam("wifi-gateway") && request->hasParam("wifi-ssid") && request->hasParam("wifi-password"))
        {
          connectToWifi(request->getParam("wifi-ssid")->value(), request->getParam("wifi-password")->value(),\
                        request->getParam("wifi-ip")->value(), request->getParam("wifi-gateway")->value());
        }
        request->send(200, "application/json", "{\"status\": \"connecting\" }");
      });
  }
  
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}