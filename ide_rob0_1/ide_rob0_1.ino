#include <SoftwareSerial.h> // Serial Monitor lib

#include <Ultrasonic.h> // Ultrasonic Sensor lib - https://github.com/ErickSimoes/Ultrasonic

#include <protothreads.h>//
#include <lc-switch.h>//
#include <pt-sleep.h>//
#include <pt-sem.h>//
#include <lc-addrlabels.h>//      ProtoThreads lib - https://gitlab.com/airbornemint/arduino-protothreads
#include <pt.h>//
#include <lc.h>//

#include <Servo.h> // Servo lib


// #include <TimedAction.h>

/* Pins are defind in the classes dedicated to the module functions

Ex: class drive{} corresponds to the motors, therefore the pins connecting to the motor driver board are defined there


// Motion Sensor Pins

int us1s = 2; // sig pin for us1
int us2s = 3; // sig pin for us2
int us3e = 4; // echo pin for us3 
int us3t = 5; // trig pin for us3

// Motor Driver Pins
int mr1 = 8; // motor1 on board input4
int mr2 = 9; // motor1 on board input3
int ml1 = 10; // motor2 on board input2
int ml2 = 11; // motor2 on board input1

// Servo Pin
int sl = 12; // Pin for servo1
int sr = 13; // Pin for servo2
*/


// Protothreading for:
// Motor1
// Motor2
// Servos
// US data collection
// Logic

static struct pt pt1, pt2; // each protothread needs one of these


// US Functions, default unit of measurement is cm
class us {
    private:
    
    Ultrasonic us1(2, 3); // US1 pins
    Ultrasonic us2(4); // US2 pins
    Ultrasonic us3(5); // US3 pins
    
    public:
    
    void s1(){ 
      //collect data on US1
      Serial.print("US-1: ");
      Serial.print(us1.read()); //log data to serial monitor
      Serial.println("cm");
    }
    
    void s2(){
      //collect data on US2
      Serial.print("US-2: ");
      Serial.print(us2.read()); //log data to serial monitor
      Serial.println("cm");
    }
    
    void s3(){
      //collect data on US3
      Serial.print("US-3: ");
      Serial.print(us3.read()); //log data to serial monitor
      Serial.println("cm");
    }
};



// Servo movement for Front US Functions

class swivel: public us { // control servo movement
	
	private:

        //Setup servo objects & attach serv1 to pin12, and serv2 to pin13
        
        Servo serv1;
        Servo serv2;
        
        serv1.attach(12);
        serv2.attach(13);
        
        int pos1 = 0;
        int pos2 = 0;
        

	public: 
	
	void sl() { // rotate to the left in respect to the robot orientation
          for(pos1 = 0; pos1 < 180; pos1 += 1)  // goes from 0 degrees to 180 degrees 
          {                                  // in steps of 1 degree 
            serv1.write(pos1);              // tell servo to go to position in variable 'pos' 
            delay(15);                       // waits 15ms for the servo to reach the position 
          }
	}

	void sr() { // rotate to the right in respect to the robot orientation
          for(pos1 = 180; pos1 >= 1; pos1 -= 1)     // goes from 180 degrees to 0 degrees 
          {                                
            serv1.write(pos1);              // tell servo to go to position in variable 'pos' 
            delay(15);                       // waits 15ms for the servo to reach the position 
          }
	}

        void su() { // cant servo2 upward
          for(pos2 = 0; pos2 < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
          {                                  // in steps of 1 degree 
            serv2.write(pos2);              // tell servo to go to position in variable 'pos' 
            delay(15);                       // waits 15ms for the servo to reach the position 
          }
	}

	void sd() { // cant servo2 downward
          for(pos2 = 180; pos2 >= 1; pos2 -= 1)     // goes from 180 degrees to 0 degrees 
          {                                
            serv2.write(pos2);              // tell servo to go to position in variable 'pos' 
            delay(15);                       // waits 15ms for the servo to reach the position 
          }
	}

};



// Motor Functions

class drive: public swivel{ // allocate power to wheels, and creat proportions of power
       
       private:
       
       int mr1 = 8; // motor1 on board input4
       int mr2 = 9; // motor1 on board input3
       int ml1 = 10; // motor2 on board input2
       int ml2 = 11; // motor2 on board input1
       
       // basic power
       
       void fp(int inp) { // forward, more power
            digitalWrite(inp,HIGH);
       }

       void bp(int inp) { // backward, less power
            digitalWrite(inp,LOW);
       }
       
       
       public:
        
       // forward & backward
       
       void f() {
           fp(mr1);
           bp(mr2);
           fp(ml1);
           bp(ml2);
       }
       
       void b() {
           bp(mr1);
           fp(mr2);
           bp(ml1);
           fp(ml2);
       }
       
       // turning
       
       void r() {
           bp(mr1);
           fp(mr2);
           fp(ml1);
           bp(ml2);
       }
       
       void l() {
           fp(mr1);
           bp(mr2);
           bp(ml1);
           fp(ml2);
       }
       
       // stopping 
       
       void s(){
           bp(mr1);
           bp(mr2);
           bp(ml1);
           bp(ml2);
       }
    
};

// loop

void drivelogic() {
  
  drive d;
  
  // Motor logic (dictate motor movement with )
  bool turnr = false;
  bool turnl = false;
  bool stp = false;
  bool go = true;
  
  // motor movement
  d.f();
  
  if (turnr = true){
    d.r();
    delay(100);
    d.f(); 
  }
  
  if (turnl = true){
    d.l();
    delay(100);
    d.f();
  }
  
  if (stp = true){
    d.s();
    delay(100);
    d.f();
  }
}


void servlogic() {
  
  // Servo Logic (dictate servo swivel)
  bool rserv = false;
  bool lserv = false;
  bool userv = false;
  bool dserv = false;
  
  // Servo movement
  if (rserv = true){
    swivel.sl();
  }
  
  if (lserv = true){
    swivel.sr();
  }
  
  if (userv = true){
    swivel.su();
  }
  
  if (dserv = true){
    swivel.sd();
  }
  
}

// setup

void setup() {
  Serial.begin(9600);
  PT_INIT(&pt1);  // initialise the two
  PT_INIT(&pt2);  // protothread variables
}

void loop() {
  protothread1(&pt1, 900); // schedule the two protothreads
  protothread2(&pt2, 1000); // by calling them infinitely
}
