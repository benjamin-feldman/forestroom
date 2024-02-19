#include "Arduino.h"
#include "WiFi.h"
#include <OSCMessage.h>

WiFiUDP Udp;
int builtInLedPin = 1;
bool debugMode = true;

int updateRate = 16;

const char* ssid = "NETGEAR30";
const char* password =  "PWD";

IPAddress staticIP(10, 10, 10, 15);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(10, 10, 10, 1);

unsigned int localPort = 8888;

int enablePin1 = 12;
int motorPin1A = 14;
int motorPin1B = 27;
int dutyCycle1 = 0;
int direction1 = 1;
int motorSpeed1;

int enablePin2 = 32;
int motorPin2A = 26;
int motorPin2B = 25;
int dutyCycle2 = 0;
int direction2 = 1;
int motorSpeed2;

const int pwmFrequency = 30000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 1;
const int pwmResolution = 8;

/**
 * @brief Initializes the Arduino board and sets up the necessary configurations.
 * 
 * This function is called once when the Arduino board is powered on or reset.
 * It configures the pin modes, initializes the LEDC PWM channels, attaches pins to the channels,
 * configures the WiFi connection, and starts the UDP communication.
 */
void setup() {
  pinMode(builtInLedPin, OUTPUT);

  pinMode(motorPin1A, OUTPUT);
  pinMode(motorPin1B, OUTPUT);
  pinMode(enablePin1, OUTPUT);
  digitalWrite(motorPin1A, HIGH);
  digitalWrite(motorPin1B, LOW);

  pinMode(motorPin2A, OUTPUT);
  pinMode(motorPin2B, OUTPUT);
  pinMode(enablePin2, OUTPUT);
  digitalWrite(motorPin2A, HIGH);
  digitalWrite(motorPin2B, LOW);

  ledcSetup(pwmChannel1, pwmFrequency, pwmResolution);
  ledcSetup(pwmChannel2, pwmFrequency, pwmResolution);

  ledcAttachPin(enablePin1, pwmChannel1);
  ledcAttachPin(enablePin2, pwmChannel2);

  if (debugMode) Serial.begin(115200);
  while (!Serial);

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    if (debugMode) Serial.println("Configuration failed.");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (debugMode) Serial.print("Connecting...\n\n");
  }

  if (debugMode) {
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

void getMotorSpeed1(OSCMessage &msg) {
  if (msg.isInt(0)) {
    motorSpeed1 = msg.getInt(0);
    dutyCycle1 = abs(motorSpeed1);
  }
}

void getMotorSpeed2(OSCMessage &msg) {
  if (msg.isInt(0)) {
    motorSpeed2 = msg.getInt(0);
    dutyCycle2 = abs(motorSpeed2);
  }
}

void updateMotorDirection1() {
  if (motorSpeed1 > 0) {
    if (direction1 == -1) {
      digitalWrite(motorPin1A, HIGH);
      digitalWrite(motorPin1B, LOW);
      direction1 = 1;
    }
  }
  
  if (motorSpeed1 < 0) {
    if (direction1 == 1) {
      digitalWrite(motorPin1A, LOW);
      digitalWrite(motorPin1B, HIGH);
      direction1 = -1;
    }
  }
}

void updateMotorDirection2() {
  if (motorSpeed2 > 0) {
    if (direction2 == -1) {
      digitalWrite(motorPin2A, HIGH);
      digitalWrite(motorPin2B, LOW);
      direction2 = 1;
    }
  }
  if (motorSpeed2 < 0) {
    if (direction2 == 1) {
      digitalWrite(motorPin2A, LOW);
      digitalWrite(motorPin2B, HIGH);
      direction2 = -1;
    }
  }
}

void receiveMessage() {
  OSCMessage inmsg;
  int packetSize = Udp.parsePacket();

  if (packetSize > 0) {
    while (packetSize--) {
      inmsg.fill(Udp.read());
    }
    if (!inmsg.hasError()) {
      inmsg.dispatch("/motorSpeed1", getMotorSpeed1);
      inmsg.dispatch("/motorSpeed2", getMotorSpeed2);
    }
  }
}

void loop() {
  receiveMessage();
  updateMotorDirection1();
  updateMotorDirection2();
  ledcWrite(pwmChannel1, dutyCycle1);
  ledcWrite(pwmChannel2, dutyCycle2);
  if (debugMode) {
    Serial.print("Motor 1 Speed: ");
    Serial.println(dutyCycle1);
    Serial.print("Motor 2 Speed: ");
    Serial.println(dutyCycle2);
  }
  delay(updateRate);
}
