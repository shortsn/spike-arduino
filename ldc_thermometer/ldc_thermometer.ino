#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

int temp_sensor = 0;
float temperatur = 0;

void setup() {
  analogReference(INTERNAL);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);

  temperatur = analogRead(temp_sensor) / 9.31;
  lcd.print("Temp: ");
  lcd.print(temperatur, 1);
  delay(1000);
}

