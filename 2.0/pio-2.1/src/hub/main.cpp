#include <Arduino.h>
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

/*

This will be a type of skid-steering

In app: 
 - Have two sliders, one for each motor.
 - Moving the sliders up and down gives values on a range of {-255,255}
 - Up is forward, down is backward - up is towards 255, down is towards -255
 - Values less than 0 (halfway between -255,255) will result in the wheels
   changing direction in order to move backwards

Ex: Send 255 on right slider

m1 = Low
            > Right motor moves forward
m2 = High


Ex: Send -255 on right slider

m1 = High
            > Right motor moves backward
m2 = Low

*/

// Function to write High/Low(x4) and PWM values to motor pins
void drive(int rv, int lv) {
    digitalWrite(m1, rv > 0 ? LOW : HIGH);
    digitalWrite(m2, rv < 0 ? LOW : HIGH);
    digitalWrite(m3, lv > 0 ? LOW : HIGH);
    digitalWrite(m4, lv < 0 ? LOW : HIGH);

    analogWrite(p1, abs(rv));
    analogWrite(p2, abs(lv));
}

// Decode the packet recieved from the ESP8266mod
void handle_packet(int packet_len) {
    unsigned char packet_data[packet_len];

    for (auto i = 0; i < packet_len; i++) {
        packet_data[i] = Wire.read();
    }

    MotorVoltage voltage = MotorVoltage_init_zero;
    auto stream = pb_istream_from_buffer(packet_data, packet_len);
    if (!pb_decode(&stream, &MotorVoltage_msg, &voltage)) {
        Serial.print("Warning: Message decoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
    } else {
        drive(voltage.rv, voltage.lv);
    }
}

// need this just bc for compilation
void loop(){}