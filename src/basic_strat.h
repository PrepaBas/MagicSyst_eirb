#ifndef basic_strat_h
#define basic_strat_h

#include <Arduino.h>
#include <advanced_movement.h> 
#include <table.h>
#include <StepperMotor.h>

extern RobotCoupe robot;
extern table table_coupe;

#define HIGH_SPEED 10000
#define LOW_SPEED 1000
void function1(){
    robot.motors.enable_steppers();
    robot.motors.param.max_speed=HIGH_SPEED;
    delay(200);
    robot.set_x(100);
    robot.set_y(865);
    robot.set_theta(0);

    robot.follow_to({775, 570});
    robot.motors.param.max_speed=LOW_SPEED;
    robot.go_to({775, 220});
    delay(100);
    robot.go_to_reverse({775, 600}); 
    
    robot.motors.param.max_speed=HIGH_SPEED;
    robot.follow_to({200, 865});
    robot.angle_to(0);
    robot.go_to_reverse({100, 865});
    delay(500);
    robot.motors.disable_steppers();

}


#endif