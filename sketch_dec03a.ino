#include <iarduino_RTC.h>


iarduino_RTC time(RTC_DS1302,10,8,9);
int xPin = A1;

int yPin = A0;

int buttonPin = 2;

int xPosition = 0;

int yPosition = 0;

int buttonState = 0;
void setup() {
    
    delay(300);
    Serial.begin(9600);
    time.begin(); //time initialization
    //the time chip already set

    pinMode(xPin, INPUT);

    pinMode(yPin, INPUT);

// активируем подтягивающий резистор на пине кнопки

pinMode(buttonPin, INPUT_PULLUP);
}
void loop(){
     //Serial.println(time.gettime("d m Y, H:i:s, D")); // showing real time
     //Serial.println();
     //delay(1000); 
     xPosition = analogRead(xPin);

     yPosition = analogRead(yPin);
     buttonState = digitalRead(buttonPin);

     Serial.print("X: ");

     Serial.print(xPosition);
     Serial.print(" | Y: ");

     Serial.print(yPosition);

     Serial.print(" | Button: ");

     Serial.println(buttonState);

     delay(100); // добавляем задержку между считыванием данных
}
