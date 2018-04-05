#include <SoftwareSerial.h>

const byte rxPin = 0;
const byte txPin = 1;

// set up a new serial object
SoftwareSerial serialObject = SoftwareSerial(rxPin, txPin);
int index;
String inputString = "";
boolean stringComplete = false;

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  // Set the BAUD rate for the serial communication
  serialObject.begin(9600);
  index = 0;
  inputString.reserve(200);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    serialObject.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (serialObject.available()) {
    // get the new byte:
    char inChar = (char)serialObject.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
