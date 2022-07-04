#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#define WIFI_RETRY_COUNT  3
#define WIFI_RETRY_TIMEOUT 5000

typedef struct
{
  char wifi_ssid[32];
  char wifi_pw  [64];
} WiFi_Credentials;

typedef struct
{
  WiFi_Credentials  WiFi_Creds;
  char host_name[32];
  uint16_t checksum;
} ESP_Config;

ESP_Config  ESP_config;

AsyncWebServer server(80);
DNSServer dnsServer;

const char* host_name = "esp-manager";
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Request Handlers
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

bool openCaptivePortal(){
  Serial.print("Starting soft-AP ... ");
  if(WiFi.softAP(host_name)){
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

bool connectToWifi(String ssid, String pwd, String ip = "", String gw = "", String mask = "") 
{
  if (ssid == "" && ssid == "")
  {
    Serial.println("\nERROR: Undefined SSID and Password.");
    return false;
  }

  Serial.println("Connecting to station: ");
  Serial.print(ssid);
  if (ip != "" && gw != "" && mask != "")
  {
    IPAddress wifi_ip;
    IPAddress wifi_gw; 
    IPAddress wifi_mask; 
    if (wifi_ip.fromString(ip) && wifi_gw.fromString(gw) && wifi_mask.fromString(mask))
    {
      Serial.println("Using static IP...");
      if(!WiFi.config(wifi_ip, wifi_gw, wifi_mask)){
        Serial.println("\nERROR: Couldn't configure Wifi.");
        return false;
      }
    }
  }
  WiFi.disconnect();
  delay(500);

  WiFi.mode(WIFI_STA);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  if(WiFi.waitForConnectResult(WIFI_RETRY_TIMEOUT) != WL_CONNECTED) {
    Serial.println("Failed to connect.");
    return false;
  }
  Serial.print("\nCONNECTED: Mode: STA, IP: ");
  Serial.println(WiFi.localIP());
  return true;
}

bool configuremDNS()
{
  if (!MDNS.begin(host_name)) {
    Serial.print(F("\n[ERROR]: MultiWiFi Configuration..."));
    return false;
  }
  // Add Web Server service to mDNS
  MDNS.addService("http", "tcp", 80);
  Serial.print(F("\n[INFO]: mDNS service started."));
  return true;
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
        if (request->hasParam("auto-ip") && request->hasParam("ssid") && request->hasParam("password")) {
          connectToWifi(request->getParam("ssid")->value(), request->getParam("password")->value());
        } else if (request->hasParam("ip") && request->hasParam("gateway") && request->hasParam("ssid") && request->hasParam("password"))
        {
          connectToWifi(request->getParam("ssid")->value(), request->getParam("password")->value(),\
                        request->getParam("ip")->value(), request->getParam("gateway")->value());
        }
        request->send(200, "application/json", "{\"status\": \"connecting\" }");
      });
  }
  
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}