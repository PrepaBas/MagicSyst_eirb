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

bool robot_stop = false; // Flag d'arrêt d'urgence partagé entre les tâches

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

void disable_steppers(){
    driverL.toff(0); // Désactive les moteurs
    driverR.toff(0);
}
void enable_steppers(){
    driverL.toff(4); 
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
            robot_stop = true; 
            Serial.println("Emergency stop triggered! Stopping motors...");
            // 1. Sauvegarder les positions cibles actuelles avant d'arrêter
            int32_t targetL = stepperL->getPositionAfterCommandsCompleted();
            int32_t targetR = stepperR->getPositionAfterCommandsCompleted();

            // 2. Arrêt d'urgence
            stepperL->stopMove();
            stepperR->stopMove();
            
            while(stepperL->isRunning() || stepperR->isRunning()) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            // 3. Attendre que l'obstacle soit parti
            while(*robot_stop_ptr) {
                vTaskDelay(pdMS_TO_TICKS(100));
            }

            Serial.println("Resuming movement to target positions...");
            // 4. REPRISE : Renvoyer les moteurs vers la cible initiale
            stepperL->moveTo(targetL);
            stepperR->moveTo(targetR);

            robot_stop = false; 
        }
        
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

