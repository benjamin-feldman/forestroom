#include "Arduino.h"
#include "WiFi.h"
#include <OSCMessage.h>
#include <AccelStepper.h>

bool debug = true;
int probsAmplitude = 0; // percentage
int probsDelay = 0;
int probsOffset = 0;
WiFiUDP Udp;
int LED_BUILTIN = 1;

// Options
int update_rate = 8; // duration (ms) between each new OSC signal that the ESP will listen to

const char* ssid = "NETGEAR30";
const char* password =  "PWD";

IPAddress staticIP(10, 10, 10, 16);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(10, 10, 10, 1);

unsigned int localPort = 8888; // local port to listen for OSC

// Stepper
int dirPin = 14;
int stepPin = 27;
int enaPin = 12;
int motorInterfaceType = 1;
int stepperSpeed = 100; // initialized to 1 to avoid division by zero later in the code
int amplitude = 0; // amplitude of oscillation in Bounce mode
int direction = 1; // Bounce mode; direction takes values 1, -1, 1, -1, ...

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  pinMode(enaPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  digitalWrite(enaPin, HIGH);
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

static inline int8_t sgn(int val) {
  if (val < 0) return -1;
  if (val==0) return 0;
  return 1;
}

void getStepperSpeed(OSCMessage &msg) {
  if (msg.isInt(0)) {
    stepperSpeed = msg.getInt(0);
  }
}

void getAmplitude(OSCMessage &msg) {
  if (msg.isInt(0)) {
    amplitude = msg.getInt(0);
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
      inmsg.dispatch("/speed", getStepperSpeed);
      inmsg.dispatch("/amplitude", getAmplitude);
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
    digitalWrite(enaPin, LOW);
    stepper.setMaxSpeed(stepperSpeed);
    stepper.setAcceleration(stepperSpeed);
    int randomNumberAmpli = random(0,100);
    int randomNumberDelay = random(0,100);
    int varAmplitude;
    if (randomNumberAmpli < probsAmplitude){
      int randomFactor = random(60,140);
      varAmplitude = amplitude*randomFactor/100;
    }
    else{
      varAmplitude = amplitude;
    }
    if (randomNumberDelay < probsDelay){
      int randomDelay = random(200,500);
      delay(randomDelay);
    }

    int randomNumberOffset = random(0,100);
    if (randomNumberOffset < probsOffset){
      Serial.println("OFFSET");
      varAmplitude = varAmplitude + 180;
    }
    int target = direction*(varAmplitude*6400)/360;
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
    digitalWrite(enaPin, LOW);
    int stepDelay = 1.0/float(abs(stepperSpeed))*1000000;
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
    digitalWrite(enaPin, HIGH);
    delay(update_rate);
  }
}
