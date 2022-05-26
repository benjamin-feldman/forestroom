#include "Arduino.h"
#include "WiFi.h"
#include <OSCMessage.h>
#include <AccelStepper.h>

// code générique pour Stepper


WiFiUDP Udp; // instance UDP qui permet de recevoir des data via UDP
int LED_BUILTIN = 1;

// Options
int update_rate = 8; // durée (ms) entre chaque nouveau signal OSC que l'ESP va écouter
 
const char* ssid = "NETGEAR30";
const char* password =  "dailydiamond147";

IPAddress staticIP(10, 10, 10, 10);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(10, 10, 10, 1);

unsigned int localPort = 8888; // port local d'écoute pour OSC

// Stepper
int dirPin = 14;
int stepPin = 27;
int enaPin = 12;
int motorInterfaceType = 1;
int stepperSpeed = 100; // initialisé à 1 afin d'éviter une division par zéro plus loin dans le code
int amplitude = 0; // amplitude de l'oscillation dans le cas Bounce
int direction = 1; // cas Bounce ; direction va prendre les valeurs 1, -1, 1, -1, ...

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  pinMode(enaPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  digitalWrite(enaPin, HIGH);
  Serial.begin(115200);
  while (!Serial) ; // ne rien faire tant que le port Serial n'est pas ouvert (sinon on voit pas s'afficher l'IP etc)

  // connexion au réseau

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed.");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting...\n\n");
  }

  // affichage des différentes composantes réseau
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

  // initialisation du moteur
  stepper.moveTo(0);
  stepper.setMaxSpeed(7000);
  stepper.setAcceleration(1500);
}

static inline int8_t sgn(int val) {
  if (val < 0) return -1;
  if (val==0) return 0;
  return 1;
}

// getSpeed prend le message OSC en argument et assigne la valeur contenue dans l'adresse /speed à la variable stepperSpeed
// stepperSpeed en step/second
void getStepperSpeed(OSCMessage &msg) {
  if (msg.isInt(0)) {
    stepperSpeed = msg.getInt(0); // récupère les données d'Ossia
  }
}

// idem, pour amplitude
// amplitude ente 0 et 255
// amplitude <= 3 : mode Disabled
// 3 < amplitude < 250 : mode Bounce
// 250 <= amplitude : mode Constant
void getAmplitude(OSCMessage &msg) { // amplitude = 255 -> fait des tours complets
  if (msg.isInt(0)) {
    amplitude = msg.getInt(0); // récupère les données d'Ossia
  }
}

// fonction appelée une fois pour chaque loop
// écoute les adresses OSC spécifiées
// ne rien toucher sauf les deux lignes commentées
void receiveMessage() { // à ne pas trop modifier
  OSCMessage inmsg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      inmsg.fill(Udp.read());
    }
    if (!inmsg.hasError()) {
      inmsg.dispatch("/speed", getStepperSpeed); // sauf ici, où on indique l'adresse OSC à écouter
      inmsg.dispatch("/amplitude", getAmplitude); // sauf ici, où on indique l'adresse OSC à écouter
    }
    //else { auto error = inmsg.getError(); }
  }
}

//void bounceMode(int stepperSpeed, int amplitude){}

//void constantMode(stepperSpeed);
//void disabledMode();

void loop() {
  // écoute OSC
  receiveMessage();

  // cas Bounce
  // pas de update_rate ici, le temps de parcours du moteur est suffisant
  if (3 < amplitude and amplitude < 250){
    digitalWrite(enaPin, LOW);
    stepper.setMaxSpeed(stepperSpeed);
    stepper.setAcceleration(stepperSpeed);
    // target est entre 0 et 6400 (un tour complet fait 6400 pas)
    int target = direction*(amplitude*6400)/255;
    stepper.move(target);
    stepper.runToPosition();
    //changement de direction pour le prochain tour
    direction = -direction;
    Serial.println(stepperSpeed);
    Serial.println(target);
    //delay(update_rate);
  } 
  /*
  if (3 < amplitude and amplitude < 250){
    // on "enable" le stepper
    digitalWrite(enaPin, LOW);

    if (direction > 0){
      digitalWrite(dirPin, HIGH);
    }
    else{
      digitalWrite(dirPin,LOW);
    }

    int stepDelay = 1.0/float(abs(stepperSpeed))*1000000;

    // target est entre 0 et 6400 (un tour complet fait 6400 pas)
    int target = (amplitude*6400)/255;
    for (int i = 0; i < target; i++){
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(stepDelay);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(stepDelay);
    }

    //changement de direction pour le prochain tour
    direction = -direction;
    Serial.println(stepperSpeed);
    Serial.println(target);
    //delay(update_rate);
  }*/


  
  //cas Constant
  // pas de update_rate ici, le temps de parcours du moteur est suffisant
  if (amplitude >= 250) {
    // enable stepper
    digitalWrite(enaPin, LOW);
    // durée d'un step en microsecondes, obtenue à partir de la vitesse en step/s
    int stepDelay = 1.0/float(abs(stepperSpeed))*1000000; //conversion en microsec
    Serial.print("constant speed:");
    Serial.println(stepperSpeed);
    // on fait faire 200 steps afin d'éviter les accoups
    // bricolage
    int variationFactor = random(50,150);
    stepperSpeed = (float(variationFactor)/100.0)*stepperSpeed;
    for (int i = 0; i < 200; i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
    }
  }
  
  // cas Disabled
  if (amplitude <= 3){ // if amp <= 3, on débloque le stepper
    Serial.println("disabled");
    //disable stepper
    digitalWrite(enaPin, HIGH);
    delay(update_rate);
  }
}
