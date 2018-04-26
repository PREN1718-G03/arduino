char c = 0;
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  String s;
  s = "";
  while (Serial1.available() > 0) {
    c = Serial1.read();
    s += c;
  }
  if (s.length() > 0) {
    Serial.print(s);
    Serial2.print(s);
  }
  delay(10);
}
