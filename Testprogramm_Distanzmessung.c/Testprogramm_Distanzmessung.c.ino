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
#define LIFT_DELAY 35
#define DRIVE_DELAY 100
#define ZERO_STEP 100000
#define DRIVE_TRANSMISSION 2.3579 // Vollschritte pro mm
#define LIFT_TRANSMISSION  3.3506 // Vollschritte pro mm

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
  Serial.begin(1000000);
  Serial.println("LCD...");

  while (! Serial);

  Serial.println("Dose: check for LCD");

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Timer3.attachInterrupt(timer3Handler);
  Timer3.start(1000000);   //jede Sekunde aufrufen
  Timer2.attachInterrupt(timer2Handler);
  Timer1.attachInterrupt(timer1Handler);
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
  delay(1000);
  startMotor(true, DRIVE_DELAY);
  delay(5000);
  stopMotor(true);
  stopMotor(false);
  TOGGLE(DIR_DRIVE);
  delay(5000);
}

/*
  Rampe, welche während 1000 Schritten ansteigt oder sinkt
*/
/*void rampeMotor(int pin, int startdelay, int enddelay) {
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
}*/

void startMotor(int isDrive, int delayTime){
      if(isDrive){
        Timer2.start(delayTime);
      }else{
        Timer1.start(delayTime);
      }
}

void stopMotor(int isDrive){
      if(isDrive){
        Timer2.stop();
      }else{
        Timer1.stop();
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
  xAxis = driveStepCounter/(10 * DRIVE_TRANSMISSION);  //Schritte in cm umwandeln
  zAxis = liftStepCounter/(10 * LIFT_TRANSMISSION);    //Schritte in cm umwandeln
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

