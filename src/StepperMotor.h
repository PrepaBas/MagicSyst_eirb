#ifndef kinematics_h
#define kinematics_h

#include "Arduino.h"

/* objects */
/**
 * @brief step modes
 */
typedef enum {
    STEP_MODE_FULL = 1,
    STEP_MODE_HALF = 2,
    STEP_MODE_QUARTER = 4,
    STEP_MODE_EIGHTH = 8,
    STEP_MODE_SIXTEENTH = 16,
} step_mode_t;

/**
 * @brief stepper parameters
 * @param max_speed
 * @param acceleration
 * @param deceleration
 * @param step_mode
 */
typedef struct {
    float max_speed;    // motor max speed [step/second]
    float acceleration; // moror acceleration [step/seconde^2]
    float deceleration;  // motor deceleration [step/second^2]
    step_mode_t step_mode;   // motor step mode
} stepper_parameters_t;

/**
 * @brief driver pinout connections
 */
typedef struct {
    uint8_t dir1_pin;        // direction pin
    uint8_t dir2_pin;        // direction pin
    uint8_t step1_pin;       // step pin
    uint8_t step2_pin;
    uint8_t ms1_pin;       // microstepping pin
    uint8_t ms2_pin;       // microstepping pin
    uint8_t ms3_pin;       // microstepping pin
    uint8_t en_pin;        // reset pin, pull low to disable step pin and force motor to reset position
} stepper_pinout_t;

/**
 * @brief class to make 2 stepper motors turn simultaniously
 */
class StepperMotor {
    public:
    stepper_parameters_t param = {10000, 8000, 8000, STEP_MODE_SIXTEENTH};
    stepper_pinout_t pinout;
    StepperMotor();
    void begin();
    void move(uint8_t dir, uint32_t steps);
    void disable_steppers();
    void enable_steppers();
};

#endif