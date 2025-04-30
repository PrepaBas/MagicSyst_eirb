#ifndef observer_h
#define observer_h
#include "StepperMotor.h"
#include "basic_strat.h"

extern stepper_parameters_t stepper_param;
extern uint32_t remaining_steps;
extern uint32_t steps_done;
extern enum security_protocol protocol;

extern TaskHandle_t currentTask;

typedef struct {
    int* stop_flag;
    enum security_protocol protocol;
}security_api_t;

void securityTaskcode(void* parameters);
#endif