#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>

// Motor A (Left)
const int IN1 = 5;
const int IN2 = 18;

// Motor B (Right)
const int IN3 = 19;
const int IN4 = 21;

WebServer server(80);

// WiFi Access Point
const char *ssid = "RobotSoccer";
const char *password = "12345678";

// === Motor Control Functions ===
void stopMotors()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Stop");
}

void moveForward()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Forward");
}

void moveBackward()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Backward");
}

void turnLeft()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Left");
}

void turnRight()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Right");
}

// === Setup ===
void setup()
{
  Serial.begin(115200);

  // Motor pin setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopMotors();

  // Access Point
  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Home page (with control buttons)
  server.on("/", []()
            { server.send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head>
      <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Robot Control</title>
        <style>
          body {
            margin: 0;
            padding: 0;
            display: flex;
            align-items: center;
            justify-content: center;
            height: 100vh;
            background-color: #f0f0f0;
            font-family: Arial, sans-serif;
          }
  
          .circle-container {
            position: relative;
            width: 300px;
            height: 300px;
          }
  
          .btn {
            position: absolute;
            width: 80px;
            height: 80px;
            border: none;
            border-radius: 50%;
            background-color: #6b1878;
            color: white;
            font-size: 20px;
            cursor: pointer;
            transition: background-color 0.2s;
          }
  
          .btn:hover {
            background-color: #9e3aad;
          }
  
          #up {
            top: 10px;
            left: 50%;
            transform: translateX(-50%);
          }
  
          #down {
            bottom: 10px;
            left: 50%;
            transform: translateX(-50%);
          }
  
          #left {
            left: 10px;
            top: 50%;
            transform: translateY(-50%);
          }
  
          #right {
            right: 10px;
            top: 50%;
            transform: translateY(-50%);
          }
  
          #stop {
            top: 50%;
            left: 50%;
            width: 100px;
            height: 100px;
            font-size: 22px;
            background-color: #d9534f;
            transform: translate(-50%, -50%);
          }
  
        </style>
        <script>
          function sendCommand(cmd) {
            fetch("/" + cmd)
              .then(response => console.log("Sent:", cmd))
              .catch(error => console.error("Error sending", cmd));
          }
        </script>
      </head>
      <body>
        <div class="circle-container">
          <button id="up" class="btn" onclick="sendCommand('forward')">↑</button>
          <button id="down" class="btn" onclick="sendCommand('backward')">↓</button>
          <button id="left" class="btn" onclick="sendCommand('left')">←</button>
          <button id="right" class="btn" onclick="sendCommand('right')">→</button>
          <button id="stop" class="btn" onclick="sendCommand('stop')">STOP</button>
        </div>
      </body>
      </html>
    )rawliteral"); });

  // === API Routes ===
  server.on("/forward", []()
            {
    moveForward();
    server.send(200, "application/json", "{\"action\":\"forward\"}"); });

  server.on("/backward", []()
            {
    moveBackward();
    server.send(200, "application/json", "{\"action\":\"backward\"}"); });

  server.on("/left", []()
            {
    turnLeft();
    server.send(200, "application/json", "{\"action\":\"left\"}"); });

  server.on("/right", []()
            {
    turnRight();
    server.send(200, "application/json", "{\"action\":\"right\"}"); });

  server.on("/stop", []()
            {
    stopMotors();
    server.send(200, "application/json", "{\"action\":\"stop\"}"); });

  server.onNotFound([]()
                    { server.send(404, "application/json", "{\"error\":\"Not Found\"}"); });

  server.begin();
}

void loop()
{
  server.handleClient();
}
