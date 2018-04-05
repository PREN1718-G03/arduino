#include <SoftwareSerial.h>

const byte rxPin = 0;
const byte txPin = 1;

// set up a new serial object
SoftwareSerial serialObject = SoftwareSerial(rxPin, txPin);
int index;

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  // Set the BAUD rate for the serial communication
  serialObject.begin(9600);
  index = 0;
}

void loop() {
  serialObject.println(index);
  index++;
  delay(1000);
}
