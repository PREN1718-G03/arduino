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
#define LIFT_FREQUENCY 300
#define DRIVE_FREQUENCY 17000
#define CARGO_START_DISTANCE_MM 580
#define DRIVE_TRANSMISSION 143.317523646   // schritte pro mm 
#define LIFT_TRANSMISSION  1.286977421568  // schritte pro mm kalibriert am 24.05.18

// Initialisierung

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display
bool isDriving = false;
int distanceMemory = 0;
double xAxis = 0;
double zAxis = 0;
DuePWM pwm( LIFT_FREQUENCY, DRIVE_FREQUENCY );

//Initialisierung Kommunikation
bool sentCommand = false;
bool receivedData = false;
bool isPlaced = false;
char incomingByte = 0;
int height = 0;
int oldHeight = 0;
int liftHeight;
int distanceToPillar = 0;
int16_t distanceToTarget = 0;

void setup() {

  // Initialisierung des LCD-Display
  Serial.begin(9600);

  while (! Serial);

  Serial1.begin(9600);    //Kommunikation via Rx1 Tx1

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Timer3.attachInterrupt(getPiData);
  Timer3.start(1000000);   //jede 1000ms aufrufen
  Timer2.attachInterrupt(catchDriveStepTimer);
  Timer1.attachInterrupt(catchLiftStepTimer);
  /*Serial.println("oldHeight : " + oldHeight);
    Serial.println("height : " + height);*/
  lcd.setBacklight(255);
  lcd.begin(16, 2); // initialize the lcd
  lcd.setCursor(0, 0);  //Set LCD cursor to upper left corner, column 0, row 0
  lcd.print("x-Achse");  //Print Message on First Row
  lcd.setCursor(0, 1);  //Set cursor to first column of second row
  lcd.print("z-Achse"); //Print blanks to clear the row
  //Serial.println("lcd wurde initialisiert");

  // Initialisiere Motor Pins
  pinMode(DRIVE_DIR, OUTPUT);
  digitalWrite(DRIVE_DIR, HIGH);
  pinMode(DRIVE_STEP, OUTPUT);
  pinMode(LIFT_DIR, OUTPUT);
  digitalWrite(LIFT_DIR, LOW);
  pinMode(LIFT_STEP, OUTPUT);
  pinMode(STOP_SWITCH, INPUT_PULLUP);
  //Serial.print("\nInitialisierungen wurden ausgeführt.");
}

/*Main Programm*/
void loop() {
  delay(2000);
  moveDriveDistance(CARGO_START_DISTANCE_MM);
  moveLiftDistance(350);
  liftHeight = height;
  while (!isPlaced && digitalRead(STOP_SWITCH)) {
      moveDriveDistance(4000);
      while((distanceToTarget <= 0) && digitalRead(STOP_SWITCH)){}
    if (distanceToTarget > 0) {
      Timer3.stop();
      catchDriveStepTimer();
      /*if (xAxis > 190) {
        distanceToTarget += 100;
      } else if (xAxis > 160) {
        distanceToTarget += 700;
      } else */
      /*if ((xAxis < 140) && (xAxis > 80)) {
        distanceToTarget += 250;
      } else if (xAxis > 80) {
        distanceToTarget += 450;
      }else{
        distanceToTarget += 400;
      }*/
      moveDriveDistance(distanceToTarget);
      xAxis += distanceToTarget / 10;
      getPiData();
      moveLiftDistance(-330 - zAxis * 10); // auf Höhe absetzen, bei der die Last aufgenommen wurde (20mm weniger wegen Zielplattform)
      int zAxisOld = zAxis;
      moveLiftDistance(100 + 10 * zAxisOld); // Höher als Hindernisse ziehen
      zAxis = 0;
      printLCD();
      isPlaced = true;
    }
  }
  moveToPillar();
  while (true) {} //in endlosschleife fangen
}

void moveToPillar(void) {
  while (isDriving) {
    delay(500);
  }
  isDriving = true;
  digitalWrite(DRIVE_DIR, false);   //vorwärts fahren
  pwm.pinFreq2(DRIVE_STEP);
  pwm.pinDuty(DRIVE_STEP, 127);
  while (digitalRead(STOP_SWITCH)) {}
  pwm.stop(DRIVE_STEP);
}

void moveDriveDistance(double mm_Distance) {
  while (isDriving) {
    delay(1);
  }
  isDriving = true;
  if (mm_Distance > 0) {
    digitalWrite(DRIVE_DIR, false);
  }
  else {
    digitalWrite(DRIVE_DIR, true);
  }
  pwm.pinFreq2(DRIVE_STEP);
  pwm.pinDuty(DRIVE_STEP, 127);
  //Umrechnung von s in us
  Timer2.start(1000000 * abs(mm_Distance) * DRIVE_TRANSMISSION / DRIVE_FREQUENCY);
}

void moveLiftDistance(int mm_height) {
  while (isDriving) {
    delay(500);
  }
  isDriving = true;
  distanceMemory = mm_height;
  if (mm_height > 0) {
    digitalWrite(LIFT_DIR, false);
  }
  else {
    digitalWrite(LIFT_DIR, true);
  }
  pwm.pinFreq2(LIFT_STEP);
  pwm.pinDuty(LIFT_STEP, 127);
  Timer1.start(1000000 * abs(mm_height) * LIFT_TRANSMISSION / LIFT_FREQUENCY);
}

void catchDriveStepTimer() {
  Timer2.stop();
  pwm.stop(DRIVE_STEP);
  isDriving = false;
}

void catchLiftStepTimer() {
  Timer1.stop();
  zAxis += distanceMemory / 10;
  pwm.stop(LIFT_STEP);
  isDriving = false;
}

void printLCD() {
  lcd.setCursor(8, 0);    //Set Cursor again to eigth column of second row
  lcd.print(xAxis, 1);    //Print measured distance
  lcd.print("cm      ");  //Print your units.
  lcd.setCursor(8, 1);    //Set Cursor again to eigth column of second row
  lcd.print(zAxis, 1);    //Print measured distance
  lcd.print("cm      ");  //Print your units.
}

void getPiData() {
  oldHeight = height;
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
    if (oldHeight != 0) {
      zAxis += (double)(height - oldHeight) / 10;
    }
    else {
      zAxis = 0;
    }
    distanceToPillar = Serial1.read() * 256 + Serial1.read();
    xAxis = 342 - (double)distanceToPillar / 10;
    distanceToTarget = Serial1.read() * 256 + Serial1.read();
    while (Serial1.available() > 0) {
      incomingByte = Serial1.read();
      if (incomingByte = '\n') {
        receivedData = true;
      }
    }
    printLCD();
  }
  if (receivedData) {
    receivedData = false;
    sentCommand = false;
  }
}

