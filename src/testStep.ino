/* Example sketch to control a stepper motor with TB6600 stepper motor driver, AccelStepper library and Arduino: acceleration and deceleration. More info: https://www.makerguides.com */

// Include the AccelStepper library:
#include <AccelStepper.h>

// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define dirPin 14
#define stepPin 27
#define motorInterfaceType 1

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  // Set the maximum speed and acceleration:
  stepper.setMaxSpeed(20000);
  stepper.setAcceleration(5000);
}

/**
 * The main loop function that runs repeatedly in the Arduino program.
 * It sets the target position of the stepper motor, runs the motor to the target position,
 * then moves the motor back to zero position and runs it again.
 * This loop is executed continuously until the Arduino is powered off or reset.
 */
void loop() {
  // Set the target position:
  stepper.moveTo(8000);
  // Run to target position with set speed and acceleration/deceleration:
  stepper.runToPosition();

  delay(100);

  // Move back to zero:
  stepper.moveTo(0);
  stepper.runToPosition();

  delay(100);
}
