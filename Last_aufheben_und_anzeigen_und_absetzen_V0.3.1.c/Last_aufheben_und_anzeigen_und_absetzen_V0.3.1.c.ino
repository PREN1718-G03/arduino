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
#include <DueTimer.h>


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
#define ZERO_STEP 100000
#define DRIVE_TRANSMISSION 75 // Miktroschritte pro mm
#define LIFT_TRANSMISSION 60  // Miktroschritte pro mm

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
long driveStepCounter = -ZERO_STEP-2000;
long liftStepCounter = 0;
float xAxis;
float zAxis;

//Funktions Prototypen erstellen
//void stepWhileDelay(int, int, int);
void rampeMotor(int, int, int);
void printLCD(int, int);
void stepMotor(int);
void timerHandler(void);
void calcAxis(void);

void setup() {

  // Initialisierung des LCD-Display
  Serial.begin(115200);
  Serial.println("LCD...");

  while (! Serial);

  Serial.println("Dose: check for LCD");

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Timer3.attachInterrupt(timerHandler);
  Timer3.start(1000000);   //alle 500ms aufrufen
  lcd.setBacklight(255);
  lcd.begin(16, 2); // initialize the lcd
  lcd.setCursor(0, 0);  //Set LCD cursor to upper left corner, column 0, row 0
  lcd.print("x-Achse");  //Print Message on First Row
  lcd.setCursor(0, 1);  //Set cursor to first column of second row
  lcd.print("z-Achse"); //Print blanks to clear the row
  Serial.println("lcd wurde initialisiert");
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
  //printLCD(driveStepCounter/6000,liftStepCounter/6000);
  Serial.println("lcd wurde ausgegeben");
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
  //printLCD(driveStepCounter/6000,liftStepCounter/6000);
  delay(4000);
  hasSwitchedFlag = 0;
  rampeMotor(STEP_DRIVE, 500, DRIVE_DELAY);
  for (long zaehler = 0; zaehler <= ZERO_STEP; zaehler++) { // 100'000 Schritte in X-Richtung
    stepMotor(STEP_DRIVE, DRIVE_DELAY);
  }
  rampeMotor(STEP_DRIVE, DRIVE_DELAY, 500);
  //printLCD(driveStepCounter/6000,liftStepCounter/6000);
  digitalWrite(DIR_LIFTMOT, LOW);                   // Richtung in Z-Achse
  rampeMotor(STEP_LIFTMOT, 500, LIFT_DELAY);
  for (int zaehler2 = 0; zaehler2 < l_stepLiftMot; zaehler2++) {
    stepMotor(STEP_LIFTMOT, LIFT_DELAY);
  }
  rampeMotor(STEP_LIFTMOT, 40, 500);
  rampeMotor(STEP_DRIVE, 500, 35);

  for(int zaehler2 = 0; zaehler2 < 90000; zaehler2++){ //Einige Schritte nach vorne fahren
    stepMotor(STEP_DRIVE, DRIVE_DELAY);
  }
  //printLCD(driveStepCounter/6000,liftStepCounter/6000);
  rampeMotor(STEP_DRIVE, 35, 500);
  TOGGLE(DIR_LIFTMOT);
  rampeMotor(STEP_LIFTMOT, 500, LIFT_DELAY);
  l_stepLiftMot += 4000;
  for(int zaehler2 = 0; zaehler2 < l_stepLiftMot; zaehler2++){ // Last auf Boden fahren
    stepMotor(STEP_LIFTMOT, LIFT_DELAY);
  }
  //printLCD(driveStepCounter/6000,liftStepCounter/6000);
  rampeMotor(STEP_LIFTMOT, LIFT_DELAY, 500);
  
  TOGGLE(DIR_LIFTMOT);

  rampeMotor(STEP_LIFTMOT, 500, LIFT_DELAY);
  for(int zaehler2 = 0; zaehler2 < l_stepLiftMot; zaehler2++){    //Hacken anheben
    stepMotor(STEP_LIFTMOT, LIFT_DELAY);
  }
  //printLCD(driveStepCounter/6000,liftStepCounter/6000);
  rampeMotor(STEP_LIFTMOT, LIFT_DELAY, 500);
  
  myCounter = 0;
  rampeMotor(STEP_DRIVE, 500, DRIVE_DELAY);  
  while (digitalRead(END_SWITCH)) {       //an Endpfosten fahren
    stepMotor(STEP_DRIVE, DRIVE_DELAY);
    if (myCounter >= 12000) {
      myCounter = 0;
      //printLCD(driveStepCounter/6000,liftStepCounter/6000);
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
      stepMotor(pin, startdelay);
    }
    else if (zaehler < 250) {
      stepMotor(pin, startdelay - deltaDelay);
    }
    else if (zaehler < 330) {
      stepMotor(pin, startdelay - 2 * deltaDelay);
    }
    else if (zaehler < 400) {
      stepMotor(pin, startdelay - 3 * deltaDelay);
    }
    else if (zaehler < 460) {
      stepMotor(pin, startdelay - 4 * deltaDelay);
    }
    else if (zaehler < 520) {
      stepMotor(pin, startdelay - 5 * deltaDelay);
    }
    else if (zaehler < 590) {
      stepMotor(pin, startdelay - 6 * deltaDelay);
    }
    else if (zaehler < 670) {
      stepMotor(pin, startdelay - 7 * deltaDelay);
    }
    else if (zaehler < 760) {
      stepMotor(pin, startdelay - 8 * deltaDelay);
    }
    else if (zaehler < 860) {
      stepMotor(pin, startdelay - 9 * deltaDelay);
    }
    else {
      stepMotor(pin, startdelay - 10 * deltaDelay);
    }
  }
}

void stepMotor(int stepPin, int delayTime){
      delayMicroseconds(delayTime);
      TOGGLE(stepPin);
      delayMicroseconds(delayTime);
      TOGGLE(stepPin);
      if(stepPin == STEP_DRIVE){
        if(digitalRead(stepPin-1)){
          driveStepCounter--;
        }else{
          driveStepCounter++;
        }
      }else{
        if(digitalRead(stepPin-1)){
          liftStepCounter--;
        }else{
          liftStepCounter++;
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

void printLCD(float xAxis, float zAxis) {
  lcd.setCursor(8, 1);   //Set Cursor again to eigth column of second row
  lcd.print(zAxis, 1); //Print measured distance
  lcd.print("cm      ");  //Print your units.
  lcd.setCursor(8, 0);   //Set Cursor again to eigth column of second row
  lcd.print(xAxis, 1); //Print measured distance
  lcd.print("cm      ");  //Print your units.
}

void calcAxis(){
  xAxis = (float)driveStepCounter/(10 * DRIVE_TRANSMISSION);  //Schritte in cm umwandeln
  zAxis = (float)liftStepCounter/(10 * LIFT_TRANSMISSION);    //Schritte in cm umwandeln
}

void timerHandler(){
  calcAxis();
  printLCD(xAxis, zAxis);
}

