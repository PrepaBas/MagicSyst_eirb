
#include <ESP32Servo.h>
#include <Arduino.h>
#include "NewPing.h"
#include "observer.h"
#include "advanced_movement.h"


#define PIN 21
#define SPEED_OF_SOUND 340
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

void securityTaskcode(void *parameters)
{   
  //NewPing fc(17, 17, 200); // front center
  //NewPing fr(17, 17, 200); // front right
  //NewPing fl(17, 17, 200); // front left
  NewPing rr(21, 21, 8000); // rear right
  //NewPing rl(17, 17, 200); // rear left
  //NewPing cn(17, 17, 200); // cans near
  //NewPing cf(17, 17, 200); // cans far

  int *robot_stop_ptr = (int *)parameters;
  
  for (;;)
  {
    //unsigned long fc_distance = fc.ping_cm();
    //unsigned long fr_distance = fr.ping_cm();
    //unsigned long fl_distance = fl.ping_cm();
    //unsigned long rl_distance = rl.ping_cm();
    //unsigned long cn_distance = cn.ping_cm();
    //unsigned long cf_distance = cf.ping_cm();
    
    unsigned long rr_distance = rr.ping_cm(); if(!rr_distance) rr_distance = 200;
    if(rr_distance>20){
      
    }
    else{
      //Serial.println("entering stop mode");
      uint32_t cut_steps = stop_robot(robot_stop_ptr);
      //Serial.println(cut_steps);
      vTaskDelay(pdMS_TO_TICKS(3000));
      resume_robot(robot_stop_ptr, cut_steps);
    }
    
    vTaskDelay(pdMS_TO_TICKS(30));
  }
}

// #define FRONT_EMPTY_STOP_DISTANCE 16
// #define FRONT_LOADED_STOP_DISTANCE 30
// void securityTaskcode(void *parameters)
// {   // 21, 19, 18, 5, 17, 16, 4
//   NewPing fc(1, 1, 50); // front center
//   NewPing fr(19, 19, 50); // front right
//   NewPing fl(5, 5, 50); // front left
//   NewPing rr(1, 1, 50); // rear right
//   NewPing rl(17, 17, 50); // rear left
//   NewPing cn(21, 21, 50); // cans near
//   NewPing cf(1, 1, 50); // cans far

//   int *robot_stop_ptr = (int *)parameters;

//   status_t status = NO_STOP;

//   unsigned long fc_distance;
//   unsigned long fr_distance;
//   unsigned long fl_distance;
//   unsigned long rl_distance;
//   unsigned long rr_distance;
//   unsigned long cn_distance;
//   unsigned long cf_distance;
//   uint32_t cut_steps = 0;
//   bool detect;
//   unsigned long stop_timer;

//   // for(;;){
//   //       fr_distance = fr.ping_cm();
//   //       if(!fr_distance) fr_distance = 200;
//   //       Serial.print("fr = "); Serial.println(fr_distance);

//   //       fl_distance = fl.ping_cm();
//   //       if(!fl_distance) fl_distance = 200;
//   //       Serial.print("fl = "); Serial.println(fl_distance);

//   //       fr_distance = rr.ping_cm();
//   //       if(!rr_distance) rr_distance = 200;
//   //       Serial.print("rr = "); Serial.println(rr_distance);

//   //       rl_distance = rl.ping_cm();
//   //       if(!rl_distance) rl_distance = 200;
//   //       Serial.print("rl = "); Serial.println(rl_distance);
//   //     Serial.println("-------------");
//   //   vTaskDelay(pdMS_TO_TICKS(200));
//   // }

//   for (;;)
//   {
//     detect = 0;
//     switch(protocol){
//       case EMPTY_COMMUTE:
//         // front right sensor
//         fr_distance = fr.ping_cm();
//         if(!fr_distance) fr_distance = 200; 
//         if(fr_distance<FRONT_EMPTY_STOP_DISTANCE) detect = 1;
//         // Serial.print("fr = "); Serial.println(fr_distance);

//         // front right sensor
//         fl_distance = fl.ping_cm();
//         if(!fl_distance) fl_distance = 200; 
//         if(fl_distance<FRONT_EMPTY_STOP_DISTANCE) detect = 1;
//         // Serial.print("fl = "); Serial.println(fl_distance);

//         // center right sensor
//         fc_distance = fc.ping_cm();
//         if(!fc_distance) fc_distance = 200; 
//         if(fc_distance<FRONT_EMPTY_STOP_DISTANCE) detect = 1;
//         break;

//       case LOADED_COMMUTE:
//       // front right sensor
//         fr_distance = fr.ping_cm();
//         if(!fr_distance) fr_distance = 200; 
//         if(fr_distance<FRONT_LOADED_STOP_DISTANCE) detect = 1;

//         // front right sensor
//         fl_distance = fl.ping_cm();
//         if(!fl_distance) fl_distance = 200; 
//         if(fl_distance<FRONT_LOADED_STOP_DISTANCE) detect = 1;

//         // center right sensor
//         fc_distance = fc.ping_cm();
//         if(!fc_distance) fc_distance = 200; 
//         if(fc_distance<FRONT_LOADED_STOP_DISTANCE) detect = 1;
//         break;

//       case EMPTY_APPROACH:
//       break;
//       case LOADED_APPROACH:
//       break;
//       case BACKING:
//       break;
//       case NO_SECURITY:
//       break;
//     }
//     switch(status){
//       case NO_STOP:
//         if(detect){
//           Serial.println("stopping robot");
//           uint32_t cut_steps = stop_robot(robot_stop_ptr);
//           stop_timer = esp_timer_get_time();
//           status = YES_STOP;
//         }
//         break;

//       case YES_STOP:
//         if(detect){
//           stop_timer = esp_timer_get_time();
//         }
//         else if(esp_timer_get_time() - stop_timer > 3000000){
//           Serial.println("resuming robot");
//           resume_robot(robot_stop_ptr, cut_steps);
//           status = NO_STOP;
//         }
//         break;
//     }
//     vTaskDelay(pdMS_TO_TICKS(100));
//   }
// }
