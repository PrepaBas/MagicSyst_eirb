
#include <ESP32Servo.h>
#include "NewPing.h"
#include "observer.h"
#include "advanced_movement.h"


uint32_t stop_robot(int *robot_stop_ptr)
{
  *robot_stop_ptr = 1; // update status
  uint32_t cut_steps = 0; 
  if(remaining_steps > 100){ // allow for slow stop
    cut_steps = remaining_steps - 100;
    remaining_steps = 100;
  }
  while(remaining_steps>1){vTaskDelay(pdMS_TO_TICKS(10));} // wait for complete stop of robot
  new_position(); // update position as the normal function is paused and might be canceled
  return cut_steps;
}

void resume_robot(int* robot_stop_ptr, uint32_t cut_steps){
  remaining_steps = cut_steps;
  *robot_stop_ptr = 0;
}

void securityTaskcode(void *parameters)
{
  //NewPing fc(17, 17, 200); // front center
  //NewPing fr(17, 17, 200); // front right
  //NewPing fl(17, 17, 200); // front left
  NewPing rr(21, 21, 200); // rear right
  //NewPing rl(17, 17, 200); // rear left
  //NewPing cn(17, 17, 200); // cans near
  //NewPing cf(17, 17, 200); // cans far

  int *robot_stop_ptr = (int *)parameters;

  for (;;)
  {
    vTaskDelay(pdMS_TO_TICKS(100));
    //unsigned long fc_distance = fc.ping_cm();
    //unsigned long fr_distance = fr.ping_cm();
    //unsigned long fl_distance = fl.ping_cm();
    unsigned long rr_distance = rr.ping_cm(); if(rr_distance == 0) rr_distance = 200;
    //unsigned long rl_distance = rl.ping_cm();
    //unsigned long cn_distance = cn.ping_cm();
    //unsigned long cf_distance = cf.ping_cm();
    if(rr_distance<20){
      Serial.println("entering stop mode");
      uint32_t cut_steps = stop_robot(robot_stop_ptr);
      Serial.println(cut_steps);
      delay(3000);
      resume_robot(robot_stop_ptr, cut_steps);
    }
    /*
    if(fc_distance < 20){
      int cut_steps = stop_robot(robot_stop_ptr);
      vTaskDelay(pdMS_TO_TICKS(3000));
      resume_robot(robot_stop_ptr, cut_steps);
      */
    }
}
