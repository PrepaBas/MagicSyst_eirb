#ifndef BASIC_STRAT_H
#define BASIC_STRAT_H

#include "Arduino.h"
#include "advanced_movement.h" 
#include "table.h"
#include "StepperMotor.h"

#define HIGH_SPEED 10000
#define LOW_SPEED 2000
#define END_TASK int* task_on_ptr = (int*) parameters;\
*task_on_ptr = 0;\
vTaskDelete(NULL);

void function1(){
    enable_steppers();
    stepper_param.max_speed=HIGH_SPEED;
    vTaskDelay(pdMS_TO_TICKS(200));
    set_x(100);
    set_y(865);
    set_theta(0);

    follow_to({775, 570});
    stepper_param.max_speed=LOW_SPEED;
    go_to({775, 220});
    vTaskDelay(pdMS_TO_TICKS(1000));
    go_to_reverse({775, 600}); 
    
    stepper_param.max_speed=HIGH_SPEED;
    follow_to({200, 865});
    angle_to(0);
    go_to_reverse({100, 865});
    vTaskDelay(pdMS_TO_TICKS(1000));
    disable_steppers();

}

void deposit_bl_cans(void* parameters){
    /* go to cans */
    follow_to({775, 570});

    /* contact cans */
    stepper_param.max_speed=LOW_SPEED;
    go_to({775, 328});
    vTaskDelay(pdMS_TO_TICKS(1000));
    go_to_reverse({775, 500}); 
    
    /* go to deposit location */
    stepper_param.max_speed=HIGH_SPEED;
    anti_follow_to({250, 300});

    /* deposit cans */
    stepper_param.max_speed=LOW_SPEED;
    go_to({250, 240});
    vTaskDelay(pdMS_TO_TICKS(1000));
    go_to_reverse({250, 400});

    END_TASK
}

void deposit_tl_cans(void* parameters){
    stepper_param.max_speed=HIGH_SPEED;

    /* go to cans */
    follow_to({825, 1500});

    /* contact cans */
    stepper_param.max_speed=LOW_SPEED;
    go_to({825, 1580});
    vTaskDelay(pdMS_TO_TICKS(1000));
    go_to_reverse({825, 1500}); 

    END_TASK
}


void go_home(void* parameters){
    stepper_param.max_speed=HIGH_SPEED;
    follow_to({200, 865});
    go_to_reverse({100, 865});

    END_TASK
}

#endif