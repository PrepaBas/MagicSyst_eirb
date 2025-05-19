/* Includes ------------------------------------------------------------------*/

/* Arduino specific header files. */
#include "Arduino.h"

/* file header file */
#include "advanced_movement.h"
#include "StepperMotor.h"
#include "table.h"
#include "FastTrig.h"

struct position position = {0, 0, 0};
move_type_t last_move_type = STRAIGHT_FORWARD;
float baseWidth;
float wheelRadius;


table table_coupe = {{650, 670}, {650, 1300}, {2340, 670}, {2340, 1300}, {3000, 2000}};
 
/* Setters and Getters -------------------------------------------------------*/
void movement_begin(float base, float wheel, float x, float y, float theta){
    baseWidth = base;
    wheelRadius = wheel;
    set_x(x);
    set_y(y);
    set_theta(theta);
}

void set_x(float x){
    position.x = x;
}

void set_y(float y){
    position.y = y;
}

void set_theta(float theta){
    position.theta = theta;
}

float get_theta(){
    return position.theta;
}

uint64_t distance_to_steps(float distance, float radius, uint16_t steps_per_revolution){
    uint64_t steps = steps_per_revolution * distance / (2. * PI * radius);
    return steps;
}

float steps_to_distance(uint64_t steps, float radius, uint16_t steps_per_revolution){
    float d = steps*2*PI*radius/steps_per_revolution;
    return d;
}
float restrict_angle(float in_angle){
    float out_angle = in_angle;
    while(out_angle > 180){
        out_angle += -360;
    }
    while(out_angle < -180){
        out_angle += 360;
    }
    return out_angle;
}

void new_position(){
    /* update position using movement type and steps difference */
    uint8_t step_mode = 16;
    float d = steps_to_distance(steps_done, wheelRadius, 200*step_mode);   
    float theta = get_theta();
    Serial.print(theta);
    switch(last_move_type){
        case ROTATE_LEFT:
        {
            float full_angle = theta - d*360./(PI*baseWidth);
            set_theta(full_angle);
            break;
        }
        case STRAIGHT_FORWARD:
            set_x(position.x + d*icos(theta));
            set_y(position.y + d*isin(theta));
            break;
        case STRAIGHT_BACKWARD:
            set_x(position.x - d*icos(theta));
            set_y(position.y - d*isin(theta)); 
            break;
        case ROTATE_RIGHT:
            set_theta(theta + d*360./(PI*baseWidth));
            break;
        default:
            break;
        }  
    steps_done = 0;
}

void move_straight (char direction, float distance){
    /* Move robot in a straight line
     * direction :  1 for Forward
     *              0 for Backward */
    uint8_t step_mode = get_step_mode();
    uint64_t m_steps =  distance_to_steps(distance, wheelRadius, 200*step_mode);
    set_direction(direction?LOW:HIGH, direction?HIGH:LOW);
    steps_done = 0;
    remaining_steps = m_steps;
    last_move_type = direction?STRAIGHT_BACKWARD:STRAIGHT_FORWARD;
    while(remaining_steps){vTaskDelay(pdMS_TO_TICKS(30));}
    new_position(); // update position    
}

void rotate (int direction, float angle){
    /* Rotate robot in place
     * direction :  0 for left rotation (anti-clockwise)
     *              1 for right rotation (clockwise) */
    float distance = angle * baseWidth * PI / 360.;
    uint8_t step_mode = get_step_mode();
    uint64_t m_steps =  distance_to_steps(distance, wheelRadius, 200*step_mode);
    set_direction(direction?LOW:HIGH, direction?LOW:HIGH);
    steps_done = 0;
    remaining_steps = m_steps;
    last_move_type = direction?ROTATE_RIGHT:ROTATE_LEFT;
    while(remaining_steps){vTaskDelay(pdMS_TO_TICKS(30));}
    new_position(); // update position
}


void angle_to(float angle){
    float move_angle = angle - position.theta;

    // Making sure that angle is [-180; 180] */
    move_angle = restrict_angle(move_angle);

    /* Choosing rotation direction */
    if(move_angle >= 0){
        rotate(1, move_angle);
    }
    else{
        rotate(0, - move_angle);
    }

}

