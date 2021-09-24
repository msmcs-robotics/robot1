// src-url: http://robotics.hobbizine.com/arduinoann.html


#include <Arduino.h>
#include <Ultrasonic.h> // lib link - https://github.com/JRodrigoTech/Ultrasonic-HC-SR04
//#include <pt.h>   // include protothread library - https://roboticsbackend.com/arduino-protothreads-tutorial/
#include <math.h>

/*
quick search for function names in order:
void setup() // setup
void pwmotor() // allocate power to motors
void toTerminal() // print to terminal
void InputToOutput // run inputs through nn-weights
void trainn() // train nn
void drivenn() // drive with nn
void loop() // run
*/

/******************************************************************
 * Network Configuration - customized per network 
 ******************************************************************/

// setup each protothread
//static struct pt pt1, pt2;

// Create ultrasonic sensor objects to use in logic
Ultrasonic us1(A0,A1); // (Trig PIN,Echo PIN)
Ultrasonic us2(A2,A3); // pinout, pinin
Ultrasonic us3(A4,A5);

// setup motor pin variables
int m4 = 11;
int m3 = 10;
int m2 = 6;
int m1 = 5;

// randomizer pin
int rpin = 2;

//warning LED
int LEDWARN = 13;

const int PatternCount = 8;
const int InputNodes = 3;
const int HiddenNodes = 4;
const int OutputNodes = 4;
const float LearningRate = 0.3;
const float Momentum = 0.9;
const float InitialWeightMax = 0.5;
const float Success = 0.0004;

// US-1 is in front
// US-2 is on the left
// US-3 is on the right

float Input[PatternCount][InputNodes] = {
  { 0,0,0 }, // 0 // not in 10cm
  { 1,0,0 }, // 1 // 10cm on 1
  { 0,1,0 }, // 2 // 10cm on 2
  { 0,0,1 }, // 3 // 10cm on 3
  { 1,1,0 }, // 4 // 10cm on 1,2
  { 0,1,1 }, // 5 // 10cm on 2,3
  { 1,0,1 }, // 6 // 10cm on 1,3
  { 1,1,1 }, // 7 // 10cm on 1,2,3
 
}; 

// motor input: 1, 2, 3, 4
// wheels are full power unless stopped, 
// so possible output is lower, and can
// integrate slowing down in later versions
const float Target[PatternCount][OutputNodes] = {
  { 0.2, 1, 0.2, 1 }, // go forward
  { 1, 0.2, 1, 0.2 }, // go backwards
  { 1, 0.2, 0.2, 1 }, // turn right
  { 0.2, 1, 1, 0.2 }, // turn left
  { 1, 0.2, 0.5, 0.2 }, // go back and turn right
  { 0.5, 1, 0.5, 1 }, // go forward but slower
  { 0.5, 0.2, 1, 0.2 }, // go back and turn left
  { 0, 0, 0, 0 } // do nothing, wait till clear 
};

/******************************************************************
 * End Network Configuration
 ******************************************************************/


int i, j, p, q, r;
int ReportEvery1000;
int RandomizedIndex[PatternCount];
long  TrainingCycle;
float Rando;
float Error;
float Accum;


float Hidden[HiddenNodes];
float Output[OutputNodes];
float HiddenWeights[InputNodes+1][HiddenNodes];
float OutputWeights[HiddenNodes+1][OutputNodes];
float HiddenDelta[HiddenNodes];
float OutputDelta[OutputNodes];
float ChangeHiddenWeights[InputNodes+1][HiddenNodes];
float ChangeOutputWeights[HiddenNodes+1][OutputNodes];

void setup() {
  Serial.begin(9600);
  // initialise protothread vars
  //PT_INIT(&pt1);
  //PT_INIT(&pt2);
  
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
  randomSeed(random(1,99));
  ReportEvery1000 = 1;
  for( p = 0 ; p < PatternCount ; p++ ) {    
    RandomizedIndex[p] = p ;
  }
}  

