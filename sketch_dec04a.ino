
#include <LiquidCrystal_I2C.h>
#include <iarduino_RTC.h>
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
#define Filter 17

unsigned int timer;
bool brightness, nightTime;
/*standart settings*/
int8_t NightBright = 5;
int8_t DayBright = 127;
int8_t MaxBright = 255;
int8_t Temperature = 24;

uint8_t HourLOn = 7;
uint8_t MinuteLOn = 0;
uint8_t HourLOff = 22;
uint8_t MinuteLOff = 0;

bool FilterIsOn = true;

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
void fourthPage(int8_t &, int8_t &, int8_t &, int8_t &);
void fifthPage(bool &);
/* Time dependent procedures (feeder,heater,light,filter)*/
void TurnHeaterOn(int, int);
void TurnHeaterOff(int, int);

void TurnLightOn(int8_t, int8_t);
void TurnLightOff(int8_t, int8_t);

void FilterControl(bool);

void setup()
{
    pinMode(Heater, OUTPUT);
    pinMode(Light, OUTPUT);
    pinMode(Filter, OUTPUT);
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

    if (page > 5)
    {
        page = 1;
    }
    if (page <= 0)
    {
        page = 5;
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
    case 4:
    {
        fourthPage(HourLOn, MinuteLOn, HourLOff, MinuteLOff);
        break;
    }
    case 5:
    {
        fifthPage(FilterIsOn);
        break;
    }
    }  
    delay(300);
    TurnHeaterOn(temperature, Temperature);
    TurnHeaterOff(temperature, Temperature);
    TurnLightOn(atoi(t.gettime("H")), atoi(t.gettime("i")), HourLOn, MinuteLOn, HourLOff, MinuteLOff);
    TurnLightOff(atoi(t.gettime("H")), atoi(t.gettime("i")), HourLOn, MinuteLOn, HourLOff, MinuteLOff);
    FilterControl(FilterIsOn);
}

void firstPage()
{
    int8_t line = 0;
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
                int8_t Hour = atoi(t.gettime("H"));
                int8_t Minutes = atoi(t.gettime("i"));
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
                        if (Minutes < 0)
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
    short line = 0;
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
void fourthPage(uint8_t &LH_On, uint8_t &LM_On, uint8_t &LH_Off, uint8_t &LM_Off)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Light");
    lcd.setCursor(0, 1);
    lcd.print("options");
    if(digitalRead(okBtn)==HIGH){
        short line = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        if(LH_On < 10){
            lcd.print("Time On: 0");
        }else lcd.print("Time On: ");
        lcd.print(LH_On);
        lcd.print(":");
        if(LM_On < 10){
            lcd.print("0");
            lcd.print(LM_On);
        }else lcd.print(LM_On);
        lcd.setCursor(0, 1);
        if(LH_Off < 10){
            lcd.print("Time Off: 0");
        }else lcd.print("Time Off: ");
        lcd.print(LH_Off);
        lcd.print(":");
        if(LM_Off < 10){
            lcd.print("0");
            lcd.print(LM_Off);
        }else lcd.print(LM_Off);
        delay(300);
        while(1){
            lcd.setCursor(15, line);
            lcd.write(byte(1));
            if (digitalRead(downBtn) == HIGH)
            {
                --line;
                if (line < 0)
                {
                    line = 1;
                }
            }
            if (digitalRead(upBtn) == HIGH)
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
            if(digitalRead(okBtn) == HIGH){
                break;
            }
            delay(300);
        }
        if(line == 0){
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Hour Light On");
            lcd.setCursor(15, 0);
            lcd.write(byte(2));
            int8_t Hour = LH_On;
            int8_t Minutes = LM_On;
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
                         Hour = 0;
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
                    if (Minutes < 0)
                    {
                        Minutes = 0;
                    }
                    lcd.setCursor(0, 1);
                    lcd.write("  ");
                    lcd.setCursor(0, 1);
                    lcd.print(Minutes);
                }
                if (digitalRead(okBtn) == HIGH)
                {
                    LH_On = Hour;
                    LM_On = Minutes;
                    break;
                }
                delay(300);
            }
        }
        if(line == 1){
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Hour Light Off");
            lcd.setCursor(15, 0);
            lcd.write(byte(2));
            int8_t Hour = LH_Off;
            int8_t Minutes = LM_Off;
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
                         Hour = 0;
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
                    if (Minutes < 0)
                    {
                        Minutes = 0;
                    }
                    lcd.setCursor(0, 1);
                    lcd.write("  ");
                    lcd.setCursor(0, 1);
                    lcd.print(Minutes);
                }
                if (digitalRead(okBtn) == HIGH)
                {
                    LH_Off = Hour;
                    LM_Off = Minutes;
                    break;
                }
                delay(300);
            }
        }
    }
    
}
void fifthPage(bool &flag){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Filter");
    lcd.setCursor(0,1);
    if(flag){
        lcd.print("NOW IS ON");
    }else lcd.print("NOW IS OFF");
    if (digitalRead(okBtn) == HIGH){
        flag = !flag;
    }
}

void TurnHeaterOn( int Temp, int nowTemp){
    if(nowTemp < Temp){
        analogWrite( Heater, 255);
    }
}
void TurnHeaterOff( int Temp, int nowTemp){
    if(nowTemp > Temp){
        analogWrite( Heater, 0);
    }
}
void TurnLightOn(int8_t HOUR, int8_t MINUTE, int8_t HON, int8_t MINON, int8_t HOFF, int8_t MINOFF){
    if(HOUR >= HON && HOUR < HOFF){
        if(MINUTE >= MINON && HOUR == HON){
            analogWrite(Light, 0);
        }
        if(HOUR >= HON){
            analogWrite(Light, 0);
        }
    }
}
void TurnLightOff(int8_t HOUR, int8_t MINUTE, int8_t HON, int8_t MINON, int8_t HOFF, int8_t MINOFF){
    if(HOUR == HOFF){
        if(MINUTE >= MINOFF){
            analogWrite(Light, 255);
        }
    }
    if(HOUR > HOFF){
        analogWrite(Light, 255);
    }
}
void FilterControl(bool flag){
    Serial.print(flag);
    switch(flag){
        case true:
        {
            digitalWrite(Filter, LOW);
            break;
        }
        case false:
        {
            digitalWrite(Filter, HIGH);
            break;
        }
    }
}