void go_to(struct position pos){
    /* Move robot to coordinates.
     * First rotate then move in straight line */
     

    float x = pos.x - position.x;
    float y = pos.y - position.y;
    float radius = sqrt(x*x+y*y);
    float angle = atan2(y, x)*360/(2 * PI);

    angle_to(angle);
    move_straight(0, radius);    
}

void go_to_reverse(struct position pos){
    /* Move robot to coordinates.
     * First rotate then move in straight line facing backwards */

    
    float x = pos.x - position.x;
    float y = pos.y - position.y;
    float radius = sqrt(x*x+y*y);
    float angle = atan2(y, x)*360/(2 * PI);

    angle_to(angle+180);
    move_straight(1, radius);

    
}

struct position corner_position(int c){
    switch(c){
        case 0 :
            return table_coupe.bl;
        case 2 :
            return table_coupe.br;
        case 4 :
            return table_coupe.tr;
        case 6 :
            return table_coupe.tl;
        default :
            return table_coupe.tl;
    }
}

int what_zone (struct position pos) {
    
    if (pos.x < table_coupe.bl.x){
        if(pos.y < table_coupe.bl.y) 
            return 0; // left_bot
        if(pos.y < table_coupe.tl.y)
            return 7; // left_mid
        else 
            return 6; // left_top
    }
    if (pos.x < table_coupe.br.x){
        if(pos.y < table_coupe.bl.y) 
            return 1; // mid_bot
        if(pos.y < table_coupe.tl.y){ // middle section
            if(pos.x<pos.y){
                if(pos.x<table_coupe.extreme.y - pos.y) 
                    return 7; // left_mid
                else 
                    return 5; // mid_top
            }
            else{
                if(pos.x<table_coupe.extreme.y - pos.y) 
                    return 1; // mid_bot
                else 
                    return 3; // right_middle
            }
        }
        else 
            return 5; // mid_top
    }
    else{
        if(pos.y < table_coupe.bl.y) 
            return 2; // right_bot
        if(pos.y < table_coupe.tl.y)
            return 3; // right_mid
        else 
            return 4; // right_top
    }
}

/**
 * robots go to coordinates following imaginary path to avoid obvious obstacles
 */
