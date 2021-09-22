#include <Arduino.h>
#include <Ultrasonic.h> 

//US1 is in front
//US2 is facing left
//US3 is facing right
Ultrasonic us1(A0,A1); // (Trig PIN,Echo PIN)
Ultrasonic us2(A2,A3); // pinout, pinin
Ultrasonic us3(A4,A5);

int m4 = 13;
int m3 = 12;
int m2 = 11;
int m1 = 10;


void setup() {    
  Serial.begin(9600);  
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
}

void loop()
{
  if (digitalRead(m1) != 0 || digitalRead(m2) != 1 || digitalRead(m3) != 0 || digitalRead(m4) != 1) {
  forw();
  } else {}
  Serial.print("US-1 " + us1.Ranging(CM)); // CM or INC
  Serial.println(" cm" );
  delay(10);
  Serial.print("US-2 " + us2.Ranging(CM)); // CM or INC
  Serial.println(" cm" );
  delay(10);
  Serial.print("US-3 " + us3.Ranging(CM)); // CM or INC
  Serial.println(" cm" );
  delay(10);
  
  if (us1.Ranging(CM) <= 10) {
    Serial.print("WARNING");
    back();
    delay(200);
  } else if (us2.Ranging(CM) <= 10) {
    Serial.print("WARNING");;
    rturn();
    delay(200);
  } else if (us3.Ranging(CM) <= 10) {
    Serial.print("WARNING");
    lturn();
    delay(200);
  } else {}
}

void forw() {
  digitalWrite(m1, LOW);// right wheel forward
  digitalWrite(m2, HIGH);//
  
  digitalWrite(m3, LOW);// left wheel forward
  digitalWrite(m4, HIGH);//
}

void back() {
  digitalWrite(m3, HIGH);// left wheel back
  digitalWrite(m4, LOW);//
  
  digitalWrite(m1, HIGH);// right wheel back ward
  digitalWrite(m2, LOW);//

}

void rturn() {
  digitalWrite(m1, HIGH);// right wheel back ward
  digitalWrite(m2, LOW);//
  digitalWrite(m3, LOW);// left wheel forward
  digitalWrite(m4, HIGH);//
}

void lturn() {
  digitalWrite(m3, HIGH);// left wheel back
  digitalWrite(m4, LOW);//

  digitalWrite(m1, LOW);// right wheel forward
  digitalWrite(m2, HIGH);//  
}
