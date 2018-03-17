#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, INPUT);
  digitalWrite(4, HIGH);
  //int counter = 0, counter2 = 0;
}

void loop() {

  /**
   *  Warte bis der Start-Schalter eingeschaltet wird.
   *  Danach führe 20'000 Schritte mit dem Antriebsmotor aus.
  **/
  while(!digitalRead(7));
  for(int counter = 0;counter < 20000; counter++){
    delayMicroseconds(50);
    digitalWrite(3,HIGH); // turn on pin 3
    digitalWrite(3,LOW); // turn off pin 3
  }

  /**
   *  Der Aufzugsmotor wird für 10'000 Schritte betrieben
  **/
  for(int counter2 = 0;counter2 < 10000; counter2++){
    delayMicroseconds(50);
    digitalWrite(8,HIGH); // turn on pin 3
    digitalWrite(8,LOW); // turn off pin 3
  }
  /**
   *  Solange der Entschalter nicht gedrückt wird, soll der Antrieb weiterlaufen
  **/
  while(digitalRead(7)){
    delayMicroseconds(50);
    digitalWrite(3,HIGH); // turn on pin 3
    digitalWrite(3,LOW); // turn off pin 3
  }

}
