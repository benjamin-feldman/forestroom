/* obsolete */

#include "Arduino.h"
#include "WiFi.h"
#include <OSCMessage.h>
#include <AccelStepper.h>

// Constants
const bool DEBUG_MODE = true;
const int UPDATE_RATE_MS = 8;
const int LED_PIN = 1;
const int MAX_SPEED = 6400;
const int LOCAL_PORT = 8888;
const int DIR_PIN = 14;
const int STEP_PIN = 27;
const int MOTOR_INTERFACE_TYPE = 1;
const char* SSID = "NETGEAR30";
const char* PASSWORD = "PWD";
const IPAddress STATIC_IP(10, 10, 10, 16);
const IPAddress GATEWAY(10, 10, 10, 1);
const IPAddress SUBNET(255, 255, 255, 0);
const IPAddress DNS(10, 10, 10, 1);

WiFiUDP udp; 
int stepperSpeed;

AccelStepper stepper(MOTOR_INTERFACE_TYPE, STEP_PIN, DIR_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait until the Serial port is opened

  if (!WiFi.config(STATIC_IP, GATEWAY, SUBNET, DNS, DNS)) {
    Serial.println("Configuration failed.");
  }

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting...");
  }

  // Display network information
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

  udp.begin(LOCAL_PORT);
  stepper.setMaxSpeed(MAX_SPEED);
}

void getSpeed(OSCMessage &msg) {
  if (msg.isInt(0)) {
    stepperSpeed = msg.getInt(0); // Get data from Ossia
  }
}

void receiveMessage() {
  OSCMessage inMessage;
  int packetSize = udp.parsePacket();

  if (packetSize > 0) {
    while (packetSize--) {
      inMessage.fill(udp.read());
    }
    if (!inMessage.hasError()) {
      inMessage.dispatch("/speed", getSpeed); // Specify the OSC address to listen to
    }
  }
}

int count = 0;

void loop() {
  if (count % 100000 == 0) {
    receiveMessage();
    stepper.setSpeed(stepperSpeed);
    if (DEBUG_MODE) {
      Serial.println(stepperSpeed);
    }
    delay(UPDATE_RATE_MS);
    count = 0;
  }
  stepper.runSpeed();
  count++;
}
