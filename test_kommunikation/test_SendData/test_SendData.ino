bool sentCommand = false;
bool receivedData = false;
char incomingByte = 0;
int height = 0;
int distanceToPillar = 0;
int distanceToTarget = 0;


void setup() {
  setup_arduino_uno();
  // setup_arduino_due();
}

void setup_arduino_uno() {
  Serial.begin(9600);
}

void loop() {
  sendData_uno();
  // sendData_due();
  delay(500);
}

void sendData_uno() {
  if (!sentCommand) {
    // Flush the read queue
    while (Serial.available > 0) {
        Serial.read();
    }
    Serial.println("SendData");
    sentCommand = true;
  }
  if (Serial.available() > 0) {
    height = Serial.read() * 256 + Serial.read();
    distanceToPillar = Serial.read() * 256 + Serial.read();
    distanceToTarget = Serial.read() * 256 + Serial.read();
    while (Serial.available() > 0){
      incomingByte = Serial.read();
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

void sendData_due() {
    if (!sentCommand) {
    // Flush the read queue
    while (Serial1.available > 0) {
        Serial1.read();
    }
    Serial1.println("SendData");
    sentCommand = true;
  }
  if (Serial1.available() > 0) {
    height = Serial1.read() * 256 + Serial1.read();
    distanceToPillar = Serial1.read() * 256 + Serial1.read();
    distanceToTarget = Serial1.read() * 256 + Serial1.read();
    while (Serial1.available() > 0){
      incomingByte = Serial1.read();
      if (incomingByte = '\n') {
        receivedData = true;
      }
    }
  }
  if (receivedData) {
    Serial.print("Height: ");
    Serial.print(height, DEC);
    Serial.print("Distance to pillar: ");
    Serial.print(distanceToPillar, DEC);
    Serial.print("Distance to target: ");
    Serial.println(distanceToTarget, DEC);
    receivedData = false;
    sentCommand = false;
  }
}