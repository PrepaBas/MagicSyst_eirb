#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"

// put function declarations here:

RobotCoupe robot(263, 72/2);
void setup() {
  delay(1000);
  Serial.begin(115200);
  robot.motors.pinout = {12, 32, 13, 33, 25, 26, 27, 14};
  robot.motors.begin();
  digitalWrite(robot.motors.pinout.ms1_pin, HIGH);
  digitalWrite(robot.motors.pinout.ms2_pin, HIGH);
  robot.set_x(300);
  robot.set_y(300);
}

void loop(){
  delay(1000);
  //robot.move_straight(1, 100);
  //robot.move_straight(0, 5);
  robot.follow_to({1000, 1657});
  while(1){delay(100);}

}