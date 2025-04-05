#ifndef RANK_H
#define RANK_H

#include "Arduino.h"
#include "advanced_movement.h" 
#include "table.h"
#include "basic_strat.h"

typedef void (*Function) (void*); // makes it easy to handle function pointers

typedef struct function_rank{
    Function function;
    int rank;
    struct function_rank* prev_function;
}function_rank_t;

typedef struct function_input{
    Function function;
    struct position start;
    struct position end;
    uint16_t runtime; //[s]
    uint8_t color;
    int scale;
}function_input;

function_rank_t* function_rank_remove(function_rank_t* start_ptr, function_rank_t* to_remove_ptr){
    function_rank_t* function_rank = start_ptr;
    if(start_ptr==NULL || to_remove_ptr==NULL) return start_ptr;
    if(start_ptr == to_remove_ptr){
        start_ptr = to_remove_ptr->prev_function;
        free(to_remove_ptr);
        return start_ptr;
    } 
    while(function_rank != NULL){
        if(function_rank->prev_function == to_remove_ptr){
            function_rank->prev_function = to_remove_ptr->prev_function;
            free(to_remove_ptr);
            break;
        }
        function_rank = function_rank->prev_function;
    }
    return start_ptr;
}   


function_rank* function_rank_begin(Function functions[]){
    int i=0;
    function_rank_t* previous_function_rank=NULL;
    while(functions[i]!=NULL){
        function_rank_t* another_function_rank = (function_rank_t*) malloc(sizeof(function_rank_t));
        another_function_rank->function = functions[i++];
        another_function_rank->rank = 0;
        another_function_rank->prev_function = previous_function_rank;
        previous_function_rank = another_function_rank;
    }
    return previous_function_rank;
}


#endif