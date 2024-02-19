#include "Arduino.h"
#include "WiFi.h"
#include <OSCMessage.h>

WiFiUDP Udp;
int LED_BUILTIN = 1;
bool debug = true;

int update_rate = 16;

const char* ssid = "NETGEAR30";
const char* password =  "PWD";

IPAddress staticIP(10, 10, 10, 15);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(10, 10, 10, 1);

unsigned int localPort = 8888;

int enable1Pin = 12;
int motor1Pin1 = 14;
int motor1Pin2 = 27;
int dutyCycle1 = 0;
int direction1 = 1;
int motorSpeed1;

int enable2Pin = 32;
int motor2Pin1 = 26;
int motor2Pin2 = 25;
int dutyCycle2 = 0;
int direction2 = 1;
int motorSpeed2;

const int freq = 30000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 1;
const int resolution = 8;

/**
 * @brief Initializes the Arduino board and sets up the necessary configurations.
 * 
 * This function is called once when the Arduino board is powered on or reset.
 * It configures the pin modes, initializes the LEDC PWM channels, attaches pins to the channels,
 * configures the WiFi connection, and starts the UDP communication.
 */
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);

  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);

  ledcSetup(pwmChannel1, freq, resolution);
  ledcSetup(pwmChannel2, freq, resolution);

  ledcAttachPin(enable1Pin, pwmChannel1);
  ledcAttachPin(enable2Pin, pwmChannel2);

  if (debug) Serial.begin(115200);
  while (!Serial);

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    if (debug) Serial.println("Configuration failed.");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (debug) Serial.print("Connecting...\n\n");
  }

  if (debug) {
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Subnet Mask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("DNS 1: ");
    Serial.println(WiFi.dnsIP(0));
    Serial.print("DNS 2: ");
    Serial.println(WiFi.dnsIP(1));
  }

  Udp.begin(localPort);
}

void getSpeed1(OSCMessage &msg) {
  if (msg.isInt(0)) {
    motorSpeed1 = msg.getInt(0);
    dutyCycle1 = abs(motorSpeed1);
  }
}

void getSpeed2(OSCMessage &msg) {
  if (msg.isInt(0)) {
    motorSpeed2 = msg.getInt(0);
    dutyCycle2 = abs(motorSpeed2);
  }
}

void updateDirection1() {
  if (motorSpeed1 > 0) {
    if (direction1 == -1) {
      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW);
      direction1 = 1;
    }
  }
  
  if (motorSpeed1 < 0) {
    if (direction1 == 1) {
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, HIGH);
      direction1 = -1;
    }
  }
}

void updateDirection2() {
  if (motorSpeed2 > 0) {
    if (direction2 == -1) {
      digitalWrite(motor2Pin1, HIGH);
      digitalWrite(motor2Pin2, LOW);
      direction2 = 1;
    }
  }
  if (motorSpeed2 < 0) {
    if (direction2 == 1) {
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, HIGH);
      direction2 = -1;
    }
  }
}

void receiveMessage() {
  OSCMessage inmsg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      inmsg.fill(Udp.read());
    }
    if (!inmsg.hasError()) {
      inmsg.dispatch("/motorSpeed1", getSpeed1);
      inmsg.dispatch("/motorSpeed2", getSpeed2);
    }
  }
}

void loop() {
  receiveMessage();
  updateDirection1();
  updateDirection2();
  ledcWrite(pwmChannel1, dutyCycle1);
  ledcWrite(pwmChannel2, dutyCycle2);
  if (debug) {
    Serial.print("speed1 :");
    Serial.println(dutyCycle1);
    Serial.print("speed2 :");
    Serial.println(dutyCycle2);
  }
  delay(update_rate);
}