void pwmotor(int pw1, int pw2, int pw3, int pw4) {
  int dpw1 = map(pw1, 0, 100, 0, 255);
  dpw1 = constrain(dpw1, 0, 1023);
  analogWrite(m1, dpw1);
  
  int dpw2 = map(pw2, 0, 100, 0, 255);
  dpw2 = constrain(dpw2, 0, 1023);
  analogWrite(m2, dpw2);
  
  int dpw3 = map(pw3, 0, 100, 0, 255);
  dpw3 = constrain(dpw3, 0, 1023);
  analogWrite(m3, dpw3);
  
  int dpw4 = map(pw4, 0, 100, 0, 255);
  dpw4 = constrain(dpw4, 0, 1023);
  analogWrite(m4, dpw4);
}

void toTerminal() {

  for( p = 0 ; p < PatternCount ; p++ ) { 
    Serial.println(); 
    Serial.print ("  Training Pattern: ");
    Serial.println (p);      
    Serial.print ("  Input ");
    for( i = 0 ; i < InputNodes ; i++ ) {
      Serial.print (Input[p][i], DEC);
      Serial.print (" ");
    }
    Serial.print ("  Target ");
    for( i = 0 ; i < OutputNodes ; i++ ) {
      Serial.print (Target[p][i], DEC);
      Serial.print (" ");
    }
/******************************************************************
* Compute hidden layer activations
******************************************************************/

    for( i = 0 ; i < HiddenNodes ; i++ ) {    
      Accum = HiddenWeights[InputNodes][i] ;
      for( j = 0 ; j < InputNodes ; j++ ) {
        Accum += Input[p][j] * HiddenWeights[j][i] ;
      }
      Hidden[i] = 1.0/(1.0 + exp(-Accum)) ;
    }

/******************************************************************
* Compute output layer activations and calculate errors
******************************************************************/

    for( i = 0 ; i < OutputNodes ; i++ ) {    
      Accum = OutputWeights[HiddenNodes][i] ;
      for( j = 0 ; j < HiddenNodes ; j++ ) {
        Accum += Hidden[j] * OutputWeights[j][i] ;
      }
      Output[i] = 1.0/(1.0 + exp(-Accum)) ; 
    }
    Serial.print ("  Output ");
    for( i = 0 ; i < OutputNodes ; i++ ) {       
      Serial.print (Output[i], 5);
      Serial.print (" ");
    }
  }
}

void InputToOutput(float In1, float In2, float In3)
{
  float TestInput[] = {0, 0, 0, 0};
  TestInput[0] = In1;
  TestInput[1] = In2;
  TestInput[2] = In3;

  /******************************************************************
    Compute hidden layer activations
  ******************************************************************/

  for ( i = 0 ; i < HiddenNodes ; i++ ) {
    Accum = HiddenWeights[InputNodes][i] ;
    for ( j = 0 ; j < InputNodes ; j++ ) {
      Accum += TestInput[j] * HiddenWeights[j][i] ;
    }
    Hidden[i] = 1.0 / (1.0 + exp(-Accum)) ;
  }

  /******************************************************************
    Compute output layer activations and calculate errors
  ******************************************************************/

  for ( i = 0 ; i < OutputNodes ; i++ ) {
    Accum = OutputWeights[HiddenNodes][i] ;
    for ( j = 0 ; j < HiddenNodes ; j++ ) {
      Accum += Hidden[j] * OutputWeights[j][i] ;
    }
    Output[i] = 1.0 / (1.0 + exp(-Accum)) ;
  }
#ifdef DEBUG
  Serial.print ("  Output ");
  for ( i = 0 ; i < OutputNodes ; i++ ) {
    Serial.print (Output[i], 5);
    Serial.print (" ");
  }
#endif
}

