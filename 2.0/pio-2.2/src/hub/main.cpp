#include <Arduino.h>
#include <Wire.h>
#include <pb_decode.h>
#include <Ultrasonic.h> // lib link - https://github.com/JRodrigoTech/Ultrasonic-HC-SR04
#include <SD.h>

#include "ref.pb.h"


// I2C packets
const auto BAUD_RATE = 115200;
const auto I2C_ADDRESS = 8;
void handle_packet(int packet_len);

// setup SD card pin
const int chipSelect = 4;

// Ultrasonic Sensors
Ultrasonic us1(A0,A1); // (Trig PIN,Echo PIN)
Ultrasonic us2(A2,A3); // pinout, pinin
Ultrasonic us3(A4,A5);

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
    
    //sd card setup
    pinMode(10, OUTPUT);
    if (!SD.begin(chipSelect)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
    return;
    }
}

// Function to write High/Low(x4) and PWM values to motor pins
void drive(uint8_t rv, uint8_t lv) {
    digitalWrite(m1, rv > 0 ? HIGH : LOW);
    digitalWrite(m2, rv < 0 ? HIGH : LOW);
    digitalWrite(m3, lv > 0 ? HIGH : LOW);
    digitalWrite(m4, lv < 0 ? HIGH : LOW);

    analogWrite(p1, abs(rv));
    analogWrite(p2, abs(lv));

    File dataFile = SD.open("manual-data.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(rv, " , ", lv);
      dataFile.close();
      // print to the serial port too:
      Serial.println("...");
    } else {
      Serial.println("error opening manual-data.txt");
    }
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
        // map PWM to a scale of 0 to 100
        drive(voltage.rv, voltage.lv);
    }
}

void calc() {

  int u1 = us1.Ranging(CM);
  int u2 = us2.Ranging(CM);
  int u3 = us3.Ranging(CM);
  
  int rv;
  int lv;
  
  if (u1 <= 20) {
    rv = map(u1, 1, 51, 0, 255);
    lv = map(u1, 1, 51, 0, 255);
  } else if (u2 <= 20) {
    rv = map(u1, 1, 51, 0, 255);
    lv = map(u1, 1, 51, 0, 255);
  } else if (u3 <= 20) {
    rv = map(u3, 1, 51, 0, 255);
    lv = map(u3, 1, 51, 0, 255);
  } else {
    rv = 255;
    lv = 255;
  }
  digitalWrite(m1, rv > 175? HIGH : LOW);
  digitalWrite(m2, rv < 175 ? HIGH : LOW);
  digitalWrite(m3, lv > 175 ? HIGH : LOW);
  digitalWrite(m4, lv < 175 ? HIGH : LOW);
  analogWrite(p1, abs(rv));
  analogWrite(p2, abs(lv));

    File dataFile = SD.open("auto-data.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(rv, " , ", lv);
      dataFile.close();
      // print to the serial port too:
      Serial.println("...");
    } else {
      Serial.println("error opening auto-data.txt");
    }
  
}



void loop(){
     //calc()
}