#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <ESPAsyncWebServer.h>


Adafruit_SSD1306 display(128, 64, &Wire);

const char *ssid = "Tarun-FTTH";
const char *password = "Guh97924";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Car Controller</title>
  </head>
  <body>
    <style>
      body {
        background: #eee;
      }
      #joystick-container {
        width: 350px;
        height: 350px;
        border-radius: 50%;
        border: 5px solid grey;
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
      }

      #joystick {
        width: 250px;
        height: 250px;
        background-color: black;
        border-radius: 50%;
        color: black;
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
      }
    </style>
    <div id="joystick-container">
      <div id="joystick"></div>
    </div>
    <h2 style="text-align: center">JOYSTICK</h2>
    <div id="info" style="text-align: center"></div>
    <script>
      const joystickContainer = document.getElementById('joystick-container');
      const joystick = document.getElementById('joystick');
      const info = document.getElementById('info');
      var x = 0;
      var y = 0;

      joystickContainer.addEventListener('touchmove', (event) => {
        event.preventDefault();
        x = event.touches[0].clientX - joystickContainer.offsetLeft + 175;
        y = event.touches[0].clientY - joystickContainer.offsetTop + 175;
       if (x > 115 && x < 235 && y > 115 && y < 235) {
          joystick.style.left = x + 'px';
          joystick.style.top = y + 'px';
          info.innerHTML = `<h3>X:${x} Y:${y}</h3>`;
        }
        info.innerHTML = `<h3>X:${x} Y:${y}</h3>`;

        var xhr = new XMLHttpRequest();
        xhr.open('GET', `/?posX=${x}&posY=${y}`, true);
        xhr.send();
      });

      joystickContainer.addEventListener('touchend', () => {
        joystick.style.left = '50%';
        joystick.style.top = '50%';
        x = 0;
        y = 0;
        info.innerHTML = `<h3>X:${x} Y:${y}</h3>`;
        var xhr = new XMLHttpRequest();
        xhr.open('GET', `/?posX=${x}&posY=${y}`, true);
        xhr.send();
      });

      info.innerHTML = `<h3>X:${x} Y:${y}</h3>`;
    </script>
  </body>
</html>

)")rawliteral";

AsyncWebServer server(80);

#define left1 13
#define left2 12
#define right1 14
#define right2 27

int posX;
int posY;

void setup()
{
  Serial.begin(115200);
  // DC motor driver
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    display.println(".");
    display.display();
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    if (request->hasParam("posX") && request->hasParam("posY")) {
      posX = request->getParam("posX")->value().toInt();
      posY = request->getParam("posY")->value().toInt();
    }
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
}

void loop()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connected to WiFi");
  display.println(WiFi.localIP());
  display.println("");
  display.println("Position X: " + String(posX));
  display.println("Position Y: " + String(posY));
  //  display.println(posX);
  //  display.println(posY);
  display.println("");
  display.println("Tarun Bhati");
  display.println("ibhati0991@gmail.com");
  display.display();
  // Your server code here
  Serial.println(posX);
  Serial.println(posY);

  if (posY < 130)
  {
    digitalWrite(left1, HIGH);
    digitalWrite(left2, LOW);
    digitalWrite(right1, LOW);
    digitalWrite(right2, HIGH);

  } else if (posY > 215)
  {
    digitalWrite(left1, LOW);
    digitalWrite(left2, HIGH);
    digitalWrite(right1, HIGH);
    digitalWrite(right2, LOW);
  }

  if (posX < 130)
  {
    digitalWrite(left1, HIGH);
    digitalWrite(left2, LOW);
    digitalWrite(right1, HIGH);
    digitalWrite(right2, LOW);
  } else if (posX > 215)
  {
    digitalWrite(left1, LOW);
    digitalWrite(left2, HIGH);
    digitalWrite(right1, LOW);
    digitalWrite(right2, HIGH);
  }

  if (posX == 0 && posY == 0) {
    digitalWrite(left1, LOW);
    digitalWrite(left2, LOW);
    digitalWrite(right1, LOW);
    digitalWrite(right2, LOW);
  }
}
