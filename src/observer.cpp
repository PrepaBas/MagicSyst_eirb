
#include <ESP32Servo.h>
#include "NewPing.h"
#include "observer.h"
#include "advanced_movement.h"


uint32_t stop_robot(int *robot_stop_ptr)
{
  *robot_stop_ptr = 1; // update status
  vTaskSuspend(currentTask); // prevent any further actions
  uint32_t cut_steps = 0; 
  if(remaining_steps > 200){ // allow for slow stop
    cut_steps = remaining_steps - 200;
    remaining_steps = 200;
  }
  while(remaining_steps){} // wait for complete stop of robot
  new_position(); // update position as the normal function is paused and might be canceled
  return cut_steps;
}

void resume_robot(int* robot_stop_ptr, uint32_t cut_steps){
  remaining_steps = cut_steps;
  vTaskResume(currentTask);
  *robot_stop_ptr = 0;
}

void securityTaskcode(void *parameters)
{/*
  NewPing fc(17, 17, 200); // front center
  NewPing fr(17, 17, 200); // front right
  NewPing fl(17, 17, 200); // front left
  NewPing rr(17, 17, 200); // rear right
  NewPing rl(17, 17, 200); // rear left
  NewPing cn(17, 17, 200); // cans near
  NewPing cf(17, 17, 200); // cans far

  int *robot_stop_ptr = (int *)parameters;

  for (;;)
  {
    unsigned long fc_distance = fc.ping_cm();
    unsigned long fr_distance = fr.ping_cm();
    unsigned long fl_distance = fl.ping_cm();
    unsigned long rr_distance = rr.ping_cm();
    unsigned long rl_distance = rl.ping_cm();
    unsigned long cn_distance = cn.ping_cm();
    unsigned long cf_distance = cf.ping_cm();
    vTaskDelay(pdMS_TO_TICKS(10));
    
    if(fc_distance < 20){
      int cut_steps = stop_robot(robot_stop_ptr);
      vTaskDelay(pdMS_TO_TICKS(3000));
      resume_robot(robot_stop_ptr, cut_steps);
    }
      */
  for(;;){
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}
