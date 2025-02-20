#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"

// put function declarations here:

RobotCoupe robot(263, 72/2);

void setup() {
  delay(5000);
  robot.motors.pinout = {0, 2, 1, 5, 6, 10, 11, 7};
  robot.motors.begin();
  digitalWrite(robot.motors.pinout.ms1_pin, HIGH);
  digitalWrite(robot.motors.pinout.ms2_pin, HIGH);
  digitalWrite(robot.motors.pinout.sleep_pin, LOW);
  robot.set_x(300);
  robot.set_y(300);
}

void loop(){

  robot.follow_to({1000, 2651});
  delay(500);
  robot.follow_to({300, 300});
  while(1){}

}