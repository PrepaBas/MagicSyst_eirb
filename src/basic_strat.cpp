#include "basic_strat.h"
#include "advanced_movement.h" // déplacé ici si nécessaire
#include "servom.h"
#include "StepperMotor.h"
#include "FastTrig.h"

enum security_protocol protocol = NO_SECURITY;

#define FRONT_OFFSET 160
#define SAFETY_RADIUS 270

// Création de l'objet Servo
Servo gauche;
Servo droite;
int eq_g = 85; //+ sens déploiment
int eq_d = 85; //- sens deploiment

void grab_cans(struct position cans_position){
    float x = cans_position.x;
    float y = cans_position.y;
    float t = cans_position.theta;
    follow_to({x-SAFETY_RADIUS*icos(t), y-SAFETY_RADIUS*isin(t)});
    rise_fork();
    angle_to(t);
    set_speed(0.6);
    move_straight(0, SAFETY_RADIUS - FRONT_OFFSET + 20);
    lower_fork();
    set_speed(1);
    protocol = LOADED_COMMUTE;
}

void release_cans(struct position cans_position){
    float x = cans_position.x;
    float y = cans_position.y;
    float t = cans_position.theta;
    follow_to({x-SAFETY_RADIUS*icos(t), y-SAFETY_RADIUS*isin(t)});
    angle_to(t);
    set_speed(0.6);
    move_straight(0, SAFETY_RADIUS - FRONT_OFFSET);
    rise_fork();
    protocol = BACKING;
    move_straight(1, SAFETY_RADIUS - FRONT_OFFSET+50);
    fold_fork();
    set_speed(1);
    protocol = EMPTY_COMMUTE;
}

void orange(void* param){
    enable_steppers();
    set_speed(1);
    fold_fork();
    protocol = EMPTY_COMMUTE;
    set_x(1140);
    set_y(87.5+25);
    set_theta(90);


    
    grab_cans({1100, 960, 90});
    release_cans({1250, 100, -90});
    
    grab_cans({1910, 960, 90});
    release_cans({1250, 200, -90});

    grab_cans({820, 1720, 90});

    go_to({450, 1700});
}

void blue(void* param){
    enable_steppers();
    set_speed(1);
    fold_fork();
    protocol = EMPTY_COMMUTE;

    set_x(1860);
    set_y(87.5+25);
    set_theta(0);


    
    grab_cans({1100, 960, 90});
    release_cans({1750, 150, 270});
    
    grab_cans({1910, 960, 90});
    release_cans({1750, 300, 270});

    // grab_cans({820, 1720, 90});

    // go_to({450, 1700});
}


void baniere(void *param)
{
    set_x(500);
    set_y(500);
    gauche.attach(15);
    droite.attach(2);
    gauche.write(eq_g);
    droite.write(eq_d);
    vTaskDelay(pdMS_TO_TICKS(100));
    set_speed(0.2);
    // move_straight(1, 300);
    move_straight(0, 25);
    //move_straight(0, 15);
    vTaskDelay(pdMS_TO_TICKS(500));
    for (int i = 0; i < 60; i++)
    {
        droite.write(eq_d - i);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (int i = 0; i < 60; i++)
    {
        gauche.write(eq_g + i);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
    set_speed(1);
    //vTaskDelete(NULL);

    gauche.detach();
    droite.detach();
}


void function1()
{
    enable_steppers();
    set_speed(1);
    protocol = EMPTY_COMMUTE;
    vTaskDelay(pdMS_TO_TICKS(200));
    set_x(85);
    set_y(865);
    set_theta(0);

    follow_to({775, 570});
    set_speed(0.5);
    protocol = EMPTY_APPROACH;
    go_to({775, 220});
    vTaskDelay(pdMS_TO_TICKS(1000));
    protocol = BACKING;
    go_to_reverse({775, 600});

    protocol = LOADED_COMMUTE;
    set_speed(1.);
    follow_to({200, 865});
    angle_to(0);
    protocol = BACKING;
    go_to_reverse({100, 865});
    protocol = EMPTY_COMMUTE;
}

void deposit_bl_cans(void *parameters)
{
    Serial.println("begining deposit_bl_cans");
    set_speed(1.);
    rise_fork();
    /* go to cans */
    follow_to({775, 570});

    /* contact cans */
    set_speed(0.5);
    go_to({775, 328});
    lower_fork();
    go_to_reverse({775, 500});

    /* go to deposit location */
    set_speed(1);
    anti_follow_to({300, 300});

    /* deposit cans */
    set_speed(0.5);
    go_to({300, 240});
    rise_fork();
    go_to_reverse({300, 400});

    vTaskDelete(NULL);
}

void deposit_tl_cans(void *parameters)
{
    Serial.println("begining deposit_tl_cans");
    set_speed(1);
    rise_fork();

    /* go to cans */
    follow_to({825, 1500});

    /* contact cans */
    set_speed(0.5);
    go_to({825, 1580});
    lower_fork();
    go_to_reverse({825, 1500});
    set_speed(1.);
    vTaskDelete(NULL);
}

void go_home(void *parameters)
{
    Serial.println("begining go_home");
    lower_fork();
    rise_fork();
    set_speed(1.);
    follow_to({200, 865});
    go_to_reverse({100, 865});
    rise_fork();
    vTaskDelete(NULL);
}

void deposit_bl_cans_2(void *parameters)
{
    Serial.println("begining deposit_bl_cans2");
    protocol = EMPTY_COMMUTE;
    set_speed(1.);
    rise_fork();
    vTaskDelay(pdMS_TO_TICKS(1000));
    follow_to({400, 400});
    set_speed(0.5);
    go_to({250, 400});
    lower_fork();
    go_to_reverse({400, 400});
    set_speed(1.);
    go_to({300, 865});
    go_to({250, 865});
    rise_fork();
    go_to_reverse({500, 865});
    vTaskDelete(NULL);
}
