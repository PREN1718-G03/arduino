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
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <DueTimer.h>
#include "C:\Users\JanB\Documents\GitHub\DuePWM\DuePWM.h"
#include "C:\Users\JanB\Documents\GitHub\DuePWM\DuePWM.cpp"

// Definition aller Pins

// Motor Pins definieren
#define DRIVE_DIR 3
#define DRIVE_STEP 7
#define LIFT_DIR 5
#define LIFT_STEP 6
#define STOP_SWITCH 10
#define LIFT_FREQUENCY 200
#define DRIVE_FREQUENCY 200
#define CARGO_START_DISTANCE_MM 750
#define DRIVE_TRANSMISSION (72.3825877 / 16)//2.3579 // schritte pro mm
#define LIFT_TRANSMISSION  (52.5086788 / 16)//102.85946673 // schritte pro mm

// Initialisierung

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display
bool isDriving = false;
int distanceMemory = 0;
double xAxis = ((double)CARGO_START_DISTANCE_MM) / 10;
double zAxis = 0;
int hightList[35] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
DuePWM pwm( LIFT_FREQUENCY, DRIVE_FREQUENCY );

//Initialisierung Kommunikation
bool sentCommand = false;
bool receivedData = false;
char incomingByte = 0;
int height = 0;
int distanceToPillar = 0;
int16_t distanceToTarget = 0;

void setup() {

  // Initialisierung des LCD-Display
  Serial.begin(9600);
  Serial.println("LCD...");

  while (! Serial);

  Serial.println("Dose: check for LCD");

  Serial1.begin(9600);    //Kommunikation via Rx1 Tx1

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Timer3.attachInterrupt(printLCD);
  Timer3.start(1000000);   //jede Sekunde aufrufen
  Timer2.attachInterrupt(catchDriveStepTimer);
  Timer1.attachInterrupt(catchLiftStepTimer);
  lcd.setBacklight(255);
  lcd.begin(16, 2); // initialize the lcd
  lcd.setCursor(0, 0);  //Set LCD cursor to upper left corner, column 0, row 0
  lcd.print("T_Dist");  //Print Message on First Row
  lcd.setCursor(0, 1);  //Set cursor to first column of second row
  lcd.print("P_Dist"); //Print blanks to clear the row
  Serial.println("lcd wurde initialisiert");

  // Initialisiere Motor Pins
  pinMode(DRIVE_DIR, OUTPUT);
  digitalWrite(DRIVE_DIR, HIGH);
  pinMode(DRIVE_STEP, OUTPUT);
  pinMode(LIFT_DIR, OUTPUT);
  digitalWrite(LIFT_DIR, LOW);
  pinMode(LIFT_STEP, OUTPUT);
  pinMode(STOP_SWITCH, INPUT_PULLUP);
  Serial.print("\nInitialisierungen wurden ausgeführt.");
}

/*Main Programm*/
void loop() {
  getPiData();
  delay(3000);
}

void moveDriveDistance(int mm_Distance) {
  while(isDriving);
  isDriving = true;
  distanceMemory = mm_Distance;
  if (mm_Distance > 0) {
    digitalWrite(DRIVE_DIR, true);
  }
  else {
    digitalWrite(DRIVE_DIR, false);
  }
  pwm.pinFreq2(DRIVE_STEP);
  pwm.pinDuty(DRIVE_STEP, 127);
  //Umrechnung von s in us
  Timer2.start(1000000 * mm_Distance * DRIVE_TRANSMISSION / DRIVE_FREQUENCY);
}

void moveLiftDistance(int mm_Hight) {
  while(isDriving);
  isDriving = true;
  distanceMemory = mm_Hight;
  if (mm_Hight > 0) {
    digitalWrite(DRIVE_DIR, true);
  }
  else {
    digitalWrite(DRIVE_DIR, false);
  }
  pwm.pinFreq2(LIFT_STEP);
  pwm.pinDuty(LIFT_STEP, 127);
  Timer1.start(1000000 * mm_Hight * LIFT_TRANSMISSION / LIFT_FREQUENCY);
}

void catchDriveStepTimer() {
  xAxis += (double)distanceMemory * 10 * cos(0.1418970);
  pwm.stop(DRIVE_STEP);
  isDriving = false;
}

void catchLiftStepTimer() {
  zAxis += distanceMemory * 10;
  pwm.stop(LIFT_STEP);
  isDriving = false;
}

void printLCD() {
  lcd.setCursor(8, 0);    //Set Cursor again to eigth column of second row
  //if(distanceToTarget > 0){
    lcd.print(distanceToTarget, 10);    //Print measured distance
  /*}
  else{
    lcd.print("-1", 1);    //Print measured distance
  }*/
  lcd.print("mm      ");  //Print your units.
  lcd.setCursor(8, 1);    //Set Cursor again to eigth column of second row
  lcd.print(distanceToPillar, 1);    //Print measured distance
  lcd.print("mm      ");  //Print your units.
}

void getPiData() {
  if (!sentCommand) {
    // Flush the read queue
    while (Serial1.available() > 0) {
      Serial1.read();
    }
    Serial1.println("SendData");
    sentCommand = true;
  }
  if (Serial1.available() > 0) {
    height = Serial1.read() * 256 + Serial1.read();
    distanceToPillar = Serial1.read() * 256 + Serial1.read();
    distanceToTarget = Serial1.read() * 256 + Serial1.read();
    while (Serial1.available() > 0) {
      incomingByte = Serial1.read();
      if (incomingByte = '\n') {
        receivedData = true;
      }
    }
  }
  if (receivedData) {
    Serial.print("Height: ");
    Serial.print(height, DEC);
    Serial.print(" Distance to pillar: ");
    Serial.print(distanceToPillar, DEC);
    Serial.print(" Distance to target: ");
    Serial.println(distanceToTarget, DEC);
    receivedData = false;
    sentCommand = false;
  }
}