void follow_to(struct position pos){
    /* Zones finding */
    int start_zone = what_zone(position);
    int end_zone = what_zone(pos);

    /* Place robot to nearest point in circulation path*/
    if(start_zone== 0){
        go_to(table_coupe.bl);
    }
    else if(start_zone== 1){
        struct position destination = {position.x, table_coupe.bl.y};
        go_to(destination);
    }
    else if(start_zone== 2){
        go_to(table_coupe.br);
    }
    else if(start_zone== 3){
        struct position destination = {table_coupe.br.x, position.y};
        go_to(destination);
    }
    else if(start_zone== 4){
        go_to(table_coupe.tr);
    }
    else if(start_zone== 5){
        struct position destination = {position.x, table_coupe.tr.y};
        go_to(destination);
    }
    else if(start_zone== 6){
        go_to(table_coupe.tl);
    }
    else if(start_zone== 7){
        struct position destination = {table_coupe.bl.x, position.y};
        go_to(destination);
    }
    /* Robot is now on circulation path */
    /* Path rotation direction finding*/
    // Imaginary line crossing starting position and center of stage
    float a = (position.y - table_coupe.extreme.y/2)/(position.x - table_coupe.extreme.x/2);
    float b = position.y - a * position.x;

    // Checking if destination point is above or under this line
    int rotation_direction;
    if(a*pos.x + b > pos.y){ // point under the line
        if(position.x < table_coupe.extreme.x /2){
            rotation_direction = -1;
        }
        else{
            rotation_direction = 1;
        }
    }
    else{ // point above the line
        if(position.x < table_coupe.extreme.x /2){
            rotation_direction = 1;
        }
        else{
            rotation_direction = -1;
        }
    }
    /* rotation direction found */
    /* circulating robot following circulation path */
    int x_zone = start_zone;
    while(x_zone != end_zone){
        x_zone = (x_zone - rotation_direction)%8;
        if(x_zone == -1){ x_zone = 7;}
        if(x_zone%2 == 0){ // next zone is a corner
            go_to(corner_position(x_zone));
        }
     }
    /* Robot stopped at nearest corner from desired zone */
    /* last movement is to go to end position*/
    if(end_zone== 0){
        go_to(pos);
    }
    else if(end_zone== 1){
        struct position destination = {pos.x, table_coupe.bl.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 2){
        go_to(pos);
    }
    else if(end_zone== 3){
        struct position destination = {table_coupe.br.x, pos.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 4){
        go_to(pos);
    }
    else if(end_zone== 5){
        struct position destination = {pos.x, table_coupe.tr.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 6){
        go_to(pos);
    }
    else if(end_zone== 7){
        struct position destination = {table_coupe.bl.x, pos.y};
        go_to(destination);
        go_to(pos);
    }
    /* Robot is at desired position */
}

/**
 * robots go to coordinates following imaginary path to avoid obvious obstacles
 * goes the opposite way of follow_to()
 */
void anti_follow_to(struct position pos){
    /* Zones finding */
    int start_zone = what_zone(position);
    int end_zone = what_zone(pos);

    /* Place robot to nearest point in circulation path*/
    if(start_zone== 0){
        go_to(table_coupe.bl);
    }
    else if(start_zone== 1){
        struct position destination = {position.x, table_coupe.bl.y};
        go_to(destination);
    }
    else if(start_zone== 2){
        go_to(table_coupe.br);
    }
    else if(start_zone== 3){
        struct position destination = {table_coupe.br.x, position.y};
        go_to(destination);
    }
    else if(start_zone== 4){
        go_to(table_coupe.tr);
    }
    else if(start_zone== 5){
        struct position destination = {position.x, table_coupe.tr.y};
        go_to(destination);
    }
    else if(start_zone== 6){
        go_to(table_coupe.tl);
    }
    else if(start_zone== 7){
        struct position destination = {table_coupe.bl.x, position.y};
        go_to(destination);
    }
    /* Robot is now on circulation path */
    /* Path rotation direction finding*/
    // Imaginary line crossing starting position and center of stage
    float a = (position.y - table_coupe.extreme.y/2)/(position.x - table_coupe.extreme.x/2);
    float b = position.y - a * position.x;

    // Checking if destination point is above or under this line
    int rotation_direction;
    if(a*pos.x + b > pos.y){ // point under the line
        if(position.x < table_coupe.extreme.x /2){
            rotation_direction = 1;
        }
        else{
            rotation_direction = -1;
        }
    }
    else{ // point above the line
        if(position.x < table_coupe.extreme.x /2){
            rotation_direction = -1;
        }
        else{
            rotation_direction = 1;
        }
    }
    /* rotation direction found (opposite of follow_to)*/
    /* circulating robot following circulation path */
    int x_zone = start_zone;
    while(x_zone != end_zone){
        x_zone = (x_zone - rotation_direction)%8;
        if(x_zone == -1){ x_zone = 7;}
        if(x_zone%2 == 0){ // next zone is a corner
            go_to(corner_position(x_zone));
        }
     }
    /* Robot stopped at nearest corner from desired zone */
    /* last movement is to go to end position*/
    if(end_zone== 0){
        go_to(pos);
    }
    else if(end_zone== 1){
        struct position destination = {pos.x, table_coupe.bl.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 2){
        go_to(pos);
    }
    else if(end_zone== 3){
        struct position destination = {table_coupe.br.x, pos.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 4){
        go_to(pos);
    }
    else if(end_zone== 5){
        struct position destination = {pos.x, table_coupe.tr.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 6){
        go_to(pos);
    }
    else if(end_zone== 7){
        struct position destination = {table_coupe.bl.x, pos.y};
        go_to(destination);
        go_to(pos);
    }
    /* Robot is at desired position */
}
