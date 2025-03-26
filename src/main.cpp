#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"
#include "table.h"
#include "basic_strat.h"
#include "rank.h"
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
RobotCoupe robot(263., 72.7/2);


void setup() {
  delay(1000);
  Serial.begin(115200);
  robot.motors.pinout = {12, 32, 13, 33, 25, 26, 27, 14};
  robot.motors.begin();
  delay(100);
  robot.motors.remaining_steps=0;  
  robot.set_x(100);
  robot.set_y(865);
  robot.set_theta(0);
  robot.motors.enable_steppers();
  robot.motors.param.max_speed = 10000;
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


#define DISPATCH_TASK_TILL_END(taskcode)  task_on = 1;\
xTaskCreate(taskcode, "currentTask", 10000, &task_on, 1, &currentTask);\
while(task_on){vTaskDelay(10);} 

TaskHandle_t currentTask;

void dispatchTaskcode(void* parameters) {
  /* example of run starting blue */
  // setting initial position 
  vTaskDelay(pdMS_TO_TICKS(100));
  
  int task_on = 1;
  function_rank_t* function_rank_ptr = function_rank_begin();
  while(function_rank_ptr != NULL){
    function_rank_t* function_rank_ptr_temp = function_rank_ptr;
    while(function_rank_ptr_temp != NULL){
      if(function_rank_ptr_temp->prev_function == NULL) break;
      function_rank_ptr_temp = function_rank_ptr_temp->prev_function;
    }
    DISPATCH_TASK_TILL_END(*(function_rank_ptr_temp->function));
    function_rank_ptr = function_rank_remove(function_rank_ptr, function_rank_ptr_temp);
  }


  vTaskDelete ( NULL );
}


void securityTaskcode(void* parameters){
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

