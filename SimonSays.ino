/*
    /////////////////////////////////////////////////////////////
   //														                              //
  //                        Simon Says                       //
 //														                            	//
/////////////////////////////////////////////////////////////
 */

//Uebung_01: Stammdaten


/*=========================================================
Description: einfaches Blink-Signal auf internes LED Platine
Author: M. Mühlethaler copy from C. Meier
Date: 21.10.2024
Version: V01.00.00
=========================================================*/

#include "arduino_config.h"


void setup() {
  //Serielle Schnittstelle aktivieren
  Serial.begin(9600);
  //Define pinModes Buttons
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(b4, INPUT_PULLUP);

  //Define pinModes LED
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
}

void loop() {
  checkButtonPressed();
}

//Funktion zum auslesen der Buttons
void checkButtonPressed(){
    for (int i = 0; i < (sizeof(buttonPins) / sizeof (buttonPins[0])); i++) {
    if (digitalRead(buttonPins[i]) == HIGH) {
      digitalWrite(ledPins[i],HIGH);
      Serial.print("Button ");
      Serial.print(i);
      Serial.println(" AN");
    } else {
      digitalWrite(ledPins[i],LOW);
      Serial.print("Button ");
      Serial.print(i);
      Serial.println(" AUS");
    }
  }
  delay(100);
}
