#include <Servo.h>
#include "Esp8266Otto.h"
#include "SerialLogger.h"
#include "ControllerServer.h"

EspOtto otto;

// 配置参数
const char *apSSID = "ESP12E_Scale";
const char *apPassword = "12345678";
const char *configPath = "/config";
const char *savePath = "/save";

// 网络服务器和DNS服务器
AsyncWebServer server(80);

// 创建WebSocket服务器路径为/ws
AsyncWebSocket ws("/ws");

DNSServer dnsServer;

void setup()
{
  Serial.begin(9600);

  otto.init();
  otto.home();

  // 设置AP模式的固定IP（关键修改点）
  IPAddress localIP(192, 168, 1, 100);  // AP的IP地址
  IPAddress subnet(255, 255, 255, 0);   // 子网掩码
  WiFi.softAPConfig(localIP, localIP, subnet);  // AP模式的IP配置函数

  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPassword);

  // 设置HTTP服务器路由
  dnsServer.start(53, "*", WiFi.softAPIP());

  // 初始化WebSocket
  // ws.onEvent(onWsEvent);
  // 处理 WebSocket 事件
  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, 
                AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket 客户端 #%u 已连接\n", client->id());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket 客户端 #%u 已断开连接\n", client->id());
        break;
      case WS_EVT_DATA:
        // 处理接收到的数据
        Serial.printf("收到数据: %.*s\n", (int)len, (char*)data);
        // 回显数据给客户端
        client->text("已收到: " + String((char*)data));
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
    }
  });
  server.addHandler(&ws);

  // 设置HTTP服务器路由
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->redirect(configPath); });

  server.on(configPath, HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String page = generateConfigPage();
    request->send(200, "text/html", page); });

  // 启动服务器
  server.begin();

  Serial.println("AP模式已启动");
  Serial.print("Web界面地址: http://");
  Serial.println(WiFi.softAPIP());
}

String generateConfigPage()
{
  // 生成WiFi配置HTML页面
  String page = R"(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP12E电子秤配置</title>
      <style>
        body { font-family: Arial; max-width: 400px; margin: 0 auto; padding: 20px; }
        h1 { text-align: center; }
        form { background: #f9f9f9; padding: 20px; border-radius: 5px; }
        input { width: 100%; padding: 10px; margin: 8px 0; border: 1px solid #ddd; border-radius: 4px; }
        button { width: 100%; background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; border-radius: 4px; cursor: pointer; }
        button:hover { background-color: #45a049; }
      </style>
    </head>
    <body>
      <h1>ESP12E电子秤配置</h1>
      <form action="/save" method="get">
        <label for="ssid">家庭WiFi名称:</label>
        <input type="text" id="ssid" name="ssid" required>
        
        <label for="password">WiFi密码:</label>
        <input type="password" id="password" name="password" required>
        
        <button type="submit">保存配置</button>
      </form>
    </body>
    </html>
  )";
  return page;
}

void loop()
{
}