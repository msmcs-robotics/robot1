#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>

const auto BAUD_RATE = 115200;
const auto HUB_I2C_ADDRESS = 8;
const auto LISTEN_PORT = 1496;

WiFiUDP Udp;

// cppcheck-suppress unusedFunction
void setup() {
    Serial.begin(BAUD_RATE);
    Wire.begin();

    if (!WiFi.softAP("MSMCS-Robotics-Robot1", "nottheactualpassword")) {
        Serial.println("Error: Starting AP failed");
    } else {
        Serial.println("Started AP");
    }

    if (Udp.begin(LISTEN_PORT) != 1) {
        Serial.println("Error: Binding to UDP socket failed");
    } else {
        Serial.println("Started UDP listener");
    }
}

// cppcheck-suppress unusedFunction
void loop() {
    auto packet_len = Udp.parsePacket();

    if (packet_len > 0) {
        unsigned char packet_data[packet_len];

        // There is no need to check the return value here since we know a
        // buffer is available after calling Udp.parsePacket().
        Udp.read(packet_data, packet_len);

        Wire.beginTransmission(HUB_I2C_ADDRESS);
        Wire.write(packet_data, packet_len);
        Wire.endTransmission();
    }
}
