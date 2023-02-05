#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

Adafruit_SSD1306 display(128, 64, &Wire);

const char *ssid = "Tarun-FTTH";
const char *password = "Guh97924";

// Insert Firebase project API Key
#define API_KEY "AIzaSyCcmEk_LTg6LArbbMhk7Unpk8SyPszPfBo"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp32-car-aa3ea-default-rtdb.firebaseio.com"


#define left1 13
#define left2 12
#define right1 14
#define right2 27

int posX;
int posY;

unsigned long sendDataPrevMillis = 0;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

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

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  getrtdbData();

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

void getrtdbData() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 100 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getInt(&fbdo, "/positions/esp32Car/x")) {
      if (fbdo.dataType() == "int") {
        posX = fbdo.intData();
        Serial.println(posX);
      }
    } else if (Firebase.RTDB.getInt(&fbdo, "/positions/esp32Car/y")) {
      if (fbdo.dataType() == "int") {
        posY = fbdo.intData();
        Serial.println(posY);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
  }
}
