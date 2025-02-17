#include "Arduino.h"
#include "StepperMotor.h"


StepperMotor::StepperMotor(){

}

void StepperMotor::begin(){
    pinMode(StepperMotor::pinout.step_pin, OUTPUT);
    pinMode(StepperMotor::pinout.dir_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms1_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms2_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms3_pin, OUTPUT);
    pinMode(StepperMotor::pinout.sleep_pin, OUTPUT);
    pinMode(StepperMotor::pinout.rst_pin, OUTPUT);
}


bool StepperMotor::move(uint8_t dir, uint32_t steps){
    digitalWrite(StepperMotor::pinout.dir_pin, dir?HIGH:LOW);
    uint16_t speed_timer; // defines frequency at wich the step_pin is activated to generate shaft speed


    /* search nuber of steps during acceleration phase */
    float inv_acc = 1/StepperMotor::param.acceleration;
    uint32_t steps_acc = 0.5*StepperMotor::param.max_speed*StepperMotor::param.max_speed*inv_acc;
    uint32_t steps_dec = steps_acc;
    /* rotation of motor */
    uint32_t n = 0; // step counter
    if(steps >= 2* steps_acc){ // case of three distinct speed phases : acceleration, cruise, deceleration
        /* start of acceleration */
        float speed = 0;

        /* instanciation of timers */
        unsigned long t1; // start of previous step pulse.
        unsigned long t2; // actual time
        unsigned long t0 = micros(); // start time since begining of movement
        

        while(n < steps_acc){
            t2 = micros();
            if(speed_timer >= t2-t1 || n == 0){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(StepperMotor::pinout.step_pin, HIGH);
                speed_timer = inv_acc / (t2 - t0); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v
                n++; 
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step_pin, LOW); // timing of low-pull on step_pin is not critical
            }
        }
        /* end of acceleration */
        /* begining of cruise */
        speed_timer = 1/StepperMotor::param.max_speed; // timer for speed is constant
        while(n < steps-steps_dec){
            t2 = micros();
            if(speed_timer >= t2-t1){ // waiting speed_timer time to elapse.
                digitalWrite(StepperMotor::pinout.step_pin, HIGH);
                n++; 
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step_pin, LOW); // timing of low-pull on step_pin is not critical
            }
        }
        /* end of cruise */
        /* begining of deceleration */
        t0=micros(); // reset time for ease of calculation
        while(n < steps){
            t2 = micros();
            if(speed_timer >= t2-t1){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(StepperMotor::pinout.step_pin, HIGH);
                speed_timer = 1/(-StepperMotor::param.deceleration * (t2-t0) + StepperMotor::param.max_speed); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v
                n++; 
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step_pin, LOW); // timing of low-pull on step_pin is not critical
            }
        }
    }

}
