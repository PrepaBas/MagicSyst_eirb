#ifndef basic_strat_h
#define basic_strat_h

#include <Arduino.h>
#include <advanced_movement.h> 
#include <table.h>
#include <StepperMotor.h>

extern RobotCoupe robot;
extern table table_coupe;

#define HIGH_SPEED 10000
#define LOW_SPEED 2000
#define END_TASK int* task_on_ptr = (int*) parameters;\
*task_on_ptr = 0;\
vTaskDelete(NULL);

void function1(){
    robot.motors.enable_steppers();
    robot.motors.param.max_speed=HIGH_SPEED;
    vTaskDelay(pdMS_TO_TICKS(200));
    robot.set_x(100);
    robot.set_y(865);
    robot.set_theta(0);

    robot.follow_to({775, 570});
    robot.motors.param.max_speed=LOW_SPEED;
    robot.go_to({775, 220});
    vTaskDelay(pdMS_TO_TICKS(1000));
    robot.go_to_reverse({775, 600}); 
    
    robot.motors.param.max_speed=HIGH_SPEED;
    robot.follow_to({200, 865});
    robot.angle_to(0);
    robot.go_to_reverse({100, 865});
    vTaskDelay(pdMS_TO_TICKS(1000));
    robot.motors.disable_steppers();

}

void deposit_bl_cans(void* parameters){
    /* go to cans */
    robot.follow_to({775, 570});

    /* contact cans */
    robot.motors.param.max_speed=LOW_SPEED;
    robot.go_to({775, 328});
    vTaskDelay(pdMS_TO_TICKS(1000));
    robot.go_to_reverse({775, 500}); 
    
    /* go to deposit location */
    robot.motors.param.max_speed=HIGH_SPEED;
    robot.anti_follow_to({250, 300});

    /* deposit cans */
    robot.motors.param.max_speed=LOW_SPEED;
    robot.go_to({250, 240});
    vTaskDelay(pdMS_TO_TICKS(1000));
    robot.go_to_reverse({250, 400});

    END_TASK
}

void deposit_tl_cans(void* parameters){
    robot.motors.param.max_speed=HIGH_SPEED;

    /* go to cans */
    robot.follow_to({825, 1500});

    /* contact cans */
    robot.motors.param.max_speed=LOW_SPEED;
    robot.go_to({825, 1580});
    vTaskDelay(pdMS_TO_TICKS(1000));
    robot.go_to_reverse({825, 1500}); 

    END_TASK
}


void go_home(void* parameters){
    robot.motors.param.max_speed=HIGH_SPEED;
    robot.follow_to({200, 865});
    robot.go_to_reverse({100, 865});

    END_TASK
}

#endif