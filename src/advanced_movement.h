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


class RobotCoupe {
  // Attributs : private par defaut
  private:

  struct position _position;
  move_type_t _last_move_type = STRAIGHT_FORWARD;
  float _baseWidth;
  float _wheelRadius;

  // Methodes : public par defaut:
  public:
  RobotCoupe(float baseWidth, float wheelRadius);
  ~RobotCoupe();

  // stepper motors instanciation
  StepperMotor motors;

  uint64_t steps_done = 0;
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
};


#endif