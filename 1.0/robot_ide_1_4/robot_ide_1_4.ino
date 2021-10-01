#include <Arduino.h>
#include <Ultrasonic.h> // lib link - https://github.com/JRodrigoTech/Ultrasonic-HC-SR04
#include <pt.h>   // include protothread library - https://roboticsbackend.com/arduino-protothreads-tutorial/
#include <SD.h>
int m1 = 3;
int m2 = 7;
int m3 = 8;
int m4 = 9;
int p1 = 5;
int p2 = 6;
const int chipSelect = 4;
static struct pt pt1;
Ultrasonic us1(A0,A1); // (Trig PIN,Echo PIN)
Ultrasonic us2(A2,A3); // pinout, pinin
Ultrasonic us3(A4,A5);
void setup() {    
  Serial.begin(9600); 
  pinMode(10, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  PT_INIT(&pt1);
  pinMode(A0, OUTPUT);
  pinMode(A1, INPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, INPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, INPUT);
  pinMode(m1, OUTPUT);     
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);
  pinMode(m4, OUTPUT);
  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);
}

static int prot1(struct pt *pt) {
  PT_BEGIN(pt); 
  int u1 = us1.Ranging(CM);
  int u2 = us2.Ranging(CM);
  int u3 = us3.Ranging(CM);
  
  int rv = 255;
  int lv = 255;
  
  // as robot gets closer to an object, subtract speed
  
  //u1, if approaching from front
  rv -= map(u1, 1, 51, 0, 255);
  lv -= map(u1, 1, 51, 0, 255);
  
  //u2, if approaching from left side, make right wheel spin slower to turn right
  rv -= map(u2, 1, 51, 0, 255);
  
  //u3, //u2, if approaching from right side, make left wheel spin slower to turn left
  lv -= map(u3, 1, 51, 0, 255);

  /*
  
  */
  digitalWrite(m1, rv > 175 ? HIGH : LOW);
  digitalWrite(m2, rv < 175 ? HIGH : LOW);
  digitalWrite(m3, lv > 175 ? HIGH : LOW);
  digitalWrite(m4, lv < 175 ? HIGH : LOW);
  analogWrite(p1, abs(rv));
  analogWrite(p2, abs(lv));

  File dataFile = SD.open("auto-data.txt", FILE_WRITE); 
  if (dataFile) {
    dataFile.println(rv+" , "+lv);
    dataFile.close();
    // print to the serial port too:
    Serial.println("...");
  } else {
    Serial.println("error opening auto-data.txt");
  }
  PT_END(pt);
}
void loop() {
  prot1(&pt1);
}
