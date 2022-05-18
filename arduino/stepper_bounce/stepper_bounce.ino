#include "Arduino.h"
#include "WiFi.h"
#include <OSCMessage.h>
#include <AccelStepper.h>


// Create a new instance of the AccelStepper class:

WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
int LED_BUILTIN = 1;
bool state = 1;
bool DEBUG = true;

// Options
int update_rate = 8;
 
const char* ssid = "NETGEAR30";
const char* password =  "dailydiamond147";

IPAddress staticIP(10, 10, 10, 10);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(10, 10, 10, 1);

unsigned int localPort = 8888; // local port to listen for OSC packets

// Stepper
int dirPin = 14;
int stepPin = 27;
int enaPin = 12;
int motorInterfaceType = 1;
int stepperSpeed = 1;
int amplitude;

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  pinMode(enaPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  Serial.begin(115200);
  while (!Serial) ; // ne rien faire tant que le port Serial n'est pas ouvert (sinon on voit pas s'afficher l'IP etc)

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed.");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting...\n\n");
  }



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
  stepper.moveTo(0);
  stepper.setMaxSpeed(20000);
  stepper.setAcceleration(20000);
}

void getSpeed(OSCMessage &msg) {
  if (msg.isInt(0)) {
    stepperSpeed = msg.getInt(0); // récupère les données d'Ossia
  }
}

void getAmplitude(OSCMessage &msg) { // amplitude = 255 -> fait des tours complets
  if (msg.isInt(0)) {
    amplitude = msg.getInt(0); // récupère les données d'Ossia
  }
}

void receiveMessage() { // à ne pas trop modifier
  OSCMessage inmsg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      inmsg.fill(Udp.read());
    }
    if (!inmsg.hasError()) {
      inmsg.dispatch("/speed", getSpeed); // sauf ici, où on indique l'adresse OSC à écouter
      inmsg.dispatch("/amplitude", getAmplitude); // sauf ici, où on indique l'adresse OSC à écouter
    }
    //else { auto error = inmsg.getError(); }
  }
}

int direction = 1;

void loop() {
  receiveMessage();
  
  if (10 < amplitude and amplitude < 250){
    digitalWrite(enaPin, LOW);
    stepper.setSpeed(stepperSpeed);
    int target = direction*(amplitude*6400)/255;
    stepper.move(target);
    stepper.runToPosition();
    direction = -direction;
    Serial.println(stepperSpeed);
    Serial.println(target);
    delay(update_rate);
  }
  
  if (amplitude >= 250) {
    digitalWrite(enaPin, LOW);
    int stepDelay = 1.0/float(abs(stepperSpeed))*1000000; //conversion en microsec
    Serial.print("constant speed:");
    Serial.println(stepperSpeed);
    for (int i = 0; i < 200; i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
    }
  }
  
  if (amplitude <= 10){ // if amp <= 10, on débloque le stepper
    Serial.println("disabled");
    digitalWrite(enaPin, HIGH);
    delay(update_rate);
  }
}
