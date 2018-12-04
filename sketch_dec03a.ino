#include <iarduino_RTC.h>


iarduino_RTC time(RTC_DS1302,10,8,9);
int xPin = A0;

int yPin = A1;

int buttonPin = 2;

int xPosition = 0;

int yPosition = 0;

int buttonState = 0;
void setup() {
    
    delay(300);
    Serial.begin(9600);
    time.begin(); //time initialization
    //the time already set

    pinMode(xPin, INPUT);

    pinMode(yPin, INPUT);

pinMode(buttonPin, INPUT_PULLUP);
}
void loop(){
     Serial.println(time.gettime("d m Y, H:i:s, D")); // showing real time
     Serial.println();
     delay(1000); 
     xPosition = analogRead(xPin);

     yPosition = analogRead(yPin);
     buttonState = digitalRead(buttonPin);

     

     delay(100); // добавляем задержку между считыванием данных
}
