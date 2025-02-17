#include <Arduino.h>
#include <StepperMotor.h>
// #include "advanced_movement.h"

StepperMotor motor;

void setup() {
  
  motor.pinout = {2, 3, 1, 1, 1, 1, 1};
  delay(200);
  motor.begin();
  delay(200);
}

void loop() {
  motor.move(0, 1000);
  delay(1000);
}