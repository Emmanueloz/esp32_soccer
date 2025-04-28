#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>

enum MotorDirection
{
  UP,
  DOWN,
  LEFT,
  RIGHT,
  UP_LEFT,
  UP_RIGHT,
  DOWN_LEFT,
  DOWN_RIGHT,
  TURN_LEFT,
  TURN_RIGHT,
  STOP
};

enum Wheels
{
  FRONT_RIGHT,
  BACK_RIGHT,
  FRONT_LEFT,
  BACK_LEFT
};

enum MotorForce
{
  FORWARD = 1,
  BACKWARD = -1,
  STOPPED = 0
};

struct MotorPins
{
  int pinIN1;
  int pinIN2;
};

std::vector<MotorPins> motorPins = {
    {19, 21}, // Wheels.FRONT_RIGHT
    {13, 12}, // Wheels.BACK_RIGHT
    {5, 18},  // Wheels.FRONT_LEFT
    {32, 33}, // Wheels.BACK_LEFT
};

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

void rotateMotor(Wheels wheel, MotorForce force)
{
  if (force == FORWARD)
  {
    digitalWrite(motorPins[wheel].pinIN1, HIGH);
    digitalWrite(motorPins[wheel].pinIN2, LOW);
  }
  else if (force == BACKWARD)
  {
    digitalWrite(motorPins[wheel].pinIN1, LOW);
    digitalWrite(motorPins[wheel].pinIN2, HIGH);
  }
  else
  {
    digitalWrite(motorPins[wheel].pinIN1, LOW);
    digitalWrite(motorPins[wheel].pinIN2, LOW);
  }
}

void processCarMovement(MotorDirection direction)
{
  switch (direction)
  {

  case UP:
    Serial.println("UP");
    rotateMotor(FRONT_RIGHT, FORWARD);
    rotateMotor(BACK_RIGHT, FORWARD);
    rotateMotor(FRONT_LEFT, FORWARD);
    rotateMotor(BACK_LEFT, FORWARD);
    break;

  case DOWN:
    Serial.println("DOWN");
    rotateMotor(FRONT_RIGHT, BACKWARD);
    rotateMotor(BACK_RIGHT, BACKWARD);
    rotateMotor(FRONT_LEFT, BACKWARD);
    rotateMotor(BACK_LEFT, BACKWARD);
    break;

  case LEFT:
    Serial.println("LEFT");
    rotateMotor(FRONT_RIGHT, FORWARD);
    rotateMotor(BACK_RIGHT, BACKWARD);
    rotateMotor(FRONT_LEFT, BACKWARD);
    rotateMotor(BACK_LEFT, FORWARD);
    break;

  case RIGHT:
    Serial.println("RIGHT");
    rotateMotor(FRONT_RIGHT, BACKWARD);
    rotateMotor(BACK_RIGHT, FORWARD);
    rotateMotor(FRONT_LEFT, FORWARD);
    rotateMotor(BACK_LEFT, BACKWARD);
    break;

  case UP_LEFT:
    Serial.println("UP_LEFT");
    rotateMotor(FRONT_RIGHT, FORWARD);
    rotateMotor(BACK_RIGHT, STOPPED);
    rotateMotor(FRONT_LEFT, STOPPED);
    rotateMotor(BACK_LEFT, FORWARD);
    break;

  case UP_RIGHT:
    Serial.println("UP_RIGHT");
    rotateMotor(FRONT_RIGHT, STOPPED);
    rotateMotor(BACK_RIGHT, FORWARD);
    rotateMotor(FRONT_LEFT, FORWARD);
    rotateMotor(BACK_LEFT, STOPPED);
    break;

  case DOWN_LEFT:
    Serial.println("DOWN_LEFT");
    rotateMotor(FRONT_RIGHT, STOPPED);
    rotateMotor(BACK_RIGHT, BACKWARD);
    rotateMotor(FRONT_LEFT, BACKWARD);
    rotateMotor(BACK_LEFT, STOPPED);
    break;

  case DOWN_RIGHT:
    Serial.println("DOWN_RIGHT");
    rotateMotor(FRONT_RIGHT, BACKWARD);
    rotateMotor(BACK_RIGHT, STOPPED);
    rotateMotor(FRONT_LEFT, STOPPED);
    rotateMotor(BACK_LEFT, BACKWARD);
    break;

  case TURN_LEFT:
    Serial.println("TURN_LEFT");
    rotateMotor(FRONT_RIGHT, FORWARD);
    rotateMotor(BACK_RIGHT, FORWARD);
    rotateMotor(FRONT_LEFT, BACKWARD);
    rotateMotor(BACK_LEFT, BACKWARD);
    break;

  case TURN_RIGHT:
    Serial.println("TURN_RIGHT");
    rotateMotor(FRONT_RIGHT, BACKWARD);
    rotateMotor(BACK_RIGHT, BACKWARD);
    rotateMotor(FRONT_LEFT, FORWARD);
    rotateMotor(BACK_LEFT, FORWARD);
    break;

  case STOP:
    Serial.println("STOP");
    rotateMotor(FRONT_RIGHT, STOPPED);
    rotateMotor(BACK_RIGHT, STOPPED);
    rotateMotor(FRONT_LEFT, STOPPED);
    rotateMotor(BACK_LEFT, STOPPED);
    break;

  default:
    Serial.println("DEFAULT STOP");
    rotateMotor(FRONT_RIGHT, STOPPED);
    rotateMotor(BACK_RIGHT, STOPPED);
    rotateMotor(FRONT_LEFT, STOPPED);
    rotateMotor(BACK_LEFT, STOPPED);
    break;
  }
}

