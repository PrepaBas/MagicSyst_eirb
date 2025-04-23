#ifndef BASIC_STRAT_H
#define BASIC_STRAT_H

#include "Arduino.h"
#include "table.h"
#include <ESP32Servo.h>
#include "StepperMotor.h"

#define HIGH_SPEED 10000
#define LOW_SPEED 2000

extern stepper_parameters_t stepper_param;

// Déclarations uniquement
void function1();
void deposit_bl_cans(void* parameters);
void deposit_tl_cans(void* parameters);
void go_home(void* parameters);
void deposit_bl_cans_2(void* parameters);

#endif
