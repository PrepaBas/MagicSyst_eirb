#include "servom.h"
#include <ESP32Servo.h>
#include <Arduino.h>

Servo leftm;
Servo rightm;
#define UP 70
#define DOWN 0
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
    leftm.write(DOWN);
    rightm.write(180-DOWN);
    
    vTaskDelay(pdMS_TO_TICKS(UD_DELAY));
}

void rise_fork(){
    rightm.write(180-UP);
    leftm.write(UP);
   
    vTaskDelay(pdMS_TO_TICKS(UD_DELAY));
}