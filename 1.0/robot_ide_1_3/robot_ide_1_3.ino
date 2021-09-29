#include <Arduino.h>
#include <Ultrasonic.h> // lib link - https://github.com/JRodrigoTech/Ultrasonic-HC-SR04
#include <pt.h>   // include protothread library - https://roboticsbackend.com/arduino-protothreads-tutorial/
#include <SD.h> // Defualt SD Card lib
#include <SPI.h> // Req for SD Card

// setup motor pin variables

int m1 = 2;
int m2 = 3;
int m3 = 4;
int m4 = 7;

int p1 = 5;
int p2 = 6;

//setup Motor pin voltage variables
boolean a;
boolean b; // bools will be high and low
boolean c;
boolean d;
int pa;
int pb; // ints will be PWM

//Wifi Module uses x pins, adding later

// Open a new file to log data on SD card
File datalog = SD.open("datalog.txt", FILE_WRITE);
// close file if needed 
//datalog.close();

// setup each protothread
static struct pt pt1;

// Create ultrasonic sensor objects to use in logic
Ultrasonic us1(A0,A1); // (Trig PIN,Echo PIN)
Ultrasonic us2(A2,A3); // pinout, pinin
Ultrasonic us3(A4,A5);

// start serial monitor if needed (debugging), and
// set pinout
void setup() {    
  Serial.begin(9600); 
 
  // init SD Card
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  
  // initialise protothread vars
  PT_INIT(&pt1);
  
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
  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
}

// the infinite loop to run on the arduino
void loop()
{
  // schedule threads indefinitely
  prot1(&pt1);
}

static int prot1(struct pt *pt) {
  PT_BEGIN(pt);
  if (us1.Ranging(CM) <= 10) {
    Serial.print("WARNING, going back");
    a == HIGH;
    b == LOW;
    c == HIGH;
    d == LOW;
    pa == 255;
    pb == 255;
    dm(a, b, c, d, pa, pb);
    delay(10);
  } else if (us2.Ranging(CM) <= 10) {
    Serial.print("WARNING, going right");;
    a == HIGH;
    b == LOW;
    c == LOW;
    d == HIGH;
    pa == 255;
    pb == 255;
    dm(a, b, c, d, pa, pb);
    delay(10);
  } else if (us3.Ranging(CM) <= 10) {
    Serial.print("WARNING, going left");
    a == LOW;
    b == HIGH;
    c == HIGH;
    d == LOW;
    pa == 255;
    pb == 255;
    dm(a, b, c, d, pa, pb);
    delay(10);
  } else {
    Serial.print("Moving...");
    a == LOW;
    b == HIGH;
    c == LOW;
    d == HIGH;
    pa == 255;
    pb == 255;
    dm(a, b, c, d, pa, pb);
  }
  PT_END(pt);
}

// Allocate voltage to motor pins for PWM  
void dm(boolean a, boolean b, boolean c, boolean d, int pa, int pb) {
  digitalWrite(m1, a);
  digitalWrite(m2, b);
  digitalWrite(m3, c);
  digitalWrite(m4, d);
  analogWrite(p1, pa);
  analogWrite(p2, pb);
  
  // working on converting values to strings
  /*if (datalog) {
    datalog.println("Bools: "+"\nm1 - "+a+"\nm2 - "+b+"\nm3 - "+c+"\nm4 - "+d);
    datalog.println("PWM: "+"\nMotorA - "+pa+"\nMotorB - "+pb);
  }*/
}
