#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"
#include "table.h"
#include "basic_strat.h"

TaskHandle_t Task1;
TaskHandle_t moveTask;



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

void moveTaskcode(void* parameters){
  vTaskDelay(pdMS_TO_TICKS(5000));
  robot.motors.remaining_steps=0;
  robot.motors.current_speed=0;
  robot.motors.enable_steppers();
  robot.motors.param.max_speed = 8000;
  vTaskDelay(pdMS_TO_TICKS(5000));
  robot.motors.move_task();
}

void Task2code(void* parameters){
  int i=0;
  for(;;){
    Serial.print(i++);
    Serial.print("\n");
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void loop(){
  
  xTaskCreate(moveTaskcode, "moveTask", 10000, NULL, 2, &moveTask);   
  xTaskCreate(Task2code, "Task2", 10000, NULL, 2, &Task1);      
  vTaskDelay(pdMS_TO_TICKS(5000));
  robot.move_straight(1, 1000);
  //deposit_bl_cans();
  //deposit_tl_cans();
  while(1){}
  robot.motors.enable_steppers();
  robot.motors.param.max_speed = 8000;
  deposit_bl_cans();
  deposit_tl_cans();
  go_home();
  while(1){delay(100);}
}

