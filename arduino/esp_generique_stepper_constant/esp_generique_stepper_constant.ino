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

IPAddress staticIP(10, 10, 10, 16);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(10, 10, 10, 1);

unsigned int localPort = 8888; // local port to listen for OSC packets

// Stepper
int dirPin = 14;
int stepPin = 27;
int motorInterfaceType = 1;
int stepperSpeed;

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
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

  stepper.setMaxSpeed(6400);  
}

void getSpeed(OSCMessage &msg) {
  if (msg.isInt(0)) {
    stepperSpeed = msg.getInt(0); // récupère les données d'Ossia
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
    }
    //else { auto error = inmsg.getError(); }
  }
}


int count = 0;
void loop() {
  if (count % 100000 == 0){
      receiveMessage();
      stepper.setSpeed(stepperSpeed);
      Serial.println(stepperSpeed);
      //Serial.println(count);
      delay(update_rate);
      count = 0
  }
  stepper.runSpeed();
  count = count + 1;
}
