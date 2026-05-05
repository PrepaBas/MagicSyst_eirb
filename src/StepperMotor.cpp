#include "StepperMotor.h"
#include <FastAccelStepper.h>
#include <TMCStepper.h>


#define TMCSerial Serial1 

// Objets pour la génération d'impulsions matérielles
FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepperL = NULL;
FastAccelStepper *stepperR = NULL;

// Objets pour la configuration UART (TMC)
TMC2209Stepper driverL(&TMCSerial, R_SENSE, 0); 
TMC2209Stepper driverR(&TMCSerial, R_SENSE, 1); 

// Paramètres physiques
stepper_parameters_t stepper_param = {30000.0, 10.0, 200.0, 200.0 , STEP_MODE_128};

void begin_steppers() {
    // 1. Initialisation UART (indispensable pour le courant/microsteps)
    TMCSerial.begin(115200, SERIAL_8N1, 6, 5);
    delay(500);

    driverL.begin();
    driverL.rms_current(800); 
    driverL.microsteps(128);

    driverR.begin();
    driverR.rms_current(800);
    driverR.microsteps(128);

    enable_steppers(); // Active les moteurs après configuration

    // 2. Initialisation du moteur de mouvement matériel
    engine.init();
    stepperL = engine.stepperConnectToPin(STEP_L);
    stepperR = engine.stepperConnectToPin(STEP_R);

    if (stepperL && stepperR) {
        // Configuration des pins DIR
        stepperL->setDirectionPin(DIR_L);
        stepperR->setDirectionPin(DIR_R);

        // Application des rampes (en Pas/s et Pas/s²)
        stepperL->setAcceleration(stepper_param.acceleration * stepper_param.step_mode);
        stepperR->setAcceleration(stepper_param.acceleration * stepper_param.step_mode);
        
        stepperL->setSpeedInHz(stepper_param.max_speed * stepper_param.step_mode);
        stepperR->setSpeedInHz(stepper_param.max_speed * stepper_param.step_mode);
    }

    if (driverL.test_connection() == 0 && driverR.test_connection() == 0) {
        Serial.println("TMC2209 connected successfully");
    }
    else {
        Serial.println("Failed to connect to TMC2209");
    }
}


int is_stepper_running(){
    return stepperL->isRunning() || stepperR->isRunning();
}

void stepper_move(uint64_t steps, int dirL, int dirR){
    if(dirL == 1) stepperL->move(steps);
    else stepperL->move(-steps);

    if(dirR == 1) stepperR->move(steps);
    else stepperR->move(-steps);
}

int stop_stepper(){
    stepperL->stopMove();
    stepperR->stopMove();
    return 0;
}

step_mode_t get_step_mode(){
    return stepper_param.step_mode;
}

int32_t get_pose_left(){
    // Récupère la position actuelle du moteur gauche
    return stepperL->getCurrentPosition();
}

int32_t get_pose_right(){
    // Récupère la position actuelle du moteur droit
    return stepperR->getCurrentPosition();
}

void disable_steppers(){
    driverL.toff(0); // Désactive les moteurs
    driverR.toff(0);
}
void enable_steppers(){
    driverL.toff(4); // Réactive les moteurs avec la configuration précédente
    driverR.toff(4);
}

void set_direction(uint8_t dirL, uint8_t dirR) {
    // Inutile avec moveTo(), mais conservé si tu utilises moveRelative
}

void set_speed(float percentage) {
    uint32_t speed = stepper_param.max_speed * percentage;
    if (speed < stepper_param.min_speed) speed = stepper_param.min_speed;
    stepperL->setSpeedInHz(speed);
    stepperR->setSpeedInHz(speed);
}

// Remplacement de move_task par une commande asynchrone
void prepare_movement(long stepsL, long stepsR) {
    stepperL->move(stepsL);
    stepperR->move(stepsR);
}

void moveTaskcode(void *parameters) {
    int *robot_stop_ptr = (int *)parameters;
    
    for (;;) {
        if (*robot_stop_ptr) {
            Serial.println("Emergency stop triggered! Stopping motors...");
            // 1. Sauvegarder les positions cibles actuelles avant d'arrêter
            int32_t targetL = stepperL->getPositionAfterCommandsCompleted();
            int32_t targetR = stepperR->getPositionAfterCommandsCompleted();

            // 2. Arrêt d'urgence
            stepperL->stopMove();
            stepperR->stopMove();
            
            // 3. Attendre que l'obstacle soit parti
            while(*robot_stop_ptr) {
                vTaskDelay(pdMS_TO_TICKS(100));
            }

            Serial.println("Resuming movement to target positions...");
            // 4. REPRISE : Renvoyer les moteurs vers la cible initiale
            stepperL->moveTo(targetL);
            stepperR->moveTo(targetR);
        }
        
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}