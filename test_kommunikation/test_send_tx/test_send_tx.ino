#include <SoftwareSerial.h>

#define rxPin 0
#define txPin 1

// set up a new serial object
SoftwareSerial serialObject = SoftwareSerial(rxPin, txPin);
int index;
String inputString = "";
boolean stringComplete = false;
boolean rxPinChange = false;

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  // Set the BAUD rate for the serial communication
  serialObject.begin(9600);
  serialObject.listen();
  index = 0;
  inputString.reserve(200);
}

void loop() {
  read_serial();
  if (stringComplete) {
    serialObject.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void read_serial() {
  if (serialObject.available()>0){
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
