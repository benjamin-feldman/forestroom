/* Example sketch to control a stepper motor with TB6600 stepper motor driver, AccelStepper library and Arduino: acceleration and deceleration. More info: https://www.makerguides.com */

#include <AccelStepper.h>

// Constants
const int DIR_PIN = 14;
const int STEP_PIN = 27;
const int MOTOR_INTERFACE_TYPE = 1;
const int MAX_SPEED = 20000;
const int ACCELERATION = 5000;
const int TARGET_POSITION = 8000;
const int RETURN_POSITION = 0;
const int DELAY_MS = 100;

// Create a new instance of the AccelStepper class
AccelStepper stepper(MOTOR_INTERFACE_TYPE, STEP_PIN, DIR_PIN);

void setup() {
  // Set the maximum speed and acceleration
  stepper.setMaxSpeed(MAX_SPEED);
  stepper.setAcceleration(ACCELERATION);
}

/**
 * The main loop function that runs repeatedly in the Arduino program.
 * It sets the target position of the stepper motor, runs the motor to the target position,
 * then moves the motor back to zero position and runs it again.
 * This loop is executed continuously until the Arduino is powered off or reset.
 */
void loop() {
  // Set the target position
  stepper.moveTo(TARGET_POSITION);
  // Run to target position with set speed and acceleration/deceleration
  stepper.runToPosition();

  delay(DELAY_MS);

  // Move back to zero
  stepper.moveTo(RETURN_POSITION);
  stepper.runToPosition();

  delay(DELAY_MS);
}
