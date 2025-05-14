
#include <ESP32Servo.h>
#include "NewPing.h"
#include "observer.h"
#include "advanced_movement.h"


#define PIN 21
#define SPEED_OF_SOUND 340

float test(){
    uint64_t t2;
    uint64_t t1 = esp_timer_get_time();
    uint64_t t0 = esp_timer_get_time();

    // Output signal
    pinMode(PIN, OUTPUT);
    digitalWrite(PIN, HIGH);
    while(t1 - t0 < 20){
        t1 = esp_timer_get_time();
    }
    digitalWrite(PIN, LOW);
    t0 = esp_timer_get_time();
    pinMode(PIN, INPUT);
    // Wait for signal reception
    t0 = esp_timer_get_time();
    for(;;){
        t2 = esp_timer_get_time();
        if(digitalRead(PIN)) break;
        if(t2-t0 > 1000000) break;
    }
    float distance = t2-t0;//SPEED_OF_SOUND * t2-t0 * 0.0001;
    return distance;
    
}

uint32_t stop_robot(int *robot_stop_ptr)
{
  *robot_stop_ptr = 1; // update status
  uint32_t cut_steps = 0; 
  if(remaining_steps > 100){ // allow for slow stop
    cut_steps = remaining_steps - 100;
    remaining_steps = 100;
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
