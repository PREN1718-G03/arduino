#include <liquidcrystal.h> // Lade Liquid Crystal Bibliothek

 Liquidcrystal LCD (11,10,9,2,3,4,5);  // Erstellt Liquid Crystal Object genannt LCD </liquidcrystal.h>

 #define trigPin 13 // Sensor Echo Pin auf Arduino Pin 13 verbunden
 #define echoPin 12 // Sensor Trip Pin an Pin Arduino 

 Leere setup () 
 {  
   pinMode (trigPin, OUTPUT);
   pinMode (echoPin, INPUT);
  
   LCD.begin (16,2);  // Teile Arduino, um Ihre 16 Spalte 2 Zeile LCD starten
   LCD.setCursor (0,0);  // Set LCD Cursor zum linken oberen Ecke, Spalte 0, Zeile 0
   LCD.print ("Target Entfernung:");  // Print Nachricht auf Erste Zeile
 }

 Leere Schleife () {
   lange Zeit, Strecke;
   digital (trigPin, LOW);
   delayMicroseconds (2);
   digital (trigPin, HIGH);
   delayMicroseconds (10);
   digital (trigPin, LOW);
   Dauer = pulseIn (echoPin, HIGH);
   Abstand = (Dauer / 2) / 29,1;

   LCD.setCursor (0,1);  // Setzt Cursor auf ersten Spalte der zweiten Reihe
   LCD.print ("");  // Print Rohlinge, um die Zeile zu löschen
   LCD.setCursor (0,1);  // Setze Cursor erneut, um erste Spalte der zweiten Reihe
   LCD.print (Entfernung);  // Print gemessene Abstand
   LCD.print ("cm");  // Print Ihre Einheiten.
   Verzögerung (250);  // Pause Dinge absetzen zu lassen
 }
