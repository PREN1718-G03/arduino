 #include <LiquidCrystal.h> //Load Liquid Crystal Library
LiquidCrystal LCD(48, 49, 50, 51, 52, 53);  //Create Liquid Crystal Object called LCD
 
int trigPinX=A10; //Sensor Trip pin connected to Arduino pin A10
int echoPinX=A11;  //Sensor Echo pin connected to Arduino pin A11
int echoPinY=A9;  //Sensor Echo pin connected to Arduino pin A9
int trigPinY=A8;  //Sensor Echo pin connected to Arduino pin A8
int myCounter=0;  //declare your variable myCounter and set to 0
//int servoControlPin=6; //Servo control line is connected to pin 6
float pingTimeX;  //time for ping to travel from sensor to target and return for x-Axis
float pingTimeY;  //time for ping to travel from sensor to target and return for y-Axis
float targetDistanceX; //Distance to Target in centimeters
float targetDistanceY; //Distance to Target in centimeters
float speedOfSound=1236; //Speed of sound in kilometers per hour when temp is 20 degrees Celsius.

void setup() {
  
  Serial.begin(9600);
  pinMode(trigPinX, OUTPUT);
  pinMode(echoPinX, INPUT);
  pinMode(trigPinY, OUTPUT);
  pinMode(echoPinY, INPUT);
   
  LCD.begin(16,2); //Tell Arduino to start your 16 column 2 row LCD
  LCD.setCursor(0,0);  //Set LCD cursor to upper left corner, column 0, row 0
  LCD.print("y-Achse");  //Print Message on First Row
  LCD.setCursor(0,1);  //Set cursor to first column of second row
  LCD.print("x-Achse"); //Print blanks to clear the row
}
 
void loop() {
  
  digitalWrite(trigPinX, LOW); //Set trigger pin low
  delayMicroseconds(2000); //Let signal settle
  digitalWrite(trigPinX, HIGH); //Set trigPin high
  delayMicroseconds(15); //Delay in high state
  digitalWrite(trigPinX, LOW); //ping has now been sent
  delayMicroseconds(10); //Delay in high state
  
  pingTimeX = pulseIn(echoPinX, HIGH);  //pingTime is presented in microceconds
  pingTimeX=pingTimeX/1000000; //convert pingTime to seconds by dividing by 1000000 (microseconds in a second)
  pingTimeX=pingTimeX/3600; //convert pingtime to hourse by dividing by 3600 (seconds in an hour)
  targetDistanceX= speedOfSound * pingTimeX;  //This will be in miles, since speed of sound was miles per hour
  targetDistanceX=targetDistanceX/2; //Remember ping travels to target and back from target, so you must divide by 2 for actual target distance.
  targetDistanceX= targetDistanceX*100000;    //Convert miles to cm by multipling by 160934.4 (cm per mile)

  LCD.setCursor(8,1);   //Set Cursor again to eigth column of second row
  LCD.print(targetDistanceX, 1); //Print measured distance
  LCD.print("cm      ");  //Print your units.
  digitalWrite(trigPinY, LOW); //Set trigger pin low
  delayMicroseconds(2000); //Let signal settle
  digitalWrite(trigPinY, HIGH); //Set trigPin high
  delayMicroseconds(15); //Delay in high state
  digitalWrite(trigPinY, LOW); //ping has now been sent
  delayMicroseconds(10); //Delay in high state
  
  pingTimeY = pulseIn(echoPinY, HIGH);  //pingTime is presented in microceconds
  pingTimeY=pingTimeY/1000000; //convert pingTime to seconds by dividing by 1000000 (microseconds in a second)
  pingTimeY=pingTimeY/3600; //convert pingtime to hourse by dividing by 3600 (seconds in an hour)
  targetDistanceY= speedOfSound * pingTimeY;  //This will be in miles, since speed of sound was miles per hour
  targetDistanceY=targetDistanceY/2; //Remember ping travels to target and back from target, so you must divide by 2 for actual target distance.
  targetDistanceY= targetDistanceY*100000;    //Convert miles to cm by multipling by 160934.4 (cm per mile)

  LCD.setCursor(8,0);   //Set Cursor again to eigth column of second row
  LCD.print(targetDistanceY, 1); //Print measured distance

  LCD.print("cm      ");  //Print your units.
  delay(1000); //pause to let things settle
  
  
  }  
