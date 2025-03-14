/* Includes ------------------------------------------------------------------*/

/* Arduino specific header files. */
#include "Arduino.h"

/* file header file */
#include "advanced_movement.h"
#include "StepperMotor.h"

table table_coupe = {{650, 670}, {650, 1300}, {2340, 670}, {2340, 1300}, {3000, 2000}};


/* ---------------------------------------------------------------------*/
RobotCoupe::RobotCoupe(float baseWidth, float wheelRadius){
    _baseWidth = baseWidth;
    _wheelRadius = wheelRadius;
    motors.begin();
    
}

/* Setters and Getters -------------------------------------------------------*/

void RobotCoupe::set_x(float x){
    _position.x = x;
}

void RobotCoupe::set_y(float y){
    _position.y = y;
}

void RobotCoupe::set_theta(float theta){
    _position.theta = theta;
}

void RobotCoupe::move_straight (char direction, float distance){
    /* Move robot in a straight line
     * direction :  1 for Forward
     *              0 for Backward */
    long m_steps = motors.param.step_mode * 200 * distance / (2. * PI * _wheelRadius); // Implicit conversion
    digitalWrite(motors.pinout.dir2_pin, direction?HIGH:LOW);
    digitalWrite(motors.pinout.dir1_pin, direction?LOW:HIGH);
    RobotCoupe::motors.remaining_steps = m_steps;
    while(RobotCoupe::motors.remaining_steps){Serial.println("moving_straight");}
}

void RobotCoupe::rotate (int direction, float angle){
    /* Rotate robot in place
     * direction :  0 for left rotation (anti-clockwise)
     *              1 for right rotation (clockwise) */
    float distance = angle * _baseWidth / 360.;
    long m_steps = motors.param.step_mode * 200 * distance / (2. * _wheelRadius);
    digitalWrite(motors.pinout.dir2_pin, direction?HIGH:LOW);
    digitalWrite(motors.pinout.dir1_pin, direction?HIGH:LOW);
    RobotCoupe::motors.remaining_steps = m_steps;
    while(RobotCoupe::motors.remaining_steps){Serial.println("rotating");}
}


void RobotCoupe::angle_to(float angle){
    float move_angle = angle - _position.theta;

    // Making sure that angle is [-180; 180] */
    while(move_angle > 180){
        move_angle += -360;
    }
    while(move_angle < -180){
        move_angle += 360;
    }

    /* Choosing rotation direction */
    if(move_angle >= 0){
        RobotCoupe::rotate(1, move_angle);
    }
    else{
        RobotCoupe::rotate(0, - move_angle);
    }

    /* Updating to new angle */
    RobotCoupe::set_theta(angle);
}

void RobotCoupe::go_to(struct position pos){
    /* Move robot to coordinates.
     * First rotate then move in straight line */
    
    float x = pos.x - _position.x;
    float y = pos.y - _position.y;
    float radius = sqrt(x*x+y*y);
    float angle = atan2(y, x)*360/(2 * PI);
    RobotCoupe::angle_to(angle);
    RobotCoupe::move_straight(0, radius);

    // Update new position
    RobotCoupe::set_x(pos.x);
    RobotCoupe::set_y(pos.y);
    
}

void RobotCoupe::go_to_reverse(struct position pos){
    /* Move robot to coordinates.
     * First rotate then move in straight line facing backwards */
    
    float x = pos.x - _position.x;
    float y = pos.y - _position.y;
    float radius = sqrt(x*x+y*y);
    float angle = atan2(y, x)*360/(2 * PI);
    RobotCoupe::angle_to(angle+180);
    RobotCoupe::move_straight(1, radius);

    // Update new position
    RobotCoupe::set_x(pos.x);
    RobotCoupe::set_y(pos.y);
    
}

