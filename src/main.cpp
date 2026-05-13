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


#define BAU_PIN 255

void bauTaskcode(void* param){
  int *robot_stop_ptr = (int *)param;
  for(;;){
    if(!digitalRead(BAU_PIN)){
      *robot_stop_ptr = 1;
      disable_steppers();
      servo_terminate();
      vTaskDelete(currentTask);
      vTaskDelete(dispatchTask);
      vTaskDelete(securityTask);
      vTaskSuspend(moveTask);
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
  //dir1/dir2/step1/step2/mso
  begin_steppers();
  // servo_begin(23, 22);
  // rise_fork();
  movement_begin(75.8, 71.16/50.*16./2., 100, 865, 0); // wheel diameter is 72.8 

  // init robot variables
  // pinMode(BAU_PIN, INPUT);
  // while(!digitalRead(BAU_PIN)){
  //   // Serial.println("waiting closing of bau");
  //   delay(100);
  // }
  delay(100);
  // Serial.println("starting emergency_stop_task");
  static int robot_stop = 0;
  vTaskDelay(pdMS_TO_TICKS(30));
  // xTaskCreate(bauTaskcode, "emergency_stop_task", 1000, &robot_stop, 4, &bau);
  vTaskDelay(pdMS_TO_TICKS(300));
  enable_steppers();
  set_speed(1.0);

  
    
  // Dispatch tasks
  Serial.println("starting securityTask");
  vTaskDelay(pdMS_TO_TICKS(30));
  #ifdef SAVARY
  xTaskCreate(securityTaskcode, "securityTask", 10000, &robot_stop, 3, &securityTask);   
  #endif
  #ifndef SAVARY 
  xTaskCreate(securityTaskcode, "securityTask", 10000, &robot_stop, 2, &securityTask);   
  #endif
  vTaskDelay(pdMS_TO_TICKS(700));

  Serial.println("starting moveTask");
  vTaskDelay(pdMS_TO_TICKS(30));
  xTaskCreate(moveTaskcode, "moveTask", 10000, &robot_stop, 3, &moveTask);  
  vTaskDelay(pdMS_TO_TICKS(700)); 

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

