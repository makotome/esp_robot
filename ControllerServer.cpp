#include "ControllerServer.h"
#include "ControllerHtml.h"

ControllerServer::ControllerServer() : server(80) // 初始化 AsyncWebServer 在端口 80
{
    // 如果有其他成员需要初始化，可以在这里进行
}

void ControllerServer::setWiFiApMode(const char *ssid, const char *password)
{

    // 设置AP模式的固定IP（关键修改点）
    IPAddress localIP(192, 168, 1, 100);         // AP的IP地址
    IPAddress subnet(255, 255, 255, 0);          // 子网掩码
    WiFi.softAPConfig(localIP, localIP, subnet); // AP模式的IP配置函数

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    dnsServer.start(53, "*", WiFi.softAPIP());
}

void ControllerServer::setWiFi(const char *ssid, const char *password)
{
    wifiSsid = ssid;
    wifiPassword = password;
}

void ControllerServer::connectWiFi()
{
    WiFi.begin(wifiSsid, wifiPassword);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
}

void ControllerServer::initServer()
{
    // 设置HTTP服务器路由
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->redirect("/controller"); });

    server.on("/controller", HTTP_GET, [this](AsyncWebServerRequest *request)
              {
      String page = this->controllerPage();
      request->send(200, "text/html", page); });
}