void trainn() {
/******************************************************************
* Initialize HiddenWeights and ChangeHiddenWeights 
******************************************************************/

  for( i = 0 ; i < HiddenNodes ; i++ ) {    
    for( j = 0 ; j <= InputNodes ; j++ ) { 
      ChangeHiddenWeights[j][i] = 0.0 ;
      Rando = float(random(100))/100;
      HiddenWeights[j][i] = 2.0 * ( Rando - 0.5 ) * InitialWeightMax ;
    }
  }
/******************************************************************
* Initialize OutputWeights and ChangeOutputWeights
******************************************************************/

  for( i = 0 ; i < OutputNodes ; i ++ ) {    
    for( j = 0 ; j <= HiddenNodes ; j++ ) {
      ChangeOutputWeights[j][i] = 0.0 ;  
      Rando = float(random(100))/100;        
      OutputWeights[j][i] = 2.0 * ( Rando - 0.5 ) * InitialWeightMax ;
    }
  }
  Serial.println("Initial/Untrained Outputs: ");
  toTerminal();
/******************************************************************
* Begin training 
******************************************************************/

  for( TrainingCycle = 1 ; TrainingCycle < 2147483647 ; TrainingCycle++) {    

/******************************************************************
* Randomize order of training patterns
******************************************************************/

    for( p = 0 ; p < PatternCount ; p++) {
      q = random(PatternCount);
      r = RandomizedIndex[p] ; 
      RandomizedIndex[p] = RandomizedIndex[q] ; 
      RandomizedIndex[q] = r ;
    }
    Error = 0.0 ;
/******************************************************************
* Cycle through each training pattern in the randomized order
******************************************************************/
    for( q = 0 ; q < PatternCount ; q++ ) {    
      p = RandomizedIndex[q];

/******************************************************************
* Compute hidden layer activations
******************************************************************/

      for( i = 0 ; i < HiddenNodes ; i++ ) {    
        Accum = HiddenWeights[InputNodes][i] ;
        for( j = 0 ; j < InputNodes ; j++ ) {
          Accum += Input[p][j] * HiddenWeights[j][i] ;
        }
        Hidden[i] = 1.0/(1.0 + exp(-Accum)) ;
      }

/******************************************************************
* Compute output layer activations and calculate errors
******************************************************************/

      for( i = 0 ; i < OutputNodes ; i++ ) {    
        Accum = OutputWeights[HiddenNodes][i] ;
        for( j = 0 ; j < HiddenNodes ; j++ ) {
          Accum += Hidden[j] * OutputWeights[j][i] ;
        }
        Output[i] = 1.0/(1.0 + exp(-Accum)) ;   
        OutputDelta[i] = (Target[p][i] - Output[i]) * Output[i] * (1.0 - Output[i]) ;   
        Error += 0.5 * (Target[p][i] - Output[i]) * (Target[p][i] - Output[i]) ;
      }

/******************************************************************
* Backpropagate errors to hidden layer
******************************************************************/

      for( i = 0 ; i < HiddenNodes ; i++ ) {    
        Accum = 0.0 ;
        for( j = 0 ; j < OutputNodes ; j++ ) {
          Accum += OutputWeights[i][j] * OutputDelta[j] ;
        }
        HiddenDelta[i] = Accum * Hidden[i] * (1.0 - Hidden[i]) ;
      }


/******************************************************************
* Update Inner-->Hidden Weights
******************************************************************/


      for( i = 0 ; i < HiddenNodes ; i++ ) {     
        ChangeHiddenWeights[InputNodes][i] = LearningRate * HiddenDelta[i] + Momentum * ChangeHiddenWeights[InputNodes][i] ;
        HiddenWeights[InputNodes][i] += ChangeHiddenWeights[InputNodes][i] ;
        for( j = 0 ; j < InputNodes ; j++ ) { 
          ChangeHiddenWeights[j][i] = LearningRate * Input[p][j] * HiddenDelta[i] + Momentum * ChangeHiddenWeights[j][i];
          HiddenWeights[j][i] += ChangeHiddenWeights[j][i] ;
        }
      }

/******************************************************************
* Update Hidden-->Output Weights
******************************************************************/

      for( i = 0 ; i < OutputNodes ; i ++ ) {    
        ChangeOutputWeights[HiddenNodes][i] = LearningRate * OutputDelta[i] + Momentum * ChangeOutputWeights[HiddenNodes][i] ;
        OutputWeights[HiddenNodes][i] += ChangeOutputWeights[HiddenNodes][i] ;
        for( j = 0 ; j < HiddenNodes ; j++ ) {
          ChangeOutputWeights[j][i] = LearningRate * Hidden[j] * OutputDelta[i] + Momentum * ChangeOutputWeights[j][i] ;
          OutputWeights[j][i] += ChangeOutputWeights[j][i] ;
        }
      }
    }

/******************************************************************
* Every 1000 cycles send data to terminal for display
******************************************************************/
    ReportEvery1000 = ReportEvery1000 - 1;
    if (ReportEvery1000 == 0)
    {
      Serial.println(); 
      Serial.println(); 
      Serial.print ("TrainingCycle: ");
      Serial.print (TrainingCycle);
      Serial.print ("  Error = ");
      Serial.println (Error, 5);

      toTerminal();

      if (TrainingCycle==1)
      {
        ReportEvery1000 = 999;
      }
      else
      {
        ReportEvery1000 = 1000;
      }
    }    


/******************************************************************
* If error rate is less than pre-determined threshold then end
******************************************************************/

    if( Error < Success ) break ;  
  }
  Serial.println ();
  Serial.println(); 
  Serial.print ("TrainingCycle: ");
  Serial.print (TrainingCycle);
  Serial.print ("  Error = ");
  Serial.println (Error, 5);

  toTerminal();

  Serial.println ();  
  Serial.println ();
  Serial.println ("Training Set Solved! ");
  Serial.println ("--------"); 
  Serial.println ();
  Serial.println ();  
  ReportEvery1000 = 1;
}

