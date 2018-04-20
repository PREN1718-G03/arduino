
char c = 0;
void setup() {
 
  Serial.begin(250000);
  Serial1.begin(250000);
  Serial2.begin(250000);

  Serial2.print("Hello");
}


void loop() {
  String s;
  s = "";

  while (Serial1.available() > 0) {
    c = Serial1.read();
    s += c;

  }
  if (s.length() > 0) {
    Serial.println(s);
    Serial2.print(s);
  }
  
  delay(1000);
}
