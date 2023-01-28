#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
// for display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// for pcda9685 module
#include <HCPCA9685.h>
#define I2CAdd 0x40
/* Create an instance of the library */
HCPCA9685 HCPCA9685(I2CAdd);

#define left1 2
#define left2 3
#define right1 4
#define right2 5

int roll = 0;
int pitch = 0;
int throttle = 0;
int yaw = 0;
int aux1 = 0;
int aux2 = 0;

Servo ch1;
Servo ch2;
Servo ch3;
Servo ch4;

struct Signal
{
  byte throttle;
  byte pitch;
  byte roll;
  byte yaw;
  byte aux1;
  byte aux2;
};
Signal data;

RF24 radio(9, 10);
const byte address[6] = "00001";
void ResetData()
{
  // Define the inicial value of each data input.
  // The middle position for Potenciometers. (254/2=127)
  data.roll = 127;
  data.pitch = 127;
  data.throttle = 127;
  data.yaw = 127;
  data.aux1 = 0;
  data.aux2 = 0;
}
void setup()
{
  Serial.begin(9600);
  // DC motor driver
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);

  // Set the pins for each PWM signal
  // ch1.attach(2);
  // ch2.attach(3);
  // ch3.attach(4);
  // ch4.attach(5);
  // Configure the NRF24 module
  ResetData();
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.startListening(); // start the radio comunication for receiver

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  /* Initialise the library and set it to 'servo mode' */
  HCPCA9685.Init(SERVO_MODE);

  /* Wake the device up */
  HCPCA9685.Sleep(false);
}

unsigned long lastRecvTime = 0;
void recvData()
{
  if (radio.available())
  {
    radio.read(&data, sizeof(Signal));
    lastRecvTime = millis(); // receive the data | data alınıyor
  }
}

void loop()
{
  recvData();

  unsigned long now = millis();
  if (now - lastRecvTime > 1000)
  {
    ResetData(); // Signal lost.. Reset data
  }

  yaw = map(data.yaw, 0, 255, 1000, 2000);
  pitch = map(data.pitch, 0, 255, 1000, 2000);
  throttle = map(data.throttle, 0, 255, 1000, 2000);
  roll = map(data.roll, 0, 255, 1000, 2000);
  aux1 = data.aux1;
  aux2 = data.aux2;

  HCPCA9685.Servo(0, map(data.throttle, 0, 255, 420, 10));
  HCPCA9685.Servo(1, map(data.yaw, 0, 255, 420, 10));
  HCPCA9685.Servo(2, map(data.roll, 0, 255, 420, 10));
  HCPCA9685.Servo(3, map(data.pitch, 0, 255, 420, 10));
  // Write the PWM signal
  // ch1.writeMicroseconds(roll);
  // ch2.writeMicroseconds(pitch);
  // ch3.writeMicroseconds(throttle);
  // ch4.writeMicroseconds(yaw);

  digitalWrite(6, aux1);
  digitalWrite(7, aux2);
  // print stuff on lcd
  printtext();
  // car function
  driveCar();
}

void driveCar(void)
{
  //reset forward backward to zero
  if (throttle < 1700 && throttle > 1300)
  {
    digitalWrite(left1, LOW);
    digitalWrite(left2, LOW);
    digitalWrite(right1, LOW);
    digitalWrite(right2, LOW);
  }else
  if (throttle > 1700)
  {
    digitalWrite(left1, LOW);
    digitalWrite(left2, HIGH);
    digitalWrite(right1, HIGH);
    digitalWrite(right2, LOW);
  }else
   if (throttle < 1300)
  {
    digitalWrite(left1, HIGH);
    digitalWrite(left2, LOW);
    digitalWrite(right1, LOW);
    digitalWrite(right2, HIGH);
  }

  if (roll > 1700)
  {
    digitalWrite(left1, HIGH);
    digitalWrite(left2, LOW);
    digitalWrite(right1, HIGH);
    digitalWrite(right2, LOW);
  }else
  if (roll < 1300)
  {
    digitalWrite(left1, LOW);
    digitalWrite(left2, HIGH);
    digitalWrite(right1, LOW);
    digitalWrite(right2, HIGH);
  }
}

void printtext(void)
{
  display.clearDisplay();
  display.setTextSize(1.2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Throttle-" + String(throttle));
  display.println("Yaw-" + String(yaw));
  display.println("Roll-" + String(roll));
  display.println("Pitch-" + String(pitch));
  display.println("Aux1-" + String(aux1));
  display.println("Aux2-" + String(aux2));
  display.println("Tarun Bhati");
  display.println("ibhati0991@gmail.com");
  display.display();
}
