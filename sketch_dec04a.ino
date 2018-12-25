#include <SmartDelay.h>
#include <iarduino_RTC.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
iarduino_RTC t(RTC_DS1302,6,8,7);
const int leftBtn = 2;
const int rightBtn = 3;
const int upBtn = 4;
const int downBtn = 5;
const int okBtn = 11;
unsigned int timer;
unsigned int InterfaceTimer;
bool brightness,nightTime;
int NightBright = 5;
int DayBright = 127;
int MaxBright = 255;
int prevMin;
int NowBright;
const int BrightnessPin = 9;
SmartDelay waitTime(20000000UL);

int page;
byte point[8] = {
0b00000,
0b00000,
0b00100,
0b01110,
0b11111,
0b01110,
0b00100,
0b00000
};

byte UpDown[8] =
{
0b00100,
0b01110,
0b11111,
0b00000,
0b00000,
0b11111,
0b01110,
0b00100
};

byte ProgressLine[8] =
{
0b11111,
0b11111,
0b11111,
0b11111,
0b11111,
0b11111,
0b11111,
0b11111
};


void firstPage();
void secondPage(int , int );
void thirdPage();

void setup(){
  
  pinMode(leftBtn,INPUT);
  pinMode(rightBtn,INPUT);
  Serial.begin(9600);
  t.begin();
  lcd.begin(16,2);
  lcd.createChar(1,point);
  lcd.createChar(2,UpDown);
  lcd.createChar(3,ProgressLine);
  lcd.clear();
  analogWrite(9,10);
  brightness = false; //false - small brightness
  timer = 0;
  page = 1;
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print(t.gettime("d-m-Y"));
  lcd.setCursor(3,1);
  lcd.print(t.gettime("H:i D"));
}
void loop(){
  prevMin = atoi(t.gettime("i"));
  if (page >= 3){
    page = 1;
  }
  if (page <= 0){
    page = 3;
  }
  
  if (atoi(t.gettime("H")) >= 22 || atoi(t.gettime("H")) <= 7){
    nightTime = true; // true means that is a night now
  }
  else nightTime = false;
  if (nightTime = true){
    NowBright = NightBright;
  }
  else NowBright = DayBright;
  if ((digitalRead(leftBtn)==HIGH || digitalRead(rightBtn)==HIGH )){
    analogWrite(BrightnessPin,MaxBright);
    brightness = true;
  }
  if (brightness){
    ++timer;
  }
  if (timer >= 60 && brightness){
    analogWrite(BrightnessPin,NowBright);
    brightness = false;
    timer=0;
  }
  if (digitalRead(rightBtn)==HIGH){
    ++page;
  }
  if (digitalRead(leftBtn)==HIGH){
    --page;
  }
  switch(page){
    case 1:{
      firstPage();
      break;
    }
    case 2:{
      secondPage(NightBright,DayBright);
      break;
    }
    case 3:{
      thirdPage();
      break;
    }
  }
  Serial.println(NightBright);
  delay(300);
}


void firstPage(){
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print(t.gettime("d-m-Y"));
  lcd.setCursor(3,1);
  lcd.print(t.gettime("H:i D"));
}
void secondPage(int Night,int Day){
  bool flag = false;
  int temp;
  if (nightTime){
    temp = (int)(Night/255*16);
  }else temp = (int)(Day/255*16);
  
  int i;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LCD brightness");
  lcd.setCursor(15,1);
  lcd.write(byte(1));
  lcd.setCursor(0,1);
  lcd.print("options");
  if (digitalRead(upBtn)==HIGH || digitalRead(downBtn)==HIGH){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set brightness ");
    lcd.write(byte(2));
    i = 0;
    Serial.println(temp);
    while(1){
      if (i>=temp){
        break;
      }
      lcd.setCursor(i,1);
      lcd.write(byte(3));
      Serial.print(1);
      ++i;
    }
    i = 0;  
    while(1){
      if (( digitalRead(upBtn)==HIGH ) && ( i < 16 ) && nightTime){
        lcd.setCursor(i,1);
        lcd.write(byte(3));
        ++i;
      }
      if (( digitalRead(upBtn)==HIGH ) && ( i < 16 ) && !nightTime){
        lcd.setCursor(i,1);
        lcd.write(byte(3));
        ++i;
      }
      delay(300);
      if ((digitalRead(okBtn)==HIGH) && nightTime){         
        NightBright = (int)(i/16*255);
        Serial.println(NightBright);
        break;
      }
      if ((digitalRead(okBtn)==HIGH) && !nightTime){
        DayBright = round(i/16*255);
        break;
      }
    }
  }
}
void thirdPage(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temperature");
  lcd.setCursor(15,1);
  lcd.write(byte(1));
  lcd.setCursor(0,1);
  lcd.print("options");
}