String ControllerServer::test()
{
    String page = R"(
    <!DOCTYPE html>
<html>
<head>
  <title>ESP8266 WebSocket</title>
</head>
<body>
  <h1>ESP8266 WebSocket 测试</h1>
  <div id="messages"></div>
  <input type="text" id="message" placeholder="输入消息">
  <button onclick='sendMessage()' > 发送</ button>

                  <script>
                  // 创建 WebSocket 连接
                  const socket = new WebSocket('ws://' + location.hostname + ':80/ws');

    // 连接成功时触发
    socket.onopen = function()
    {
        document.getElementById('messages').innerHTML += '<p>已连接到服务器</p>';
    };

    // 收到消息时触发
    socket.onmessage = function(event)
    {
        document.getElementById('messages').innerHTML += '<p>' + event.data + '</p>';
    };

    // 连接关闭时触发
    socket.onclose = function()
    {
        document.getElementById('messages').innerHTML += '<p>连接已关闭</p>';
    };

    // 发送消息
    function sendMessage()
    {
        const message = document.getElementById('message').value;
        if (message)
        {
            socket.send(message);
            document.getElementById('message').value = '';
        }
    }
  </script>
</body>
</html>
  )";
}
String ControllerServer::controllerPage()
{
    // 生成WiFi配置HTML页面
    String page = R"(
    <!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP8266机器人遥控器</title>
  <script src="https://cdn.tailwindcss.com"></script>
  <link href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.7.2/css/all.min.css" rel="stylesheet">
  
  <!-- 配置Tailwind自定义颜色 -->
  <script>
    tailwind.config = {
      theme: {
        extend: {
          colors: {
            primary: '#165DFF',
            secondary: '#36D399',
            danger: '#F87272',
            warning: '#FBBD23',
            dark: '#1E293B',
            remote: {
              50: '#F8FAFC',
              100: '#F1F5F9',
              200: '#E2E8F0',
              300: '#CBD5E1',
              400: '#94A3B8',
              500: '#64748B',
              600: '#475569',
              700: '#334155',
              800: '#1E293B',
              900: '#0F172A',
            }
          },
          fontFamily: {
            sans: ['Inter', 'system-ui', 'sans-serif'],
          },
        },
      }
    }
  </script>
  
  <style type="text/tailwindcss">
    @layer utilities {
      .content-auto {
        content-visibility: auto;
      }
      .btn-direction {
        @apply bg-gradient-to-br from-remote-700 to-remote-800 text-white font-bold 
               py-5 px-5 rounded-lg shadow-lg transform transition-all duration-200 
               hover:scale-105 hover:shadow-xl active:scale-95 active:bg-remote-900 
               focus:outline-none focus:ring-2 focus:ring-primary/50;
      }
      .btn-center {
        @apply bg-gradient-to-br from-primary to-primary/80 text-white font-bold 
               py-5 px-5 rounded-lg shadow-lg transform transition-all duration-200 
               hover:scale-105 hover:shadow-xl active:scale-95 active:bg-primary/90 
               focus:outline-none focus:ring-2 focus:ring-primary/50;
      }
      .btn-function {
        @apply bg-gradient-to-br from-remote-600 to-remote-700 text-white font-bold 
               py-3 px-4 rounded-lg shadow-lg transform transition-all duration-200 
               hover:scale-105 hover:shadow-xl active:scale-95 active:bg-remote-800 
               focus:outline-none focus:ring-2 focus:ring-primary/50;
      }
      .btn-number {
        @apply bg-gradient-to-br from-remote-500 to-remote-600 text-white font-bold 
               py-3 px-4 rounded-lg shadow-lg transform transition-all duration-200 
               hover:scale-105 hover:shadow-xl active:scale-95 active:bg-remote-700 
               focus:outline-none focus:ring-2 focus:ring-primary/50;
      }
      .remote-container {
        @apply bg-gradient-to-br from-remote-800 to-remote-900 rounded-3xl shadow-2xl 
               p-6 max-w-md mx-auto border border-remote-700 overflow-hidden;
      }
      .remote-header {
        @apply text-center mb-6 pb-4 border-b border-remote-700;
      }
      .remote-display {
        @apply bg-remote-900 rounded-xl p-4 mb-6 shadow-inner;
      }
      .status-indicator {
        @apply h-3 w-3 rounded-full bg-gray-500 inline-block 
               transition-colors duration-300;
      }
      .status-indicator.connected {
        @apply bg-green-500;
      }
      .direction-pad {
        @apply grid grid-cols-3 grid-rows-3 gap-2 mb-8;
      }
      .number-pad {
        @apply grid grid-cols-3 gap-3 mb-8;
      }
      .function-pad {
        @apply grid grid-cols-4 gap-3;
      }
      .battery-indicator {
        @apply h-2 w-16 bg-remote-700 rounded-full overflow-hidden inline-block;
      }
      .battery-level {
        @apply h-full bg-green-500 rounded-full transition-all duration-500;
      }
    }
  </style>
</head>
<body class="bg-gradient-to-br from-remote-50 to-remote-200 min-h-screen flex flex-col items-center justify-center p-4">
  <div class="max-w-md w-full">
    <!-- 遥控器容器 -->
    <div class="remote-container">
      <!-- 头部区域 -->
      <div class="remote-header">
        <h1 class="text-2xl font-bold text-white mb-2">ESP8266机器人遥控器</h1>
        <div class="flex justify-between items-center">
          <div class="flex items-center">
            <span class="status-indicator mr-2" id="connectionStatus"></span>
            <span class="text-remote-300 text-sm" id="connectionText">未连接</span>
          </div>
          <div class="flex items-center">
            <i class="fa-solid fa-battery-three-quarters text-remote-300 mr-1"></i>
            <div class="battery-indicator">
              <div class="battery-level" id="batteryLevel" style="width: 80%"></div>
            </div>
          </div>
        </div>
      </div>
      
      <!-- 显示屏 -->
      <div class="remote-display">
        <div class="flex justify-between mb-2">
          <span class="text-remote-400 text-xs">IP地址</span>
          <span class="text-remote-400 text-xs">连接时间</span>
        </div>
        <div class="flex justify-between mb-3">
          <span class="text-white text-sm font-mono" id="robotIP">--</span>
          <span class="text-white text-sm" id="connectionTime">--</span>
        </div>
        
        <div class="text-center mt-4">
          <h2 class="text-xl font-bold text-white mb-1">当前状态</h2>
          <div class="bg-remote-800 rounded-lg p-3 mt-2">
            <div class="flex justify-center items-center">
              <div class="robot-eyes mr-3" id="robotEyes">
                <div class="h-3 w-3 bg-white rounded-full inline-block mx-1"></div>
                <div class="h-3 w-3 bg-white rounded-full inline-block mx-1"></div>
              </div>
              <span class="text-2xl font-bold text-white" id="robotStatus">就绪</span>
            </div>
          </div>
        </div>
      </div>
      
      <!-- 方向键区 -->
      <div class="direction-pad mb-8">
        <div class="col-start-2 row-start-1 flex items-center justify-center">
          <button id="forwardBtn" class="btn-direction w-full h-full flex items-center justify-center">
            <i class="fa-solid fa-arrow-up text-xl"></i>
          </button>
        </div>
        <div class="col-start-1 row-start-2 flex items-center justify-center">
          <button id="leftBtn" class="btn-direction w-full h-full flex items-center justify-center">
            <i class="fa-solid fa-arrow-left text-xl"></i>
          </button>
        </div>
        <div class="col-start-2 row-start-2 flex items-center justify-center">
          <button id="stopBtn" class="btn-center w-full h-full flex items-center justify-center">
            <i class="fa-solid fa-stop text-xl"></i>
          </button>
        </div>
        <div class="col-start-3 row-start-2 flex items-center justify-center">
          <button id="rightBtn" class="btn-direction w-full h-full flex items-center justify-center">
            <i class="fa-solid fa-arrow-right text-xl"></i>
          </button>
        </div>
        <div class="col-start-2 row-start-3 flex items-center justify-center">
          <button id="backwardBtn" class="btn-direction w-full h-full flex items-center justify-center">
            <i class="fa-solid fa-arrow-down text-xl"></i>
          </button>
        </div>
      </div>
      
      <!-- 数字键区 -->
      <div class="number-pad mb-8">
        <button id="btn1" class="btn-number">1</button>
        <button id="btn2" class="btn-number">2</button>
        <button id="btn3" class="btn-number">3</button>
        <button id="btn4" class="btn-number">4</button>
        <button id="btn5" class="btn-number">5</button>
        <button id="btn6" class="btn-number">6</button>
        <button id="btn7" class="btn-number">7</button>
        <button id="btn8" class="btn-number">8</button>
        <button id="btn9" class="btn-number">9</button>
      </div>
      
      <!-- 功能键区 -->
      <div class="function-pad">
        <button id="powerBtn" class="btn-function bg-gradient-to-br from-danger to-danger/80">
          <i class="fa-solid fa-power-off mr-1"></i> 电源
        </button>
        <button id="danceBtn" class="btn-function">
          <i class="fa-solid fa-music mr-1"></i> 跳舞
        </button>
        <button id="happyBtn" class="btn-function">
          <i class="fa-solid fa-face-smile mr-1"></i> 开心
        </button>
        <button id="sleepBtn" class="btn-function">
          <i class="fa-solid fa-bed mr-1"></i> 睡觉
        </button>
      </div>
    </div>
    
    <!-- 页脚 -->
    <div class="text-center text-remote-500 text-xs mt-4">
      <p>ESP8266远程控制 &copy; 2023</p>
    </div>
  </div>

  <script>
    // WebSocket连接
    const ws = new WebSocket(`ws://${window.location.host}/ws`);
    let connectionTime = null;
    let batteryLevel = 80; // 模拟电池电量
    
    // 更新连接状态
    function updateConnectionStatus(connected) {
      const statusIndicator = document.getElementById('connectionStatus');
      const connectionText = document.getElementById('connectionText');
      const connectionTimeEl = document.getElementById('connectionTime');
      const robotIPEl = document.getElementById('robotIP');
      
      if (connected) {
        statusIndicator.classList.add('connected');
        connectionText.textContent = '已连接';
        connectionText.classList.add('text-green-500');
        connectionTime = new Date();
        connectionTimeEl.textContent = connectionTime.toLocaleTimeString();
        robotIPEl.textContent = window.location.hostname;
      } else {
        statusIndicator.classList.remove('connected');
        connectionText.textContent = '未连接';
        connectionText.classList.remove('text-green-500');
        connectionTimeEl.textContent = '--';
        robotIPEl.textContent = '--';
      }
    }
    
    // 更新机器人状态和表情
    function updateRobotStatus(status) {
      const statusEl = document.getElementById('robotStatus');
      const robotEyes = document.getElementById('robotEyes');
      
      // 重置所有表情类
      robotEyes.innerHTML = `
        <div class="h-3 w-3 bg-white rounded-full inline-block mx-1"></div>
        <div class="h-3 w-3 bg-white rounded-full inline-block mx-1"></div>
      `;
      
      // 更新状态文本
      statusEl.textContent = status;
      
      // 更新表情
      switch(status) {
        case '前进':
          robotEyes.innerHTML = `
            <div class="h-3 w-3 bg-white rounded-full inline-block mx-1 transform scale-x-75"></div>
            <div class="h-3 w-3 bg-white rounded-full inline-block mx-1 transform scale-x-75"></div>
          `;
          break;
        case '后退':
          robotEyes.innerHTML = `
            <div class="h-3 w-3 bg-white rounded-full inline-block mx-1 transform scale-x-75"></div>
            <div class="h-3 w-3 bg-white rounded-full inline-block mx-1 transform scale-x-75"></div>
          `;
          break;
        case '左转':
          robotEyes.innerHTML = `
            <div class="h-3 w-1 bg-white rounded-full inline-block mx-1 transform scale-y-75"></div>
            <div class="h-3 w-3 bg-white rounded-full inline-block mx-1"></div>
          `;
          break;
        case '右转':
          robotEyes.innerHTML = `
            <div class="h-3 w-3 bg-white rounded-full inline-block mx-1"></div>
            <div class="h-3 w-1 bg-white rounded-full inline-block mx-1 transform scale-y-75"></div>
          `;
          break;
        case '跳舞':
          robotEyes.innerHTML = `
            <div class="h-1 w-3 bg-white rounded-full inline-block mx-1"></div>
            <div class="h-1 w-3 bg-white rounded-full inline-block mx-1"></div>
          `;
          setInterval(() => {
            const eyes = robotEyes.querySelectorAll('div');
            eyes.forEach(eye => {
              eye.classList.toggle('scale-y-0');
            });
          }, 300);
          break;
        case '开心':
          robotEyes.innerHTML = `
            <div class="h-1 w-3 bg-white rounded-full inline-block mx-1 transform rotate-[-45deg]"></div>
            <div class="h-1 w-3 bg-white rounded-full inline-block mx-1 transform rotate-[45deg]"></div>
          `;
          break;
        case '睡觉':
          robotEyes.innerHTML = `
            <div class="h-1 w-3 bg-white rounded-full inline-block mx-1"></div>
            <div class="h-1 w-3 bg-white rounded-full inline-block mx-1"></div>
          `;
          break;
        case '就绪':
          // 默认表情
          break;
      }
    }
    
    // 发送控制命令
    function sendCommand(command) {
      if (ws.readyState === WebSocket.OPEN) {
        const message = JSON.stringify({ command });
        ws.send(message);
        updateRobotStatus(command);
        
        // 模拟电池消耗
        if (batteryLevel > 0) {
          batteryLevel -= 1;
          document.getElementById('batteryLevel').style.width = `${batteryLevel}%`;
          
          // 低电量警告
          if (batteryLevel < 20) {
            document.getElementById('batteryLevel').className = 'battery-level bg-red-500 rounded-full transition-all duration-500';
          } else if (batteryLevel < 50) {
            document.getElementById('batteryLevel').className = 'battery-level bg-yellow-500 rounded-full transition-all duration-500';
          } else {
            document.getElementById('batteryLevel').className = 'battery-level bg-green-500 rounded-full transition-all duration-500';
          }
        }
        
        // 添加按钮动画效果
        const button = document.getElementById(`${command.toLowerCase()}Btn`);
        if (button) {
          button.classList.add('scale-95');
          setTimeout(() => {
            button.classList.remove('scale-95');
          }, 200);
        }
      }
    }
    
    // WebSocket事件处理
    ws.onopen = () => {
      console.log('WebSocket连接已建立');
      updateConnectionStatus(true);
      updateRobotStatus('就绪');
    };
    
    ws.onclose = () => {
      console.log('WebSocket连接已关闭');
      updateConnectionStatus(false);
      updateRobotStatus('连接丢失');
      
      // 尝试自动重连
      setTimeout(() => {
        location.reload();
      }, 3000);
    };
    
    ws.onerror = (error) => {
      console.error('WebSocket错误:', error);
      updateRobotStatus('连接错误');
    };
    
    // 按钮事件监听 - 方向控制
    document.getElementById('forwardBtn').addEventListener('click', () => sendCommand('前进'));
    document.getElementById('backwardBtn').addEventListener('click', () => sendCommand('后退'));
    document.getElementById('leftBtn').addEventListener('click', () => sendCommand('左转'));
    document.getElementById('rightBtn').addEventListener('click', () => sendCommand('右转'));
    document.getElementById('stopBtn').addEventListener('click', () => sendCommand('停止'));
    
    // 按钮事件监听 - 功能控制
    document.getElementById('danceBtn').addEventListener('click', () => sendCommand('跳舞'));
    document.getElementById('happyBtn').addEventListener('click', () => sendCommand('开心'));
    document.getElementById('sleepBtn').addEventListener('click', () => sendCommand('睡觉'));
    document.getElementById('powerBtn').addEventListener('click', () => sendCommand('停止'));
    
    // 数字键功能映射
    document.getElementById('btn1').addEventListener('click', () => sendCommand('左转'));
    document.getElementById('btn2').addEventListener('click', () => sendCommand('后退'));
    document.getElementById('btn3').addEventListener('click', () => sendCommand('右转'));
    document.getElementById('btn4').addEventListener('click', () => { /* 保留 */ });
    document.getElementById('btn5').addEventListener('click', () => sendCommand('停止'));
    document.getElementById('btn6').addEventListener('click', () => { /* 保留 */ });
    document.getElementById('btn7').addEventListener('click', () => { /* 保留 */ });
    document.getElementById('btn8').addEventListener('click', () => sendCommand('前进'));
    document.getElementById('btn9').addEventListener('click', () => { /* 保留 */ });
    
    // 添加眨眼动画
    setInterval(() => {
      if (document.getElementById('robotStatus').textContent === '就绪') {
        const eyes = document.querySelectorAll('#robotEyes div');
        eyes.forEach(eye => {
          eye.classList.add('scale-y-0');
          setTimeout(() => eye.classList.remove('scale-y-0'), 150);
        });
      }
    }, 3000);
  </script>
</body>
</html>
  )";
    return page;
}

