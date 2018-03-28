#include <SoftwareSerial.h>

const byte rxPin = 0;
const byte txPin = 1;

// set up a new serial object
SoftwareSerial SerialObject (rxPin, txPin);

void setup() {
  // put your setup code here, to run once:
  SerialObject.begin()
}

void loop() {
  // put your main code here, to run repeatedly:

}
