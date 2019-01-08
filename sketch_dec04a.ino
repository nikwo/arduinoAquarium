
#include <iarduino_RTC.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10
OneWire bus(ONE_WIRE_BUS);

DallasTemperature sensors(&bus);

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
iarduino_RTC t(RTC_DS1302, 6, 8, 7); //RST, CLK, DAT
/* keypad */
#define leftBtn 2
#define rightBtn 3
#define upBtn 4
#define downBtn 5
#define okBtn 11

#define Light 15
#define Heater 14

bool HeatOn = false;
bool LightOn = false;

unsigned int timer;
bool brightness, nightTime;
/*standart settings*/
uint8_t NightBright = 5;
uint8_t DayBright = 127;
uint8_t MaxBright = 255;
uint8_t Temperature = 24;
char TimeLightOn[4] = {'0', '7', '0', '0'};
char TimeLightOff[4] = {'2', '2', '0', '0'};

int temperature;
char temperatureString[6] = "-";

int prevMin;
int NowBright;
const int BrightnessPin = 9;

int page;
byte point[8] = {
    0b00000,
    0b00000,
    0b00100,
    0b01110,
    0b11111,
    0b01110,
    0b00100,
    0b00000};

byte UpDown[8] =
    {
        0b00100,
        0b01110,
        0b11111,
        0b00000,
        0b00000,
        0b11111,
        0b01110,
        0b00100};

byte ProgressLine[8] =
    {
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111};

void firstPage();
void secondPage(int, int);
void thirdPage(int);
void fourthPage();
/* Time dependent procedures (feeder,heater,light,filter)*/
void TurnHeaterOn(int, int);
void TurnHeaterOff(int, int);

void TurnLightOn();
void TurnLightOff();

void TurnFilterOn();
void TurnFilterOff();

void setup()
{
    pinMode(Heater, OUTPUT);
    pinMode(Light, OUTPUT);
    sensors.begin();
    pinMode(leftBtn, INPUT);
    pinMode(rightBtn, INPUT);
    Serial.begin(9600);
    t.begin();
    lcd.begin(16, 2);
    lcd.createChar(1, point);
    lcd.createChar(2, UpDown);
    lcd.createChar(3, ProgressLine);
    lcd.clear();
    analogWrite(BrightnessPin, 10);
    brightness = false; //false - small brightness
    timer = 0;
    page = 1;
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print(t.gettime("d-m-Y"));
    lcd.setCursor(3, 1);
    lcd.print(t.gettime("H:i D"));
}
void loop()
{
    sensors.requestTemperatures();
    temperature = (int)sensors.getTempCByIndex(0);

    
    if (atoi(t.gettime("H")) >= 22 || atoi(t.gettime("H")) <= 7)
    {
        nightTime = true; // true means that is a night now
    }
    else
        nightTime = false;
    if (nightTime)
    {
        NowBright = NightBright;
    }
    else
        NowBright = DayBright;
    if ((digitalRead(leftBtn) == HIGH || digitalRead(rightBtn) == HIGH))
    {
        analogWrite(BrightnessPin, MaxBright);
        brightness = true;
    }
    if (brightness)
    {
        ++timer;
    }
    if (timer >= 60 && brightness)
    {
        analogWrite(BrightnessPin, NowBright);
        brightness = false;
        timer = 0;
    }
    if (digitalRead(rightBtn) == HIGH)
    {
        ++page;
    }
    if (digitalRead(leftBtn) == HIGH)
    {
        --page;
    }

    if (page > 3)
    {
        page = 1;
    }
    if (page <= 0)
    {
        page = 3;
    }

    switch (page)
    {
    case 1:
    {
        firstPage();
        break;
    }
    case 2:
    {
        secondPage(NightBright, DayBright);
        break;
    }
    case 3:
    {
        thirdPage(Temperature);
        break;
    }
    }
    delay(300);
    TurnHeaterOn(temperature, Temperature);
    TurnHeaterOff(temperature, Temperature);
}

