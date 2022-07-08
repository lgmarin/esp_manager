#ifndef wifi_h
#define wifi_h
#include <definitions.h>
#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

long lastScanMillis;
long currentMillis;

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
} Wifi_Config;

// GLOBAL VARIABLES
extern Wifi_Config  Wifi_config;
extern const char* host_name = "esp-manager";
extern const char* ssid;
extern const char* password;

// PUBLIC FUNCTIONS
bool openCaptivePortal();
String scanNetworks();
bool configuremDNS();
bool connectToWifi(String ssid, String pwd, String ip = "", String gw = "", String mask = "");
void dnsProcessNext();

#endif