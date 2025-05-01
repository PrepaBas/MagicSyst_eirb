
#include "StepperMotor.h"
#include "Arduino.h"


/* Global Variables */
stepper_parameters_t stepper_param = {10000, 500, 11000, 11000, STEP_MODE_SIXTEENTH};
stepper_pinout_t stepper_pinout;
float current_speed = 0; // [Steps/sec]
uint32_t remaining_steps = 0;
uint32_t steps_done = 0;



/**
 * @brief init stepper motor pinout and disable steppers
 * @paragraph NONE
 */
void begin_steppers(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7,uint8_t pin8){
    stepper_pinout = (stepper_pinout_t){pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8};
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

void set_direction(uint8_t first_stepper_dir, uint8_t second_stepper_dir){
    digitalWrite(stepper_pinout.dir2_pin, second_stepper_dir);
    digitalWrite(stepper_pinout.dir1_pin, first_stepper_dir);
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
 * @brief turn motors up to stop, following movement type and number of steps. max_speed speed can be changed midway
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