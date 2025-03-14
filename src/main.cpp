#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"
#include "table.h"
#include "basic_strat.h"

TaskHandle_t Task1;

TaskHandle_t Task2;
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
  robot.motors.remaining_steps=0;
  robot.motors.current_speed=0;
  robot.motors.enable_steppers();
  robot.motors.param.max_speed = 8000;
  uint64_t t1;
  uint64_t t0 = esp_timer_get_time();
  Serial.println("move_task init");
  for(;;){
    robot.motors.move_task(&t0, &t1);
  }
}

void Task1code(void* parameters){
  
  vTaskDelay(pdMS_TO_TICKS(3000));
  robot.motors.param.max_speed=10000;
  robot.move_straight(1, 1000000000);

  
  vTaskDelay(pdMS_TO_TICKS(4000));
  robot.motors.param.max_speed=15000;
  robot.move_straight(1, 1000000000);
  for(;;){
    Serial.println("chokobar");
  }
}

void Task2code(void* parameters){
  int i=0;
  
  vTaskDelay(pdMS_TO_TICKS(5000));
  robot.motors.remaining_steps = 100;
  vTaskDelay(pdMS_TO_TICKS(7000));
  robot.motors.remaining_steps = 3000;
  vTaskDelay(pdMS_TO_TICKS(10000));
  for(;;){
    Serial.println("chokobar");
  }
}

void loop(){
  vTaskDelay(pdMS_TO_TICKS(3000));
  xTaskCreate(moveTaskcode, "moveTask", 10000, NULL, 3, &moveTask);  
  delay(500);  
  xTaskCreate(Task1code, "Task1", 10000, NULL, 1, &Task1);   
  delay(500);
  xTaskCreate(Task2code, "Task2", 10000, NULL, 2, &Task2);    
  delay(500);
  

  while(1){Serial.println("no loop");}
  robot.motors.enable_steppers();
  robot.motors.param.max_speed = 8000;
  deposit_bl_cans();
  deposit_tl_cans();
  go_home();
  while(1){delay(100);}
}

