#include <Arduino.h>
#include "time.hpp"
#include "stopwatch.hpp"
#include <atomic>

timeout read_data { msec(300) }; 
byte qrd[12];
byte sum_qrd = 0; 

void setup() {
    Serial.begin (1000000);
    Serial1.begin(1000000, SERIAL_8N1, 16, 17, false, 20000000UL ); // trida HardwareSerial (speed, config, Rx, Tx, invert, timeout )
    Serial.print ("Starting.../n");
    Serial.print ("RBC initialized/n");
    pinMode(9, OUTPUT); 
    digitalWrite(9,HIGH); // zapina napajeni pro IR na portu 12 az po nabootovani
    analogReadResolution(10); // nastavuje doporucenou hodnotu rozliseni prevodniku na 10 bitu 
    read_data.cancel();
}


static const uint8_t analog_pins[] = { 36, 39, 34, 35, 32, 33, 25, 26 ,27, 14, 12, 13 }; // zadni lista, predni lista, stred vpravo, stred vlevo 

void loop() 
{
    if (Serial.available()) {
        char c = Serial.read();
        switch (c)
        {
        case '\r':
            Serial.write('\n');
            break;
        case 'D':
            read_data.cancel();
            Serial.println("Debug output disabled");
            break;
        case 'd':
            read_data.start();
            Serial.println("Debug output enabled");
            break;
        default:
            Serial.write(c);
            break;
        }
    }
    for(int i = 0; i != 12; ++i)
        qrd[i] = analogRead(analog_pins[i])/4;
    if(Serial1.available() && Serial1.read() == 0x80) {
        Serial1.write(0x80); // hlavicka paketu
        
        Serial1.write(0x42); // hlavicka paketu
        sum_qrd = 0;
        for(int i = 0; i != 12; ++i) {
            Serial1.write(qrd[i]);
            sum_qrd = sum_qrd + qrd[i]; // tvorba checksum
        }
        Serial1.write(sum_qrd);
    }
    if (read_data) {
        read_data.ack();
        for(int i = 0; i != 12; ++i) {
            Serial.print(qrd[i]); Serial.print("  ");
        }
        Serial.print(sum_qrd);  Serial.print("  ");
        Serial.println (millis());
    }
}

