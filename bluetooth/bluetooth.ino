#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 7);
// creates a "virtual" serial port/UART
// connect BT module TX to D10
// connect BT module RX to D11
// connect BT Vcc to 5V, GND to GND

char myChar;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() // run over and over
{
  while (mySerial.available() )  {
    myChar = mySerial.read();
    Serial.print(myChar);
  }
  while (Serial.available() )  {
    myChar = Serial.read();
    mySerial.print(myChar);
  }
}

