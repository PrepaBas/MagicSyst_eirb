#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"
// put function declarations here:

void setup() {
}

void loop() {
  motor.move(0, 1000);
  delay(1000);
}