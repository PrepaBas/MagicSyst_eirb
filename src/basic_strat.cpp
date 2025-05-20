#include "basic_strat.h"
#include "advanced_movement.h" // déplacé ici si nécessaire
#include "servom.h"
#include "StepperMotor.h"






enum security_protocol protocol = NO_SECURITY;

// Création de l'objet Servo
Servo gauche;
Servo droite;
int eq_g = 85; //+ sens déploiment
int eq_d = 80; //- sens deploiment
void baniere(void *param)
{
    gauche.attach(23);
    droite.attach(22);
    gauche.write(eq_g);
    droite.write(eq_d);
    vTaskDelay(pdMS_TO_TICKS(100));
    set_speed(0.5);
    move_straight(1, 99);
    move_straight(0, 25);
    //move_straight(0, 15);
    vTaskDelay(pdMS_TO_TICKS(500));
    for (int i = 0; i < 60; i++)
    {
        gauche.write(eq_g + i);
        droite.write(eq_d - i);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelay(pdMS_TO_TICKS(500));
    set_speed(0.6);
    move_straight(0, 300);
    set_speed(1);
    vTaskDelete(NULL);
}

void function1()
{
    enable_steppers();
    set_speed(1);
    protocol = EMPTY_COMMUTE;
    vTaskDelay(pdMS_TO_TICKS(200));
    set_x(100);
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
