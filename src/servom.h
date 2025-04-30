#ifndef servom_h
#define servom_h

#include <ESP32Servo.h>
#include <Arduino.h>

void servo_begin(uint8_t left_servo_pin, uint8_t right_servo_pin);
void lower_fork();
void rise_fork();
#endif