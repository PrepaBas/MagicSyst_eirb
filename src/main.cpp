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


/* Extern from stepperMotor*/
extern uint32_t remaining_steps;
extern uint32_t steps_done;

void setup() {
  delay(1000);

  // begins and pins declaration
  Serial.begin(115200);
  begin_steppers(12, 32, 13, 33, 25, 26, 27, 14);
  servo_begin(23, 22);
  rise_fork();
  lower_fork();
  rise_fork();
  movement_begin(263, 72.2/2, 100, 865, 0);

  // init robot variables
  enable_steppers();
  set_speed(100);
  int robot_stop = 0;

  // Dispatch tasks
  xTaskCreate(securityTaskcode, "securityTask", 10000, &robot_stop, 2, &securityTask);    
  delay(500);
  xTaskCreate(moveTaskcode, "moveTask", 10000, NULL, 3, &moveTask);  
  delay(500); 
  follow_to({1000, 3000});
  xTaskCreate(dispatchTaskcode, "dispatchTask", 10000, &robot_stop, 1, &dispatchTask);   
  delay(500);
}


void loop(){ // do nothing
  
  while(1){
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

