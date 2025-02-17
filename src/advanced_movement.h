  /*
  RobotCoupe.h - Library for controling 2 weeled robots using XNucleoIHM02A1.
  Created by Bastien T. France, january, 2025.
  Released into the public domain.
*/

#ifndef movement_h
#define movement_h

#include "Arduino.h"
#include "StepperMotor.h"

struct position {
    float x;
    float y;
    float theta;
};

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
  StepperMotor motors[2];

  int normal_speed = 300;
  int low_speed = 50;

  // Begin 
  void begin();

  // Setters
  void set_x(float x);
  void set_y(float y);
  void set_theta(float theta);
  void set_baseWidth(float baseWidth);
  void set_wheelRadius(float wheelRadius);

  // Autres methodes
  void move_straight(char direction, float distance);
  void rotate (int direction, float angle);
  void angle_to(float angle);
  void go_to(struct position pos);
  void follow_to(struct position pos);
};


#endif