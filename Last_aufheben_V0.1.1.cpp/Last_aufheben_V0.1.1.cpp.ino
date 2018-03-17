/**
* Projekt : PREN 2 Gruppe 3
* Titel : Last Aufheben V 0.1.1
* Datum : 16.03.18
* Autor : Jan Odermatt
* Beschreibung :
*   Dieses Programm bewegt die Laufkatze bis zur Last, bewegt den Hacken zur Last, 
*   hebt diese auf und fährt bis zum Endpfosten
**/

#include <Arduino.h>

#define DIR_DRIVE 3
#define STEP_DRIVE 4
#define DIR_LIFTMOT 5
#define STEP_LIFTMOT 6
#define START_SWITCH 7
#define END_SWITCH 8

/*Initialisierung*/
void setup() {
  pinMode(DIR_DRIVE, OUTPUT);
  digitalWrite(DIR_DRIVE, LOW);
  pinMode(STEP_DRIVE, OUTPUT);
  pinMode(DIR_LIFTMOT, OUTPUT);
  digitalWrite(DIR_LIFTMOT, LOW);
  pinMode(STEP_LIFTMOT, OUTPUT);
  pinMode(START_SWITCH, INPUT);
  pinMode(END_SWITCH, INPUT);
}

/*Main Programm*/
void loop() {
  long l_stepLiftMot = 0;
  while(!digitalRead(START_SWITCH));
  for(long zaehler = 0;zaehler <= 30000;zaehler++){
    if(l_stepLiftMot <= 30000){
      digitalWrite(STEP_LIFTMOT, HIGH);
    }
    digitalWrite(STEP_DRIVE, HIGH);
    delayMicroseconds(100);
    if(l_stepLiftMot <= 30000){
      digitalWrite(STEP_LIFTMOT, LOW);
      l_stepLiftMot++;
    }
    digitalWrite(STEP_DRIVE, LOW);
    delayMicroseconds(100);
  }

  digitalWrite(DIR_LIFTMOT, HIGH);

  while(!digitalRead(END_SWITCH)){
    if(l_stepLiftMot >= 0){
      digitalWrite(STEP_LIFTMOT, HIGH);
    }
    digitalWrite(STEP_DRIVE, HIGH);
    delayMicroseconds(100);
    if(l_stepLiftMot >= 0){
      digitalWrite(STEP_LIFTMOT, LOW);
      l_stepLiftMot--;
    }
    digitalWrite(STEP_DRIVE, LOW);
    delayMicroseconds(100);
  }
}
