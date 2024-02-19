#include "Arduino.h"
#include "WiFi.h"
#include <OSCMessage.h>
#include <AccelStepper.h>

bool debug = true;
int amplitudePercentage = 0;
int delayPercentage = 0;
int offsetPercentage = 0;
WiFiUDP Udp;
int LED_BUILTIN = 1;

// Options
int updateRate = 8; // duration (ms) between each new OSC signal that the ESP will listen to

const char* ssid = "NETGEAR30";
const char* password =  "PWD";

IPAddress staticIP(10, 10, 10, 16);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(10, 10, 10, 1);

unsigned int localPort = 8888; // local port to listen for OSC

// Stepper
int directionPin = 14;
int stepPin = 27;
int enablePin = 12;
int motorInterfaceType = 1;
int stepperSpeed = 100; // initialized to 1 to avoid division by zero later in the code
int amplitude = 0; // amplitude of oscillation in Bounce mode
int direction = 1; // Bounce mode; direction takes values 1, -1, 1, -1, ...

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, directionPin);

void setup() {
  pinMode(enablePin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  digitalWrite(enablePin, HIGH);
  Serial.begin(115200);
  while (!Serial) ; // wait until the Serial port is open (to display IP, etc.)

  // connect to the network
  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed.");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting...\n\n");
  }

  // display network information
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

  Udp.begin(localPort);

  // initialize the motor
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

/**
 * The main loop function that runs repeatedly in the Arduino sketch.
 * It receives messages, controls the stepper motor based on the amplitude value,
 * and switches between different modes (Bounce, Constant, Disabled).
 */
void loop() {
  receiveMessage();

  // Bounce mode
  if (3 < amplitude && amplitude < 355){
    digitalWrite(enablePin, LOW);
    stepper.setMaxSpeed(stepperSpeed);
    stepper.setAcceleration(stepperSpeed);
    int randomAmplitudePercentage = random(0,100);
    int randomDelayPercentage = random(0,100);
    int varAmplitude;
    if (randomAmplitudePercentage < amplitudePercentage){
      int randomFactor = random(60,140);
      varAmplitude = amplitude * randomFactor / 100;
    }
    else{
      varAmplitude = amplitude;
    }
    if (randomDelayPercentage < delayPercentage){
      int randomDelay = random(200,500);
      delay(randomDelay);
    }

    int randomOffsetPercentage = random(0,100);
    if (randomOffsetPercentage < offsetPercentage){
      Serial.println("OFFSET");
      varAmplitude = varAmplitude + 180;
    }
    int target = direction * (varAmplitude * 6400) / 360;
    if (debug){
      Serial.print("speed:");
      Serial.println(stepperSpeed);
      Serial.print("target:");
      Serial.println(target);
    }
    stepper.move(target);
    stepper.runToPosition();
    direction = -direction;
  } 

  // Constant mode
  if (amplitude >= 355) {
    digitalWrite(enablePin, LOW);
    int stepDelay = 1.0 / float(abs(stepperSpeed)) * 1000000;
    if (debug){
      Serial.print("constant speed:");
      Serial.println(stepperSpeed);
    }
    for (int i = 0; i < 200; i++){
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(stepDelay);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(stepDelay);
    }
  }
  
  // Disabled mode
  if (amplitude <= 3){
    if (debug){
      Serial.println("disabled");
    }
    digitalWrite(enablePin, HIGH);
    delay(updateRate);
  }
}
