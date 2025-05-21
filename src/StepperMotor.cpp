
#include "StepperMotor.h"
#include "Arduino.h"

/* Global Variables */
stepper_parameters_t stepper_param = {300, 10, 700, 700, STEP_MODE_16};
stepper_pinout_t stepper_pinout;
float current_speed = 0; // [Steps/sec]
float target_speed = 2000;
uint32_t remaining_steps = 0;
uint32_t steps_done = 0;

/**
 * @brief init stepper motor pinout and disable steppers
 * @paragraph NONE
 */
void begin_steppers(uint8_t dir1, uint8_t dir2, uint8_t step1, uint8_t step2, uint8_t ms2, uint8_t ms1, uint8_t ms0, uint8_t enable)
{
    stepper_pinout = (stepper_pinout_t){dir1, dir2, step1, step2, ms0, ms1, ms2, enable};
    pinMode(stepper_pinout.step1_pin, OUTPUT);
    pinMode(stepper_pinout.step2_pin, OUTPUT);
    pinMode(stepper_pinout.dir1_pin, OUTPUT);
    pinMode(stepper_pinout.dir2_pin, OUTPUT);
    pinMode(stepper_pinout.ms1_pin, OUTPUT);
    pinMode(stepper_pinout.ms2_pin, OUTPUT);

    //pinMode(stepper_pinout.ms3_pin, OUTPUT);

    // set step mode to 16th of a step
    digitalWrite(stepper_pinout.ms1_pin, HIGH);
    digitalWrite(stepper_pinout.ms2_pin, HIGH);
    //digitalWrite(stepper_pinout.ms3_pin, LOW);

    pinMode(stepper_pinout.en_pin, OUTPUT);
    disable_steppers();
    stepper_param.acceleration*=stepper_param.step_mode;
    stepper_param.deceleration*=stepper_param.step_mode;
    stepper_param.min_speed*=stepper_param.step_mode;
    stepper_param.max_speed *=stepper_param.step_mode;
}

step_mode_t get_step_mode()
{
    return stepper_param.step_mode;
}

void set_direction(uint8_t first_stepper_dir, uint8_t second_stepper_dir)
{
    digitalWrite(stepper_pinout.dir2_pin, second_stepper_dir);
    digitalWrite(stepper_pinout.dir1_pin, first_stepper_dir);
}

void set_speed(float percentage_of_max_speed)
{
    target_speed = stepper_param.max_speed * percentage_of_max_speed;
    if (target_speed < stepper_param.min_speed)
    {
        target_speed = stepper_param.min_speed;
    }
}
/**
 * @brief cut courant of stepper motors
 */
void disable_steppers()
{
    digitalWrite(stepper_pinout.en_pin, HIGH);
}

/**
 * @brief enable courant of stepper motors
 */
void enable_steppers()
{
    digitalWrite(stepper_pinout.en_pin, LOW);
}

/**
 * @brief send intruction for 1 step and delay up to calculated time for speed control.
 */
int move_task(uint64_t *t0, uint64_t *t1)
{

    uint32_t steps_dec = 0.5 * current_speed * current_speed / stepper_param.deceleration; // steps required idealy for complete stop at current speed

    if (remaining_steps > 0)
    {

        /* remaining_steps allow for later deceleration */
        if (remaining_steps > steps_dec)
        {

            /* current_speed is inferior to max_speed -> acceleration */
            if (current_speed < target_speed * 0.99)
            {
                if (current_speed < stepper_param.min_speed)
                {
                    current_speed = stepper_param.min_speed;
                }
                else
                {
                    current_speed = stepper_param.acceleration / current_speed + current_speed; // v_{n+1}=a*t+v_n avec t=1/v
                    if (current_speed > target_speed)
                    {
                        current_speed = target_speed;
                    }
                }
            }

            /* current_speed is superior to max_speed -> deceleration */
            else if (current_speed > 1.01 * target_speed)
            {
                current_speed = -stepper_param.deceleration / current_speed + current_speed; // v_{n+1}=a*t+v_n avec t=1/v
                if (current_speed < target_speed)
                {
                    current_speed = target_speed;
                }
            }

            /* current_speed is max_speed */
            else
            {
                current_speed = target_speed;
            }
        }

        /* Remaining steps only allow for deceleration */
        else
        {
            current_speed = (stepper_param.min_speed - current_speed) / (remaining_steps) + current_speed;
            if (current_speed < stepper_param.min_speed)
            {
                current_speed = stepper_param.min_speed;
            }
        }
        uint64_t waiting_time = 1000000. / current_speed;
        digitalWrite(stepper_pinout.step1_pin, HIGH);
        digitalWrite(stepper_pinout.step2_pin, HIGH);
        while (*t1 - *t0 < waiting_time)
        {
            *t1 = esp_timer_get_time(); // delay for waiting_time
            // vTaskDelay(pdMS_TO_TICKS(1));
        }
        digitalWrite(stepper_pinout.step1_pin, LOW);
        digitalWrite(stepper_pinout.step2_pin, LOW);
        remaining_steps -= 1;
        *t0 = *t1;
    }
    else
    {
        //Serial.println("no steps");
        vTaskDelay(pdMS_TO_TICKS(1));
        return 0;
    }
    return 1;
}

