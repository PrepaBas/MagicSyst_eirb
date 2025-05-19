
#include <ESP32Servo.h>
#include "NewPing.h"
#include "observer.h"
#include "advanced_movement.h"


#define PIN 21
#define SPEED_OF_SOUND 340
#define STEPS_TO_STOP 200

#define STEPS_TO_STOP 200


uint32_t stop_robot(int *robot_stop_ptr)
{
  uint16_t steps_to_stop = STEPS_TO_STOP;
  *robot_stop_ptr = 1; // update status
  uint32_t cut_steps = 0; 
  if(remaining_steps > steps_to_stop){ // allow for slow stop
    cut_steps = remaining_steps - steps_to_stop;
    remaining_steps = steps_to_stop;
  }
  while(remaining_steps>1){vTaskDelay(pdMS_TO_TICKS(10));} // wait for complete stop of robot
  return cut_steps;
}

void resume_robot(int* robot_stop_ptr, uint32_t cut_steps){
  remaining_steps = cut_steps;
  *robot_stop_ptr = 0;
}



#define FRONT_EMPTY_STOP_DISTANCE 16

#define FRONT_LOADED_STOP_DISTANCE 30
void securityTaskcode(void *parameters)
{   // 21, 19, 18, 5, 17, 16, 4
  NewPing fc(17, 17, 200); // front center
  NewPing fr(19, 19, 200); // front right
  NewPing fl(5, 5, 200); // front left
  NewPing rr(17, 21, 200); // rear right
  NewPing rl(17, 17, 200); // rear left
  NewPing cn(17, 17, 200); // cans near
  NewPing cf(17, 17, 200); // cans far
{   // 21, 19, 18, 5, 17, 16, 4
  NewPing fc(17, 17, 200); // front center
  NewPing fr(19, 19, 200); // front right
  NewPing fl(5, 5, 200); // front left
  NewPing rr(17, 21, 200); // rear right
  NewPing rl(17, 17, 200); // rear left
  NewPing cn(17, 17, 200); // cans near
  NewPing cf(17, 17, 200); // cans far

  int *robot_stop_ptr = (int *)parameters;

  status_t status = NO_STOP;

  unsigned long fc_distance;
  unsigned long fr_distance;
  unsigned long fl_distance;
  unsigned long rl_distance;
  unsigned long rr_distance;
  unsigned long cn_distance;
  unsigned long cf_distance;
  uint32_t cut_steps = 0;
  bool detect;
  unsigned long stop_timer;


  status_t status = NO_STOP;

  unsigned long fc_distance;
  unsigned long fr_distance;
  unsigned long fl_distance;
  unsigned long rl_distance;
  unsigned long rr_distance;
  unsigned long cn_distance;
  unsigned long cf_distance;
  uint32_t cut_steps = 0;
  bool detect;
  unsigned long stop_timer;

  for (;;)
  {
    detect = 0;
    switch(protocol){
      case EMPTY_COMMUTE:
        // front right sensor
        fr_distance = fr.ping_cm();
        if(!fr_distance) fr_distance = 200; 
        if(fr_distance<FRONT_EMPTY_STOP_DISTANCE) detect = 1;

        // front right sensor
        fl_distance = fl.ping_cm();
        if(!fl_distance) fl_distance = 200; 
        if(fl_distance<FRONT_EMPTY_STOP_DISTANCE) detect = 1;

        // center right sensor
        fl_distance = fl.ping_cm();
        if(!fc_distance) fc_distance = 200; 
        if(fc_distance<FRONT_EMPTY_STOP_DISTANCE) detect = 1;
        break;

      // case LOADED_COMMUTE:
      // break;
      // case EMPTY_APPROACH:
      // break;
      // case LOADED_APPROACH:
      // break;
      // case BACKING:
      // break;
      // case NO_SECURITY:
      // break;
    }
    switch(status){
      case NO_STOP:
        if(detect){
          uint32_t cut_steps = stop_robot(robot_stop_ptr);
          stop_timer = esp_timer_get_time();
          status = YES_STOP;
        }
        break;

      case YES_STOP:
        if(detect){
          stop_timer = esp_timer_get_time();
        }
        else if(esp_timer_get_time() - stop_timer > 3000000){
          resume_robot(robot_stop_ptr, cut_steps);
          status = NO_STOP;
        }
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