position corner_positon(int c){
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
void RobotCoupe::follow_to(struct position pos){
    /* Zones finding */
    int start_zone = what_zone(_position);
    int end_zone = what_zone(pos);

    /* Place robot to nearest point in circulation path*/
    if(start_zone== 0){
        RobotCoupe::go_to(table_coupe.bl);
    }
    else if(start_zone== 1){
        struct position destination = {_position.x, table_coupe.bl.y};
        RobotCoupe::go_to(destination);
    }
    else if(start_zone== 2){
        RobotCoupe::go_to(table_coupe.br);
    }
    else if(start_zone== 3){
        struct position destination = {table_coupe.br.x, _position.y};
        RobotCoupe::go_to(destination);
    }
    else if(start_zone== 4){
        RobotCoupe::go_to(table_coupe.tr);
    }
    else if(start_zone== 5){
        struct position destination = {_position.x, table_coupe.tr.y};
        RobotCoupe::go_to(destination);
    }
    else if(start_zone== 6){
        RobotCoupe::go_to(table_coupe.tl);
    }
    else if(start_zone== 7){
        struct position destination = {table_coupe.bl.x, _position.y};
        RobotCoupe::go_to(destination);
    }
    /* Robot is now on circulation path */
    /* Path rotation direction finding*/
    // Imaginary line crossing starting position and center of stage
    float a = (_position.y - table_coupe.extreme.y/2)/(_position.x - table_coupe.extreme.x/2);
    float b = _position.y - a * _position.x;

    // Checking if destination point is above or under this line
    int rotation_direction;
    if(a*pos.x + b > pos.y){ // point under the line
        if(_position.x < table_coupe.extreme.x /2){
            rotation_direction = -1;
        }
        else{
            rotation_direction = 1;
        }
    }
    else{ // point above the line
        if(_position.x < table_coupe.extreme.x /2){
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
            RobotCoupe::go_to(corner_positon(x_zone));
        }
     }
    /* Robot stopped at nearest corner from desired zone */
    /* last movement is to go to end position*/
    if(end_zone== 0){
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 1){
        struct position destination = {pos.x, table_coupe.bl.y};
        RobotCoupe::go_to(destination);
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 2){
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 3){
        struct position destination = {table_coupe.br.x, pos.y};
        RobotCoupe::go_to(destination);
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 4){
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 5){
        struct position destination = {pos.x, table_coupe.tr.y};
        RobotCoupe::go_to(destination);
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 6){
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 7){
        struct position destination = {table_coupe.bl.x, pos.y};
        RobotCoupe::go_to(destination);
        RobotCoupe::go_to(pos);
    }
    /* Robot is at desired position */
}

/**
 * robots go to coordinates following imaginary path to avoid obvious obstacles
 * goes the opposite way of follow_to()
 */
void RobotCoupe::anti_follow_to(struct position pos){
    /* Zones finding */
    int start_zone = what_zone(_position);
    int end_zone = what_zone(pos);

    /* Place robot to nearest point in circulation path*/
    if(start_zone== 0){
        RobotCoupe::go_to(table_coupe.bl);
    }
    else if(start_zone== 1){
        struct position destination = {_position.x, table_coupe.bl.y};
        RobotCoupe::go_to(destination);
    }
    else if(start_zone== 2){
        RobotCoupe::go_to(table_coupe.br);
    }
    else if(start_zone== 3){
        struct position destination = {table_coupe.br.x, _position.y};
        RobotCoupe::go_to(destination);
    }
    else if(start_zone== 4){
        RobotCoupe::go_to(table_coupe.tr);
    }
    else if(start_zone== 5){
        struct position destination = {_position.x, table_coupe.tr.y};
        RobotCoupe::go_to(destination);
    }
    else if(start_zone== 6){
        RobotCoupe::go_to(table_coupe.tl);
    }
    else if(start_zone== 7){
        struct position destination = {table_coupe.bl.x, _position.y};
        RobotCoupe::go_to(destination);
    }
    /* Robot is now on circulation path */
    /* Path rotation direction finding*/
    // Imaginary line crossing starting position and center of stage
    float a = (_position.y - table_coupe.extreme.y/2)/(_position.x - table_coupe.extreme.x/2);
    float b = _position.y - a * _position.x;

    // Checking if destination point is above or under this line
    int rotation_direction;
    if(a*pos.x + b > pos.y){ // point under the line
        if(_position.x < table_coupe.extreme.x /2){
            rotation_direction = 1;
        }
        else{
            rotation_direction = -1;
        }
    }
    else{ // point above the line
        if(_position.x < table_coupe.extreme.x /2){
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
            RobotCoupe::go_to(corner_positon(x_zone));
        }
     }
    /* Robot stopped at nearest corner from desired zone */
    /* last movement is to go to end position*/
    if(end_zone== 0){
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 1){
        struct position destination = {pos.x, table_coupe.bl.y};
        RobotCoupe::go_to(destination);
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 2){
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 3){
        struct position destination = {table_coupe.br.x, pos.y};
        RobotCoupe::go_to(destination);
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 4){
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 5){
        struct position destination = {pos.x, table_coupe.tr.y};
        RobotCoupe::go_to(destination);
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 6){
        RobotCoupe::go_to(pos);
    }
    else if(end_zone== 7){
        struct position destination = {table_coupe.bl.x, pos.y};
        RobotCoupe::go_to(destination);
        RobotCoupe::go_to(pos);
    }
    /* Robot is at desired position */
}