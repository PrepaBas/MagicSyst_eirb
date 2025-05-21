#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"
#include "rank.h"
#include "table.h"
#include "basic_strat.h"
#include "servom.h"
#include "observer.h"

// DECLARATIONS of taskhandle
TaskHandle_t dispatchTask;
TaskHandle_t securityTask;
TaskHandle_t currentTask;
TaskHandle_t moveTask;
TaskHandle_t bau;



/* Extern from stepperMotor*/
extern uint32_t remaining_steps;
extern uint32_t steps_done;
#define BAU_PIN 36

void bauTaskcode(void* param){
  for(;;){
    if(!digitalRead(BAU_PIN)){
      servo_terminate();
      vTaskSuspend(currentTask);
      vTaskSuspend(dispatchTask);
      vTaskSuspend(securityTask);
      disable_steppers();
      while(1){
        vTaskDelay(pdMS_TO_TICKS(10000));
      }
    }
    vTaskDelay(pdMS_TO_TICKS(30));
  }
}


void setup() {
  delay(1000);

  // begins and pins declaration
  Serial.begin(115200);
  begin_steppers(12, 32, 13, 33, 25, 26, 27, 14);
  servo_begin(23, 22);
  // rise_fork();
  movement_begin(261.9, 72.8/2, 100, 865, 0); // wheel diameter is 72.8 

  // init robot variables
  pinMode(BAU_PIN, INPUT);
  while(!digitalRead(BAU_PIN)){
    Serial.println("waiting closing of bau");
    delay(100);
  }
  delay(100);
  Serial.println("starting emergency_stop_task");
  vTaskDelay(pdMS_TO_TICKS(30));
  xTaskCreate(bauTaskcode, "emergency_stop_task", 1000, NULL, 4, &bau);
  vTaskDelay(pdMS_TO_TICKS(300));
  enable_steppers();
  set_speed(1);
  int robot_stop = 0;

  
    
  // Dispatch tasks
  Serial.println("starting securityTask");
  vTaskDelay(pdMS_TO_TICKS(30));
  xTaskCreate(securityTaskcode, "securityTask", 10000, &robot_stop, 2, &securityTask);    
  vTaskDelay(pdMS_TO_TICKS(300));
  Serial.println("starting moveTask");
  vTaskDelay(pdMS_TO_TICKS(30));
  xTaskCreate(moveTaskcode, "moveTask", 10000, &robot_stop, 3, &moveTask);  
  vTaskDelay(pdMS_TO_TICKS(300)); 
  Serial.println("starting dispatchtyTask");
  vTaskDelay(pdMS_TO_TICKS(30));
  xTaskCreate(dispatchTaskcode, "dispatchTask", 10000, &robot_stop, 1, &dispatchTask);   
  vTaskDelay(pdMS_TO_TICKS(300));
  Serial.println("all task are launched");
}


void loop(){ // do nothing
  
  while(1){
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

