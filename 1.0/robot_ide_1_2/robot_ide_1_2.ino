#include <Arduino.h>
#include <Ultrasonic.h> // lib link - https://github.com/JRodrigoTech/Ultrasonic-HC-SR04
#include <pt.h>   // include protothread library - https://roboticsbackend.com/arduino-protothreads-tutorial/
#include <SD.h> // Defualt SD Card lib
#include <SPI.h> // Req for SD Card

//Wifi Module uses x pins, adding later

// Open a new file to log data on SD card
File datalog = SD.open("datalog.txt", FILE_WRITE);

// setup each protothread
static struct pt pt1, pt2;

// Create ultrasonic sensor objects to use in logic
Ultrasonic us1(A0,A1); // (Trig PIN,Echo PIN)
Ultrasonic us2(A2,A3); // pinout, pinin
Ultrasonic us3(A4,A5);

// setup motor pin variables
int m4 = 9;
int m3 = 6;
int m2 = 5;
int m1 = 3;

// start serial monitor if needed (debugging), and
// set pinout
void setup() {    
  // Serial.begin(9600); 
 
  // initialise protothread vars
  PT_INIT(&pt1);
  PT_INIT(&pt2);
  
  //Ultrasonic Sensors (analog pins)
  pinMode(A0, OUTPUT);
  pinMode(A1, INPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, INPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, INPUT);
  
  // motors (digital pins)
  pinMode(m1, OUTPUT);     
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);
  pinMode(m4, OUTPUT);
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
}

// the infinite loop to run on the arduino
void loop()
{
  
  // Serial monitoring is turned off to save on 
  // performance, but will be useful for debugging if needed
  
  /*Serial.print("US-1 " + us1.Ranging(CM)); // CM or INC
  Serial.println(" cm" );
  delay(10);
  Serial.print("US-2 " + us2.Ranging(CM)); // CM or INC
  Serial.println(" cm" );
  delay(10);
  Serial.print("US-3 " + us3.Ranging(CM)); // CM or INC
  Serial.println(" cm" );
  delay(10);*/
  
  // schedule threads indefinitely
  prot1(&pt1);
  prot2(&pt2);
}


/* This thread determines if the robot is
   setup to move forward, if not, then start
   moving the robot forward 
*/
static int prot1(struct pt *pt) {
  PT_BEGIN(pt);
  if (digitalRead(m1) != 0 || digitalRead(m2) != 1 || digitalRead(m3) != 0 || digitalRead(m4) != 1) {
  forw();
  }
  PT_END(pt);
}

/* This thread gathers and interprets data
   from the ultrasonic sensors and makes
   decisions for movement based on that data.
   
   Specifically, if an object is measured to 
   be at or less than 10cm away, then the robot
   will react in accordance to the logic below.
   
   SideNote: 
   us1 is in front
   us2 is facing left, so turn right
   us3 is facing right, so turn left
*/
static int prot2(struct pt *pt) {
  PT_BEGIN(pt);
  if (us1.Ranging(CM) <= 10) {
    Serial.print("WARNING");
    back();
    delay(10);
  } else if (us2.Ranging(CM) <= 10) {
    Serial.print("WARNING");;
    rturn();
    delay(10);
  } else if (us3.Ranging(CM) <= 10) {
    Serial.print("WARNING");
    lturn();
    delay(10);
  }
  PT_END(pt);
}

  // Allocate voltage to motor pins so that
  // the robot moves forward
void forw() {
  digitalWrite(m1, LOW);// right wheel forward
  digitalWrite(m2, HIGH);//
  digitalWrite(m3, LOW);// left wheel forward
  digitalWrite(m4, HIGH);//
  datalog.println("forw");
}
  // Allocate voltage to move backwards
void back() {
  digitalWrite(m3, HIGH);// left wheel back
  digitalWrite(m4, LOW);//
  digitalWrite(m1, HIGH);// right wheel back ward
  digitalWrite(m2, LOW);//
  datalog.println("back");
}
  // Turn right
void rturn() {
  digitalWrite(m1, HIGH);// right wheel back ward
  digitalWrite(m2, LOW);//
  digitalWrite(m3, LOW);// left wheel forward
  datalog.println("right");
}
  // Turn left
void lturn() {
  digitalWrite(m3, HIGH);// left wheel back
  digitalWrite(m4, LOW);//
  digitalWrite(m1, LOW);// right wheel forward
  datalog.println("left"); 
}

