#ifndef BASIC_STRAT_H
#define BASIC_STRAT_H

#include "Arduino.h"
#include "table.h"
#include <ESP32Servo.h>
#include "StepperMotor.h"

#define HIGH_SPEED 10000
#define LOW_SPEED 2000

enum security_protocol{
    EMPTY_COMMUTE,
    LOADED_COMMUTE,
    EMPTY_APPROACH,
    LOADED_APPROACH,
    BACKING,
    NO_SECURITY,
};
// Déclarations uniquement
void baniere(void* param);
void function1();
void deposit_bl_cans(void* parameters);
void deposit_tl_cans(void* parameters);
void go_home(void* parameters);
void deposit_bl_cans_2(void* parameters);
//hard strat
void blue(void* param);
void orange(void* param);

#endif
