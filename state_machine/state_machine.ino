#include <FiniteStateMachine.h>
// Display
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

State Offline = State(offline);
State Online = State(online);

FSM state = FSM(Offline);

void setup() {
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
}

void loop() {
  state.update();
}

void offline() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("offline");
}

void online() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("online");
}