// // 替换为你的网络信息
// const char *ssid = "你的WiFi名称";
// const char *password = "你的WiFi密码";

// // 定义电机控制引脚
// const int motor1Pin1 = D1; // 左电机
// const int motor1Pin2 = D2;
// const int motor2Pin1 = D3; // 右电机
// const int motor2Pin2 = D4;

// // 创建异步Web服务器，监听80端口
// AsyncWebServer server(80);
// // 创建WebSocket服务器，路径为/ws
// AsyncWebSocket ws("/ws");

// // 初始化电机引脚
// void initMotors()
// {
//     pinMode(motor1Pin1, OUTPUT);
//     pinMode(motor1Pin2, OUTPUT);
//     pinMode(motor2Pin1, OUTPUT);
//     pinMode(motor2Pin2, OUTPUT);

//     // 初始状态停止
//     digitalWrite(motor1Pin1, LOW);
//     digitalWrite(motor1Pin2, LOW);
//     digitalWrite(motor2Pin1, LOW);
//     digitalWrite(motor2Pin2, LOW);
// }

// // 电机控制函数
// void forward()
// {
//     digitalWrite(motor1Pin1, HIGH);
//     digitalWrite(motor1Pin2, LOW);
//     digitalWrite(motor2Pin1, HIGH);
//     digitalWrite(motor2Pin2, LOW);
// }

