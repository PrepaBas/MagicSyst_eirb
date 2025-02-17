#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"

// put function declarations here:

RobotCoupe robot(263, 72/2);

void setup() {
  robot.motors[0].pinout = {2, 3, 4, 5, 6, 7, 8};
  robot.motors[0].begin();
}

void loop() {
  robot.motors[0].move(0, 1000);
  delay(1000);
}