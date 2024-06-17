#include "Arduino.h"
#include "WiFi.h"
#include <OSCMessage.h>
#include <AccelStepper.h>

// Constants
const bool DEBUG = true;
const int DEFAULT_AMPLITUDE = 0;
const int DEFAULT_DELAY = 0;
const int DEFAULT_OFFSET = 0;
const int UPDATE_RATE_MS = 8; 
const int STEP_DELAY_MIN_US = 200;
const int STEP_DELAY_MAX_US = 500;
const int STEP_FACTOR_MIN = 60;
const int STEP_FACTOR_MAX = 140;
const int BOUNCE_MODE_MIN_AMPLITUDE = 4;
const int BOUNCE_MODE_MAX_AMPLITUDE = 354;
const int CONSTANT_MODE_MIN_AMPLITUDE = 355;
const int DISABLED_MODE_MAX_AMPLITUDE = 3;
const int DEGREE_TO_STEP_CONVERSION = 6400 / 360;
const int DIRECTION_FORWARD = 1;
const int DIRECTION_BACKWARD = -1;
const int LED_BUILTIN = 1;

// WiFi configuration
const char* SSID = "NETGEAR30";
const char* PASSWORD = "PWD";
const IPAddress STATIC_IP(10, 10, 10, 16);
const IPAddress GATEWAY(10, 10, 10, 1);
const IPAddress SUBNET(255, 255, 255, 0);
const IPAddress DNS(10, 10, 10, 1);

// OSC configuration
const unsigned int LOCAL_PORT = 8888;

// Stepper motor configuration
const int DIRECTION_PIN = 14;
const int STEP_PIN = 27;
const int ENABLE_PIN = 12;
const int MOTOR_INTERFACE_TYPE = 1;
int stepperSpeed = 100;
int amplitude = DEFAULT_AMPLITUDE;
int direction = DIRECTION_FORWARD;
int amplitudePercentage = DEFAULT_AMPLITUDE;
int delayPercentage = DEFAULT_DELAY;
int offsetPercentage = DEFAULT_OFFSET;

WiFiUDP Udp;
AccelStepper stepper(MOTOR_INTERFACE_TYPE, STEP_PIN, DIRECTION_PIN);

void setup() {
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIRECTION_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);
  Serial.begin(115200);
  while (!Serial); 

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

  Udp.begin(LOCAL_PORT);
  stepper.moveTo(0);
}

static inline int8_t sign(int value) {
  if (value < 0) return -1;
  if (value == 0) return 0;
  return 1;
}

void setStepperSpeed(OSCMessage &msg) {
  if (msg.isInt(0)) {
    stepperSpeed = msg.getInt(0);
  }
}

void setAmplitude(OSCMessage &msg) {
  if (msg.isInt(0)) {
    amplitude = msg.getInt(0);
  }
}

void receiveMessage() {
  OSCMessage incomingMsg;
  int packetSize = Udp.parsePacket();

  if (packetSize > 0) {
    while (packetSize--) {
      incomingMsg.fill(Udp.read());
    }
    if (!incomingMsg.hasError()) {
      incomingMsg.dispatch("/speed", setStepperSpeed);
      incomingMsg.dispatch("/amplitude", setAmplitude);
    }
  }
}

void loop() {
  receiveMessage();

  if (amplitude >= BOUNCE_MODE_MIN_AMPLITUDE && amplitude <= BOUNCE_MODE_MAX_AMPLITUDE) {
    digitalWrite(ENABLE_PIN, LOW);
    stepper.setMaxSpeed(stepperSpeed);
    stepper.setAcceleration(stepperSpeed);

    int randomAmplitudePercentage = random(0, 100);
    int randomDelayPercentage = random(0, 100);
    int varAmplitude = amplitude;

    if (randomAmplitudePercentage < amplitudePercentage) {
      int randomFactor = random(STEP_FACTOR_MIN, STEP_FACTOR_MAX);
      varAmplitude = amplitude * randomFactor / 100;
    }

    if (randomDelayPercentage < delayPercentage) {
      int randomDelay = random(STEP_DELAY_MIN_US, STEP_DELAY_MAX_US);
      delay(randomDelay);
    }

    int randomOffsetPercentage = random(0, 100);
    if (randomOffsetPercentage < offsetPercentage) {
      Serial.println("OFFSET");
      varAmplitude += 180;
    }

    int target = direction * varAmplitude * DEGREE_TO_STEP_CONVERSION;
    if (DEBUG) {
      Serial.print("speed: ");
      Serial.println(stepperSpeed);
      Serial.print("target: ");
      Serial.println(target);
    }

    stepper.move(target);
    stepper.runToPosition();
    direction = -direction;
  } else if (amplitude >= CONSTANT_MODE_MIN_AMPLITUDE) {
    digitalWrite(ENABLE_PIN, LOW);
    int stepDelay = 1.0 / abs(stepperSpeed) * 1000000;

    if (DEBUG) {
      Serial.print("constant speed: ");
      Serial.println(stepperSpeed);
    }

    for (int i = 0; i < 200; i++) {
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(stepDelay);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(stepDelay);
    }
  } else if (amplitude <= DISABLED_MODE_MAX_AMPLITUDE) {
    if (DEBUG) {
      Serial.println("disabled");
    }
    digitalWrite(ENABLE_PIN, HIGH);
    delay(UPDATE_RATE_MS);
  }
}
