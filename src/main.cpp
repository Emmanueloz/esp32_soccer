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
const char *ssid = "RobotSoccerForward";
const char *password = "robotSF2024";

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
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Home page (with control buttons)
  server.on("/", []()
            { server.send(200, "text/html", R"rawliteral(
    <!DOCTYPE html>
      <html lang="es">
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
        <title>Control de Robot - Modo Horizontal</title>
        <style>
          * {
            box-sizing: border-box;
            touch-action: manipulation;
          }

          body {
            margin: 0;
            padding: 0;
            font-family: Arial, sans-serif;
            background-color: #f4f4f8;
            height: 100vh;
            width: 100vw;
            display: flex;
            flex-direction: column;
            overflow: hidden;
          }

          .header {
            width: 100%;
            background-color: #6b1878;
            color: white;
            text-align: center;
            padding: 5px;
          }

          h1 {
            margin: 0;
            font-size: 20px;
          }

          .controls-wrapper {
            display: flex;
            flex: 1;
            width: 100%;
          }

          /* Controles izquierda */
          .left-controls {
            flex: 1;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 10px;
          }

          /* Controles derecha */
          .right-controls {
            flex: 1;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 10px;
          }

          /* D-pad izquierdo */
          .left-pad {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            grid-template-rows: repeat(3, 1fr);
            gap: 5px;
            width: 100%;
            max-width: 200px;
          }

          /* Botones de acción derecha */
          .right-pad {
            display: flex;
            flex-direction: column;
            gap: 10px;
            width: 100%;
            max-width: 200px;
          }

          .dir-btn {
            height: 60px;
            background-color: #6b1878;
            color: white;
            border: none;
            border-radius: 15px;
            font-size: 24px;
            display: flex;
            align-items: center;
            justify-content: center;
          }

          .dir-btn:active {
           background-color: #9e3aad;
            transform: scale(0.95);
          }

          .turn-btn {
            height: 80px;
            background-color: #5c5c8a;
            color: white;
            border: none;
            border-radius: 15px;
            font-size: 20px;
            display: flex;
            align-items: center;
            justify-content: center;
          }

          .turn-btn:active {
            background-color: #7b7bb2;
            transform: scale(0.95);
          }

          .stop-btn {
            height: 80px;
            background-color: #e74c3c;
            color: white;
            border: none;
            border-radius: 15px;
            font-size: 24px;
            display: flex;
            align-items: center;
            justify-content: center;
          }

          .stop-btn:active {
            background-color: #c0392b;
            transform: scale(0.95);
          }

          .empty {
            background-color: transparent;
          }

          /* Indicador de estado */
          .status-bar {
            width: 100%;
            padding: 5px;
            background-color: #ddd;
            text-align: center;
            font-weight: bold;
          }

          @media (orientation: portrait) {
            /* Mensaje si el dispositivo está en vertical */
            body::before {
              content: "Por favor, gira tu dispositivo para modo horizontal";
              position: fixed;
              top: 0;
              left: 0;
              width: 100%;
              height: 100%;
              background-color: rgba(0, 0, 0, 0.8);
              color: white;
              display: flex;
              justify-content: center;
              align-items: center;
              text-align: center;
              padding: 20px;
              font-size: 20px;
              z-index: 1000;
            }
          }
      </style>
    </head>
    <body>
      <header class="header">
        <h1>Control de Robot</h1>
      </header>

      <div class="controls-wrapper">
      <!-- Control izquierdo: D-pad de movimiento -->
      <div class="left-controls">
        <div class="left-pad">
        <button class="dir-btn" ontouchstart="sendCommand('up_left')" onmousedown="sendCommand('up_left')" ontouchend="sendCommand('stop')" onmouseup="sendCommand('stop')">↖</button>
        <button class="dir-btn" ontouchstart="sendCommand('up')" onmousedown="sendCommand('up')" ontouchend="sendCommand('stop')" onmouseup="sendCommand('stop')">↑</button>
        <button class="dir-btn" ontouchstart="sendCommand('up_right')" onmousedown="sendCommand('up_right')" ontouchend="sendCommand('stop')" onmouseup="sendCommand('stop')">↗</button>
        
        <button class="dir-btn" ontouchstart="sendCommand('left')" onmousedown="sendCommand('left')" ontouchend="sendCommand('stop')" onmouseup="sendCommand('stop')">←</button>
        <button class="dir-btn empty"></button>
        <button class="dir-btn" ontouchstart="sendCommand('right')" onmousedown="sendCommand('right')" ontouchend="sendCommand('stop')" onmouseup="sendCommand('stop')">→</button>
        
        <button class="dir-btn" ontouchstart="sendCommand('down_left')" onmousedown="sendCommand('down_left')" ontouchend="sendCommand('stop')" onmouseup="sendCommand('stop')">↙</button>
        <button class="dir-btn" ontouchstart="sendCommand('down')" onmousedown="sendCommand('down')" ontouchend="sendCommand('stop')" onmouseup="sendCommand('stop')">↓</button>
        <button class="dir-btn" ontouchstart="sendCommand('down_right')" onmousedown="sendCommand('down_right')" ontouchend="sendCommand('stop')" onmouseup="sendCommand('stop')">↘</button>
        </div>
      </div>

      <!-- Control derecho: Botones de giro y parada -->
      <div class="right-controls">
      <div class="right-pad">
        <button class="turn-btn" ontouchstart="sendCommand('turn_left')" onmousedown="sendCommand('turn_left')" ontouchend="sendCommand('stop')" onmouseup="sendCommand('stop')">
          ⟲ GIRAR IZQUIERDA
        </button>
        <button class="turn-btn" ontouchstart="sendCommand('turn_right')" onmousedown="sendCommand('turn_right')" ontouchend="sendCommand('stop')" onmouseup="sendCommand('stop')">
          ⟳ GIRAR DERECHA
        </button>
        <button class="stop-btn" ontouchstart="sendCommand('stop')" onmousedown="sendCommand('stop')">
          🛑 PARAR
        </button>
        </div>
      </div>
    </div>

    <footer class="status-bar" id="status">Listo</footer>

    <script>
    // Elemento de estado
    const statusElement = document.getElementById('status');
    
    // Función para enviar comandos
    function sendCommand(cmd) {
      fetch("/" + cmd)
        .then(response => {
          console.log("Enviado:", cmd);
          updateStatus(cmd);
        })
        .catch(error => {
          console.error("Error enviando", cmd);
          statusElement.textContent = "Error al enviar comando";
        });
      
      // Vibración táctil
      if (navigator.vibrate) {
        navigator.vibrate(50);
      }
    }
    
    // Actualizar el indicador de estado
    function updateStatus(cmd) {
      const commandText = {
        'up': 'Adelante',
        'down': 'Atrás',
        'left': 'Izquierda',
        'right': 'Derecha',
        'up_left': 'Adelante-Izquierda',
        'up_right': 'Adelante-Derecha',
        'down_left': 'Atrás-Izquierda',
        'down_right': 'Atrás-Derecha',
        'turn_left': 'Girando Izquierda',
        'turn_right': 'Girando Derecha',
        'stop': 'Detenido'
      };
      
      statusElement.textContent = commandText[cmd] || cmd;
    }
    
    // Evitar comportamientos no deseados en dispositivos móviles
    document.addEventListener('touchmove', function(e) {
      e.preventDefault();
    }, { passive: false });
    
    document.addEventListener('gesturestart', function(e) {
      e.preventDefault();
    });
    
    // Comprobar orientación al cargar
    window.addEventListener('load', function() {
      if (window.orientation === 0 || window.orientation === 180) {
        // Está en vertical, mostrar mensaje
        alert("Por favor, gira tu dispositivo para modo horizontal");
      }
    });
  </script>
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
