#include <Arduino.h>
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

//setup Motor pin voltage variables
int mv;
int pwmA;
int pwmB;


void setup() {
    Serial.begin(BAUD_RATE);
    Wire.begin(I2C_ADDRESS);
    Wire.onReceive(handle_packet);
  
    // motors (digital pins)
    pinMode(m1, OUTPUT);     
    pinMode(m2, OUTPUT);
    pinMode(m3, OUTPUT);
    pinMode(m4, OUTPUT);
    pinMode(p1, OUTPUT);
    pinMode(p2, OUTPUT);
}

// Function to write High/Low(x4) and PWM values to motor pins

void dm(int mv, int pa, int pb) {
  if (mv == 1) {
    //go forward
    digitalWrite(m1, LOW);// right wheel forward
    digitalWrite(m2, HIGH);//
    digitalWrite(m3, LOW);// left wheel forward
    digitalWrite(m4, HIGH);//
  } else if (mv == 2) {
    // go back
    digitalWrite(m3, HIGH);// left wheel back
    digitalWrite(m4, LOW);//
    digitalWrite(m1, HIGH);// right wheel back 
    digitalWrite(m2, LOW);//
  } else if (mv == 3) {
    // turn right
    digitalWrite(m1, HIGH);// right wheel back ward
    digitalWrite(m2, LOW);//
    digitalWrite(m3, LOW);// left wheel forward
    digitalWrite(m4, HIGH);//
  } else if (mv == 4) {
    // turn left
    digitalWrite(m3, HIGH);// left wheel back
    digitalWrite(m4, LOW);//
    digitalWrite(m1, LOW);// right wheel forward
    digitalWrite(m2, HIGH);//
  }
  analogWrite(p1, pwmA); //write speed to Right motor
  analogWrite(p2, pwmB); //write speed to Left motor
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
        dmm(mvals.imv, mvals.ipwmA, mvals.ipwmB);
    }
}

void loop() {
}




















/*void dm(boolean a, boolean b, boolean c, boolean d, int pa, int pb) {
  digitalWrite(m1, a);
  digitalWrite(m2, b);
  digitalWrite(m3, c);
  digitalWrite(m4, d);
  analogWrite(p1, pa);
  analogWrite(p2, pb);
}
*/