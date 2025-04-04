#ifndef steppermotor_h
#define steppermotor_h

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
 */
typedef struct {
    float max_speed;    // motor max speed [step/second]
    float min_speed;
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


stepper_parameters_t stepper_param = {15000, 500, 7000, 11000, STEP_MODE_SIXTEENTH};
stepper_pinout_t stepper_pinout;
float current_speed = 0; // [Steps/sec]
uint32_t remaining_steps = 0;
uint32_t steps_done = 0;

TaskHandle_t moveTask;
void moveTaskcode(void* parameters);
void begin_steppers();
void move(uint32_t steps);
int move_task(uint64_t* t0, uint64_t* t1);
void disable_steppers();
void enable_steppers();

#endif