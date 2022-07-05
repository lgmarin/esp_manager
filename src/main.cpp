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
} WiFi_Credential;

typedef struct
{
  IPAddress ip_addr;
  IPAddress gw_addr;
  IPAddress mask;
} Ip_Config;

typedef struct
{
  WiFi_Credential  WiFi_Cred;
  Ip_Config Ip_config;
  char host_name[32];
  bool dyn_ip = false;
  bool ap_mode = false;
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
  Serial.print(F("\n[INFO]: Starting soft-AP..."));
  if(WiFi.softAP(host_name)){
    Serial.print(F("\n[SUCCESS]: Captive Portal Started at IP: ")); Serial.print(WiFi.softAPIP());
    if (!dnsServer.start(53, "*", WiFi.softAPIP()))
  		Serial.print(F("\n[INFO]: Failed to start DNS service."));
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
    Serial.print(F("\n[INFO]: Scanning networks... "));
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
    Serial.print(F("\n[ERROR]: Empty SSID or Password."));
    return false;
  }

  Serial.print(F("\n[INFO]: Connecting to station: ")); Serial.print(ssid);

  if (ip != "" && gw != "" && mask != "")
  {
    IPAddress wifi_ip;
    IPAddress wifi_gw;
    IPAddress wifi_mask;
    if (wifi_ip.fromString(ip) && wifi_gw.fromString(gw) && wifi_mask.fromString(mask))
    {
      if(!WiFi.config(wifi_ip, wifi_gw, wifi_mask)){
        Serial.print(F("\n[ERROR]: Unable to configure wifi. Using Dynamic IP."));
      }
      Serial.print(F("\n[INFO]: Using static IP..."));
    }
  }
  WiFi.disconnect();
  delay(500);

  WiFi.mode(WIFI_STA);

  Serial.print(F("\n[INFO]: Connecting to WiFi..."));
  WiFi.begin(ssid, password);
  
  if(WiFi.waitForConnectResult(WIFI_RETRY_TIMEOUT) != WL_CONNECTED) {
    Serial.print(F("\n[ERROR]: Failed to connect."));
    return false;
  }
  Serial.print(F("\n[SUCCESS]: CONNECTED: Mode: STA, SSID: ")); Serial.print(WiFi.SSID());
  Serial.print(F(" IP: ")); Serial.print(WiFi.localIP());
  return true;
}

bool configuremDNS()
{
  if (!MDNS.begin(host_name)) {
    Serial.print(F("\n[ERROR]: mDNS service error."));
    return false;
  }
  // Add Web Server service to mDNS
  MDNS.addService("http", "tcp", 80);
  Serial.print(F("\n[INFO]: mDNS service started. Host: ")); Serial.print(host_name);
  return true;
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
  dnsServer.processNextRequest();
}