#ifndef STEPPER_H
#define STEPPER_H

#include "Arduino.h"

/* objects */
/**
 * @brief step modes
 */
typedef enum {
    STEP_MODE_FULL = 1,
    STEP_MODE_2 = 2,
    STEP_MODE_4 = 4,
    STEP_MODE_8 = 8,
    STEP_MODE_16 = 16,
    STEP_MODE_32 = 32,
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

void moveTaskcode(void* parameters);
void set_direction(uint8_t first_stepper_dir, uint8_t second_stepper_dir);
void begin_steppers(uint8_t dir1, uint8_t dir2, uint8_t step1, uint8_t step2, uint8_t ms2, uint8_t ms1, uint8_t ms0, uint8_t enable);
void set_speed(float percentage_of_max_speed);
int move_task(uint64_t* t0, uint64_t* t1);
void disable_steppers();
void enable_steppers();
step_mode_t get_step_mode();

#endif