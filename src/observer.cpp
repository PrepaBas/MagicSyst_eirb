#include <ESP32Servo.h>
#include <Arduino.h>
#include "NewPing.h"
#include "observer.h"
#include "advanced_movement.h"
#include "Wire.h"
#include "VL53L0X.h"

// #define PIN 21
#define SPEED_OF_SOUND 340
#define STEPS_TO_STOP 150

#define VL53L0X_SDA_PIN 8
#define VL53L0X_SCL_PIN 9

extern position position;


VL53L0X f_sensor;
 
uint32_t stop_robot(int *stop_ptr)
{
  *stop_ptr = 1; // update status
  uint32_t cut_steps = 0; 
  if(remaining_steps > STEPS_TO_STOP){ // allow for slow stop
    cut_steps = remaining_steps - STEPS_TO_STOP;
    remaining_steps = STEPS_TO_STOP;
  }
  return cut_steps;
}

void resume_robot(int* stop_ptr, uint32_t cut_steps){
  remaining_steps += cut_steps;
  *stop_ptr = 0;
  vTaskDelay(pdMS_TO_TICKS(1));
}

#define MAX_D 200
#define FRONT_EMPTY_STOP_DISTANCE 28
#define FRONT_LOADED_STOP_DISTANCE 34
void securityTaskcode(void *parameters)
{
  Wire.begin(VL53L0X_SDA_PIN, VL53L0X_SCL_PIN);
  int *robot_stop_ptr = (int *)parameters;

  if (!f_sensor.init()) {
    Serial.println(F("Failed to boot VL53L0X"));
  }
  Serial.println("VL53L0X booted");
  f_sensor.startContinuous(50); // 100ms pour plus de réactivité

  status_t status = NO_STOP;
  unsigned long stop_timer;

  for (;;)
  {
    bool detect = false;
    uint16_t f_distance = f_sensor.readRangeContinuousMillimeters();
    Serial.print("Distance: ");
    Serial.println(f_distance);

    if (f_sensor.timeoutOccurred() || f_distance < 150) { // Seuil à 150mm pour sécurité
      detect = true;
    }

    switch(status){
      case NO_STOP:
        if(detect){
          Serial.println("Obstacle ! Arrêt du robot.");
          *robot_stop_ptr = 1;
          // Freinage matériel immédiat
          // stop_stepper();
          
          stop_timer = millis();
          status = YES_STOP;
        }
        break;

      case YES_STOP:
        if(detect){
          stop_timer = millis(); // On reset le timer tant qu'on détecte
        }
        else if(millis() - stop_timer > 1000){ // Attendre 1s après disparition
          Serial.println("Route libre, reprise...");
          *robot_stop_ptr = 0;
          status = NO_STOP;
          
          // Note : Avec FastAccelStepper, si le robot avait encore des pas à faire
          // il faudra relancer la commande de mouvement ici si stopMove() a effacé la cible.
        }
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(20)); // On scanne plus souvent (20Hz)
  }
}