// void backward()
// {
//     digitalWrite(motor1Pin1, LOW);
//     digitalWrite(motor1Pin2, HIGH);
//     digitalWrite(motor2Pin1, LOW);
//     digitalWrite(motor2Pin2, HIGH);
// }

// void left()
// {
//     digitalWrite(motor1Pin1, LOW);
//     digitalWrite(motor1Pin2, HIGH);
//     digitalWrite(motor2Pin1, HIGH);
//     digitalWrite(motor2Pin2, LOW);
// }

// void right()
// {
//     digitalWrite(motor1Pin1, HIGH);
//     digitalWrite(motor1Pin2, LOW);
//     digitalWrite(motor2Pin1, LOW);
//     digitalWrite(motor2Pin2, HIGH);
// }

// void stop()
// {
//     digitalWrite(motor1Pin1, LOW);
//     digitalWrite(motor1Pin2, LOW);
//     digitalWrite(motor2Pin1, LOW);
//     digitalWrite(motor2Pin2, LOW);
// }

// // WebSocket消息处理
// void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
//                void *arg, uint8_t *data, size_t len)
// {
//     switch (type)
//     {
//     case WS_EVT_CONNECT:
//         Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
//         break;
//     case WS_EVT_DISCONNECT:
//         Serial.printf("WebSocket client #%u disconnected\n", client->id());
//         break;
//     case WS_EVT_DATA:
//         // 解析接收到的JSON消息
//         DynamicJsonDocument doc(128);
//         DeserializationError error = deserializeJson(doc, data, len);