void moveTaskcode(void *parameters)
{
    // init task
    Serial.println("beginng movetask");
    vTaskDelay(pdMS_TO_TICKS(100));
    current_speed = 0;
    enable_steppers();
    set_speed(0);

    // Timers are used to allow for non-blocking stepper control as well as to allow for cumputation time.
    // step falling edge is called, then computation, then th rising edge, then remaining time until timers match is elapsed, then timers are quickly updated; and the loop restarts
    uint64_t t1;
    uint64_t t0 = esp_timer_get_time();
    Serial.println("move_task init");

    int *robot_stop_ptr = (int *)parameters;
    // loop task
    for (;;)
    {
        // Serial.println("beginng step");
        if (move_task(&t0, &t1))
        {
            steps_done++;
        }
        if(*robot_stop_ptr)
        {
            while(remaining_steps == 1)
            {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
    }
}

////////////////////////////////////////////////////////////

bool parity = true;
uint64_t waiting_time;

int move_task_este(uint64_t *t0, uint64_t *t1)
{

    uint32_t steps_dec = 0.5 * current_speed * current_speed / stepper_param.deceleration; // steps required idealy for complete stop at current speed

    if (remaining_steps > 0)
    {
        if (parity){
            /* remaining_steps allow for later deceleration */
            if (remaining_steps > steps_dec)
            {

                /* current_speed is inferior to max_speed -> acceleration */
                if (current_speed < target_speed * 0.99)
                {
                    if (current_speed < stepper_param.min_speed)
                    {
                        current_speed = stepper_param.min_speed;
                    }
                    else
                    {
                        current_speed = stepper_param.acceleration / current_speed + current_speed; // v_{n+1}=a*t+v_n avec t=1/v
                        if (current_speed > target_speed)
                        {
                            current_speed = target_speed;
                        }
                    }
                }

                /* current_speed is superior to max_speed -> deceleration */
                else if (current_speed > 1.01 * target_speed)
                {
                    current_speed = -stepper_param.deceleration / current_speed + current_speed; // v_{n+1}=a*t+v_n avec t=1/v
                    if (current_speed < target_speed)
                    {
                        current_speed = target_speed;
                    }
                }

                /* current_speed is max_speed */
                else
                {
                    current_speed = target_speed;
                }
            }

            /* Remaining steps only allow for deceleration */
            else
            {
                current_speed = (stepper_param.min_speed - current_speed) / (remaining_steps) + current_speed;
                if (current_speed < stepper_param.min_speed)
                {
                    current_speed = stepper_param.min_speed;
                }
            }
            waiting_time = 1000000. / current_speed;
            digitalWrite(stepper_pinout.step1_pin, HIGH);
            digitalWrite(stepper_pinout.step2_pin, HIGH);
            parity = false;          
        }
        else{
            digitalWrite(stepper_pinout.step1_pin, LOW);
            digitalWrite(stepper_pinout.step2_pin, LOW);
            remaining_steps -= 1;
            *t0 = *t1;
            parity=true;
        }
        // while (*t1 - *t0 < waiting_time)
        // {
        //     *t1 = esp_timer_get_time(); // delay for waiting_time
        //     // vTaskDelay(pdMS_TO_TICKS(1));
        // }
        vTaskDelay(pdMS_TO_TICKS(waiting_time));
        
    }
    else
    {
        //Serial.println("no steps");
        vTaskDelay(pdMS_TO_TICKS(1));
        return 0;
    }
    return 1;
}