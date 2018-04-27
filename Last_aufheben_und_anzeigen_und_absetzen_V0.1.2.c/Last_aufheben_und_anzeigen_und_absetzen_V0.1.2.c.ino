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
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>


// Definition aller Pins

// Motor Pins definieren
#define DIR_DRIVE 3
#define STEP_DRIVE 4
#define DIR_LIFTMOT 5
#define STEP_LIFTMOT 6
#define START_SWITCH 9
#define END_SWITCH 10
#define STARTSCHLAUFE 13
#define LIFT_DELAY 40
#define DRIVE_DELAY 35

// Makros definieren
#define TOGGLE(x); digitalWrite(x, digitalRead(x) ? LOW : HIGH)

// Initialisierung
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display
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
//void stepWhileDelay(int, int, int);
void rampeMotor(int, int, int);
void printLCD(int, int);

void setup() {

  // Initialisierung des LCD-Display
  Serial.begin(115200);
  Serial.println("LCD...");

  while (! Serial);

  Serial.println("Dose: check for LCD");

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  lcd.setBacklight(255);
  lcd.begin(16, 2); // initialize the lcd
  lcd.setCursor(0, 0);  //Set LCD cursor to upper left corner, column 0, row 0
  lcd.print("z-Achse");  //Print Message on First Row
  lcd.setCursor(0, 1);  //Set cursor to first column of second row
  lcd.print("x-Achse"); //Print blanks to clear the row

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
  printLCD(1100,1010);
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
  printLCD(2022,2020);
  delay(4000);
  hasSwitchedFlag = 0;
  rampeMotor(STEP_DRIVE, 500, DRIVE_DELAY);
  for (long zaehler = 0; zaehler <= 100000; zaehler++) { // 100'000 Schritte in X-Richtung
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(DRIVE_DELAY);
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(DRIVE_DELAY);
  }
  rampeMotor(STEP_DRIVE, DRIVE_DELAY, 500);

  digitalWrite(DIR_LIFTMOT, LOW);                   // Richtung in Z-Achse
  rampeMotor(STEP_LIFTMOT, 500, LIFT_DELAY);
  for (int zaehler2 = 0; zaehler2 < l_stepLiftMot; zaehler2++) {
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(LIFT_DELAY);
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(LIFT_DELAY);
  }
  rampeMotor(STEP_LIFTMOT, 40, 500);
  rampeMotor(STEP_DRIVE, 500, 35);

  for(int zaehler2 = 0; zaehler2 < 90000; zaehler2++){ //Einige Schritte nach vorne fahren
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(35);
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(35);
  }
  rampeMotor(STEP_DRIVE, 35, 500);
  TOGGLE(DIR_LIFTMOT);
  rampeMotor(STEP_LIFTMOT, 500, LIFT_DELAY);
  l_stepLiftMot += 4000;
  for(int zaehler2 = 0; zaehler2 < l_stepLiftMot; zaehler2++){ // Last auf Boden fahren
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(LIFT_DELAY);
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(LIFT_DELAY);
  }
  rampeMotor(STEP_LIFTMOT, LIFT_DELAY, 500);
  
  TOGGLE(DIR_LIFTMOT);

  rampeMotor(STEP_LIFTMOT, 500, LIFT_DELAY);
  for(int zaehler2 = 0; zaehler2 < l_stepLiftMot; zaehler2++){    //Hacken anheben
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(LIFT_DELAY);
    TOGGLE(STEP_LIFTMOT);
    delayMicroseconds(LIFT_DELAY);
  }
  rampeMotor(STEP_LIFTMOT, LIFT_DELAY, 500);
  
  myCounter = 0;
  rampeMotor(STEP_DRIVE, 500, DRIVE_DELAY);  
  while (digitalRead(END_SWITCH)) {       //an Endpfosten fahren
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(DRIVE_DELAY);
    TOGGLE(STEP_DRIVE);
    delayMicroseconds(DRIVE_DELAY);
    if (myCounter >= 12000) {
      myCounter = 0;
      printLCD(2000,2000);
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

/*stepWhileDelay(int stepPin, int delayMicrosecs, int motorDelayMicroseconds){
  int counter = delayMicrosecs / motorDelayMicroseconds;
  for(int z = 0; z < counter; z++){
    delayMicroseconds(motorDelayMicroseconds);
    TOGGLE(stepPin);
  }
}*/

void printLCD(int xAxis, int zAxis) {
  lcd.setCursor(8, 1);   //Set Cursor again to eigth column of second row
  lcd.print(xAxis, 1); //Print measured distance
  lcd.print("cm      ");  //Print your units.
  lcd.setCursor(8, 0);   //Set Cursor again to eigth column of second row
  lcd.print(zAxis, 1); //Print measured distance
  lcd.print("cm      ");  //Print your units.
}
