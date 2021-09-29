#include <Arduino.h>
#include <Ultrasonic.h> // lib link - https://github.com/JRodrigoTech/Ultrasonic-HC-SR04
#include <pt.h>   // include protothread library - https://roboticsbackend.com/arduino-protothreads-tutorial/

//Wifi Module uses x pins, adding later

// setup each protothread
static struct pt pt1, pt2;

// Create ultrasonic sensor objects to use in logic
Ultrasonic us1(A0,A1); // (Trig PIN,Echo PIN) pinout, pinin
Ultrasonic us2(A2,A3); // on the left
Ultrasonic us3(A4,A5); // on teh right

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
}

// the infinite loop to run on the arduino
void loop()
{

  
  // schedule threads indefinitely
  prot1(&pt1);
  prot2(&pt2);
}

static int prot1(struct pt *pt) {
  PT_BEGIN(pt);
  if (digitalRead(m1) != 0 || digitalRead(m2) != 1 || digitalRead(m3) != 0 || digitalRead(m4) != 1) {
  forw();
  }
  PT_END(pt);
}

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
}
  // Allocate voltage to move backwards
void back() {
  digitalWrite(m3, HIGH);// left wheel back
  digitalWrite(m4, LOW);//
  digitalWrite(m1, HIGH);// right wheel back ward
  digitalWrite(m2, LOW);//

}
  // Turn right
void rturn() {
  digitalWrite(m1, HIGH);// right wheel back ward
  digitalWrite(m2, LOW);//
  digitalWrite(m3, LOW);// left wheel forward
  digitalWrite(m4, HIGH);//
}
  // Turn left
void lturn() {
  digitalWrite(m3, HIGH);// left wheel back
  digitalWrite(m4, LOW);//
  digitalWrite(m1, LOW);// right wheel forward
  digitalWrite(m2, HIGH);//  
}

