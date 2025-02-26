#include "Arduino.h"
#include "StepperMotor.h"


StepperMotor::StepperMotor(){

}

void StepperMotor::begin(){
    pinMode(StepperMotor::pinout.step1_pin, OUTPUT);
    pinMode(StepperMotor::pinout.step2_pin, OUTPUT);
    pinMode(StepperMotor::pinout.dir1_pin, OUTPUT);
    pinMode(StepperMotor::pinout.dir2_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms1_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms2_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms3_pin, OUTPUT);
    pinMode(StepperMotor::pinout.en_pin, OUTPUT);
    digitalWrite(StepperMotor::pinout.en_pin, LOW);
}


void StepperMotor::move(uint8_t dir, uint32_t steps){
    // digitalWrite(StepperMotor::pinout.dir_pin, dir?HIGH:LOW);
    unsigned long speed_timer = 1; // defines frequency at wich the step_pin is activated to generate shaft speed


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
        Serial.print("Acceleration\n");
        while(n < steps_acc){
            t2 = micros();
            if(speed_timer <= t2-t1){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(StepperMotor::pinout.step1_pin, HIGH);
                digitalWrite(StepperMotor::pinout.step2_pin, HIGH);
                speed_timer = inv_acc / (t2 - t0); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v
                if(speed_timer >1000) speed_timer = 1000;
                n++;
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step1_pin, LOW); // timing of low-pull on step_pin is not critical
                digitalWrite(StepperMotor::pinout.step2_pin, LOW);
            }
        }
        /* end of acceleration */
        Serial.print("Cruise\n");
        /* begining of cruise */
        speed_timer = 1000000/StepperMotor::param.max_speed;//./StepperMotor::param.max_speed; // timer for speed is constant
        while(n < steps-steps_dec){
            t2 = micros();
            if(speed_timer <= t2-t1){ // waiting speed_timer time to elapse.
                digitalWrite(StepperMotor::pinout.step1_pin, HIGH);
                digitalWrite(StepperMotor::pinout.step2_pin, HIGH);
                n++; 
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step1_pin, LOW); // timing of low-pull on step_pin is not critical
                digitalWrite(StepperMotor::pinout.step2_pin, LOW);
            }
        }
        /* end of cruise */
        /* begining of deceleration */
        Serial.print("Deceleration\n");
        t0=micros(); // reset time for ease of calculation
        while(n < steps){
            t2 = micros();
            if(speed_timer <= t2-t1){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(StepperMotor::pinout.step1_pin, HIGH);
                digitalWrite(StepperMotor::pinout.step2_pin, HIGH);
                speed_timer = 1000000000000/(StepperMotor::param.max_speed*1000000-StepperMotor::param.deceleration*(t2-t0)); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v  
                if(speed_timer>1000) speed_timer = 1000;
                n++; 
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step1_pin, LOW); // timing of low-pull on step_pin is not critical
                digitalWrite(StepperMotor::pinout.step2_pin, LOW);
            }
        }
    }
    else {
        unsigned long t1; // start of previous step pulse.
        unsigned long t2; // actual time
        unsigned long t0 = micros(); // start time since begining of movement
        unsigned long top_timer;
        /* acceleration loop */
        Serial.print("Short acceleration\n");
        while(n < steps){ // loop will break before n>=steps
            t2 = micros();
            if(speed_timer <= t2-t1){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(StepperMotor::pinout.step1_pin, HIGH);
                digitalWrite(StepperMotor::pinout.step2_pin, HIGH);
                speed_timer = inv_acc/(t2-t0); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v
                if(speed_timer >1000) speed_timer = 1000;
                n++;
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step1_pin, LOW); // timing of low-pull on step_pin is not critical
                digitalWrite(StepperMotor::pinout.step2_pin, LOW);

                /* check if number of steps for deceleration */
                top_timer = speed_timer; // [step/ms]
                steps_dec = 500000000000/StepperMotor::param.deceleration/top_timer/top_timer ;
                if(steps - n < steps_dec) break; // stop acceleration phase.
    
            }
        }
        Serial.print("short Deceleration\n");
        t0 = micros(); // start time since begining of deceleration
        /* deceleration loop */
        while(n<steps){
            t2 = micros();
            if(speed_timer <= t2-t1){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(StepperMotor::pinout.step1_pin, HIGH);
                digitalWrite(StepperMotor::pinout.step2_pin, HIGH);
                speed_timer = top_timer/(1-StepperMotor::param.deceleration*(t2-t0)*top_timer*0.000000000001); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v 
                if(speed_timer >1000) speed_timer = 1000;
                n++;
                t1 = t2;
                digitalWrite(StepperMotor::pinout.step1_pin, LOW); // timing of low-pull on step_pin is not critical
                digitalWrite(StepperMotor::pinout.step2_pin, LOW);
            }
        }
    }
}
