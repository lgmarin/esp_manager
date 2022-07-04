# ESP Manager

## _ESP8266 Wifi and Configuration Manager boilerplate project_
[![ESP8266](https://img.shields.io/badge/ESP-8266-blue.svg)](https://github.com/esp8266/esp8266-wiki)
[![C++](https://img.shields.io/badge/C-++-blue.svg)]()
[![PlatformIO](https://img.shields.io/badge/Platform-IO-blue.svg)](https://platformio.org/)

<p align="center">
  <img width="250" src="https://raw.githubusercontent.com/lgmarin/esp_manager/main/img/wifi_mgr.png" alt="Material Bread logo">
</p>


## Objectives

Development of a boiler plate project with integrated WifiManager and Configuration Manager:

* Allow for easy connection and configuration for different projects
* Allow for Access Point (AP) or Station (STA) modes - configurable
* AP mode with captive portal
* STA mode with mDNS provider for easy navigation (project_name.local)

## Libraries

* [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) - Base library for ESPAsyncWebServer.
* [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) - An Async Web Server implementation for the ESP8266.
* [ESPAsync_WiFiManager](https://github.com/khoih-prog/ESPAsync_WiFiManager) - An Async Wifi Manager to setup the Wifi Information.

## References

* [Wifi Manager - Configure Wifi Connection](https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/)
* [Async WifiManager - AsyncWebServer](https://randomnerdtutorials.com/esp8266-nodemcu-wi-fi-manager-asyncwebserver/)
* [Async WifiManager](https://stonez56.blogspot.com/2021/07/asyncwifimanager-elegantota-esp8266.html)
* [Material Design Lite Pages](https://www.luisllamas.es/material-design-esp8266/)
* [Scan WIFI](https://www.engineersgarage.com/esp8266-post-data-to-thingspeak-server/)
* [Rest Server](https://www.mischianti.org/2020/05/24/rest-server-on-esp8266-and-esp32-get-and-json-formatter-part-2/)
* [AsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer#setup-event-source-in-the-browser)
* [WIFI Connect](https://github.com/ryanamaral/wifi-connect-esp8266/tree/master/html-preview)