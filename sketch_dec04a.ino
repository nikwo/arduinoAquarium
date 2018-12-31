#include <SmartDelay.h>
#include <iarduino_RTC.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
iarduino_RTC t(RTC_DS1302,6,8,7);  //RST, CLK, DAT
const int leftBtn = 2;
const int rightBtn = 3;
const int upBtn = 4;
const int downBtn = 5;
const int okBtn = 11;
unsigned int timer;
unsigned int InterfaceTimer;
bool brightness,nightTime;
/*standart settings*/
volatile int NightBright = 5;
volatile int DayBright = 127;
volatile int MaxBright = 255;
volatile int Temperature = 24;

int prevMin;
int NowBright;
const int BrightnessPin = 9;
SmartDelay waitTime(60000000UL);

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
  if (page > 3){
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
  delay(300);
}


void firstPage(){
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print(t.gettime("d-m-Y"));
  lcd.setCursor(3,1);
  lcd.print(t.gettime("H:i D"));
  delay(300);
}
void secondPage(int Night,int Day){
  int Brightness;
  bool flag = false;
  int temp;
  int line = 0;
  int i;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LCD brightness");
  lcd.setCursor(15,1);
  lcd.write(byte(1));
  lcd.setCursor(0,1);
  lcd.print("options");
  if (digitalRead(okBtn)==HIGH){
    while(1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Day bright");
      lcd.setCursor(15,line);
      lcd.write(byte(1));
      lcd.setCursor(0,1);
      lcd.print("Night bright");
      delay(300);
      if (digitalRead(upBtn)==HIGH){
         --line;
         if (line<0){
          line = 1;
         }
      }
      if (digitalRead(downBtn)==HIGH){
         ++line;
         if (line>1){
          line = 0;
         }
      }
      switch (line){
        case 0:{
          lcd.setCursor(15,1);
          lcd.print(" ");
          lcd.setCursor(15,line);
          lcd.write(byte(1));
          break;
        }
        case 1:{
          lcd.setCursor(15,0);
          lcd.print(" ");
          lcd.setCursor(15,line);
          lcd.write(byte(1));
          break;
        }
      }
      if (digitalRead(okBtn)==HIGH){
        delay(300);
        if (line == 0){  // top line in lcd
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Set day bright ");
          lcd.setCursor(15,0);
          lcd.write(byte(2));
          for (i = 0;i < round((16*Day)/255);i++){
            lcd.setCursor(i,1);
            lcd.write(byte(3));
          }
          analogWrite(BrightnessPin,round((i*255)/16));
          while(1){
            if (digitalRead(okBtn)==HIGH){
              DayBright = round((i*255)/15);
              break;
            }
            if (digitalRead(upBtn)==HIGH){
              lcd.setCursor(i,1);
              lcd.write(byte(3));
              analogWrite(BrightnessPin,round((i*255)/16));
              ++i;                       
            }
            if (digitalRead(downBtn)==HIGH){
              --i;
              lcd.setCursor(i,1);
              lcd.write(" ");
              analogWrite(BrightnessPin,round((i*255)/16));
            }
            if (i >= 16){
              --i;
            }
            if (i < 0){
              ++i;
            }
            delay(300);
          }
      }
      if (line == 1){  // bottom line in lcd
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Setnightbright ");
          lcd.setCursor(15,0);
          lcd.write(byte(2));
          for (i = 0;i < round((16*Night)/255);i++){
            lcd.setCursor(i,1);
            lcd.write(byte(3));
          }
          analogWrite(BrightnessPin,round((i*255)/16));
          while(1){
            if (digitalRead(okBtn)==HIGH){
              NightBright = round((i*255)/15);
              break;
            }
            if (digitalRead(upBtn)==HIGH){
              lcd.setCursor(i,1);
              lcd.write(byte(3));
              analogWrite(BrightnessPin,round((i*255)/16));
              ++i;                       
            }
            if (digitalRead(downBtn)==HIGH){
              --i;
              lcd.setCursor(i,1);
              lcd.write(" ");
              analogWrite(BrightnessPin,round((i*255)/16));
            }
            if (i >= 16){
              --i;
            }
            if (i < 0){
              ++i;
            }
            delay(300);
          }
        }
        delay(300);
      }
     if (digitalRead(rightBtn)==HIGH || digitalRead(leftBtn)==HIGH){
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
  if (digitalRead(okBtn)==HIGH){
    delay(100);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set temperature");
    lcd.setCursor(0,1);
    lcd.print(temp);
    while(1){
      if (digitalRead(upBtn)==HIGH){
        delay(300);
        ++temp; 
        if(temp>=35){
          --temp;
        }
        lcd.setCursor(0,1);
        lcd.write("  ");
        lcd.setCursor(0,1);
        lcd.print(temp);
      }
      if (digitalRead(downBtn)==HIGH){
        delay(300);
        --temp; 
        if(temp<=20){
          ++temp;
        }
        lcd.setCursor(0,1);
        lcd.write("  ");
        lcd.setCursor(0,1);
        lcd.print(temp);
      }
      if (digitalRead(okBtn)==HIGH){
        Temperature = temp;
        delay(300);
        break;
      }
    }
  }
}
