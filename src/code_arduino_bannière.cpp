#include <Arduino.h>
#include <Servo.h>

// Création de l'objet Servo
Servo gauche;
Servo droite;


int eq_g = 80; //+ sens déploiment
int eq_d = 80; //- sens deploiment
void setup() {
  // Initialisation du servomoteur
  gauche.attach(22);
  droite.attach(26);
  gauche.write(eq_g);
  droite.write(eq_d);
  delay(1000);
  for(int i =  0;i<65;i++){
    gauche.write(eq_g + i);
    droite.write(eq_d - i);
    delay(7);
  }

  //position des servomoteurs une fois la bannière déployée à mettre au point
}


void loop(){
  
  /*delay(7000);
  gauche.write(eq_g);
  droite.write(eq_d);
  delay(1200);*/
}
