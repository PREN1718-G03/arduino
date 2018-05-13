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
#include "C:\Users\JanB\Documents\GitHub\DuePWM\DuePWM.h"
#include "C:\Users\JanB\Documents\GitHub\DuePWM\DuePWM.cpp"


// Definition aller Pins

// Motor Pins definieren
#define DIR_DRIVE 3
#define STEP_DRIVE 7
#define DIR_LIFTMOT 5
#define STEP_LIFTMOT 6
#define START_SWITCH 9
#define END_SWITCH 10
#define STARTSCHLAUFE 13
#define LIFT_FREQ 4000
#define DRIVE_FREQ 5000
#define ZERO_STEP 100000
#define DRIVE_TRANSMISSION (72.3825877 / 16)//2.3579 // Vollschritte pro mm
#define LIFT_TRANSMISSION  (52.5086788 / 16)//102.85946673 // Vollschritte pro mm

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
long driveStepCounter = 0;
long liftStepCounter = 0;
double xAxis;
double zAxis;
double oldxAxis=45;
double oldzAxis=45;
double xMax=0;
double zMax = 0;
DuePWM pwm( LIFT_FREQ, DRIVE_FREQ );

//Funktions Prototypen erstellen
//void stepWhileDelay(int, int, int);
void rampeMotor(int, int, int);
void printLCD(int, int);
void stepMotor(int);
void timer3Handler(void);
void timer2Handler(void);
void timer1Handler(void);
void calcAxis(void);

void setup() {

  // Initialisierung des LCD-Display
  Serial.begin(115200);
  Serial.println("LCD...");

  while (! Serial);

  Serial.println("Dose: check for LCD");

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Timer3.attachInterrupt(timer3Handler);
  Timer3.start(1000000);   //jede Sekunde aufrufen
  pwm.setFreq1(DRIVE_FREQ);
  pwm.setFreq2(LIFT_FREQ);
  lcd.setBacklight(255);
  lcd.begin(16, 2); // initialize the lcd
  lcd.setCursor(0, 0);  //Set LCD cursor to upper left corner, column 0, row 0
  lcd.print("x-Achse");  //Print Message on First Row
  lcd.setCursor(0, 1);  //Set cursor to first column of second row
  lcd.print("z-Achse"); //Print blanks to clear the row
  Serial.println("lcd wurde initialisiert");
  // Initialisiere Motor Pins
  pinMode(DIR_DRIVE, OUTPUT);
  digitalWrite(DIR_DRIVE, HIGH);
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
  /*startMotor(true, DRIVE_DELAY);
  while(1){
    if(!digitalRead(END_SWITCH)){
      stopMotor(true);
    }
  }*/
  pwm.pinFreq2(STEP_DRIVE);
  pwm.pinDuty(STEP_DRIVE, 127);
  delay(5000);
  pwm.stop(STEP_DRIVE);
  delay(1000);
  Serial.println("stuck in loop");
}

void startMotor(int isDrive, int delayTime){
      if(isDrive){   
        pwm.pinFreq1(STEP_DRIVE);
        pwm.pinDuty(STEP_DRIVE, 127);
      }else{
        pwm.pinFreq2(STEP_LIFTMOT);
        pwm.pinDuty(STEP_LIFTMOT, 127);
      }
}

void stopMotor(int isDrive){
      if(isDrive){
        pwm.stop(STEP_DRIVE);
      }else{
        pwm.stop(STEP_LIFTMOT);
      }
}

/*stepWhileDelay(int stepPin, int delayMicrosecs, int motorDelayMicroseconds){
  int counter = delayMicrosecs / motorDelayMicroseconds;
  for(int z = 0; z < counter; z++){
    delayMicroseconds(motorDelayMicroseconds);
    TOGGLE(stepPin);
  }
}*/

void printLCD(double xAxis, double zAxis) {
  if(oldxAxis != xAxis){
    lcd.setCursor(8, 0);    //Set Cursor again to eigth column of second row
    lcd.print(xAxis, 1);    //Print measured distance
    lcd.print("cm      ");  //Print your units.
  }
  if(oldzAxis != zAxis){
    lcd.setCursor(8, 1);    //Set Cursor again to eigth column of second row
    lcd.print(zAxis, 1);    //Print measured distance
    lcd.print("cm      ");  //Print your units.
  }
  oldxAxis = xAxis;
  oldzAxis = zAxis;
}

void calcAxis(){
  /*xAxis = driveStepCounter/(10 * DRIVE_TRANSMISSION);  //Schritte in cm umwandeln
  zAxis = liftStepCounter/(10 * LIFT_TRANSMISSION);    //Schritte in cm umwandeln
  if(xMax < xAxis){
    xMax = xAxis;
  }
  if(zMax > zAxis){
    zMax = zAxis;
  }*/
  xAxis +=100.2;
  zAxis +=100.2;
}

void timer3Handler(){
  calcAxis();
  printLCD(xAxis, zAxis);
}


void timer2Handler(void){
  TOGGLE(STEP_DRIVE);
  if(digitalRead(STEP_DRIVE)){
    if(digitalRead(DIR_DRIVE)){
          driveStepCounter--;
        }else{
          driveStepCounter++;
        }
  }
}


void timer1Handler(void){
  TOGGLE(STEP_LIFTMOT);
  if(digitalRead(STEP_LIFTMOT)){
    if(digitalRead(DIR_LIFTMOT)){
          liftStepCounter--;
        }else{
          liftStepCounter++;
        }
  }
}

