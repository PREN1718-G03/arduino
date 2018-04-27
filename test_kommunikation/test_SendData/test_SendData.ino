bool sentCommand = false;

void setup() {
}

void setup_arduino_uno(){
  Serial.begin(9600);
}

void setup_arduino_due(){
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
}

void sendData_uno(){
  if (!sentCommand) {
    Serial.println("SendData");
    sentCommand = true;
  }
  while (Serial.available() > 0) {
    
  }
}