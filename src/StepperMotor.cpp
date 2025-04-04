#include "Arduino.h"
#include "StepperMotor.h"

/**
 * @brief init stepper motor pinout and disable steppers
 * @paragraph NONE
 */
void begin_steppers(){
    pinMode(stepper_pinout.step1_pin, OUTPUT);
    pinMode(stepper_pinout.step2_pin, OUTPUT);
    pinMode(stepper_pinout.dir1_pin, OUTPUT);
    pinMode(stepper_pinout.dir2_pin, OUTPUT);
    pinMode(stepper_pinout.ms1_pin, OUTPUT);
    pinMode(stepper_pinout.ms2_pin, OUTPUT);
    pinMode(stepper_pinout.ms3_pin, OUTPUT);

    // set step mode to 16th of step
    digitalWrite(stepper_pinout.ms1_pin, HIGH);
    digitalWrite(stepper_pinout.ms2_pin, HIGH);
    digitalWrite(stepper_pinout.ms3_pin, HIGH);

    pinMode(stepper_pinout.en_pin, OUTPUT);
    enable_steppers();
}

/**
 * @brief cut courant of stepper motors
 */
void disable_steppers(){
    digitalWrite(stepper_pinout.en_pin, HIGH);
}

/**
 * @brief enable courant of stepper motors
 */
void enable_steppers(){
    digitalWrite(stepper_pinout.en_pin, LOW);
}

/**
 * @brief Move both motors by step count. Auto acceleration/deceleration
 * @param uint8_t dir
 * @param uint32_t steps
 */
void move(uint32_t steps){
    // digitalWrite(stepper_pinout.dir_pin, dir?HIGH:LOW);
    unsigned long speed_timer = 1; // defines frequency at wich the step_pin is activated to generate shaft speed


    /* search nuber of steps during acceleration phase */
    float inv_acc = 1000000000000./stepper_param.acceleration;
    uint64_t steps_acc = 0.5*stepper_param.max_speed*stepper_param.max_speed/stepper_param.acceleration;
    uint64_t steps_dec = 0.5*stepper_param.max_speed*stepper_param.max_speed/stepper_param.deceleration;
    
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
                digitalWrite(stepper_pinout.step1_pin, HIGH);
                digitalWrite(stepper_pinout.step2_pin, HIGH);
                speed_timer = inv_acc / (t2 - t0); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v
                if(speed_timer >1000) speed_timer = 1000;
                n++;
                t1 = t2;
                digitalWrite(stepper_pinout.step1_pin, LOW); // timing of low-pull on step_pin is not critical
                digitalWrite(stepper_pinout.step2_pin, LOW);
            }
        }
        /* end of acceleration */
        /* begining of cruise */
        speed_timer = 1000000/stepper_param.max_speed;//./stepper_param.max_speed; // timer for speed is constant
        while(n < steps-steps_dec){
            t2 = micros();
            if(speed_timer <= t2-t1){ // waiting speed_timer time to elapse.
                digitalWrite(stepper_pinout.step1_pin, HIGH);
                digitalWrite(stepper_pinout.step2_pin, HIGH);
                n++; 
                t1 = t2;
                digitalWrite(stepper_pinout.step1_pin, LOW); // timing of low-pull on step_pin is not critical
                digitalWrite(stepper_pinout.step2_pin, LOW);
            }
        }
        /* end of cruise */
        /* begining of deceleration */
        t0=micros(); // reset time for ease of calculation
        while(n < steps){
            t2 = micros();
            if(speed_timer <= t2-t1){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(stepper_pinout.step1_pin, HIGH);
                digitalWrite(stepper_pinout.step2_pin, HIGH);
                speed_timer = 1000000000000/(stepper_param.max_speed*1000000-stepper_param.deceleration*(t2-t0)); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v  
                if(speed_timer>1000) speed_timer = 1000;
                n++; 
                t1 = t2;
                digitalWrite(stepper_pinout.step1_pin, LOW); // timing of low-pull on step_pin is not critical
                digitalWrite(stepper_pinout.step2_pin, LOW);
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
                digitalWrite(stepper_pinout.step1_pin, HIGH);
                digitalWrite(stepper_pinout.step2_pin, HIGH);
                speed_timer = inv_acc/(t2-t0); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v
                if(speed_timer >1000) speed_timer = 1000;
                n++;
                t1 = t2;
                digitalWrite(stepper_pinout.step1_pin, LOW); // timing of low-pull on step_pin is not critical
                digitalWrite(stepper_pinout.step2_pin, LOW);

                /* check if number of steps for deceleration */
                top_timer = speed_timer; // [step/ms]
                steps_dec = 500000000000/stepper_param.deceleration/top_timer/top_timer ;
                if(steps - n < steps_dec) break; // stop acceleration phase.
    
            }
        }
        t0 = micros(); // start time since begining of deceleration
        /* deceleration loop */
        while(n<steps){
            t2 = micros();
            if(speed_timer <= t2-t1){ // waiting speed_timer time to elapse. First step is forced throught second condition
                digitalWrite(stepper_pinout.step1_pin, HIGH);
                digitalWrite(stepper_pinout.step2_pin, HIGH);
                speed_timer = top_timer/(1-stepper_param.deceleration*(t2-t0)*top_timer*0.000000000001); // calculating time before sending next step HIGH following : v=x'=x''*t, T=(N=1)/v 
                if(speed_timer >1000) speed_timer = 1000;
                n++;
                t1 = t2;
                digitalWrite(stepper_pinout.step1_pin, LOW); // timing of low-pull on step_pin is not critical
                digitalWrite(stepper_pinout.step2_pin, LOW);
            }
        }
    }
}



