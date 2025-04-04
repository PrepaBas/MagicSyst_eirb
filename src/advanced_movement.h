#ifndef movement_h
#define movement_h

#include "Arduino.h"
#include "StepperMotor.h"
#include "table.h"

/**
 * @brief different types of movement
 */
typedef enum {
  STRAIGHT_FORWARD=0,
  STRAIGHT_BACKWARD,
  ROTATE_RIGHT,
  ROTATE_LEFT,
} move_type_t;


struct position position;
move_type_t last_move_type = STRAIGHT_FORWARD;
float baseWidth;
float wheelRadius;

// Begin 
void begin();

// Setters
void set_x(float x);
void set_y(float y);
void set_theta(float theta);

// Autres methodes
void move_straight(char direction, float distance);
void rotate (int direction, float angle);
void angle_to(float angle);
void go_to(struct position pos);
void go_to_reverse(struct position pos);
void follow_to(struct position pos);
void anti_follow_to(struct position pos);
void new_position();


#endif