#include <IRremote.h>
#include <LiquidCrystal.h>
#include "dht.h"
#define motorPin 11
#define dht_apin A0 

// Author: Donald Calhoun

dht DHT;

// Declaring constants for each pin I will use on arduino
const int rs = 12, en = 10, d4 = 5, d5 = 4, d6 = 3, d7 = 2, RECV_PIN = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int j = 0;
int curr = 0;
int prevCurr = 0;
int temp = 95;
IRrecv irrecv(RECV_PIN);
decode_results results;


void setup() {
  Serial.begin(9600);
  delay(500);
  irrecv.enableIRIn();
  irrecv.blink13(true);

  //Starting up the temperature measuring
  DHT.read11(dht_apin);
  int farh = (((DHT.temperature)/5)*9)+32;

  pinMode(motorPin, OUTPUT);
  analogWrite(motorPin, 0);

  //Starting up lcd screen
  lcd.begin(16, 2);
  lcd.print("Set Temp: ");
  lcd.print(temp);
  lcd.print("f");
  
  lcd.setCursor(0,1);
  lcd.print("Current: ");
  lcd.print(farh);
  lcd.print("f");
}

void loop() {
  int i = 0;
  String readString;
  DHT.read11(dht_apin);  //Reading the temp
  int farh = (((DHT.temperature)/5)*9)+32;  //Converting temp to farhenheit from celsius

  //Turning off fans if temp is below what I set it to
  if(farh <= temp){
      analogWrite(motorPin, map(0, 0, 9, 0, 255));
  }

  //Updates the lcd screen after running loop 5 times or every 500 miliseconds
  if(curr == (prevCurr+5)){
    prevCurr = curr;
    lcd.setCursor(0,1);
    lcd.print("Current: ");
    lcd.print(farh);
    lcd.print("f");
    lcd.setCursor(0,0);
    lcd.print("Set Temp: ");
    lcd.print(temp);
    lcd.print("f");
  }

    //While the temp is greater than what I have it set to the fans run
    while(farh > temp){
      analogWrite(motorPin, map(9, 0, 9, 0, 255));
      DHT.read11(dht_apin);
      int farh = (((DHT.temperature)/5)*9)+32;
      break;
    }

  //Allows me to use remote with IR sensor to update the set temperature
  if (irrecv.decode(&results)) {
    Serial.println(results.value, DEC);
    switch(results.value){
      //Pressing the up button on remote increases the temp
      case 0xFFE01F:
        temp = temp - 1;
        break;
      //Pressing the down button on remote decreases the temp
      case 0xFF906F:
        temp = temp + 1;
        break;
    }
    irrecv.resume(); // Receive the next value
  }

  curr = curr + 1; //Adds 1 to the counter
  delay(100);
}