/**
 * @brief turn motors up to stop following movement type and number of steps. max_speed speed can be changed midway
 * Calculation at constant acceleration/deceleration (similar to free-fall)
 */
int move_task(uint64_t* t0, uint64_t* t1){
    
    /* Initialisation of variables */  
    uint32_t steps_dec=0.5*current_speed*current_speed /stepper_param.deceleration; // steps required idealy for complete stop at current speed
        
    if(remaining_steps > 0){

        /* remaining_steps allow for later deceleration */
        if(remaining_steps > steps_dec){

            /* current_speed is inferior to max_speed -> acceleration */
            if(current_speed < stepper_param.max_speed*0.99){
                if(current_speed < stepper_param.min_speed){
                    current_speed = stepper_param.min_speed;
                }
                else{
                    current_speed = stepper_param.acceleration/current_speed+current_speed; // v_{n+1}=a*t+v_n avec t=1/v
                    if(current_speed > stepper_param.max_speed){
                        current_speed = stepper_param.max_speed;
                    }
                }
            }

            /* current_speed is superior to max_speed -> deceleration */
            else if(current_speed > 1.01* stepper_param.max_speed){
                current_speed = -stepper_param.deceleration/current_speed+current_speed; // v_{n+1}=a*t+v_n avec t=1/v
                if(current_speed < stepper_param.max_speed){
                    current_speed = stepper_param.max_speed;
                }  
            }

            /* current_speed is max_speed */
            else{
                current_speed = stepper_param.max_speed;
            }
        }

        /* Remaining steps only allow for deceleration */
        else{
            current_speed = (stepper_param.min_speed - current_speed)/(remaining_steps) + current_speed;
            if(current_speed < stepper_param.min_speed){
                current_speed = stepper_param.min_speed;
            }

        }
        uint64_t waiting_time = 1000000./current_speed;
        digitalWrite(stepper_pinout.step1_pin, HIGH);
        digitalWrite(stepper_pinout.step2_pin, HIGH);
        while(*t1-*t0 < waiting_time){*t1=esp_timer_get_time();} // delay for waiting_time
        digitalWrite(stepper_pinout.step1_pin, LOW);
        digitalWrite(stepper_pinout.step2_pin, LOW);
        remaining_steps-=1;
        *t0 = *t1;
    } 
    else {
        Serial.println("no step");
        return 0;
    }
    return 1;
}

void moveTaskcode(void* parameters){
    // init task
    vTaskDelay(pdMS_TO_TICKS(100));
    current_speed=0;
    enable_steppers();
    stepper_param.max_speed = 8000;
    uint64_t t1;
    uint64_t t0 = esp_timer_get_time();
    Serial.println("move_task init");

    // loop task
    for(;;){
      if(move_task(&t0, &t1)) steps_done++;
    }
  }