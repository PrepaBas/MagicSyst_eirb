//* Includes ------------------------------------------------------------------*/
#include "Arduino.h"
#include "advanced_movement.h"
#include "StepperMotor.h" // Contient FastAccelStepper & TMC
#include "table.h"
#include "FastTrig.h"

/* Global Variables ----------------------------------------------------------*/
struct position position = {1, 1, 0};
move_type_t last_move_type = STRAIGHT_FORWARD;
float baseWidth;
float wheelRadius;

// Définition de la table (valeurs fixes)
#define BOTTOM_LINE 650
#define TOP_LINE 1300
#define RIGHT_LINE 2360
#define LEFT_LINE 640

table table_coupe = {{LEFT_LINE, BOTTOM_LINE}, {LEFT_LINE, TOP_LINE}, {RIGHT_LINE, BOTTOM_LINE}, {RIGHT_LINE, TOP_LINE}, {3000, 2000}};

/* Setters and Getters -------------------------------------------------------*/
void movement_begin(float base, float wheel, float x, float y, float theta){
    baseWidth = base;
    wheelRadius = wheel;
    set_x(x);
    set_y(y);
    set_theta(theta);
}

void set_x(float x){ position.x = x; }
void set_y(float y){ position.y = y; }
void set_theta(float theta){ position.theta = theta; }
float get_theta(){ return position.theta; }

/* Conversions --------------------------------------------------------------*/
uint64_t distance_to_steps(float distance, float radius, uint16_t steps_per_revolution){
    return (uint64_t)(steps_per_revolution * distance / (2. * PI * radius));
}

float steps_to_distance(uint64_t steps, float radius, uint16_t steps_per_revolution){
    return (float)steps * 2. * PI * radius / (float)steps_per_revolution;
}

float restrict_angle(float in_angle){
    while(in_angle > 180) in_angle -= 360;
    while(in_angle < -180) in_angle += 360;
    return in_angle;
}

/* Odométrie Hardware --------------------------------------------------------*/
/**
 * @brief Calcule le déplacement réel depuis le dernier appel grâce aux compteurs matériels
 */
void new_position(){
    static int32_t last_pos_L = 0;
    static int32_t last_pos_R = 0;

    // Récupération des positions absolues des moteurs (en pas)
    int32_t current_pos_L = get_pose_left();
    int32_t current_pos_R = get_pose_right();

    // Calcul du delta de pas effectués (on prend la moyenne des deux moteurs pour la distance)
    int32_t deltaL = current_pos_L - last_pos_L;
    int32_t deltaR = current_pos_R - last_pos_R;

    uint8_t step_mode = get_step_mode();
    uint16_t steps_per_rev = 200 * step_mode;

    // On utilise la valeur absolue du mouvement d'un des moteurs pour la distance d parcourue
    // Dans FastAccelStepper, on gère la distance selon le type de mouvement
    float dL = steps_to_distance(abs(deltaL), wheelRadius, steps_per_rev);
    float dR = steps_to_distance(abs(deltaR), wheelRadius, steps_per_rev);
    float d = (dL + dR) / 2.0; // Distance moyenne

    float theta = get_theta();

    switch(last_move_type){
        case ROTATE_LEFT:
            set_theta(restrict_angle(theta - d * 360. / (PI * baseWidth)));
            break;
        case ROTATE_RIGHT:
            set_theta(restrict_angle(theta + d * 360. / (PI * baseWidth)));
            break;
        case STRAIGHT_FORWARD:
            set_x(position.x + d * icos(theta));
            set_y(position.y + d * isin(theta));
            break;
        case STRAIGHT_BACKWARD:
            set_x(position.x - d * icos(theta));
            set_y(position.y - d * isin(theta)); 
            break;
    }

    last_pos_L = current_pos_L;
    last_pos_R = current_pos_R;
}

