#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"
#include "table.h"
#include "basic_strat.h"

TaskHandle_t Task1;



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

void Task1code(void* parameters){
  uint32_t* param = (uint32_t*) parameters;
  while(1){
    delay(100);
    Serial.print("change danger\n");
    *param+=1;
    Serial.print("changed\n");
  }
}

void loop(){
  uint32_t* danger = (uint32_t*) malloc(sizeof(uint32_t));
  xTaskCreatePinnedToCore(Task1code, "Task1", 10000, danger, 2, &Task1, 0);         
  delay(500);
  for(;;){
    Serial.print("On core 1 : ");
    Serial.print(*danger);
    Serial.print("\n");
    delay(100);
  }
  robot.motors.enable_steppers();
  robot.motors.param.max_speed = 8000;
  delay(5000);
  deposit_bl_cans();
  deposit_tl_cans();
  go_home();
  while(1){delay(100);}
}

