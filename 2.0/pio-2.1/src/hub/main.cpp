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
 - Moving the sliders up and down gives values on a range of {0,255}
 - Up is forward, down is backward - up is towards 255, down is towards 0
 - Values less than 175 (about halfway between 0,255) will result in the wheels
   changing direction in order to move backwards
 
The math statements:
 - 255-lv/175 * 255
 Get lv as a percentage of 175, multiply that by 255 to put it on a scale of 255,
 and then subtract that result from 255 in order for the slider in the app
 to not go from near zero to full power when passing the 175 (halfway) mark.
*/
// Function to write High/Low(x4) and PWM values to motor pins
void drive(int rv, int lv) {
    digitalWrite(m1, rv > 0 ? HIGH : LOW);
    digitalWrite(m2, rv < 0 ? HIGH : LOW);
    digitalWrite(m3, lv > 0 ? HIGH : LOW);
    digitalWrite(m4, lv < 0 ? HIGH : LOW);

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