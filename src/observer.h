#ifndef observer_h
#define observer_h
#include "StepperMotor.h"
#include "basic_strat.h"

extern uint32_t remaining_steps;
extern uint32_t steps_done;
extern enum security_protocol protocol;

extern TaskHandle_t currentTask;

typedef enum observer_status{
    NO_STOP,
    YES_STOP,
}status_t;

typedef struct {
    int* stop_flag;
    enum security_protocol protocol;
}security_api_t;

void securityTaskcode(void* parameters);
#endif