#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"
#include "table.h"
#include "basic_strat.h"

// put function declarations here:

RobotCoupe robot(263.4, 72.5/2);
void setup() {
  delay(1000);
  Serial.begin(115200);
  robot.motors.pinout = {12, 32, 13, 33, 25, 26, 27, 14};
  robot.motors.begin();
  digitalWrite(robot.motors.pinout.ms1_pin, HIGH);
  digitalWrite(robot.motors.pinout.ms2_pin, HIGH);
  digitalWrite(robot.motors.pinout.ms3_pin, HIGH);
 
  robot.set_x(100);
  robot.set_y(865);
  robot.set_theta(0);
}

void loop(){
  robot.motors.enable_steppers();
  robot.motors.param.max_speed = 8000;
  delay(5000);
  function2();
  while(1){delay(100);}

}