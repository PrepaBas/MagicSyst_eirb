#ifndef movement_h
#define movement_h

#include "Arduino.h"
#include "StepperMotor.h"
#include "table.h"

//extern table table_coupe;

class RobotCoupe {
  // Attributs : private par defaut
  private:

  struct position _position;
  float _baseWidth;
  float _wheelRadius;

  // Methodes : public par defaut:
  public:
  RobotCoupe(float baseWidth, float wheelRadius);

  // stepper motors instanciation
  StepperMotor motors;

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
};


#endif