#include <Arduino.h>

// Motion Sensor Pins
int us1s 2; // sig pin for us1
int us2s 3; // sig pin for us2
int us3e 4; // echo pin for us3 
int us3t 5; // trig pin for us3

// Motor Driver Pins
int m1p 8; // motor1 on board input4
int m1n 9; // motor1 on board input3
int mlp 10; // motor2 on board input2
int mlp 11; // motor2 on board input1

// Servo Pin
int s1 12; // Pin for servo data




// US Functions


void fe() { // swivel the servo to do a broad "scan"

}


void re() { // set rules for scenarios for the right US 

}


void le() { // set rules for scenarios for the left US 

}



// Servo for Front US Functions

class swivel { // control servo movement
	
	private:


	public: 
	
	void sl() { // rotate to the left in respect to the robot orientation

	}

	void sr() { // rotate to the right in respect to the robot orientation

	}

}

// Motor Functions

class drive { // control wheels

	private:
	

	public:

	void f() { // forward

	}

	void b() { // backward

	}

}



// setup

void setup() {

}


// loop

void loop() {
	
}