#include "servom.h"
#include <ESP32Servo.h>
#include <Arduino.h>

Servo leftm;
Servo rightm;
#define UP 90
#define DOWN 0
#define UD_DELAY 500

void servo_begin(uint8_t left_servo_pin, uint8_t right_servo_pin){
    leftm.attach(left_servo_pin);
    rightm.attach(right_servo_pin);
}

void lower_fork(){
    leftm.write(UP);
    rightm.write(180-UP);
    delay(UD_DELAY);
}

void rise_fork(){
    leftm.write(DOWN);
    rightm.write(180-DOWN);
    delay(UD_DELAY);
}