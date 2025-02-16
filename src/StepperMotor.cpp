#include "Arduino.h"
#include "StepperMotor.h"


StepperMotor::StepperMotor(){

}


bool StepperMotor::rotate(uint8_t dir, uint32_t steps){
    uint16_t speed_timer; // defines frequency at wich the step_pin is activated to generate shaft speed


    /* search nuber of steps during acceleration phase */
    float inv_acc = 1/StepperMotor::param.acceleration;
    uint32_t steps_acc = 0.5*StepperMotor::param.max_speed*StepperMotor::param.max_speed*inv_acc;
    uint32_t n = 0; // step counter
    if(steps >= 2* steps_acc){ // case of three distinct speed phases : acceleration, cruise, deceleration
        /* start of acceleration */
        float speed = 0;

        /* instanciation of timers */
        unsigned long t0 = micros(); // start time of acceleration
        unsigned long t1; // start of previous step pulse
        unsigned long t2; // actual time

        while(n < steps_acc){
            t2 = micros();
            if(speed_timer >= t2-t1){ // waiting speed_timer time to elapse
                digitalWrite(StepperMotor::pinout.step_pin, HIGH);
                speed_timer = inv_acc / (t2 - t0); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v
                n++; 
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step_pin, LOW); // timing of low-pull on step_pin is not critical
            }
        }
    }
    
}
