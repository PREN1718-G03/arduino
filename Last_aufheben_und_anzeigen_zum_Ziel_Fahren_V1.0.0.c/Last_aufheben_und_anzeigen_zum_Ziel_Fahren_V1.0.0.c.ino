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
#define DRIVE_TRANSMISSION (72.3825877 / 16)//2.3579 // Vollschritte pro mm
#define LIFT_TRANSMISSION  (52.5086788 / 16)//102.85946673 // Vollschritte pro mm

// Makros definieren
//#define TOGGLE(x); digitalWrite(x, digitalRead(x) ? LOW : HIGH)

// Initialisierung


LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display
int l_stepLiftMot = 33000;
long driveStepCounter = 0;
long liftStepCounter = 0;
double xAxis;
double zAxis;
DuePWM pwm( LIFT_FREQUENCY, DRIVE_FREQUENCY );

void setup() {

  // Initialisierung des LCD-Display
  Serial.begin(115200);
  Serial.println("LCD...");

  while (! Serial);

  Serial.println("Dose: check for LCD");

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Timer3.attachInterrupt(printLCD);
  Timer3.start(1000000);   //jede Sekunde aufrufen

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
  moveDriveDistance(CARGO_START_DISTANCE_MM);
  while(true);
}

void moveDriveDistance(int mm_Distance) {
  pwm.pinFreq2(DRIVE_STEP);
  pwm.pinDuty(DRIVE_STEP, 127);
}

void moveLiftDistance(int mm_Hight) {}

void printLCD() {
    lcd.setCursor(8, 0);    //Set Cursor again to eigth column of second row
    lcd.print(xAxis, 1);    //Print measured distance
    lcd.print("cm      ");  //Print your units.
    lcd.setCursor(8, 1);    //Set Cursor again to eigth column of second row
    lcd.print(zAxis, 1);    //Print measured distance
    lcd.print("cm      ");  //Print your units.
}

void calcAxis() {
  xAxis = driveStepCounter / (10 * DRIVE_TRANSMISSION); //Schritte in cm umwandeln
  zAxis = liftStepCounter / (10 * LIFT_TRANSMISSION);  //Schritte in cm umwandeln
}

void stopInterrupt(void) {
  Serial.println("Das Programm wurde angehalten.");
  while (1);
}

