#ifndef ControllerServer_h
#define ControllerServer_h

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <DNSServer.h> // 添加 DNSServer 头文件

class ControllerServer
{
public:
    ControllerServer(); // 添加构造函数声明
    void setWiFiApMode(const char *ssid, const char *password);
    void setWiFi(const char *ssid, const char *password);
    void connectWiFi();
    void initServer();
    String controllerPage();

    AsyncWebServer *getServer()
    {
        return &server;
    }

private:
    const char *ssid;
    const char *password;
    const char *wifiSsid;
    const char *wifiPassword;
    DNSServer dnsServer;
    AsyncWebServer server;
};
#endif