#ifndef RANK_H
#define RANK_H

#include "Arduino.h"
#include "advanced_movement.h" 
#include "table.h"
#include <vector>

typedef void (*Function) (void*); // makes it easy to handle function pointers
extern TaskHandle_t currentTask;
class Macro {
public :
    Function function;
    struct position start;
    struct position end;
    uint16_t runtime; //[s]
    uint8_t color;
    int scale;

    Macro(Function function, struct position start, struct position end, uint16_t runtime, uint8_t color, int scale);

    int rank();
};

void dispatchTaskcode(void* parameters);

#endif