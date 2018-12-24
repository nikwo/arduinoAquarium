#include <iarduino_RTC.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
iarduino_RTC t(RTC_DS1302,6,8,7);
const int ButtonPin1 = 2;
unsigned int timer;
bool brightness;

void setup(){
  pinMode(ButtonPin1,INPUT);
  Serial.begin(9600);
  t.begin();
  lcd.begin(16,2);
  lcd.clear();
  analogWrite(9,10);
  brightness = false; //false - small brightness
  timer = 0;
}
void loop(){
  if (digitalRead(ButtonPin1)==HIGH && !brightness){
    analogWrite(9,240);
    brightness = true;
  }
  if (brightness){
    ++timer;
  }
  if (timer >= 60 && brightness){
    analogWrite(9,10);
    brightness = false;
    timer=0;
  }
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print(t.gettime("d-m-Y"));
  lcd.setCursor(3,1);
  lcd.print(t.gettime("H:i D"));
  delay(1000);  
}
