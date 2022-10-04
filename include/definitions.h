#ifndef definitions_h
#define definitions_h

#include <Arduino.h>

#define DEBUG                   true

/*
*           WIFI DEFINITIONS
*/
#define WIFI_RETRY_COUNT        3
#define WIFI_RETRY_TIMEOUT      6000
#define RETRY_BEFORE_AP         3
#define SCAN_PERIOD             3500
#define DEFAULT_HOSTNAME        F("ESP_MANAGER")
#define HTTP_PORT               80
#define DEFAULT_TO_AP           true


//       DISPLAY CONFIGURATION
// PINOUT I2C               
// SDA                          GPIO4 - D2
// SCK                          GPIO5 - D1
#define SCREEN_W                128
#define SCREEN_H                64
#define D_I2C_ADDR              0x3C    //0x7A //0x78
#define SLEEP_INTERVAL          60000   //1 minute to sleep
#define WAKE_BTN_PIN            14      //GPIO14 - D14