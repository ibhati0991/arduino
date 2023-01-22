#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
// for display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//for pcda9685 module
#include <HCPCA9685.h>
#define  I2CAdd 0x40
/* Create an instance of the library */
HCPCA9685 HCPCA9685(I2CAdd);


int ch_width_1 = 0;
int ch_width_2 = 0;
int ch_width_3 = 0;
int ch_width_4 = 0;
int ch_width_5 = 0;
int ch_width_6 = 0;

Servo ch1;
Servo ch2;
Servo ch3;
Servo ch4;

struct Signal {
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
  //Set the pins for each PWM signal
  ch1.attach(2);
  ch2.attach(3);
  ch3.attach(4);
  ch4.attach(5);
  //Configure the NRF24 module
  ResetData();
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.startListening(); //start the radio comunication for receiver

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
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

void printtext(void) {
  display.clearDisplay();
  display.setTextSize(1.2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  String throttle = String(ch_width_3);
  String yaw = String(ch_width_4);
  String roll = String(ch_width_1);
  String pitch = String(ch_width_2);
  display.println("Throttle-" + throttle);
  display.println("Yaw-" + yaw);
  display.println("Roll-" + roll);
  display.println("Pitch-" + pitch);
  display.println("Aux1-" + String(ch_width_5));
  display.println("Aux2-" + String(ch_width_6));
  display.println("Tarun Bhati");
  display.println("ibhati0991@gmail.com");
  display.display();
}

unsigned long lastRecvTime = 0;
void recvData()
{
  if ( radio.available() ) {
    radio.read(&data, sizeof(Signal));
    lastRecvTime = millis();   // receive the data | data alınıyor
  }
}

void loop()
{
  recvData();

  unsigned long now = millis();
  if ( now - lastRecvTime > 1000 ) {
    ResetData(); // Signal lost.. Reset data
  }

  ch_width_4 = map(data.yaw,      0, 255, 1000, 2000);     // pin D5 (PWM signal)
  ch_width_2 = map(data.pitch,    0, 255, 1000, 2000);     // pin D3 (PWM signal)
  ch_width_3 = map(data.throttle, 0, 255, 1000, 2000);     // pin D4 (PWM signal)
  ch_width_1 = map(data.roll,     0, 255, 1000, 2000);     // pin D2 (PWM signal)
  ch_width_5 = data.aux1;     // pin D6
  ch_width_6 = data.aux2;     // pin D7

  Serial.print(ch_width_1);
  Serial.print('\t');
  Serial.print(ch_width_2);
  Serial.print('\t');
  Serial.print(ch_width_3);
  Serial.print('\t');
  Serial.print(ch_width_4);
  Serial.print('\t');
  Serial.print(ch_width_5);
  Serial.print('\t');
  Serial.println(ch_width_6);

  HCPCA9685.Servo(0, map(data.throttle, 0, 255, 420, 10));
  HCPCA9685.Servo(1, map(data.yaw, 0, 255, 420, 10));
  HCPCA9685.Servo(2, map(data.roll, 0, 255, 420, 10));
  HCPCA9685.Servo(3, map(data.pitch, 0, 255, 420, 10));
  // Write the PWM signal
  ch1.writeMicroseconds(ch_width_1);
  ch2.writeMicroseconds(ch_width_2);
  ch3.writeMicroseconds(ch_width_3);
  ch4.writeMicroseconds(ch_width_4);

  digitalWrite(6, ch_width_5);
  digitalWrite(7, ch_width_6);

  printtext();
}