/* Mouvements de base --------------------------------------------------------*/
void move_straight (char direction, float distance){
    uint8_t step_mode = get_step_mode();
    long m_steps = distance_to_steps(distance, wheelRadius, 200 * step_mode);
    
    last_move_type = direction ? STRAIGHT_FORWARD : STRAIGHT_BACKWARD;

    // Pilotage différentiel (un moteur inversé physiquement pour avancer)
    if(direction == 1) { // Forward
        stepper_move(m_steps, 1, -1); // stepper_move(steps, dirL, dirR)
    } else { // Backward
        stepper_move(m_steps, -1, 1);
    }

    // Attente de la fin du mouvement hardware
    while(is_stepper_running()){
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    new_position(); 
}

void rotate (int direction, float angle){
    float arc_distance = angle * baseWidth * PI / 360.;
    uint8_t step_mode = get_step_mode();
    long m_steps = distance_to_steps(arc_distance, wheelRadius, 200 * step_mode);
    
    last_move_type = direction ? ROTATE_RIGHT : ROTATE_LEFT;

    // En rotation, les deux moteurs tournent dans le même sens physique
    if(direction == 1) { // Right (Clockwise)
        stepper_move(m_steps, 1, 1);
    } else { // Left (Anti-clockwise)
        stepper_move(m_steps, -1, -1);
    }

    while(is_stepper_running()){
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    new_position();
}

/* Navigation Haut Niveau ----------------------------------------------------*/
void angle_to(float angle){
    float move_angle = restrict_angle(angle - position.theta);
    if(move_angle >= 0) rotate(1, move_angle);
    else rotate(0, -move_angle);
}

void go_to(struct position pos){
    float x = pos.x - position.x;
    float y = pos.y - position.y;
    float radius = sqrt(x*x + y*y);
    float angle = atan2(y, x) * 180.0 / PI;

    angle_to(angle);
    move_straight(1, radius); // 1 = Forward    
}

void go_to_reverse(struct position pos){
    float x = pos.x - position.x;
    float y = pos.y - position.y;
    float radius = sqrt(x*x + y*y);
    float angle = atan2(y, x) * 180.0 / PI;

    angle_to(restrict_angle(angle + 180));
    move_straight(0, radius); // 0 = Backward
}

/* Gestion des Zones & Obstacles ---------------------------------------------*/
struct position corner_position(int c){
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
            if(pos.x<1.5*pos.y){
                if(pos.x<1.5*(table_coupe.extreme.y - pos.y)) 
                    return 7; // left_mid
                else 
                    return 5; // mid_top
            }
            else{
                if(pos.x<1.5*(table_coupe.extreme.y - pos.y)) 
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
void follow_to(struct position pos){
    /* Zones finding */
    int start_zone = what_zone(position);
    int end_zone = what_zone(pos);

    /* Place robot to nearest point in circulation path*/
    if(start_zone== 0){
        go_to(table_coupe.bl);
    }
    else if(start_zone== 1){
        struct position destination = {position.x, table_coupe.bl.y};
        go_to(destination);
    }
    else if(start_zone== 2){
        go_to(table_coupe.br);
    }
    else if(start_zone== 3){
        struct position destination = {table_coupe.br.x, position.y};
        go_to(destination);
    }
    else if(start_zone== 4){
        go_to(table_coupe.tr);
    }
    else if(start_zone== 5){
        struct position destination = {position.x, table_coupe.tr.y};
        go_to(destination);
    }
    else if(start_zone== 6){
        go_to(table_coupe.tl);
    }
    else if(start_zone== 7){
        struct position destination = {table_coupe.bl.x, position.y};
        go_to(destination);
    }
    /* Robot is now on circulation path */
    /* Path rotation direction finding*/
    // Imaginary line crossing starting position and center of stage
    float a = (position.y - table_coupe.extreme.y/2)/(position.x - table_coupe.extreme.x/2);
    float b = position.y - a * position.x;

    // Checking if destination point is above or under this line
    int rotation_direction;
    if(a*pos.x + b > pos.y){ // point under the line
        if(position.x < table_coupe.extreme.x /2){
            rotation_direction = -1;
        }
        else{
            rotation_direction = 1;
        }
    }
    else{ // point above the line
        if(position.x < table_coupe.extreme.x /2){
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
            go_to(corner_position(x_zone));
        }
     }
    /* Robot stopped at nearest corner from desired zone */
    /* last movement is to go to end position*/
    if(end_zone== 0){
        go_to(pos);
    }
    else if(end_zone== 1){
        struct position destination = {pos.x, table_coupe.bl.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 2){
        go_to(pos);
    }
    else if(end_zone== 3){
        struct position destination = {table_coupe.br.x, pos.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 4){
        go_to(pos);
    }
    else if(end_zone== 5){
        struct position destination = {pos.x, table_coupe.tr.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 6){
        go_to(pos);
    }
    else if(end_zone== 7){
        struct position destination = {table_coupe.bl.x, pos.y};
        go_to(destination);
        go_to(pos);
    }
    /* Robot is at desired position */
}

/**
 * robots go to coordinates following imaginary path to avoid obvious obstacles
 * goes the opposite way of follow_to()
 */
void anti_follow_to(struct position pos){
    /* Zones finding */
    int start_zone = what_zone(position);
    int end_zone = what_zone(pos);

    /* Place robot to nearest point in circulation path*/
    if(start_zone== 0){
        go_to(table_coupe.bl);
    }
    else if(start_zone== 1){
        struct position destination = {position.x, table_coupe.bl.y};
        go_to(destination);
    }
    else if(start_zone== 2){
        go_to(table_coupe.br);
    }
    else if(start_zone== 3){
        struct position destination = {table_coupe.br.x, position.y};
        go_to(destination);
    }
    else if(start_zone== 4){
        go_to(table_coupe.tr);
    }
    else if(start_zone== 5){
        struct position destination = {position.x, table_coupe.tr.y};
        go_to(destination);
    }
    else if(start_zone== 6){
        go_to(table_coupe.tl);
    }
    else if(start_zone== 7){
        struct position destination = {table_coupe.bl.x, position.y};
        go_to(destination);
    }
    /* Robot is now on circulation path */
    /* Path rotation direction finding*/
    // Imaginary line crossing starting position and center of stage
    float a = (position.y - table_coupe.extreme.y/2)/(position.x - table_coupe.extreme.x/2);
    float b = position.y - a * position.x;

    // Checking if destination point is above or under this line
    int rotation_direction;
    if(a*pos.x + b > pos.y){ // point under the line
        if(position.x < table_coupe.extreme.x /2){
            rotation_direction = 1;
        }
        else{
            rotation_direction = -1;
        }
    }
    else{ // point above the line
        if(position.x < table_coupe.extreme.x /2){
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
            go_to(corner_position(x_zone));
        }
     }
    /* Robot stopped at nearest corner from desired zone */
    /* last movement is to go to end position*/
    if(end_zone== 0){
        go_to(pos);
    }
    else if(end_zone== 1){
        struct position destination = {pos.x, table_coupe.bl.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 2){
        go_to(pos);
    }
    else if(end_zone== 3){
        struct position destination = {table_coupe.br.x, pos.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 4){
        go_to(pos);
    }
    else if(end_zone== 5){
        struct position destination = {pos.x, table_coupe.tr.y};
        go_to(destination);
        go_to(pos);
    }
    else if(end_zone== 6){
        go_to(pos);
    }
    else if(end_zone== 7){
        struct position destination = {table_coupe.bl.x, pos.y};
        go_to(destination);
        go_to(pos);
    }
    /* Robot is at desired position */
}
