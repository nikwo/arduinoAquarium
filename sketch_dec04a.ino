#include <iarduino_RTC.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

iarduino_RTC t(RTC_DS1302,6,8,7);
void setup(){
Serial.begin(9600);
t.begin();
lcd.begin(16,2);
lcd.clear();

}
void loop(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(t.gettime("d-m-Y"));
  lcd.setCursor(3,1);
  lcd.print(t.gettime("H:i D"));
  delay(1000);  
}
