// THe test sketch uses about 1/6 of 32k memonry max, so additional modules and logic operations might need to be rationed bc of limited memory resources
#include <SPI.h>
#include <SoftwareSerial.h> // Serial Monitor lib

#include <Ultrasonic.h> // Ultrasonic Sensor lib - https://github.com/ErickSimoes/Ultrasonic

#include <protothreads.h>//
#include <pt-sleep.h>//
#include <pt-sem.h>//
//     ProtoThreads lib - https://gitlab.com/airbornemint/arduino-protothreads
#include <pt.h>//

#include <Servo.h> // Servo lib

Ultrasonic us1(2, 3); // US1 pins
Ultrasonic us2(4); // US2 pins
Ultrasonic us3(5); // US3 pins
  
Servo serv1;
Servo serv2;

// motor power functions
void fp(int inp) { // forward, more power
  digitalWrite(inp,HIGH);
}

void bp(int inp) { // backward, less power
  digitalWrite(inp,LOW);
}
       
       
void setup() {
  Serial.begin(9600);
  serv1.attach(12);
  serv2.attach(13);
}

void loop () {
  int pos1 = 0; // servo1 position
  int pos2 = 0; // servo2 position
  
  int mr1 = 8; // motor1 on board input4
  int mr2 = 9; // motor1 on board input3
  int ml1 = 10; // motor2 on board input2
  int ml2 = 11; // motor2 on board input1
       
       
  Serial.print("US-1: ");
  Serial.print(us1.read()); //log data to serial monitor
  Serial.println("cm");
  
  Serial.print("US-2: ");
  Serial.print(us2.read()); //log data to serial monitor
  Serial.println("cm");
  
  Serial.print("US-3: ");
  Serial.print(us3.read()); //log data to serial monitor
  Serial.println("cm");
  
  for(pos1 = 0; pos1 < 180; pos1 += 1)   
  {                                  
    serv1.write(pos1);              
    delay(15);                       
  }
  
  for(pos1 = 180; pos1 >= 1; pos1 -= 1)     
  {                                
    serv1.write(pos1);              
    delay(15);                     
  }
          
  for(pos2 = 0; pos2 < 180; pos2 += 1)  
  {                                 
    serv2.write(pos2);              
    delay(15);                       
  }
          
          
  for(pos2 = 180; pos2 >= 1; pos2 -= 1)     
  {                                
    serv2.write(pos2);               
    delay(15);                        
  }
  
  // forward
  fp(mr1);
  bp(mr2);
  fp(ml1);
  bp(ml2);
           
  // backward 
  bp(mr1);
  fp(mr2);
  bp(ml1);
  fp(ml2);
  
  // right
  bp(mr1);
  fp(mr2);
  fp(ml1);
  bp(ml2);
  
  // left
  fp(mr1);
  bp(mr2);
  bp(ml1);
  fp(ml2);
           
  // stop
  bp(mr1);
  bp(mr2);
  bp(ml1);
  bp(ml2);
}
