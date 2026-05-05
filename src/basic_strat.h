#ifndef BASIC_STRAT_H
#define BASIC_STRAT_H

#include "Arduino.h"
#include "table.h"
#include <ESP32Servo.h>
#include "StepperMotor.h"

// #define SAVARY
// #define PRINT_DISTANCE

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

void line_of_truth();
void function1();
#endif
