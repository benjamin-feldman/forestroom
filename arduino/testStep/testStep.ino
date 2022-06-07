// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

// Define a stepper and the pins it will use
//AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
//AccelStepper stepper(AccelStepper::FULL4WIRE, 2, 3, 4, 5);
AccelStepper stepper(AccelStepper::DRIVER, 27,14); //pul/dir

void setup()
{ 
 // Change these to suit your stepper if you want
 stepper.setMaxSpeed(50000);
 stepper.setAcceleration(50000);
 stepper.moveTo(300);
}

void loop()
{
   // If at the end of travel go to the other end
   if (stepper.distanceToGo() == 0)
     stepper.moveTo(-stepper.currentPosition());

   stepper.run();
}