void drivenn() {
Serial.println("Running NN Drive Test");
  if (Success < Error) {
    int prog_start = 0;
    Serial.println("NN not Trained");
  }
  while (Error < Success) {
    int num;
    int farDist = 35;
    int closeDist = 7;
    float TestInput[] = {0, 0, 0, 0};
    digitalWrite(LEDWARN, LOW);

    int LL1 = us1.Ranging(CM);   // Collect sonar distances.
    int LL2 = us2.Ranging(CM);   // Collect sonar distances.
    int LL3 = us3.Ranging(CM);   // Collect sonar distances.

#ifdef DEBUG
    Serial.print("Light Level: ");
    Serial.print(LL1);
    Serial.print("\t");
    Serial.print(LL2);
    Serial.print("\t");
    Serial.print(LL3);
    
#endif

    digitalWrite(LEDWARN, HIGH);

    LL1 = map(LL1, 2, 51, 0, 100);
    LL2 = map(LL2, 2, 51, 0, 100);
    LL3 = map(LL3, 2, 51, 0, 100);

    LL1 = constrain(LL1, 0, 100);
    LL2 = constrain(LL2, 0, 100);
    LL3 = constrain(LL3, 0, 100);

    TestInput[0] = float(LL1) / 100;
    TestInput[1] = float(LL2) / 100;
    TestInput[2] = float(LL3) / 100;
    
#ifdef DEBUG
    Serial.print("Input: ");
    Serial.print(TestInput[2], 2);
    Serial.print("\t");
    Serial.print(TestInput[1], 2);
    Serial.print("\t");
    Serial.println(TestInput[0], 2);
#endif

    InputToOutput(TestInput[0], TestInput[1], TestInput[2]); //INPUT to ANN to obtain OUTPUT

    int pw1 = Output[0];
    int pw2 = Output[1];
    int pw3 = Output[2];
    int pw4 = Output[3];
    
    pw1 = int(pw1);
    pw2 = int(pw2);
    pw3 = int(pw3);
    pw4 = int(pw4);
    
#ifdef DEBUG
    Serial.print("Speed: ");
    Serial.print(pw1);
    Serial.print("\t");
    Serial.println(pw2);
    Serial.print("\t");
    Serial.println(pw3);
    Serial.print("\t");
    Serial.println(pw4);
#endif
    pwmotor(pw1, pw2, pw3, pw4);
    delay(50);
  }
}

void loop() {
  
}
