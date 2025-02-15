#ifndef kinematics_h
#define kinematics_h

#include "Arduino.h"

/* objects */
/**
 * @brief step mode
 */
typedef enum {
    STEP_MODE_FULL = 0,
    STEP_MODE_HALF,
    STEP_MODE_QUARTER,
    STEP_MODE_EIGHTH,
    STEP_MODE_SIXTEENTH,
} step_mode_t;

/**
 * @brief stepper parameters
 */
typedef struct {
    int steps;          // number of steps for full revolution in full step mode
    float max_speed;    // motor max speed
    float acceleration; // moror acceleration
    float deceleration;  // motor deceleration
    step_mode_t step_mode;   // motor step mode

} stepper_parameters_t;

typedef struct {
    uint8_t dir_pin;        // direction pin
    uint8_t step_pin;       // step pin
    uint8_t ms1p_pin;       // microstepping pin
    uint8_t ms2p_pin;       // microstepping pin
    uint8_t ms3p_pin;       // microstepping pin
    uint8_t sleep_pin;      // sleep pin, pull low to disable coils output
    uint8_t rst_pin;        // reset pin, pull low to disable step pin and force motor to reset position
} stepper_pinout_t;


class StepperMotor {
    public:
    stepper_parameters_t param;
    stepper_pinout_t pinout;
    StepperMotor();


}

#endif