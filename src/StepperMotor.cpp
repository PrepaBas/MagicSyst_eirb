#include "Arduino.h"
#include "StepperMotor.h"


StepperMotor::StepperMotor(){

}

/**
 * @brief init stepper motor pinout and disable steppers
 * @paragraph NONE
 */
void StepperMotor::begin(){
    pinMode(StepperMotor::pinout.step1_pin, OUTPUT);
    pinMode(StepperMotor::pinout.step2_pin, OUTPUT);
    pinMode(StepperMotor::pinout.dir1_pin, OUTPUT);
    pinMode(StepperMotor::pinout.dir2_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms1_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms2_pin, OUTPUT);
    pinMode(StepperMotor::pinout.ms3_pin, OUTPUT);
    pinMode(StepperMotor::pinout.en_pin, OUTPUT);
    StepperMotor::disable_steppers();
}

/**
 * @brief cut courant of stepper motors
 */
void StepperMotor::disable_steppers(){
    digitalWrite(StepperMotor::pinout.en_pin, HIGH);
}

/**
 * @brief enable courant of stepper motors
 */
void StepperMotor::enable_steppers(){
    digitalWrite(StepperMotor::pinout.en_pin, LOW);
}

/**
 * @brief Move both motors by step count. Auto acceleration/deceleration
 * @param uint8_t dir
 * @param uint32_t steps
 */
void StepperMotor::move(uint32_t steps){
    // digitalWrite(StepperMotor::pinout.dir_pin, dir?HIGH:LOW);
    unsigned long speed_timer = 1; // defines frequency at wich the step_pin is activated to generate shaft speed


    /* search nuber of steps during acceleration phase */
    float inv_acc = 1000000000000./StepperMotor::param.acceleration;
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
        delayMicroseconds(1000);
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



/**
 * @brief turn motors up to stop following movement type and number of steps. max_speed speed can be changed midway
 * Calculation at constant acceleration/deceleration (similar to free-fall)
 */
void StepperMotor::move_task(){
    
    /* Initialisation of variables */  
    uint32_t steps_dec = 0.5*StepperMotor::current_speed*StepperMotor::param.max_speed/StepperMotor::param.deceleration; // steps required idealy for complete stop at current speed      
    int64_t t1; // after calculation
    int64_t t0 = esp_timer_get_time();; // time before calculation

    for(;;){ // loop task
        if(StepperMotor::remaining_steps > 0){

            /* remaining_steps allow for later deceleration */
            if(StepperMotor::remaining_steps > steps_dec){

                /* current_speed is inferior to max_speed -> acceleration */
                if(StepperMotor::current_speed < StepperMotor::param.max_speed*0.99){
                    if(StepperMotor::current_speed < StepperMotor::param.min_speed){
                        StepperMotor::current_speed = StepperMotor::param.min_speed;
                    }
                    else{
                        StepperMotor::current_speed = StepperMotor::param.acceleration/StepperMotor::current_speed+StepperMotor::current_speed; // v_{n+1}=a*t+v_n avec t=1/v
                        if(StepperMotor::current_speed > StepperMotor::param.max_speed){
                            StepperMotor::current_speed = StepperMotor::param.max_speed;
                        }
                    }
                }

                /* current_speed is superior to max_speed -> deceleration */
                else if(StepperMotor::current_speed > 1.01* StepperMotor::param.max_speed){
                    StepperMotor::current_speed = -StepperMotor::param.deceleration/StepperMotor::current_speed+StepperMotor::current_speed; // v_{n+1}=a*t+v_n avec t=1/v
                    if(StepperMotor::current_speed < StepperMotor::param.max_speed){
                        StepperMotor::current_speed = StepperMotor::param.max_speed;
                    }  
                }

                /* current_speed is max_speed */
                else{
                    StepperMotor::current_speed = StepperMotor::param.max_speed;
                }
            }

            /* Remaining steps only allow for deceleration */
            else{
                StepperMotor::current_speed = (StepperMotor::param.min_speed - StepperMotor::current_speed)/StepperMotor::remaining_steps + StepperMotor::current_speed;
                if(StepperMotor::current_speed < StepperMotor::param.min_speed){
                    StepperMotor::current_speed = StepperMotor::param.min_speed;
                }

            }
            uint64_t waiting_time = 1000000./StepperMotor::current_speed;
            steps_dec=0.5*StepperMotor::current_speed*StepperMotor::param.max_speed/StepperMotor::param.deceleration; // steps required idealy for complete stop at current speed
            digitalWrite(StepperMotor::pinout.step1_pin, LOW);
            digitalWrite(StepperMotor::pinout.step2_pin, LOW);
            while(t1-t0 < waiting_time){t1=esp_timer_get_time();} // delay for waiting_time
            digitalWrite(StepperMotor::pinout.step1_pin, HIGH);
            digitalWrite(StepperMotor::pinout.step2_pin, HIGH);
            StepperMotor::remaining_steps-=1;
            t0 = t1;
            steps_dec = 0.5*StepperMotor::current_speed*StepperMotor::param.max_speed/StepperMotor::param.deceleration; // steps required idealy for complete stop at current speed
            
        }
    }
}