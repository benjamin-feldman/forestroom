#include "Arduino.h"
#include "WiFi.h"
#include <OSCMessage.h>

// code générique pour DC


WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
int LED_BUILTIN = 1;
//bool state = 1;
bool debug = true;

// Options
int update_rate = 16;
 
const char* ssid = "NETGEAR30";
const char* password =  "dailydiamond147";

IPAddress staticIP(10, 10, 10, 14);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(10, 10, 10, 1);

unsigned int localPort = 8888; // local port to listen for OSC packets

// Motor 1
int enable1Pin = 12;
int motor1Pin1 = 14;
int motor1Pin2 = 27;
int dutyCycle1 = 0;
int direction1 = 1;
int motorSpeed1;

// Motor 2
int enable2Pin = 32;
int motor2Pin1 = 26;
int motor2Pin2 = 25;
int dutyCycle2 = 0;
int direction2 = 1;
int motorSpeed2;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 1;
const int resolution = 8;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // sets the pins as outputs:
  // Motor 1
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  // set direction = 1 as default
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  // motor 2
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);
  // set direction = 1 as default
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannel1, freq, resolution);
  ledcSetup(pwmChannel2, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel1);
  ledcAttachPin(enable2Pin, pwmChannel2);

  if(debug)Serial.begin(115200);
  while (!Serial) ; // ne rien faire tant que le port Serial n'est pas ouvert (sinon on voit pas s'afficher l'IP etc)

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    if(debug)Serial.println("Configuration failed.");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(debug)Serial.print("Connecting...\n\n");
  }

  if(debug)Serial.print("Local IP: ");
  if(debug)Serial.println(WiFi.localIP());
  if(debug)Serial.print("Subnet Mask: ");
  if(debug)Serial.println(WiFi.subnetMask());
  if(debug)Serial.print("Gateway IP: ");
  if(debug)Serial.println(WiFi.gatewayIP());
  if(debug)Serial.print("DNS 1: ");
  if(debug)Serial.println(WiFi.dnsIP(0));
  if(debug)Serial.print("DNS 2: ");
  if(debug)Serial.println(WiFi.dnsIP(1));
  Udp.begin(localPort);

}

void getSpeed1(OSCMessage &msg) {
  if (msg.isInt(0)) {
    motorSpeed1 = msg.getInt(0); // récupère les données d'Ossia
    dutyCycle1 = abs(motorSpeed1);
  }
}

void getSpeed2(OSCMessage &msg) {
  if (msg.isInt(0)) {
    motorSpeed2 = msg.getInt(0); // récupère les données d'Ossia
    dutyCycle2 = abs(motorSpeed2);
  }
}

void updateDirection1() {
  if (motorSpeed1 > 0) {
    if (direction1 == -1) {
      //if(debug)Serial.println("Going forward");
      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW);
      direction1 = 1;
    }
  }
  
  if (motorSpeed1 < 0) {
    if (direction1 == 1) {
      //if(debug)Serial.println("Going backwards");
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, HIGH);
      direction1 = -1;
    }
  }
}


void updateDirection2() {
  if (motorSpeed2 > 0) {
    if (direction2 == -1) {
      //if(debug)Serial.println("Going forward");
      digitalWrite(motor2Pin1, HIGH);
      digitalWrite(motor2Pin2, LOW);
      direction2 = 1;
    }
  }
  if (motorSpeed2 < 0) {
    if (direction2 == 1) {
      //if(debug)Serial.println("Going backwards");
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, HIGH);
      direction2 = -1;
    }
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
      inmsg.dispatch("/motorSpeed1", getSpeed1); // sauf ici, où on indique l'adresse OSC à écouter
      inmsg.dispatch("/motorSpeed2", getSpeed2); // sauf ici, où on indique l'adresse OSC à écouter
    }
    //else { auto error = inmsg.getError(); }
  }
}

void loop() {
  receiveMessage();
  updateDirection1();
  updateDirection2();
  ledcWrite(pwmChannel1, dutyCycle1);
  ledcWrite(pwmChannel2, dutyCycle2);
  //if(debug)Serial.print(direction1);
  //if(debug)Serial.print(", ");
  //if(debug)Serial.println(direction2);
  if(debug)Serial.print("speed1 :");
  if(debug)Serial.println(dutyCycle1);
  if(debug)Serial.print("speed2 :");
  if(debug)Serial.println(dutyCycle2);
  delay(update_rate);
}