//         if (error)
//         {
//             Serial.print("JSON解析错误: ");
//             Serial.println(error.c_str());
//             return;
//         }

//         const char *command = doc["command"];
//         Serial.print("收到命令: ");
//         Serial.println(command);

//         // 根据命令控制电机
//         if (strcmp(command, "forward") == 0)
//         {
//             forward();
//         }
//         else if (strcmp(command, "backward") == 0)
//         {
//             backward();
//         }
//         else if (strcmp(command, "left") == 0)
//         {
//             left();
//         }
//         else if (strcmp(command, "right") == 0)
//         {
//             right();
//         }
//         else if (strcmp(command, "stop") == 0)
//         {
//             stop();
//         }
//         break;
//     case WS_EVT_PONG:
//     case WS_EVT_ERROR:
//         break;
//     }
// }

// void setup()
// {
//     Serial.begin(115200);
//     initMotors();

//     // 连接WiFi
//     WiFi.begin(ssid, password);
//     Serial.print("连接到WiFi");
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println();
//     Serial.print("已连接，IP地址: ");
//     Serial.println(WiFi.localIP());

//     // 初始化WebSocket
//     ws.onEvent(onWsEvent);
//     server.addHandler(&ws);

//     // 处理根路径请求
//     server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
//               { request->send_P(200, "text/html", index_html); });

//     // 启动服务器
//     server.begin();
//     Serial.println("Web服务器已启动");
// }

// void loop()
// {
//     // 处理WebSocket连接
//     ws.cleanupClients();
//     // 其他循环任务
// }