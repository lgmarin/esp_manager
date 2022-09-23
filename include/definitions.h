#ifndef definitions_h
#define definitions_h

#include <Arduino.h>

#define DEBUG                   true

/*
*           WIFI DEFINITIONS
*/
#define WIFI_RETRY_COUNT        3
#define WIFI_RETRY_TIMEOUT      6000
#define SCAN_PERIOD             3500
#define DEFAULT_HOSTNAME        F("ESP_MANAGER")
#define HTTP_PORT               80
#define DEFAULT_TO_AP           true

#endif