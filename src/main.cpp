#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"

// put function declarations here:

RobotCoupe robot(263, 72/2);

void setup() {
  robot.motors[0].pinout = {3, 4, 5, 6, 7, 8, 9};
  robot.motors[1].pinout = {10, 11, 5, 6, 7, 8, 9};
  robot.motors[0].begin();
  digitalWrite(robot.motors[0].pinout.ms1_pin, HIGH);
  digitalWrite(robot.motors[0].pinout.ms2_pin, HIGH);
  digitalWrite(robot.motors[0].pinout.ms2_pin, HIGH);
}

void loop(){
  delay(1000);
  robot.motors[0].move(1, 100);
  delay(1000);
  robot.motors[0].move(0, 100);
}