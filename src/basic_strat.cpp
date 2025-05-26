#include "basic_strat.h"
#include "advanced_movement.h" // déplacé ici si nécessaire
#include "servom.h"
#include "StepperMotor.h"

enum security_protocol protocol = NO_SECURITY;

#define FRONT_OFFSET 160
#define SAFETY_RADIUS 260

// Création de l'objet Servo
Servo gauche;
Servo droite;
int eq_g = 85; //+ sens déploiment
int eq_d = 85; //- sens deploiment

void orange(void* param){
    enable_steppers();
    set_speed(1);
    fold_fork();
    protocol = EMPTY_COMMUTE;
    set_x(3000-87.5-25);
    set_y(865);
    set_theta(180);

    // rush to mid cans
    move_straight(0, 200);
    go_to({2300, 1300}); 
    go_to({1100, 1300});
    unfold_fork();
    protocol = LOADED_COMMUTE;
    set_speed(0.6);
    
    go_to({1100, 950+FRONT_OFFSET});
    lower_fork(); // pickup

    
    set_speed(1);
    go_to({1250, 350}); // deposit cans
    angle_to(-90);
    rise_fork();

    set_speed(0.3);//crash into wall
    move_straight(0, 350-FRONT_OFFSET+100); 
    set_y(196);
    vTaskDelay(pdMS_TO_TICKS(100));

    set_speed(0.6);
    move_straight(1, SAFETY_RADIUS);
    fold_fork();
    protocol = EMPTY_COMMUTE;

    //rush to second mid cans
    set_speed(1);
    go_to({1910, 700});
    unfold_fork();

    protocol = LOADED_COMMUTE;
    set_speed(0.6);
    go_to({1910, 950-FRONT_OFFSET});
    lower_fork();

    set_speed(1);
    go_to({1260, 350});
    rise_fork();
    move_straight(1, SAFETY_RADIUS);

    fold_fork();
    set_speed(1);
    go_to({820, 1720-SAFETY_RADIUS});
    unfold_fork();
    set_speed(0.6);
    go_to({820, 1720-FRONT_OFFSET});
    lower_fork();
    set_speed(1);
    move_straight(1, SAFETY_RADIUS);
     
    go_to({450, 1700});
}

void blue(void* param){
    enable_steppers();
    set_speed(1);
    fold_fork();
    protocol = EMPTY_COMMUTE;
    set_x(0+87.5+25);
    set_y(865);
    set_theta(0);

    // rush to mid cans
    move_straight(0, 200);
    go_to({700, 1300});
    go_to({1100, 1300});
    unfold_fork();
    protocol = LOADED_COMMUTE;
    set_speed(0.6);
    
    go_to({1100, 950+FRONT_OFFSET});
    lower_fork(); // pickup

    
    set_speed(1);
    go_to({1800, 350}); // deposit cans
    angle_to(-90);
    rise_fork();

    set_speed(0.3);//crash into wall
    move_straight(0, 350-FRONT_OFFSET+100); 
    set_y(196);
    vTaskDelay(pdMS_TO_TICKS(100));

    set_speed(0.6);
    move_straight(1, SAFETY_RADIUS);
    fold_fork();
    protocol = EMPTY_COMMUTE;

    //rush to second mid cans
    set_speed(1);
    go_to({1910, 600});
    unfold_fork();

    protocol = LOADED_COMMUTE;
    set_speed(0.6);
    go_to({1910, 960-FRONT_OFFSET});
    lower_fork();

    set_speed(1);
    go_to({1300, 320});
    rise_fork();
    move_straight(1, SAFETY_RADIUS);

    fold_fork();
    set_speed(1);
    go_to({3000-820, 1720-SAFETY_RADIUS});
    unfold_fork();
    set_speed(0.6);
    go_to({3000-820, 1720-FRONT_OFFSET});
    lower_fork();
    set_speed(1);
    move_straight(1, SAFETY_RADIUS);
     
    go_to({3000-400, 2000-225});
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
    set_speed(0.5);
    move_straight(1, 100);
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

    vTaskDelay(pdMS_TO_TICKS(2000));
    set_speed(0.6);
    //move_straight(0, 300);
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
