#include <Arduino.h>
#include <Wire.h>
#include <Ultrasonic.h> // lib link - https://github.com/JRodrigoTech/Ultrasonic-HC-SR04
//#include <SD.h>

#include <SoftwareSerial.h>

// setup SD card pin
//const int chipSelect = 4;

// Ultrasonic Sensors
Ultrasonic us1(A0,A1); // (Trig PIN,Echo PIN)
Ultrasonic us2(A2,A3); // pinout, pinin
Ultrasonic us3(A4,A5);

// setup motor pin variables
int m1 = 3;
int m2 = 7;
int m3 = 8;
int m4 = 9;
int p1 = 5;
int p2 = 6;

// setup values for math
int rv1 = 0;
int rv2 = 0;
int rvf;
int lv1 = 0;
int lv2 = 0;
int lvf;

int i;

void setup () {
    // motors (digital pins)
    pinMode(m1, OUTPUT);     
    pinMode(m2, OUTPUT);
    pinMode(m3, OUTPUT);
    pinMode(m4, OUTPUT);
    pinMode(p1, OUTPUT);
    pinMode(p2, OUTPUT);
    
    Serial.begin(9600);
    /*
    //sd card setup
    pinMode(10, OUTPUT);
    if (!SD.begin(chipSelect)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
    return;
    }*/
}

void drive (int rvf, int lvf) {
  /*
  m1 & m2 on Low & High is right wheel forward
  m1 & m2 on High & Low is right wheel backward
  m3 & m4 on Low & High is left wheel forward
  m3 & m4 on High & Low is left wheel backward
  */
  digitalWrite(m1, rvf > 0 ? LOW : HIGH);
  digitalWrite(m2, rvf < 0 ? LOW : HIGH);
  digitalWrite(m3, lvf > 0 ? LOW : HIGH);
  digitalWrite(m4, lvf < 0 ? LOW : HIGH);
  analogWrite(p1, abs(rvf));
  analogWrite(p2, abs(lvf));
}

void loop () {
//-------------------------------------------------
  // get distances from objects
  int u1 = us1.Ranging(CM);
  int u2 = us2.Ranging(CM);
  int u3 = us3.Ranging(CM);

//-------------------------------------------------
  //put distances on a scale of PWM powers

  // from front sensor
  rv1 = map(u1, 1, 51, 0, 255);
  lv1 = rv1;
  
  // from left sensor
  lv2 = map(u2, 1, 51, 0, 255);
  
  //from right sensor
  rv2 = map(u3, 1, 51, 0, 255);


  if (u1 <= 10) {
    rv1 = -rv1 - 100;
    lv1 = -lv1 - 100;
    drive(rv1, lv1);
    Serial.println("Going Back");
  } else if (u2 <= 10) {
    rv2 = -rv2 - 100;
    drive(rv2, lv1);
    Serial.println("Going Right");
  } else if (u3 <= 10) {
    lv2 = -lv2 - 100;
    drive(rv1, lv2);
    Serial.println("Going Left");
  } else {
    Serial.println("Going Forw");
    drive(rv1,lv1);
  }
  //log to sd card
  /*
    File dataFile = SD.open("auto-data.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(rvf+" , "+lvf);
      dataFile.close();
      // print to the serial port too:
      Serial.println("...");
    } else {
      Serial.println("error opening auto-data.txt");*/
}
