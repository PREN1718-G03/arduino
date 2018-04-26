/**
  Projekt : PREN 2 Gruppe 3
  Titel : Last Aufheben V 0.2.0
  Datum : 16.03.18
  Autor : Jan Odermatt
  Beschreibung :
    Dieses Programm bewegt die Laufkatze bis zur Last, bewegt den Hacken zur Last,
    hebt diese auf und fährt bis zum Endpfosten
	Während dessen zeigt das LCD-Display ständig die Position der Last an.
**/
#include <Arduino.h>
#include <LiquidCrystal.h>

// Definition aller Pins

// Motor Pins definieren
#define DIR_DRIVE 3
#define STEP_DRIVE 4
#define DIR_LIFTMOT 5
#define STEP_LIFTMOT 6
#define START_SWITCH 9
#define END_SWITCH 10
#define STARTSCHLAUFE 13

// Makros definieren
#define TOGGLE(x); digitalWrite(x, digitalRead(x) ? LOW : HIGH)

// Initialisierung
LiquidCrystal LCD(48, 49, 50, 51, 52, 53);	// Kreiere LCD Objekt
int l_stepLiftMot = 33000;
short switch_state;
short hasSwitchedFlag = 0;
int myCounter = 0;
int isMeasuringDistance = 0;
float pingTimeX;  //time for ping to travel from sensor to target and return for x-Axis
float pingTimeY;  //time for ping to travel from sensor to target and return for y-Axis
float targetDistanceX; //Distance to Target in centimeters
float targetDistanceY; //Distance to Target in centimeters


//Funktions Prototypen erstellen
void rampeMotor(int, int, int);
void printLCD(void);

void setup() {

  // Initialisierung des LCD-Displays
  LCD.begin(16, 2); //Tell Arduino to start your 16 column 2 row LCD
  LCD.setCursor(0, 0);  //Set LCD cursor to upper left corner, column 0, row 0
  LCD.print("z-Achse");  //Print Message on First Row
  LCD.setCursor(0, 1);  //Set cursor to first column of second row
  LCD.print("x-Achse"); //Print blanks to clear the row

  // Initialisiere Motor Pins
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
  Serial.print("\nInitialisierungen wurden ausgeführt.");
}

/*Main Programm*/
void loop() {

  Serial.print("\nloop wurde gestartet");
  while ((!hasSwitchedFlag) || !(digitalRead(END_SWITCH))) {                          // warte bis Startswitch umgeschaltet wird während endschalter nicht betätigt
    if (!digitalRead(END_SWITCH)) {
      switch_state = digitalRead(START_SWITCH);
    }
    else {
      if (digitalRead(START_SWITCH) != switch_state) {
        hasSwitchedFlag = 1;
        switch_state = digitalRead(START_SWITCH);
      }
    }
  }
  delay(4000);
  hasSwitchedFlag = 0;
  rampeMotor(STEP_DRIVE, 500, 40);
  for (long zaehler = 0; zaehler <= 78000; zaehler++) { // 70'000 Schritte in X-Richtung
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(40);
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(40);
  }
  rampeMotor(STEP_DRIVE, 40, 500);

  digitalWrite(DIR_LIFTMOT, LOW);                   // Richtung in Z-Achse
  rampeMotor(STEP_LIFTMOT, 500, 15);
  for (int zaehler2 = 0; zaehler2 < l_stepLiftMot; zaehler2++) {
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(15);
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(15);
  }
  rampeMotor(STEP_LIFTMOT, 15, 500);
  rampeMotor(STEP_DRIVE, 500, 40);

  for(int zaehler2 = 0; zaehler2 < 60000; zaehler2++){ //Einige Schritte nach vorne fahren
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(40);
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(40);
  }
  rampeMotor(STEP_DRIVE, 40, 500);
  TOGGLE(DIR_LIFTMOT);
  rampeMotor(STEP_LIFTMOT, 500, 15);
  l_stepLiftMot += 9000;
  for(int zaehler2 = 0; zaehler2 < l_stepLiftMot; zaehler2++){ // Last auf Boden fahren
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(15);
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(15);
  }
  rampeMotor(STEP_LIFTMOT, 15, 500);
  
  TOGGLE(DIR_LIFTMOT);

  rampeMotor(STEP_LIFTMOT, 500, 15);
  for(int zaehler2 = 0; zaehler2 < l_stepLiftMot; zaehler2++){    //Hacken anheben
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(15);
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(15);
  }
  rampeMotor(STEP_LIFTMOT, 15, 500);
  
  myCounter = 0;
  rampeMotor(STEP_DRIVE, 500, 40);  
  while (digitalRead(END_SWITCH)) {       //an Endpfosten fahren
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(40);
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(40);
    if (myCounter >= 12000) {
      myCounter = 0;
      printLCD();
    } else {
      myCounter++;
    }
  }
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
    }
    else if (zaehler < 250) {
      delayMicroseconds(startdelay - deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - deltaDelay);
      TOGGLE(pin);
    }
    else if (zaehler < 330) {
      delayMicroseconds(startdelay - 2 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 2 * deltaDelay);
      TOGGLE(pin);
    }
    else if (zaehler < 400) {
      delayMicroseconds(startdelay - 3 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 3 * deltaDelay);
      TOGGLE(pin);
    }
    else if (zaehler < 460) {
      delayMicroseconds(startdelay - 4 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 4 * deltaDelay);
      TOGGLE(pin);
    }
    else if (zaehler < 520) {
      delayMicroseconds(startdelay - 5 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 5 * deltaDelay);
      TOGGLE(pin);
    }
    else if (zaehler < 590) {
      delayMicroseconds(startdelay - 6 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 6 * deltaDelay);
      TOGGLE(pin);
    }
    else if (zaehler < 670) {
      delayMicroseconds(startdelay - 7 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 7 * deltaDelay);
      TOGGLE(pin);
    }
    else if (zaehler < 760) {
      delayMicroseconds(startdelay - 8 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 8 * deltaDelay);
      TOGGLE(pin);
    }
    else if (zaehler < 860) {
      delayMicroseconds(startdelay - 9 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 9 * deltaDelay);
      TOGGLE(pin);
    }
    else {
      delayMicroseconds(startdelay - 10 * deltaDelay);
      TOGGLE(pin);
      delayMicroseconds(startdelay - 10 * deltaDelay);
      TOGGLE(pin);
    }
  }
}

void printLCD(void) {
  LCD.setCursor(8, 1);   //Set Cursor again to eigth column of second row
  LCD.print(2000, 1); //Print measured distance
  LCD.print("cm      ");  //Print your units.
  LCD.setCursor(8, 0);   //Set Cursor again to eigth column of second row
  LCD.print(2000, 1); //Print measured distance
  LCD.print("cm      ");  //Print your units.
  delayMicroseconds(1000);
}
