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
#define CARGO_START_DISTANCE_MM 620
#define DRIVE_TRANSMISSION 143.317523646   // schritte pro mm 
#define LIFT_TRANSMISSION  1.286977421568  // schritte pro mm kalibriert am 24.05.18

// Initialisierung

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display
bool isDriving = false;
int distanceMemory = 0;
double xAxis = -((double)CARGO_START_DISTANCE_MM) / 10;
double zAxis = 0;
DuePWM pwm( LIFT_FREQUENCY, DRIVE_FREQUENCY );

//Initialisierung Kommunikation
bool sentCommand = false;
bool receivedData = false;
bool isPlaced = false;
char incomingByte = 0;
int height = 0;
int oldHeight;
int liftHeight;
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
  Timer3.attachInterrupt(getPiData);
  Timer3.start(1000000);   //jede 1000ms aufrufen
  Timer2.attachInterrupt(catchDriveStepTimer);
  Timer1.attachInterrupt(catchLiftStepTimer);
  Serial.println("oldHeight : " + oldHeight);
  Serial.println("height : " + height);
  lcd.setBacklight(255);
  lcd.begin(16, 2); // initialize the lcd
  lcd.setCursor(0, 0);  //Set LCD cursor to upper left corner, column 0, row 0
  lcd.print("x-Achse");  //Print Message on First Row
  lcd.setCursor(0, 1);  //Set cursor to first column of second row
  lcd.print("z-Achse"); //Print blanks to clear the row
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
  delay(6000);
  moveDriveDistance(CARGO_START_DISTANCE_MM);
  moveLiftDistance(250); //aktuelle Höhe minus 250mm
  liftHeight = height;
  while (!isPlaced) {
    if (distanceToTarget > 0) {
      //while (distanceToTarget > 0) {
      distanceToTarget -= 550;
        moveDriveDistance((distanceToTarget));
        delay(8000);
      //}
      Timer3.stop();
      getPiData();
      moveLiftDistance(-250-zAxis * 10); // auf Höhe absetzen, bei der die Last aufgenommen wurde (20mm weniger wegen Zielplattform)
      printLCD();
      moveLiftDistance(350); // Höher als Hindernisse ziehen
      isPlaced = true;
    }
    else {
      moveDriveDistance(500);
    }
  }
  moveToPillar();
  while(true){} //in endlosschleife fangen
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

void moveDriveDistance(int mm_Distance) {
  while (isDriving) {
  delay(1);
  }
  isDriving = true;
  distanceMemory = mm_Distance;
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
  Serial.println("hat den Fahr-Motor angehalten");
}

void catchLiftStepTimer() {
  Timer1.stop();
  zAxis += distanceMemory / 10;
  pwm.stop(LIFT_STEP);
  isDriving = false;
  Serial.println("hat den Anhebe-Motor angehalten");
}

void printLCD() {
  lcd.setCursor(8, 0);    //Set Cursor again to eigth column of second row
  lcd.print(xAxis, 1);    //Print measured distance
  lcd.print("cm      ");  //Print your units.
  lcd.setCursor(8, 1);    //Set Cursor again to eigth column of second row
  lcd.print(zAxis, 1);    //Print measured distance
  lcd.print("cm      ");  //Print your units.
}

/*void initPiData() {
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
      if (receivedData) {
        Serial.print("height: ");
        Serial.print(height, DEC);
        Serial.print(" Distance to pillar: ");
        Serial.print(distanceToPillar, DEC);
        Serial.print(" Distance to target: ");
        Serial.println(distanceToTarget, DEC);
        receivedData = false;
        sentCommand = false;
      }
    }
  }
  Serial.println("Init Pi data finished");
}*/

void getPiData() {
  Serial.println("getPiData");
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
      if (abs(oldHeight - height) > 70) {
        height = oldHeight;
      }
      zAxis += (double)(height - oldHeight) / 10;
    }
    else{
      zAxis = 0;
    }
    distanceToPillar = Serial1.read() * 256 + Serial1.read();
    xAxis = (double)distanceToPillar / 10;
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
    Serial.print("height: ");
    Serial.print(height, DEC);
    Serial.print(" Distance to pillar: ");
    Serial.print(distanceToPillar, DEC);
    Serial.print(" Distance to target: ");
    Serial.println(distanceToTarget, DEC);
    receivedData = false;
    sentCommand = false;
  }
}

