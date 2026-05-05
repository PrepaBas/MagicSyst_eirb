#include "basic_strat.h"
#include "advanced_movement.h" // déplacé ici si nécessaire
#include "servom.h"
#include "StepperMotor.h"
#include "FastTrig.h"

enum security_protocol protocol = NO_SECURITY;

#define FRONT_OFFSET 160
#define SAFETY_RADIUS 280
#define EXTRA 45
#define CALES 100

// Création de l'objet Servo
Servo gauche;
Servo droite;
int eq_g = 85; //+ sens déploiment
int eq_d = 85; //- sens deploiment

void line_of_truth(){
    for(;;){
            vTaskDelay(pdMS_TO_TICKS(500));
            protocol = EMPTY_COMMUTE;
            move_straight(0, 400);

            vTaskDelay(pdMS_TO_TICKS(500));
            protocol = BACKING;
            move_straight(1, 400); 
        }
}

void function1()
{
    enable_steppers();
    set_speed(1);
    protocol = EMPTY_COMMUTE;
    
    move_straight(1, 1000);
    // move_straight(1, 1000);
    // rotate(0, 90);
    // move_straight(1, 1000);
    // rotate(0, 90);
    // move_straight(1, 1000);
    // move_straight(1, 1000);
    // rotate(0, 90);
    // move_straight(1, 1000);
    // rotate(0, 90);
    vTaskDelay(500);
    // disable_steppers();
}
