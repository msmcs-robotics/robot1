#include <Arduino.h>
#include <Ultrasonic.h> // lib link - https://github.com/JRodrigoTech/Ultrasonic-HC-SR04
//#include <pt.h>   // include protothread library - https://roboticsbackend.com/arduino-protothreads-tutorial/
#include <SD.h> // Standard SD card lib copied from the Arduino Framework
#include <Wire.h>
#include <pb_decode.h>

#include "ref.pb.h"

const auto BAUD_RATE = 115200;
const auto I2C_ADDRESS = 8;

void handle_packet(int packet_len);

// setup motor pin variables
int m1 = 3;
int m2 = 7;
int m3 = 8;
int m4 = 9;
int p1 = 5;
int p2 = 6;

// setup SD card pin and log files
const int chipSelect = 4;

//setup Motor pin voltage variables
boolean a;
boolean b; // bools will be high and low
boolean c;
boolean d;
int pa;
int pb; // ints will be for PWM

// setup each protothread
//static struct pt pt1;

// Create ultrasonic sensor objects to use in logic
Ultrasonic us1(A0,A1); // (Trig PIN,Echo PIN)
Ultrasonic us2(A2,A3); // pinout, pinin
Ultrasonic us3(A4,A5);


void setup() {
    Serial.begin(BAUD_RATE);
    Wire.begin(I2C_ADDRESS);
    Wire.onReceive(handle_packet);

    //setup for SD card
    pinMode(10, OUTPUT);
    if (!SD.begin(chipSelect)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
  
    //initialize Protothread
    //PT_INIT(&pt1);
  
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
}

// Function to write High/Low(x4) and PWM values to motor pins
void dm(boolean a, boolean b, boolean c, boolean d, int pa, int pb) {
  digitalWrite(m1, a);
  digitalWrite(m2, b);
  digitalWrite(m3, c);
  digitalWrite(m4, d);
  analogWrite(p1, pa);
  analogWrite(p2, pb);
}

// Decode the packet recieved from the ESP8266mod
void handle_packet(int packet_len) {
    File packetFile = SD.open("packetlog.txt", FILE_WRITE);
    unsigned char packet_data[packet_len];

    for (auto i = 0; i < packet_len; i++) {
        packet_data[i] = Wire.read();
    }

    MotorVals mvals = MotorVals_init_zero;
    auto stream = pb_istream_from_buffer(packet_data, packet_len);
    if (!pb_decode(&stream, &MotorVals_msg, &mvals)) {
        Serial.print("Warning: Message decoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
    } else {
        // pass args to dictate-movement function
        dm(mvals.m1, mvals.m2, mvals.m3, mvals.m4, mvals.pa, mvals.pb);
    }

    // log to SD card
    if (packetFile) {
        packetFile.println("Packet recieved!!!");
        packetFile.close();
        // print to the serial port too:
        Serial.println("Packet recieved!!!");
    } else {
        Serial.println("error opening packetlog.txt");
    }
}

// The normal struct to read from to check US distances
void checkd(int check) {
    if (check == 1) {
        File dataFile = SD.open("datalog.txt", FILE_WRITE);
        if (us1.Ranging(CM) <= 10) {
          Serial.print("WARNING, going back");
            a = HIGH;
            b = LOW;
            c = HIGH;
            d = LOW;
            pa = 255;
            pb = 255;
            if (dataFile) {
              dataFile.println("back");
              dataFile.close();
              // print to the serial port too:
              Serial.println("back");
            } else {
              Serial.println("error opening datalog.txt");
            }
            dm(a, b, c, d, pa, pb);
            delay(10);
        } else if (us2.Ranging(CM) <= 10) {
            Serial.print("WARNING, going right");;
            a = HIGH;
            b = LOW;
            c = LOW;
            d = HIGH;
            pa = 255;
            pb = 255;
            if (dataFile) {
              dataFile.println("right");
              dataFile.close();
              // print to the serial port too:
              Serial.println("right");
            } else {
              Serial.println("error opening datalog.txt");
            }
            dm(a, b, c, d, pa, pb);
            delay(10);
        } else if (us3.Ranging(CM) <= 10) {
            Serial.print("WARNING, going left");
            a = LOW;
            b = HIGH;
            c = HIGH;
            d = LOW;
            pa = 255;
            pb = 255;
            if (dataFile) {
              dataFile.println("left");
              dataFile.close();
              // print to the serial port too:
              Serial.println("left");
            } else {
              Serial.println("error opening datalog.txt");
            }
            dm(a, b, c, d, pa, pb);
            delay(10);
        } else {
            Serial.print("Moving...");
            a = LOW;
            b = HIGH;
            c = LOW;
            d = HIGH;
            pa = 255;
            pb = 255;
            if (dataFile) {
              dataFile.println("...");
              dataFile.close();
              // print to the serial port too:
              Serial.println("...");
            } else {
              Serial.println("error opening datalog.txt");
            }
            dm(a, b, c, d, pa, pb);
        }
    }
}

void loop() {
  checkd(1);
}
