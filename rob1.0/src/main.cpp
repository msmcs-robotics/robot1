
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


// US Functions


void fe() { // swivel the servo to do a broad "scan"

}


void re() { // set rules for scenarios for the right US 

}


void le() { // set rules for scenarios for the left US 

}



// Servo movement for Front US Functions

class swivel { // control servo movement
	
	private:

        bool act = false;

	public: 
	
	void sl() { // rotate to the left in respect to the robot orientation
          
	}

	void sr() { // rotate to the right in respect to the robot orientation

	}

        void su() { // cant servo2 upward
          
	}

	void sd() { // cant servo2 downward

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

// setup

void setup() {

}


// loop

void loop() {
  
  drive d;
  
  bool turnr = false;
  bool turnl = false;
  bool stp = false;
  bool go = true;
  
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
