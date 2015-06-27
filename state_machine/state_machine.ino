#include <FiniteStateMachine.h>
// Display
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// RFID
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN  10

#define CARD_NOT_SUPPORTED 1
#define TIMEOUT 2
#define COMMUNICATION_FAILED 3

#define SOH 0x01
#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define NAK 0x15

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// Bluetooth
#include <SoftwareSerial.h>

// connect BT module TX to D8
// connect BT module RX to D7
// connect BT Vcc to 5V, GND to GND
SoftwareSerial bluetooth(8, 7);

State Idle = State(EnterIdle, WaitForNewCard, no_op);
State Connect = State(IdentifyCard, WaitForAck, no_op);

FSM state = FSM(Idle);

unsigned long timestamp;

void setup() {
  bluetooth.begin(9600);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  state.update();
}

void no_op() {}

void EnterIdle() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ready");
}

void WaitForNewCard() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    state.transitionTo(Connect);
  }
}

void IdentifyCard() {
  lcd.clear();
  lcd.setCursor(0, 0);
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

  if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Error(CARD_NOT_SUPPORTED);
    return;
  }

  lcd.print(F("Card ID:"));
  lcd.setCursor(0, 1);

  String id = BufferToString(mfrc522.uid.uidByte, mfrc522.uid.size);
  lcd.print(id);
  sendBuffer(mfrc522.uid.uidByte, mfrc522.uid.size);
  timestamp = millis();
}


void WaitForAck() {
  if (abs(timestamp - millis()) > 5000) {
    Error(TIMEOUT);
    return;
  }

  while (bluetooth.available() > 0) {
    switch (bluetooth.read()) {
      case ACK:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ACK FROM SERVER");
        delay(2000);
        state.transitionTo(Idle);
        return;
      case NAK:
        Error(COMMUNICATION_FAILED);
        return;
    }
  }
}

String BufferToString(byte *buffer, byte bufferSize) {
  String result;
  for (byte i = 0; i < bufferSize; i++) {
    result += String(buffer[i], HEX);
  }
  return result;
}

void sendBuffer(byte *buffer, byte size) {
  bluetooth.write(SOH);
  bluetooth.write(STX);
  bluetooth.write(mfrc522.uid.uidByte, mfrc522.uid.size);
  bluetooth.write(ETX);
}

void Error(byte code) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Error ");
  lcd.print(code);
  state.transitionTo(Idle);
  delay(2000);
}
