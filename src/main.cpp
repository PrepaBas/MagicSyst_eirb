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
TaskHandle_t dispatchTask;
void dispatchTaskcode(void* parameters);
TaskHandle_t securityTask;
void securityTaskcode(void* parameters);

TaskHandle_t currentTask;

#define DISPATCH_TASK_TILL_END(taskcode)  dispatch_wait = 1;\
xTaskCreate(taskcode, "currentTask", 10000, &dispatch_wait, 1, &currentTask);\
while(dispatch_wait){if(*robot_stop_ptr == 1) break; vTaskDelay(10);} 


void setup() {
  delay(1000);
  Serial.begin(115200);
  stepper_pinout = {12, 32, 13, 33, 25, 26, 27, 14};
  begin();
  delay(100);
  set_x(100);
  set_y(865);
  set_theta(0);
  enable_steppers();
  stepper_param.max_speed = 10000;
  /*
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  myleftservo.setPeriodHertz(50);
  myleftservo.attach(LEFT_SERVO_PIN, 1000, 2000);
  myrightservo.setPeriodHertz(50);
  myrightservo.attach(RIGHT_SERVO_PIN, 1000, 2000);
  */ 
  int robot_stop = 0;
  xTaskCreate(securityTaskcode, "securityTask", 10000, &robot_stop, 2, &securityTask);    
  delay(500);
  xTaskCreate(moveTaskcode, "moveTask", 10000, NULL, 3, &moveTask);  
  delay(500); 
  xTaskCreate(dispatchTaskcode, "dispatchTask", 10000, &robot_stop, 1, &dispatchTask);   
  delay(500);
}

// TASKS *******************


void dispatchTaskcode(void* parameters) {
  vTaskDelay(pdMS_TO_TICKS(3000));
  int dispatch_wait = 1; 
  int* robot_stop_ptr = (int*)parameters;
  /* Creation of function structures */
  Function functions[] ={
    deposit_bl_cans,
    deposit_tl_cans,
    go_home,
    NULL
  };
  function_rank_t* function_rank_list = function_rank_begin(functions);

  /* Loop while actions are left */
  while(function_rank_list != NULL){
    /* calculate new rank */


    /* choose function to execute */
    function_rank_t* function_rank_ptr = function_rank_list;
    while(function_rank_ptr != NULL){
      if(function_rank_ptr->prev_function == NULL) break;
      function_rank_ptr = function_rank_ptr->prev_function;
    }

    /* execute function and remove it from available functions */
    DISPATCH_TASK_TILL_END(*(function_rank_ptr->function));
    while(*robot_stop_ptr == 1){
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    if(dispatch_wait == 0){
      function_rank_list = function_rank_remove(function_rank_list, function_rank_ptr);
    }
  }

  vTaskDelete ( NULL );
}


void securityTaskcode(void* parameters){
  vTaskDelay(pdMS_TO_TICKS(4500));
  int* robot_stop_ptr = (int*)parameters;
  *robot_stop_ptr = 1;
  vTaskDelete(currentTask);
  remaining_steps = 200;
  new_position();
  vTaskDelay(pdMS_TO_TICKS(5000));
  *robot_stop_ptr = 0;
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

