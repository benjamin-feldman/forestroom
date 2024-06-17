#include "Arduino.h"
#include "WiFi.h"
#include <OSCMessage.h>

// Constants
const bool DEBUG_MODE = true;
const int UPDATE_RATE_MS = 16;
const int BUILT_IN_LED_PIN = 1;
const int LOCAL_PORT = 8888;
const int PWM_FREQUENCY = 30000;
const int PWM_RESOLUTION = 8;
const int PWM_CHANNEL1 = 0;
const int PWM_CHANNEL2 = 1;

// WiFi configuration
const char* SSID = "NETGEAR30";
const char* PASSWORD = "PWD";
const IPAddress STATIC_IP(10, 10, 10, 15);
const IPAddress GATEWAY(10, 10, 10, 1);
const IPAddress SUBNET(255, 255, 255, 0);
const IPAddress DNS(10, 10, 10, 1);

// Motor 1 configuration
const int ENABLE_PIN1 = 12;
const int MOTOR_PIN1A = 14;
const int MOTOR_PIN1B = 27;
int motorSpeed1 = 0;
int dutyCycle1 = 0;
int direction1 = 1;

// Motor 2 configuration
const int ENABLE_PIN2 = 32;
const int MOTOR_PIN2A = 26;
const int MOTOR_PIN2B = 25;
int motorSpeed2 = 0;
int dutyCycle2 = 0;
int direction2 = 1;

WiFiUDP udp;

/**
 * @brief Initializes the Arduino board and sets up the necessary configurations.
 * 
 * This function is called once when the Arduino board is powered on or reset.
 * It configures the pin modes, initializes the LEDC PWM channels, attaches pins to the channels,
 * configures the WiFi connection, and starts the UDP communication.
 */
void setup() {
  pinMode(BUILT_IN_LED_PIN, OUTPUT);

  pinMode(MOTOR_PIN1A, OUTPUT);
  pinMode(MOTOR_PIN1B, OUTPUT);
  pinMode(ENABLE_PIN1, OUTPUT);
  digitalWrite(MOTOR_PIN1A, HIGH);
  digitalWrite(MOTOR_PIN1B, LOW);

  pinMode(MOTOR_PIN2A, OUTPUT);
  pinMode(MOTOR_PIN2B, OUTPUT);
  pinMode(ENABLE_PIN2, OUTPUT);
  digitalWrite(MOTOR_PIN2A, HIGH);
  digitalWrite(MOTOR_PIN2B, LOW);

  ledcSetup(PWM_CHANNEL1, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL2, PWM_FREQUENCY, PWM_RESOLUTION);

  ledcAttachPin(ENABLE_PIN1, PWM_CHANNEL1);
  ledcAttachPin(ENABLE_PIN2, PWM_CHANNEL2);

  if (DEBUG_MODE) Serial.begin(115200);
  while (!Serial);

  if (!WiFi.config(STATIC_IP, GATEWAY, SUBNET, DNS, DNS)) {
    if (DEBUG_MODE) Serial.println("Configuration failed.");
  }

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (DEBUG_MODE) Serial.println("Connecting...");
  }

  if (DEBUG_MODE) {
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

  udp.begin(LOCAL_PORT);
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
  if (motorSpeed1 > 0 && direction1 == -1) {
    digitalWrite(MOTOR_PIN1A, HIGH);
    digitalWrite(MOTOR_PIN1B, LOW);
    direction1 = 1;
  } else if (motorSpeed1 < 0 && direction1 == 1) {
    digitalWrite(MOTOR_PIN1A, LOW);
    digitalWrite(MOTOR_PIN1B, HIGH);
    direction1 = -1;
  }
}

void updateMotorDirection2() {
  if (motorSpeed2 > 0 && direction2 == -1) {
    digitalWrite(MOTOR_PIN2A, HIGH);
    digitalWrite(MOTOR_PIN2B, LOW);
    direction2 = 1;
  } else if (motorSpeed2 < 0 && direction2 == 1) {
    digitalWrite(MOTOR_PIN2A, LOW);
    digitalWrite(MOTOR_PIN2B, HIGH);
    direction2 = -1;
  }
}

void receiveMessage() {
  OSCMessage inmsg;
  int packetSize = udp.parsePacket();

  if (packetSize > 0) {
    while (packetSize--) {
      inmsg.fill(udp.read());
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
  ledcWrite(PWM_CHANNEL1, dutyCycle1);
  ledcWrite(PWM_CHANNEL2, dutyCycle2);
  if (DEBUG_MODE) {
    Serial.print("Motor 1 Speed: ");
    Serial.println(dutyCycle1);
    Serial.print("Motor 2 Speed: ");
    Serial.println(dutyCycle2);
  }
  delay(UPDATE_RATE_MS);
}
