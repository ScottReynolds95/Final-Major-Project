/*
* Arduino Wireless Communication Tutorial
*     Example 2 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define button1 3
#define led1 2
RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
boolean button1State = 0; // this arduinos button
boolean buttonState = 0; // other arduinos button
void setup() {
  pinMode(button1, INPUT);
  pinMode(2, OUTPUT);
  radio.begin();
  radio.openWritingPipe(addresses[0]); // 00002
  radio.openReadingPipe(1, addresses[1]); // 00001
  radio.setPALevel(RF24_PA_MIN);
}
void loop() {
  delay(5);
  radio.startListening();
  if ( radio.available()) {
    while (radio.available()) ;
    radio.read(&buttonState, sizeof(buttonState));
    if (buttonState == HIGH) {
      digitalWrite(led1, HIGH);
    }
    else {
      digitalWrite(led1, LOW);
    }
}
    delay(5);
    radio.stopListening();
    button1State = digitalRead(button1);
    radio.write(&button1State, sizeof(button1State));
}
  //      int angleV = 0;
//      radio.read(&angleV, sizeof(angleV));
//      myServo.write(angleV);
//    }

