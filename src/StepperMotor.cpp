#include "Arduino.h"
#include "StepperMotor.h"


StepperMotor::StepperMotor(){

}

void StepperMotor::begin(){
    pinMode(StepperMotor::pinout.step_pin, OUTPUT);
    pinMode(StepperMotor::pinout.dir1_pin, OUTPUT);
    pinMode(StepperMotor::pinout.dir2_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms1_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms2_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms3_pin, OUTPUT);
    pinMode(StepperMotor::pinout.sleep_pin, OUTPUT);
    pinMode(StepperMotor::pinout.rst_pin, OUTPUT);
}


void StepperMotor::move(uint8_t dir, uint32_t steps){
    // digitalWrite(StepperMotor::pinout.dir_pin, dir?HIGH:LOW);
    unsigned long speed_timer = 0; // defines frequency at wich the step_pin is activated to generate shaft speed


    /* search nuber of steps during acceleration phase */
    uint64_t inv_acc = 1000000000000./StepperMotor::param.acceleration;
    uint64_t steps_acc = 0.5*StepperMotor::param.max_speed*StepperMotor::param.max_speed/StepperMotor::param.acceleration;
    uint64_t steps_dec = 0.5*StepperMotor::param.max_speed*StepperMotor::param.max_speed/StepperMotor::param.deceleration;
    
    /* rotation of motor */
    uint64_t n = 0; // step counter
    if(steps > steps_acc + steps_dec){ // case of three distinct speed phases : acceleration, cruise, deceleration

        /* instanciation of timers */
        unsigned long t1; // start of previous step pulse.
        unsigned long t2; // actual time
        unsigned long t0 = micros(); // start time since begining of movement
        

        /* start of acceleration */
        while(n < steps_acc){
            t2 = micros();
            if(speed_timer <= t2-t1 || n == 0){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(StepperMotor::pinout.step_pin, HIGH);
                speed_timer = inv_acc / ((t2 - t0)); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v
                if(speed_timer >10000) speed_timer = 10000;
                n++;
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step_pin, LOW); // timing of low-pull on step_pin is not critical
            }
        }
        /* end of acceleration */
        /* begining of cruise */
        speed_timer = 1000000/StepperMotor::param.max_speed;//./StepperMotor::param.max_speed; // timer for speed is constant
        while(n < steps-steps_dec){
            t2 = micros();
            if(speed_timer <= t2-t1){ // waiting speed_timer time to elapse.
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
            if(speed_timer <= t2-t1){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(StepperMotor::pinout.step_pin, HIGH);
                speed_timer = 1000000000000/(StepperMotor::param.max_speed*1000000-StepperMotor::param.deceleration*(t2-t0)); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v
                if(speed_timer > 1000) speed_timer = 1000;
                n++; 
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step_pin, LOW); // timing of low-pull on step_pin is not critical
            }
        }
    }
    else {
        unsigned long t1; // start of previous step pulse.
        unsigned long t2; // actual time
        unsigned long t0 = micros(); // start time since begining of movement
        float instant_speed;

        /* acceleration loop */
        while(n < steps){ // loop will break before n>=steps
            t2 = micros();
            if(speed_timer <= t2-t1 || n == 0){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(StepperMotor::pinout.step_pin, HIGH);
                speed_timer = inv_acc/(t2-t0); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v
                if(speed_timer >1000) speed_timer = 1000;
                n++;
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step_pin, LOW); // timing of low-pull on step_pin is not critical

                /* check if number of steps for deceleration */
                instant_speed = 1/speed_timer // [step/ms]
                steps_dec = 0.5*instant_speed*instant_speed/StepperMotor::param.deceleration;
                if(steps_dec > steps - n) break; // stop acceleration phase.
    
            }
        }

        t0 = micros(); // start time since begining of deceleration

        /* deceleration loop */
        while(n<steps){
            if(speed_timer <= t2-t1){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(StepperMotor::pinout.step_pin, HIGH);
                speed_timer = 1000000000000/(top_speed-StepperMotor::param.deceleration*(t2-t0)); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v 
                if(speed_timer >1000) speed_timer = 1000;
                n++;
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step_pin, LOW); // timing of low-pull on step_pin is not critical
            }
        }
    }
}
