
#include <ESP32Servo.h>
#include <Arduino.h>
#include "NewPing.h"
#include "observer.h"
#include "advanced_movement.h"


#define PIN 21
#define SPEED_OF_SOUND 340
#define STEPS_TO_STOP 150

extern position position;
 
uint32_t stop_robot(int *robot_stop_ptr)
{
  *robot_stop_ptr = 1; // update status
  uint32_t cut_steps = 1; 
  if(remaining_steps > STEPS_TO_STOP){ // allow for slow stop
    cut_steps = remaining_steps - STEPS_TO_STOP;
    remaining_steps = STEPS_TO_STOP;
  }
  return cut_steps;
}

void resume_robot(int* robot_stop_ptr, uint32_t cut_steps){
  remaining_steps = cut_steps;
  *robot_stop_ptr = 0;
}

#define MAX_D 200
#define FRONT_EMPTY_STOP_DISTANCE 22
#define FRONT_LOADED_STOP_DISTANCE 30
void securityTaskcode(void *parameters)
{   // 21, 19, 18, 5, 17, 16, 4
  NewPing fc(19, 19, MAX_D); // front center
  NewPing fr(18, 18, MAX_D); // front right
  NewPing fl(5, 5, MAX_D); // front left
  NewPing rr(21, 21, MAX_D); // rear right
  NewPing rl(17, 17, MAX_D); // rear left
  //NewPing cn(21, 21, MAX_D); // cans near
  //NewPing cf(1, 1, MAX_D); // cans far

  int *robot_stop_ptr = (int *)parameters;

  status_t status = NO_STOP;

  unsigned long fc_distance;
  unsigned long fr_distance;
  unsigned long fl_distance;
  unsigned long rl_distance;
  unsigned long rr_distance;
  // unsigned long cn_distance;
  // unsigned long cf_distance;
  uint32_t cut_steps = 0;
  bool detect;
  unsigned long stop_timer;

  for (;;)
  {
    detect = 0;
//     vTaskDelay(pdMS_TO_TICKS(20));
// Serial.print(" fr : ");
//     Serial.print(fr.ping_cm());
// Serial.print(" rl : ");
//     Serial.print(fl.ping_cm());
// Serial.print(" rr : ");
//     Serial.print(rr.ping_cm());

// Serial.print(" rl : ");
//     Serial.println(rl.ping_cm());
    switch(protocol){
      case EMPTY_COMMUTE:
        // front right sensor
        fr_distance = fr.ping_cm();
        if(!fr_distance) fr_distance = MAX_D; 
        if(fr_distance<FRONT_EMPTY_STOP_DISTANCE) detect = 1;

        // front right sensor
        fl_distance = fl.ping_cm();
        if(!fl_distance) fl_distance = MAX_D; 
        if(fl_distance<FRONT_EMPTY_STOP_DISTANCE) detect = 1;

        // center right sensor
        fc_distance = fc.ping_cm();
        if(!fc_distance) fc_distance = MAX_D; 
        if(fc_distance<FRONT_EMPTY_STOP_DISTANCE) detect = 1;
        break;

      case LOADED_COMMUTE:
        // front right sensor
        fr_distance = fr.ping_cm();
        if(!fr_distance) fr_distance = MAX_D; 
        if(fr_distance<FRONT_LOADED_STOP_DISTANCE) detect = 1;

        // front right sensor
        fl_distance = fl.ping_cm();
        if(!fl_distance) fl_distance = MAX_D; 
        if(fl_distance<FRONT_LOADED_STOP_DISTANCE) detect = 1;

        // center right sensor
        fc_distance = fc.ping_cm();
        if(!fc_distance) fc_distance = MAX_D; 
        if(fc_distance<FRONT_LOADED_STOP_DISTANCE) detect = 1;
        break;
      case BACKING:
        rr_distance = rr.ping_cm();
        if(!rr_distance) rr_distance = MAX_D; 
        if(rr_distance < 15) detect = 1;
        
        rl_distance = rl.ping_cm();
        if(!rl_distance) rl_distance = MAX_D; 
        if(rl_distance < 15) detect = 1;
      break;

      case NO_SECURITY:
        vTaskDelay(pdMS_TO_TICKS(30));
      break;
    }
    switch(status){
      case NO_STOP:
        if(detect){
          Serial.println("stopping robot");
          cut_steps = stop_robot(robot_stop_ptr);
          stop_timer = esp_timer_get_time();
          status = YES_STOP;
        }
        break;

      case YES_STOP:
        if(detect){
          stop_timer = esp_timer_get_time();
        }
        else if(esp_timer_get_time() - stop_timer > 400*1000){
          Serial.println("resuming robot");
          resume_robot(robot_stop_ptr, cut_steps);
          cut_steps = 0; 
          status = NO_STOP;
        }
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(20));

    #ifdef SAVARY
    new_position();
    Serial.print(position.x);
    Serial.print(";");
    Serial.print(position.y);
    Serial.print(";");
    Serial.println(position.theta - 90);
    protocol = NO_SECURITY;
    #endif
  }
}
