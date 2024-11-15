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
#include "noten.h"


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

  //Define pinModes Buzzer
  pinMode(buzzer, OUTPUT);
}

void loop() {
  startupSeq();
  simonPlays();
  checkButtonPressed();
}

//Funktion zum auslesen der Buttons
void checkButtonPressed() {
  if (!comPlaying) {
    for (int i = 0; i < (sizeof(buttonPins) / sizeof(buttonPins[0])); i++) {  //Für alle Buttons
      if (digitalRead(buttonPins[i]) == HIGH) { //Wenn Button gedrückt
        digitalWrite(ledPins[i], HIGH);
        playTone(buttonTones[i]);
        playerSays[guess] = i;  //Füge dem Array playerSays die Gedrückte Taste hinzu
        Serial.print("PlayerSays: ");
        Serial.println(playerSays[guess]);
        Serial.print("SimonSays: ");
        Serial.println(simonSays[guess]);
        Serial.print("guess: ");
        Serial.println(guess);
        Serial.print("level: ");
        Serial.println(level);
        guess++; // Irgendwo hier ist noch ein Fehler!!!! Sagt zu früh gewonnen!
        if (playerSays[guess-1] = simonSays[guess-1]){
          if (guess == level){
          level++;
          Serial.println("gewonnen");
          delay(2000);
          comPlaying = true;
          simonPlays();
          guess = 0;
          }
        }
        else{
          Serial.println("verloren");
          guess = 0;
          level  = 0;
        }
        
        //Seriellausgabe zum testen ob das Array korrekt befüllt wird
        /*for(int i = 0; i <= guess; i++)
{
          Serial.print("Element: ");
          Serial.print(i);
          Serial.print(" : ");
          Serial.println(playerSays[i]);
        }*/
      } else {                          //Wenn nicht gedrückt
        digitalWrite(ledPins[i], LOW);  //LED ausschalten
      }
    }
    delay(100);
  }
}

void playTone(int note) {
  tone(buzzer, note, 400);
}

void simonPlays() {
  if (comPlaying) {
    int nextOne = random(0,4);
    simonSays[level-1] = nextOne;
    Serial.print("SimonSays: ");
    Serial.println(nextOne);
    for (int i = 0; i <= level-1; i++) {
      digitalWrite(ledPins[simonSays[i]], HIGH);
      playTone(buttonTones[simonSays[i]]);
      comPlaying = false;
      delay(simonSpeed);
      digitalWrite(ledPins[simonSays[i]], LOW);
    }
  }
}

void startupSeq() {
  if (startup) {
    delay(2000);
    Serial.println("Startup");
    startup = false;
  }
}