void setupMotorPins()
{
  int motorIndex = 0;
  for (const auto &motor : motorPins)
  {
    pinMode(motor.pinIN1, OUTPUT);
    pinMode(motor.pinIN2, OUTPUT);
    rotateMotor(static_cast<Wheels>(motorIndex), STOPPED);
  }
}

// === Setup ===
void setup()
{
  Serial.begin(115200);

  setupMotorPins();
  // Access Point
  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Home page (with control buttons)
  server.on("/", []()
            { server.send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html lang="en">
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Robot Soccer Control</title>
        <style>
          body {
            margin: 0;
            padding: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            background-color: #f0f0f0;
            font-family: Arial, sans-serif;
          }
          h1 {
            color: #6b1878;
            margin-bottom: 10px;
          }
          .grid-container {
            display: grid;
            grid-template-columns: repeat(3, 80px);
            grid-gap: 10px;
            justify-content: center;
            align-items: center;
          }
          .btn {
            width: 80px;
            height: 80px;
            font-size: 24px;
            border: none;
            border-radius: 20px;
            background-color: #6b1878;
            color: white;
            cursor: pointer;
            transition: background-color 0.2s;
          }
          .btn:hover {
            background-color: #9e3aad;
          }
          .special-buttons {
            margin-top: 20px;
            display: flex;
            justify-content: center;
            gap: 20px;
          }
          .turn-btn {
            width: 100px;
            height: 50px;
            font-size: 20px;
            background-color: #5c5c8a;
            border: none;
            border-radius: 10px;
            color: white;
            cursor: pointer;
          }
          .turn-btn:hover {
            background-color: #7b7bb2;
          }
        </style>
        <script>
        function sendCommand(cmd) {
          fetch("/" + cmd)
            .then(response => console.log("Sent:", cmd))
            .catch(error => console.error("Error sending", cmd));
          if (navigator.vibrate) {
            navigator.vibrate(100);
          }
        }
      </script>
      </head>
      <body>
        <h1>Robot Soccer Control</h1>
        <div class="grid-container">
          <button class="btn" onclick="sendCommand('up_left')">↖</button>
          <button class="btn" onclick="sendCommand('up')">↑</button>
          <button class="btn" onclick="sendCommand('up_right')">↗</button>
  
          <button class="btn" onclick="sendCommand('left')">←</button>
          <button class="btn" onclick="sendCommand('stop')">🛑</button>
          <button class="btn" onclick="sendCommand('right')">→</button>
  
          <button class="btn" onclick="sendCommand('down_left')">↙</button>
          <button class="btn" onclick="sendCommand('down')">↓</button>
          <button class="btn" onclick="sendCommand('down_right')">↘</button>
        </div>
  
        <div class="special-buttons">
          <button class="turn-btn" onclick="sendCommand('turn_left')">⟲ Turn Left</button>
          <button class="turn-btn" onclick="sendCommand('turn_right')">⟳ Turn Right</button>
        </div>
      </body>
      </html>
    )rawliteral"); });

  // === API Routes ===
  server.on("/up", []()
            {
    processCarMovement(UP);
    server.send(200, "application/json", "{\"action\":\"forward\"}"); });

  server.on("/down", []()
            {
    processCarMovement(DOWN);
    server.send(200, "application/json", "{\"action\":\"backward\"}"); });

  server.on("/left", []()
            {
    processCarMovement(LEFT);
    server.send(200, "application/json", "{\"action\":\"left\"}"); });

  server.on("/right", []()
            {
    processCarMovement(RIGHT);
    server.send(200, "application/json", "{\"action\":\"right\"}"); });

  server.on("/up_left", []()
            {
    processCarMovement(UP_LEFT);
    server.send(200, "application/json", "{\"action\":\"up_left\"}"); });

  server.on("/up_right", []()
            {
    processCarMovement(UP_RIGHT);
    server.send(200, "application/json", "{\"action\":\"up_right\"}"); });
  server.on("/down_left", []()
            {
    processCarMovement(DOWN_LEFT);
    server.send(200, "application/json", "{\"action\":\"down_left\"}"); });

  server.on("/down_right", []()
            {
    processCarMovement(DOWN_RIGHT);
    server.send(200, "application/json", "{\"action\":\"down_right\"}"); });
  server.on("/turn_left", []()
            {
    processCarMovement(TURN_LEFT);
    server.send(200, "application/json", "{\"action\":\"turn_left\"}"); });

  server.on("/turn_right", []()
            {
    processCarMovement(TURN_RIGHT);
    server.send(200, "application/json", "{\"action\":\"turn_right\"}"); });

  server.on("/stop", []()
            {
    processCarMovement(STOP);
    server.send(200, "application/json", "{\"action\":\"stop\"}"); });

  server.onNotFound([]()
                    { server.send(404, "application/json", "{\"error\":\"Not Found\"}"); });

  server.begin();
}

void loop()
{
  server.handleClient();
}