void firstPage()
{
    int line = 0;
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print(t.gettime("d-m-Y"));
    lcd.setCursor(3, 1);
    lcd.print(t.gettime("H:i D"));
    delay(300);
    if (digitalRead(okBtn) == HIGH)
    {
        lcd.setCursor(15, 0);
        lcd.write(byte(1));            
        delay(300);
        while (1)
        {
            if (digitalRead(upBtn) == HIGH)
            {
                --line;
                if (line < 0)
                {
                    line = 1;
                }
            }
            if (digitalRead(downBtn) == HIGH)
            {
                ++line;
                if (line > 1)
                {
                    line = 0;
                }
            }
            if (digitalRead(okBtn) == HIGH)
            {
                break;
            }
            switch (line)
            {
            case 0:
            {
                lcd.setCursor(15, 1);
                lcd.write(" ");
                lcd.setCursor(15, line);
                lcd.write(byte(1));
                break;
            }
            case 1:
            {
                lcd.setCursor(15, 0);
                lcd.write(" ");
                lcd.setCursor(15, line);
                lcd.write(byte(1));
                break;
            }
            }
        }
        if (digitalRead(okBtn) == HIGH)
        {
            if (line == 0)
            {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Set year");
                lcd.setCursor(15, 0);
                lcd.write(byte(2));
                lcd.setCursor(0, 1);
                int Year = atoi(t.gettime("Y"));
                uint8_t Month = atoi(t.gettime("m"));
                uint8_t Day = atoi(t.gettime("d"));
                lcd.print(Year);
                delay(300);
                while (1)
                {
                    if (digitalRead(upBtn) == HIGH)
                    {
                        ++Year;
                        if (Year > 2099)
                        {
                            --Year;
                        }
                        lcd.setCursor(0, 1);
                        lcd.write("    ");
                        lcd.setCursor(0, 1);
                        lcd.print(Year);
                    }
                    if (digitalRead(downBtn) == HIGH)
                    {
                        --Year;
                        if (Year < 2000)
                        {
                            ++Year;
                        }
                        lcd.setCursor(0, 1);
                        lcd.write("    ");
                        lcd.setCursor(0, 1);
                        lcd.print(Year);
                    }
                    if (digitalRead(okBtn) == HIGH)
                    {
                        break;
                        delay(300);
                    }
                    delay(300);
                }
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Set month");
                lcd.setCursor(15, 0);
                lcd.write(byte(2));
                lcd.setCursor(0, 1);
                lcd.print(Month);
                delay(300);
                while (1)
                {
                    if (digitalRead(upBtn) == HIGH)
                    {
                        ++Month;
                        if (Month > 12)
                        {
                            --Month;
                        }
                        lcd.setCursor(0, 1);
                        lcd.write("  ");
                        lcd.setCursor(0, 1);
                        lcd.print(Month);
                    }
                    if (digitalRead(downBtn) == HIGH)
                    {
                        --Month;
                        if (Month < 1)
                        {
                            ++Month;
                        }
                        lcd.setCursor(0, 1);
                        lcd.write("  ");
                        lcd.setCursor(0, 1);
                        lcd.print(Month);
                    }
                    if (digitalRead(okBtn) == HIGH)
                    {
                        break;
                    }
                    delay(300);
                }
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Set day");
                lcd.setCursor(15, 0);
                lcd.write(byte(2));
                lcd.setCursor(0, 1);
                lcd.print(Day);
                delay(300);
                while (1 == 1)
                {
                    if (digitalRead(upBtn) == HIGH)
                    {
                        ++Day;
                        switch (Month)
                        {
                        case 2:
                        {
                            if (Year % 4 == 0)
                            {
                                if (Day > 29)
                                {
                                    --Day;
                                }
                            }
                            else if (Day > 28)
                            {
                                --Day;
                            }
                        }
                        break;
                        case 4:
                        case 6:
                        case 9:
                        case 11:
                        {
                            if (Day > 30)
                            {
                                --Day;
                            }
                            break;
                        }
                        default:
                        {
                            if (Day > 31)
                            {
                                --Day;
                            }
                            break;
                        }
                        }    
                        lcd.setCursor(0, 1);
                        lcd.write("  ");
                        lcd.setCursor(0, 1);
                        lcd.print(Day);
                    }
                    if (digitalRead(downBtn) == HIGH)
                    {
                        --Day;
                        if (Day < 1)
                        {
                            ++Day;
                        }
                        lcd.setCursor(0, 1);
                        lcd.write("  ");
                        lcd.setCursor(0, 1);
                        lcd.print(Day);
                    }
                    if (digitalRead(okBtn) == HIGH)
                    {
                        t.settime(-1, -1, -1, Day, Month, Year - 2000);
                        break;
                    }
                    delay(300);
                }
            }
            if (line == 1)
            {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Set hour");
                lcd.setCursor(15, 0);
                lcd.write(byte(2));
                uint8_t Hour = atoi(t.gettime("H"));
                uint8_t Minutes = atoi(t.gettime("i"));
                lcd.setCursor(0, 1);
                lcd.print(Hour);
                delay(300);
                while (1)
                {
                    if (digitalRead(upBtn) == HIGH)
                    {
                        ++Hour;
                        if (Hour > 23)
                        {
                            --Hour;
                        }
                        lcd.setCursor(0, 1);
                        lcd.write("  ");
                        lcd.setCursor(0, 1);
                        lcd.print(Hour);
                    }
                    if (digitalRead(downBtn) == HIGH)
                    {
                        --Hour;
                        if (Hour < 0)
                        {
                            ++Hour;
                        }
                        lcd.setCursor(0, 1);
                        lcd.write("  ");
                        lcd.setCursor(0, 1);
                        lcd.print(Hour);
                    }
                    if (digitalRead(okBtn) == HIGH)
                    {
                        break;
                    }
                    delay(300);
                }
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Set minutes");
                lcd.setCursor(15, 0);
                lcd.write(byte(2));
                lcd.setCursor(0, 1);
                lcd.print(Minutes);
                delay(300);
                while (1)
                {
                    if (digitalRead(upBtn) == HIGH)
                    {
                        ++Minutes;
                        if (Minutes > 59)
                        {
                            --Minutes;
                        }
                        lcd.setCursor(0, 1);
                        lcd.write("  ");
                        lcd.setCursor(0, 1);
                        lcd.print(Minutes);
                    }
                    if (digitalRead(downBtn) == HIGH)
                    {
                        --Minutes;
                        if (Minutes < 0
                        {
                            ++Minutes;
                        }
                        lcd.setCursor(0, 1);
                        lcd.write("  ");
                        lcd.setCursor(0, 1);
                        lcd.print(Minutes);
                    }
                    if (digitalRead(okBtn) == HIGH)
                    {
                        t.settime(-1, Minutes, Hour, -1, -1, -1);
                        break;
                    }
                    delay(300);
                }
            }
        }
    }
}
void secondPage(int Night, int Day)
{
    int Brightness;
    bool flag = false;
    int temp;
    int line = 0;
    int i;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LCD brightness");
    lcd.setCursor(15, 1);
    lcd.write(byte(1));
    lcd.setCursor(0, 1);
    lcd.print("options");
    if (digitalRead(okBtn) == HIGH)
    {
        while (1)
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Day bright");
            lcd.setCursor(15, line);
            lcd.write(byte(1));
            lcd.setCursor(0, 1);
            lcd.print("Night bright");
            delay(300);
            if (digitalRead(upBtn) == HIGH)
            {
                --line;
                if (line < 0)
                {
                    line = 1;
                }
            }
            if (digitalRead(downBtn) == HIGH)
            {
                ++line;
                if (line > 1)
                {
                    line = 0;
                }
            }
            switch (line)
            {
            case 0:
            {
                lcd.setCursor(15, 1);
                lcd.print(" ");
                lcd.setCursor(15, line);
                lcd.write(byte(1));
                break;
            }
            case 1:
            {
                lcd.setCursor(15, 0);
                lcd.print(" ");
                lcd.setCursor(15, line);
                lcd.write(byte(1));
                break;
            }
            }
            if (digitalRead(okBtn) == HIGH)
            {
                delay(300);
                if (line == 0)
                { // top line in lcd
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Set day bright ");
                    lcd.setCursor(15, 0);
                    lcd.write(byte(2));
                    for (i = 0; i < round((16 * Day) / 255); i++)
                    {
                        lcd.setCursor(i, 1);
                        lcd.write(byte(3));
                    }
                    analogWrite(BrightnessPin, round((i * 255) / 16));
                    while (1)
                    {
                        if (digitalRead(okBtn) == HIGH)
                        {
                            DayBright = round((i * 255) / 15);
                            break;
                        }
                        if (digitalRead(upBtn) == HIGH)
                        {
                            lcd.setCursor(i, 1);
                            lcd.write(byte(3));
                            analogWrite(BrightnessPin, round((i * 255) / 16));
                            ++i;
                        }
                        if (digitalRead(downBtn) == HIGH)
                        {
                            --i;
                            lcd.setCursor(i, 1);
                            lcd.write(" ");
                            analogWrite(BrightnessPin, round((i * 255) / 16));
                        }
                        if (i >= 16)
                        {
                            --i;
                        }
                        if (i < 0)
                        {
                            ++i;
                        }
                        delay(300);
                    }
                }
                if (line == 1)
                { // bottom line in lcd
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Setnightbright ");
                    lcd.setCursor(15, 0);
                    lcd.write(byte(2));
                    for (i = 0; i < round((16 * Night) / 255); i++)
                    {
                        lcd.setCursor(i, 1);
                        lcd.write(byte(3));
                    }
                    analogWrite(BrightnessPin, round((i * 255) / 16));
                    while (1)
                    {
                        if (digitalRead(okBtn) == HIGH)
                        {
                            NightBright = round((i * 255) / 15);
                            break;
                        }
                        if (digitalRead(upBtn) == HIGH)
                        {
                            lcd.setCursor(i, 1);
                            lcd.write(byte(3));
                            analogWrite(BrightnessPin, round((i * 255) / 16));
                            ++i;
                        }
                        if (digitalRead(downBtn) == HIGH)
                        {
                            --i;
                            lcd.setCursor(i, 1);
                            lcd.write(" ");
                            analogWrite(BrightnessPin, round((i * 255) / 16));
                        }
                        if (i >= 16)
                        {
                            --i;
                        }
                        if (i < 0)
                        {
                            ++i;
                        }
                        delay(300);
                    }
                }
                delay(300);
            }
            if (digitalRead(rightBtn) == HIGH || digitalRead(leftBtn) == HIGH)
            {
                break;
            }
        }
    }
}
void thirdPage(int Temp)
{
    int temp = Temp;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temperature: ");
    lcd.setCursor(13, 0);
    lcd.print(temperature);
    lcd.setCursor(15, 1);
    lcd.write(byte(1));
    lcd.setCursor(0, 1);
    lcd.print("options");
    if (digitalRead(okBtn) == HIGH)
    {
        delay(300);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set temperature");
        lcd.setCursor(0, 1);
        lcd.print(temp);
        while (1)
        {
            if (digitalRead(upBtn) == HIGH)
            {
                ++temp;
                if (temp >= 35)
                {
                    --temp;
                }
                lcd.setCursor(0, 1);
                lcd.write("  ");
                lcd.setCursor(0, 1);
                lcd.print(temp);
            }
            if (digitalRead(downBtn) == HIGH)
            {
                --temp;
                if (temp <= 20)
                {
                    ++temp;
                }
                lcd.setCursor(0, 1);
                lcd.write("  ");
                lcd.setCursor(0, 1);
                lcd.print(temp);
            }
            if (digitalRead(rightBtn) == HIGH || digitalRead(leftBtn) == HIGH)
            {
                break;
            }
            if (digitalRead(okBtn) == HIGH)
            {
                Temperature = temp;
                delay(300);
                break;
            }
            delay(300);
        }
    }
}
void fourthPage()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Light On ");
    lcd.print(TimeLightOn);
    lcd.setCursor(0, 1);
    lcd.print("Light Off ");
    lcd.print(TimeLightOff);
}

void TurnHeaterOn( int Temp, int nowTemp){
    if(nowTemp < Temp){
      digitalWrite( Heater, HIGH);
    }
}
void TurnHeaterOff( int Temp, int nowTemp){
    if(nowTemp > Temp){
      digitalWrite( Heater, LOW);
    }
}
