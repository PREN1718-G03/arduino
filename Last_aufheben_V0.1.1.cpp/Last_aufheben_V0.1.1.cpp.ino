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
#define START_SWITCH 9 
#define END_SWITCH 10
#define STARTSCHLAUFE 13

/*Initialisierung*/
long l_stepLiftMot = 6000;
short switch_state, hasSwitchedFlag;

void setup() {
  pinMode(DIR_DRIVE, OUTPUT);
  digitalWrite(DIR_DRIVE, LOW);
  pinMode(STEP_DRIVE, OUTPUT);
  pinMode(DIR_LIFTMOT, OUTPUT);
  digitalWrite(DIR_LIFTMOT, LOW);
  pinMode(STEP_LIFTMOT, OUTPUT);
  pinMode(START_SWITCH, INPUT_PULLUP);
  pinMode(END_SWITCH, INPUT_PULLUP);
  digitalWrite(START_SWITCH, HIGH);       // turn on pullup resistors
  digitalWrite(END_SWITCH, HIGH);       // turn on pullup resistors
  pinMode(STARTSCHLAUFE, OUTPUT);
  switch_state = digitalRead(START_SWITCH);
  hasSwitchedFlag = 0;
}

/*Main Programm*/
void loop() {
  while((!hasSwitchedFlag) || !(digitalRead(END_SWITCH))){                          // warte bis Startswitch umgeschaltet wird während endschalter nicht betätigt
    if(!digitalRead(END_SWITCH)){
      switch_state = digitalRead(START_SWITCH);
    }
    else{
      if(digitalRead(START_SWITCH) != switch_state){
        hasSwitchedFlag = 1;
        switch_state = digitalRead(START_SWITCH);
      }
    }
  }
  hasSwitchedFlag = 0;
  for(long zaehler = 0;zaehler <= 7000;zaehler++){ // 70'000 Schritte in X-Richtung
    digitalWrite(STEP_DRIVE, HIGH);
    delayMicroseconds(40);
    digitalWrite(STEP_DRIVE, LOW);
    delayMicroseconds(40);
  }

  digitalWrite(DIR_LIFTMOT, HIGH);                   // Richtung in Z-Achse

  while(digitalRead(END_SWITCH)){
    if(l_stepLiftMot >= 0){
      digitalWrite(STEP_LIFTMOT, HIGH);
    }
    digitalWrite(STEP_DRIVE, HIGH);
    delayMicroseconds(15);
    if(l_stepLiftMot >= 0){
      digitalWrite(STEP_LIFTMOT, LOW);
      l_stepLiftMot--;
    }
    digitalWrite(STEP_DRIVE, LOW);
    delayMicroseconds(15);
  }
}
