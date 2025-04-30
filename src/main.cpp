#include <Arduino.h>
#include <time.h>
#include "StepperMotor.h"
#include "advanced_movement.h"
#include "rank.h"
#include "table.h"
#include "basic_strat.h"
#include "servom.h"

// DECLARATIONS *********************
TaskHandle_t dispatchTask;
extern void dispatchTaskcode(void* parameters);

TaskHandle_t securityTask;
extern void securityTaskcode(void* parameters);

TaskHandle_t currentTask;

TaskHandle_t moveTask;


/* Extern from stepperMotor*/
extern stepper_parameters_t stepper_param;
extern stepper_pinout_t stepper_pinout;
extern float current_speed; // [Steps/sec]
extern uint32_t remaining_steps;
extern uint32_t steps_done;


/* Extern from advanced_movement*/
extern struct position position;
extern move_type_t last_move_type;
extern float baseWidth;
extern float wheelRadius;




void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(50);
  stepper_pinout = {12, 32, 13, 33, 25, 26, 27, 14};
  set_x(100);
  set_y(865);
  set_theta(0);
  begin_steppers();
  servo_begin(23, 22);
  rise_fork();
  lower_fork();
  rise_fork();
  wheelRadius = 72.2/2;
  baseWidth = 263.;
  enable_steppers();
  stepper_param.max_speed = 10000;
  int robot_stop = 0;


  xTaskCreate(securityTaskcode, "securityTask", 10000, &robot_stop, 2, &securityTask);    
  delay(500);
  xTaskCreate(moveTaskcode, "moveTask", 10000, NULL, 3, &moveTask);  
  delay(500); 
  xTaskCreate(dispatchTaskcode, "dispatchTask", 10000, &robot_stop, 1, &dispatchTask);   
  delay(500);
}

// TASKS *******************


void loop(){
  
  while(1){
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

