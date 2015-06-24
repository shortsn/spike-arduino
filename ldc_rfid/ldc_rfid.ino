#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// creates a "virtual" serial port/UART
// connect BT module TX to D8
// connect BT module RX to D7
// connect BT Vcc to 5V, GND to GND
SoftwareSerial bluetooth(8, 7);

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9
#define SS_PIN          10

MFRC522 mfrc522(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;

bool connected = false;

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  SPI.begin();
  mfrc522.PCD_Init();

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  reset();
}


void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() && connected) {
    reset();
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  if (!connected) {
    connected = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
            &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
            &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      lcd.print(F("Card not supported"));
      return;
    }

    lcd.print(mfrc522.PICC_GetTypeName(piccType));
    lcd.setCursor(0, 1);

    String id = array_to_string(mfrc522.uid.uidByte, mfrc522.uid.size);
    lcd.print(id);
    if (bluetooth) {
      Serial.println("send bluetooth");
      bluetooth.println(id);
    }

    //byte buffer[16];

    //String(F("Hallohallohallo!")).getBytes(buffer, 16);

    //Write(1, buffer);
    //Read(1);


    mfrc522.PICC_HaltA();
    //mfrc522.PCD_StopCrypto1();
  }
}

void Read(byte block) {
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));

  byte buffer[18];
  byte size = sizeof(buffer);
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }

  Serial.println((char*)buffer);
}

void Write(byte block, byte buffer[]) {
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));

  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));
}

void reset() {
  connected = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("RFID"));
  lcd.setCursor(0, 1);
  lcd.print(array_to_string(key.keyByte, MFRC522::MF_KEY_SIZE));
}

String array_to_string(byte *buffer, byte bufferSize) {
  String result = "";
  for (byte i = 0; i < bufferSize; i++) {
    result += String(buffer[i], HEX);
  }
  return result;
}
