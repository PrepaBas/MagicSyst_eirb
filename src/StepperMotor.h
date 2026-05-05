#ifndef STEPPER_H
#define STEPPER_H

#include "Arduino.h"
#include <TMCStepper.h>


#define R_SENSE         0.11f
#define DIAG_LED        8  

// Pins d'après ton schéma KiCad
#define STEP_L          21
#define DIR_L           20
#define STEP_R          0
#define DIR_R           1

// -------------------------------


// Initialisation des deux drivers avec leurs adresses respectives

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
    STEP_MODE_64 = 64,
    STEP_MODE_128 = 128,
    STEP_MODE_256 = 256
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
    uint8_t en_pin;
} stepper_pinout_t;

void moveTaskcode(void* parameters);
void set_direction(uint8_t first_stepper_dir, uint8_t second_stepper_dir);
void begin_steppers();
void set_speed(float percentage_of_max_speed);
void disable_steppers();
void enable_steppers();
step_mode_t get_step_mode();
int stop_stepper();

int is_stepper_running();
void stepper_move(uint64_t steps, int dirL, int dirR);

int32_t get_pose_right();
int32_t get_pose_left();

#endif