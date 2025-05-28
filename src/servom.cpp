#include "servom.h"
#include <ESP32Servo.h>
#include <Arduino.h>

Servo leftm;
Servo rightm;
#define UP_L 42
#define UP_R 50

#define MID_R 22
#define MID_L 19

#define DOWN_R 5
#define DOWN_L 0
#define UD_DELAY 500 // travel time between up and down

uint8_t pin1;
uint8_t pin2;

void servo_begin(uint8_t left_servo_pin, uint8_t right_servo_pin){
    leftm.attach(left_servo_pin);
    rightm.attach(right_servo_pin);
    pin1 = left_servo_pin;
    pin2 = right_servo_pin;
}

void servo_terminate(){
    leftm.release();
    rightm.release();
    pinMode(pin1, INPUT);
    pinMode(pin2, INPUT);
}

void lower_fork(){
    leftm.write(MID_L);
    rightm.write(90-MID_R);
    
    vTaskDelay(pdMS_TO_TICKS(UD_DELAY));
}

void fold_fork(){
    leftm.write(DOWN_L);
    rightm.write(90-DOWN_R);
}
void unfold_fork(){
    leftm.write(UP_L);
    rightm.write(90-UP_R);
}

void rise_fork(){
    rightm.write(90-UP_R);
    leftm.write(UP_L);
   
    vTaskDelay(pdMS_TO_TICKS(UD_DELAY));
}