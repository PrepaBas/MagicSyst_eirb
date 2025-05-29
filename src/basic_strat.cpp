#include "basic_strat.h"
#include "advanced_movement.h" // déplacé ici si nécessaire
#include "servom.h"
#include "StepperMotor.h"
#include "FastTrig.h"

enum security_protocol protocol = NO_SECURITY;

#define FRONT_OFFSET 160
#define SAFETY_RADIUS 280
#define EXTRA 30
#define CALES 100

// Création de l'objet Servo
Servo gauche;
Servo droite;
int eq_g = 85; //+ sens déploiment
int eq_d = 85; //- sens deploiment

void grab_cans(struct position cans_position){
    float x = cans_position.x;
    float y = cans_position.y;
    float t = cans_position.theta;
    follow_to({x-(SAFETY_RADIUS+EXTRA)*icos(t), y-(SAFETY_RADIUS+EXTRA)*isin(t)});
    rise_fork();
    angle_to(t);
    set_speed(0.6);
    move_straight(0, SAFETY_RADIUS +EXTRA - FRONT_OFFSET + 20);
    lower_fork();
    protocol = BACKING;
    move_straight(1, FRONT_OFFSET);
    set_speed(1);
    protocol = LOADED_COMMUTE;
}

void grab_cans_uwu(struct position cans_position){
    float x = cans_position.x;
    float y = cans_position.y;
    float t = cans_position.theta;
    go_to({x-(SAFETY_RADIUS+EXTRA)*icos(t), y-(SAFETY_RADIUS+EXTRA)*isin(t)});
    rise_fork();
    angle_to(t);
    set_speed(0.6);
    move_straight(0, SAFETY_RADIUS +EXTRA - FRONT_OFFSET + 20);
    lower_fork();
    protocol = BACKING;
    move_straight(1, FRONT_OFFSET);
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

void release_cans_uwu(struct position cans_position){
    float x = cans_position.x;
    float y = cans_position.y;
    float t = cans_position.theta;
    go_to({x-SAFETY_RADIUS*icos(t), y-SAFETY_RADIUS*isin(t)});
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

void homologation(){
    enable_steppers();
    set_speed(1);
    fold_fork();
    protocol = EMPTY_COMMUTE;
    set_x(1160);
    set_y(87.5+25);
    set_theta(90);   

    protocol = EMPTY_COMMUTE;
    // mid left cans
    grab_cans({1100, 950, 90});
    release_cans({1250, 200, -90});

}

void homologation_bis(){
    enable_steppers();
    set_speed(1);
    fold_fork();
    protocol = EMPTY_COMMUTE;
    set_x(1160);
    set_y(87.5+25);
    set_theta(90);   

    protocol = EMPTY_COMMUTE;
    move_straight(0, 500);
    angle_to(0);
    line_of_truth();
}

void orange(void* param){
    enable_steppers();
    set_speed(1);
    fold_fork();
    protocol = EMPTY_COMMUTE;
    set_x(1160);
    set_y(87.5+25);
    set_theta(90);    

    // mid right cans
    grab_cans({1900, 950, 90});
    release_cans({1250, 100, -90});

    // mid left cans
    grab_cans({1100, 950, 90});
    release_cans({1250, 200, -90});
    
    // push cans of front
    follow_to({775, 600});
    unfold_fork();
    angle_to(270);
    move_straight(0, 600-100-FRONT_OFFSET);
    protocol = BACKING;
    move_straight(1, SAFETY_RADIUS);
    fold_fork();
    protocol = EMPTY_COMMUTE;

    // cans far left
    // grab_cans({75, 400, 180});
    // protocol = BACKING;
    // move_straight(1, SAFETY_RADIUS);
    // protocol = LOADED_COMMUTE;
    // release_cans({1250, 300, 270});

    // cans near backstages
    grab_cans({820, 1720, 90});
    protocol = BACKING;
    move_straight(1, SAFETY_RADIUS);
    protocol = LOADED_COMMUTE;
    go_to({360, 1700});
    angle_to(90);
    rise_fork();
    protocol = BACKING;
    move_straight(1, 10);

}

void blue(void* param){
    enable_steppers();
    set_speed(1);
    fold_fork();
    protocol = EMPTY_COMMUTE;
    set_x(3000-1160);
    set_y(87.5+25);
    set_theta(90);    

    // mid right cans
    grab_cans({3000-1100, 950, 90});
    release_cans({3000-1250, 100, -90});

    // mid left cans
    grab_cans({1100, 950, 90});
    release_cans({3000-1250, 200, -90});
    
    // push cans of front
    follow_to({3000-775, 600});
    unfold_fork();
    angle_to(270);
    move_straight(0, 600-100-FRONT_OFFSET);
    protocol = BACKING;
    move_straight(1, SAFETY_RADIUS);
    fold_fork();
    protocol = EMPTY_COMMUTE;

    // cans far left
    // go_to({3000-75-SAFETY_RADIUS, 400});
    // grab_cans({3000-75, 400, 0});
    // protocol = BACKING;
    // move_straight(1, SAFETY_RADIUS);
    // protocol = LOADED_COMMUTE;
    // release_cans({3000-1250, 300, -90});

    // cans near backstages
    grab_cans({3000-820, 1720, 90});
    protocol = BACKING;
    move_straight(1, 100);
    protocol = LOADED_COMMUTE;
    go_to({3000-360, 1675});
    angle_to(90);
    rise_fork();
    protocol = BACKING;
    move_straight(1, 10);

}

void blue_uwu(void* param){
    enable_steppers();
    set_speed(1);
    fold_fork();
    protocol = EMPTY_COMMUTE;
    set_x(3000-1160);
    set_y(87.5+25);
    set_theta(90);    

    // mid right cans
    move_straight(0, 100);
    grab_cans_uwu({3000-1100, 950, 90});
    release_cans_uwu({3000-1250, 100, -90});
    // go_to({3000-1300, 600});

    // // mid left cans
    // grab_cans_uwu({1100, 950, 90});
    // go_to({3000-1200, 600});
    // release_cans_uwu({3000-1300, 200, -90});
        go_to({3000-900, 700});
    // push cans of front
    go_to({3000-775, 600});
    unfold_fork();
    angle_to(270);
    set_speed(0.6);
    move_straight(0, 600-100-FRONT_OFFSET);
    set_speed(1);
    protocol = BACKING;
    move_straight(1, SAFETY_RADIUS);
    fold_fork();
    protocol = EMPTY_COMMUTE;

    // cans near backstages
    go_to({3000-650, 1000});
    grab_cans_uwu({3000-820, 1720, 90});
    protocol = BACKING;
    move_straight(1, 150);
    protocol = LOADED_COMMUTE;
    go_to({3000-360, 1675});
    angle_to(90);
    rise_fork();
    protocol = BACKING;
    move_straight(1, 10);
}

void orange_uwu(void* param){
    enable_steppers();
    set_speed(1);
    fold_fork();
    protocol = EMPTY_COMMUTE;
    set_x(1160);
    set_y(87.5+25);
    set_theta(90);    

    move_straight(0, 120);  

    // mid left cans
    grab_cans_uwu({1100, 950, 90});
    release_cans_uwu({1300, 100, -90});  


        // mid right cans
    // go_to({1200, 600});
    // grab_cans_uwu({1100, 950, 90});
    // go_to({1200, 600});
    // release_cans_uwu({1250, 100, -90});
    
    // push cans of front
    go_to({900, 650});
    go_to({775, 600});
    unfold_fork();
    angle_to(270);
    set_speed(0.6);
    move_straight(0, 600-100-FRONT_OFFSET);
    set_speed(1);
    protocol = BACKING;
    move_straight(1, SAFETY_RADIUS);
    fold_fork();
    protocol = EMPTY_COMMUTE;

    // cans near backstages
    go_to({650, 1000});
    grab_cans_uwu({820, 1720, 90});
    protocol = BACKING;
    move_straight(1, 150);
    protocol = LOADED_COMMUTE;
    go_to({360, 1675});
    angle_to(90);
    rise_fork();
    protocol = BACKING;
    move_straight(1, 10);
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
    move_straight(1, CALES-25);
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

    #ifndef SAVARY
    gauche.detach();
    droite.detach();
    #endif
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
    // Serial.println("begining deposit_bl_cans");
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
    // Serial.println("begining deposit_tl_cans");
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
    // Serial.println("begining go_home");
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
    // Serial.println("begining deposit_bl_cans2");
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
