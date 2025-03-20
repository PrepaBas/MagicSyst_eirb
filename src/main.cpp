#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"
#include "table.h"
#include "basic_strat.h"
#include <ESP32Servo.h>

/*
#define LEFT_SERVO_PIN 23 
#define RIGHT_SERVO_PIN 22
Servo myleftservo;
Servo myrightservo;
*/
// DECLARATIONS *********************
TaskHandle_t moveTask;
void moveTaskcode(void* parameters);
TaskHandle_t dispatchTask;
void dispatchTaskcode(void* parameters);
TaskHandle_t securityTask;
void securityTaskcode(void* parameters);
RobotCoupe robot(263., 72.2/2);


void setup() {
  delay(1000);
  Serial.begin(115200);
  robot.motors.pinout = {12, 32, 13, 33, 25, 26, 27, 14};
  robot.motors.begin();
  /*
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  myleftservo.setPeriodHertz(50);
  myleftservo.attach(LEFT_SERVO_PIN, 1000, 2000);
  myrightservo.setPeriodHertz(50);
  myrightservo.attach(RIGHT_SERVO_PIN, 1000, 2000);
  */ 
  xTaskCreate(securityTaskcode, "securityTask", 10000, NULL, 2, &securityTask);    
  delay(500);
  xTaskCreate(moveTaskcode, "moveTask", 10000, NULL, 3, &moveTask);  
  delay(500);  
  vTaskDelay(pdMS_TO_TICKS(3000));
  xTaskCreate(dispatchTaskcode, "dispatchTask", 10000, NULL, 1, &dispatchTask);   
  delay(500);
}

// TASKS *******************


void moveTaskcode(void* parameters){
  vTaskDelay(pdMS_TO_TICKS(100));
  robot.motors.remaining_steps=0;
  robot.motors.current_speed=0;
  robot.motors.enable_steppers();
  robot.motors.param.max_speed = 8000;
  uint64_t t1;
  uint64_t t0 = esp_timer_get_time();
  Serial.println("move_task init");
  for(;;){
    if(robot.motors.move_task(&t0, &t1)) robot.steps_done++;
  }
}

void dispatchTaskcode(void* parameters) {
  /* example of run starting blue */
  // setting initial position 
  vTaskDelay(pdMS_TO_TICKS(100));
  
  robot.set_x(100);
  robot.set_y(865);
  robot.set_theta(0);
  robot.motors.enable_steppers();
  robot.motors.param.max_speed = 10000;
  TaskHandle_t currentTask;
  xTaskCreate(deposit_bl_cans, "currentTask", 10000, NULL, 1, &currentTask);  
  vTaskDelay(pdMS_TO_TICKS(1000));
  robot.motors.remaining_steps=5000;
  vTaskDelete(currentTask); 
  //robot.new_position();
  vTaskDelay(pdMS_TO_TICKS(1000));
  
  robot.new_position();
  go_home();
  vTaskDelay(pdMS_TO_TICKS(1000));
  deposit_bl_cans(NULL);
  deposit_tl_cans();
  go_home();
  vTaskDelete ( NULL );
}


void securityTaskcode(void* parameters){
  vTaskDelay(pdMS_TO_TICKS(3000));
  robot.motors.remaining_steps = 50;
  //go_home();
  for(;;){
    vTaskDelay(pdMS_TO_TICKS(10));
    //robot.motors.param.max_speed = robot.motors.param.min_speed+(ms++%50000);
  }
}

void loop(){
  
  while(1){
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

