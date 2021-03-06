/**
  Projekt : PREN 2 Gruppe 3
  Titel : Last Aufheben V 0.1.1
  Datum : 16.03.18
  Autor : Jan Odermatt
  Beschreibung :
    Dieses Programm bewegt die Laufkatze bis zur Last, bewegt den Hacken zur Last,
    hebt diese auf und fährt bis zum Endpfosten
**/

#include <Arduino.h>
#include <DueTimer.h>

#define DIR_DRIVE 3
#define STEP_DRIVE 4
#define DIR_LIFTMOT 5
#define STEP_LIFTMOT 6
#define START_SWITCH 9
#define END_SWITCH 10
#define STARTSCHLAUFE 13

#define TOGGLE(pin) do{(digitalRead(pin)!= LOW)? digitalWrite(pin, LOW): digitalWrite(pin,HIGH);}while(0)

/*Initialisierung*/
int q = 0;
int l_stepLiftMot = 35000;
short switch_state, hasSwitchedFlag;

/*Funktions Prototypen erstellen*/
void rampeMotor(int, int, int);

void setup() {
  /*pinMode(DIR_DRIVE, OUTPUT);
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
    hasSwitchedFlag = 0;*/
  Serial.begin(9600);
  Timer1.attachInterrupt(handler).setPeriod(200).start();
}

/*Main Programm*/
void loop() {
  delayMicroseconds(20);
  q++;
  /*while((!hasSwitchedFlag) || !(digitalRead(END_SWITCH))){                          // warte bis Startswitch umgeschaltet wird während endschalter nicht betätigt
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
    rampeMotor(STEP_DRIVE, 500, 40);
    for(long zaehler = 0;zaehler <= 78000;zaehler++){ // 70'000 Schritte in X-Richtung
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(40);
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(40);
    }
    rampeMotor(STEP_DRIVE, 40, 500);

    digitalWrite(DIR_LIFTMOT, LOW);                   // Richtung in Z-Achse

    rampeMotor(STEP_LIFTMOT, 500, 15);
    for(int zaehler2 = 0; zaehler2 < (l_stepLiftMot-2000); zaehler2++){
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(15);
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(15);
    }
    rampeMotor(STEP_LIFTMOT, 15, 500);
    rampeMotor(STEP_DRIVE, 500, 40);
    while(digitalRead(END_SWITCH)){
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(40);
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(40);
    }*/
}

void handler(void) {
  Timer1.stop();
  Serial.print("\n" + q);
  delayMicroseconds(2000);
  //Timer1.start();
}

/*
  Rampe, welche während 1000 Schritten ansteigt oder sinkt
*/
void rampeMotor(int pin, int startdelay, int enddelay) {
  int isSpeedUp = startdelay - enddelay;
  int deltaDelay = isSpeedUp / 10;
  for (int zaehler = 0; zaehler < 1000; zaehler++) {
    if (zaehler < 160) {
      delayMicroseconds(startdelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay);
      TOGGLE(pin);
    } else if (zaehler < 250) {
      delayMicroseconds(startdelay - deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - deltaDelay);
      TOGGLE(pin);
    } else if (zaehler < 330) {
      delayMicroseconds(startdelay - 2 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 2 * deltaDelay);
      TOGGLE(pin);
    } else if (zaehler < 400) {
      delayMicroseconds(startdelay - 3 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 3 * deltaDelay);
      TOGGLE(pin);
    } else if (zaehler < 460) {
      delayMicroseconds(startdelay - 4 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 4 * deltaDelay);
      TOGGLE(pin);
    } else if (zaehler < 520) {
      delayMicroseconds(startdelay - 5 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 5 * deltaDelay);
      TOGGLE(pin);
    } else if (zaehler < 590) {
      delayMicroseconds(startdelay - 6 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 6 * deltaDelay);
      TOGGLE(pin);
    } else if (zaehler < 670) {
      delayMicroseconds(startdelay - 7 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 7 * deltaDelay);
      TOGGLE(pin);
    } else if (zaehler < 760) {
      delayMicroseconds(startdelay - 8 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 8 * deltaDelay);
      TOGGLE(pin);
    } else if (zaehler < 860) {
      delayMicroseconds(startdelay - 9 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 9 * deltaDelay);
      TOGGLE(pin);
    } else {
      delayMicroseconds(startdelay - 10 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 10 * deltaDelay);
      TOGGLE(pin);
    }
  }
}
