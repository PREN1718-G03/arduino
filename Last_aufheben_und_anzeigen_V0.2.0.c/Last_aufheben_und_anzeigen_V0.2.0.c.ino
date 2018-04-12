/**
* Projekt : PREN 2 Gruppe 3
* Titel : Last Aufheben V 0.2.0
* Datum : 16.03.18
* Autor : Jan Odermatt
* Beschreibung :
*   Dieses Programm bewegt die Laufkatze bis zur Last, bewegt den Hacken zur Last,
*   hebt diese auf und fährt bis zum Endpfosten
*	Während dessen zeigt das LCD-Display ständig die Position der Last an.
**/
#include <Arduino.h>
#include <LiquidCrystal.h>

// Definition aller Pins

// Ultraschall Pins definieren
#define TRIGPIN_X A10
#define ECHOPIN_X A11
#define TRIGPIN_Y A8
#define ECHOPIN_Y A9
#define SPEED_OF_SOUND 1236

// Motor Pins definieren
#define DIR_DRIVE 3
#define STEP_DRIVE 4
#define DIR_LIFTMOT 5
#define STEP_LIFTMOT 6
#define START_SWITCH 9 
#define END_SWITCH 10
#define STARTSCHLAUFE 13

// Makros definieren
#define TOGGLE(pin) do{(digitalRead(pin)!= LOW)? digitalWrite(pin, LOW): digitalWrite(pin,HIGH);}while(0)

// Initialisierung
LiquidCrystal LCD(48, 49, 50, 51, 52, 53);	// Kreiere LCD Objekt
int l_stepLiftMot = 35000;
short switch_state;
short hasSwitchedFlag = 0;
int myCounter = 0;
int servoControlPin = 6;
float pingTimeX;  //time for ping to travel from sensor to target and return for x-Axis
float pingTimeY;  //time for ping to travel from sensor to target and return for y-Axis
float targetDistanceX; //Distance to Target in centimeters
float targetDistanceY; //Distance to Target in centimeters


//Funktions Prototypen erstellen
void rampeMotor(int, int, int);
void getUltraschallDistance(void);
void printLCD(void);

void setup() {
  // Initialisiere der Ultraschallsensoren
	Serial.begin(9600);
	pinMode(TRIGPIN_X, OUTPUT);
	digitalWrite(TRIGPIN_X, LOW);
	pinMode(ECHOPIN_X, INPUT);
	pinMode(TRIGPIN_Y, OUTPUT);
	digitalWrite(TRIGPIN_Y, LOW);
	pinMode(ECHOPIN_Y, INPUT);

  // Initialisierung des LCD-Displays
	LCD.begin(16, 2); //Tell Arduino to start your 16 column 2 row LCD
	LCD.setCursor(0, 0);  //Set LCD cursor to upper left corner, column 0, row 0
	LCD.print("y-Achse");  //Print Message on First Row
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
}

/*Main Programm*/
void loop() {
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
	hasSwitchedFlag = 0;
	rampeMotor(STEP_DRIVE, 500, 40);
	for (long zaehler = 0;zaehler <= 78000;zaehler++) { // 70'000 Schritte in X-Richtung
		TOGGLE(STEP_LIFTMOT);
		delayMicroseconds(40);
		TOGGLE(STEP_LIFTMOT);
		delayMicroseconds(40);
	}
	rampeMotor(STEP_DRIVE, 40, 500);

	digitalWrite(DIR_LIFTMOT, LOW);                   // Richtung in Z-Achse
	getUltraschallDistance();
	printLCD();
	rampeMotor(STEP_LIFTMOT, 500, 15);
	for (int zaehler2 = 0; zaehler2 < (l_stepLiftMot - 2000); zaehler2++) {
		TOGGLE(STEP_LIFTMOT);
		delayMicroseconds(15);
		TOGGLE(STEP_LIFTMOT);
		delayMicroseconds(15);
	}
	rampeMotor(STEP_LIFTMOT, 15, 500);
	rampeMotor(STEP_DRIVE, 500, 40);
	while (digitalRead(END_SWITCH)) {
		TOGGLE(STEP_DRIVE);
		delayMicroseconds(40);
		TOGGLE(STEP_DRIVE);
		delayMicroseconds(40);
	}
}

/*
* Rampe, welche während 1000 Schritten ansteigt oder sinkt
*/
void rampeMotor(int pin, int startdelay, int enddelay) {
	int isSpeedUp = startdelay - enddelay;
	int deltaDelay = isSpeedUp / 10;
	for (int zaehler = 0;zaehler < 1000; zaehler++) {
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

void getUltraschallDistance(void) {
		TOGGLE(TRIGPIN_X);
		TOGGLE(TRIGPIN_Y);
		delayMicroseconds(2000);
		TOGGLE(TRIGPIN_X);
		TOGGLE(TRIGPIN_Y);
		delayMicroseconds(15);
		TOGGLE(TRIGPIN_X);
		TOGGLE(TRIGPIN_Y);
		delayMicroseconds(10);

		pingTimeX = pulseIn(ECHOPIN_X, HIGH);  //pingTime is presented in microceconds
		pingTimeX = pingTimeX / 1000000; //convert pingTime to seconds by dividing by 1000000 (microseconds in a second)
		pingTimeX = pingTimeX / 3600; //convert pingtime to hourse by dividing by 3600 (seconds in an hour)
		targetDistanceX = SPEED_OF_SOUND * pingTimeX;  //This will be in miles, since speed of sound was miles per hour
		targetDistanceX = targetDistanceX / 2; //Remember ping travels to target and back from target, so you must divide by 2 for actual target distance.
		targetDistanceX = targetDistanceX * 100000;    //Convert miles to cm by multipling by 160934.4 (cm per mile)	

		pingTimeY = pulseIn(ECHOPIN_Y, HIGH);  //pingTime is presented in microceconds
		pingTimeY = pingTimeY / 1000000; //convert pingTime to seconds by dividing by 1000000 (microseconds in a second)
		pingTimeY = pingTimeY / 3600; //convert pingtime to hourse by dividing by 3600 (seconds in an hour)
		targetDistanceY = SPEED_OF_SOUND * pingTimeY;  //This will be in miles, since speed of sound was miles per hour
		targetDistanceY = targetDistanceY / 2; //Remember ping travels to target and back from target, so you must divide by 2 for actual target distance.
		targetDistanceY = targetDistanceY * 100000;    //Convert miles to cm by multipling by 160934.4 (cm per mile)	
}

void printLCD(void) {
	LCD.setCursor(8, 1);   //Set Cursor again to eigth column of second row
	LCD.print(targetDistanceX, 1); //Print measured distance
	LCD.print("cm      ");  //Print your units.
	LCD.setCursor(8, 0);   //Set Cursor again to eigth column of second row
	LCD.print(targetDistanceY, 1); //Print measured distance
	LCD.print("cm      ");  //Print your units.
	delay(1000); //pause to let things settle
}