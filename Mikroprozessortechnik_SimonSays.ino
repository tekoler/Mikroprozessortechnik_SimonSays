/*
    /////////////////////////////////////////////////////////////
   //														                              //
  //                        Simon Says                       //
 //														                            	//
/////////////////////////////////////////////////////////////
 */


/*=========================================================
Description: einfaches Blink-Signal auf internes LED Platine
Author: M. Mühlethaler copy from C. Meier
Date: 17.11.2024
Version: V01.03.00
=========================================================*/

/*=========================================================
Updates in this Version:
-Startup Sequenz mit Ton und LED
-Button debouncer mit Millis
-Winning und Losing Ton und LED Sequenz
=========================================================*/


/*=========================================================
ToDo:
- Display mit Levelanzeige
- Randomfunktion optimieren
- Evtl. Display mit Motivationssprüchen
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

  currentMillis = millis();
  startupSeq();
  simonPlays();
  checkButtonPressed();
  
}

//Funktion zum auslesen der Buttons
void checkButtonPressed() {
  if (!comPlaying) {
    if (currentMillis - buttonMillis >= buttonPeriod) {
      for (int i = 0; i < (sizeof(buttonPins) / sizeof(buttonPins[0])); i++) {  //Für alle Buttons
        if (digitalRead(buttonPins[i]) == LOW) {
          buttonMillis = millis();
          Serial.print("Buttonpressed ");  //Wenn Button gedrückt
          digitalWrite(ledPins[i], HIGH);
          playTone(buttonTones[i]);
          playerSays[guess] = i;  //Füge dem Array playerSays die Gedrückte Taste hinzu
          Serial.print("PlayerSays: ");
          Serial.println(playerSays[guess]);
          delay(500);
          digitalWrite(ledPins[i], LOW);
          delay(200);
          if (playerSays[guess] == simonSays[guess] == 1) {  //Abfrage ob der Schritt richtig war
            guess++;
            Serial.println("Gut geraten");
            if (guess == level) {  //Abrage ob die richtige Anzahl an Schritten erreicht wurde
              // Ja -> Next Level
              Serial.println("  Next Level  ");
              winning();
              guess = 0;
              level++;
              comPlaying = true;
              delay(2000);
              playerSays[100] = {};
              simonPlays();
            } else {

              //Nein -> Warten auf weitere Eingabe
              Serial.println("Weiter gehts");
            }
          } else {
            Serial.println("verloren");
            losing();
            guess = 0;
            level = 1;
            playerSays[100] = {};
            simonSays[100] = {};
            startup = true;
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
}

void playTone(int note) {
  tone(buzzer, note, 400);
}

void simonPlays() {
  if (comPlaying) {
    int nextOne = random(0, 4);
    simonSays[level - 1] = nextOne;
    Serial.print("SimonSays: ");
    for (int i = 0; i < level; i++) {
      Serial.print(simonSays[i]);
      Serial.print(" : ");
    }
    Serial.println();
    for (int i = 0; i < level; i++) {
      Serial.print("LED AN: ");
      Serial.println(simonSays[i]);
      digitalWrite(ledPins[simonSays[i]], HIGH);
      playTone(buttonTones[simonSays[i]]);
      comPlaying = false;
      delay(simonSpeed);
      digitalWrite(ledPins[simonSays[i]], LOW);
      Serial.print("LED AUS: ");
      Serial.println(simonSays[i]);
      delay(100);
    }
  }
}

void startupSeq() {
  if (startup) {
    delay(2000);
    Serial.println("Startup");
    for (int thisNote = 0; thisNote < 13; thisNote++) { // Ton und LED Sequenz
      // play the next note:
      tone(buzzer, (startupTones[thisNote]));

      if (thisNote == 0 || thisNote == 2 || thisNote == 4 || thisNote == 6) {
        digitalWrite(led1, HIGH);
      }
      if (thisNote == 1 || thisNote == 3 || thisNote == 5 || thisNote == 7 || thisNote == 9 || thisNote == 11) {
        digitalWrite(led2, HIGH);
      }
      if (thisNote == 8 || thisNote == 12) {
        digitalWrite(led3, HIGH);
      }
      if (thisNote == 10) {
        digitalWrite(led4, HIGH);
      }
      // hold the note:
      delay(startupTonesDuration[thisNote]);
      // stop for the next note:
      noTone(buzzer);
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      delay(25);
    }
    delay(1000);
    startup = false;
    comPlaying = true;
  }
}

void losing() {   //Wird gespielt wenn verloren
  for (int thisNote = 0; thisNote < 6; thisNote++) {  //Flashes lights for failure

    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
    tone(buzzer, (losingTones[thisNote]),losingTonesDuration[thisNote]);
    delay(losingTonesDuration[thisNote]/2-50);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    delay(losingTonesDuration[thisNote]/2-50);
    noTone(buzzer);
  }
  delay(500);
}

void winning() {  // Wird gespielt wenn Game gewonnen
  for (int thisNote = 0; thisNote < 6; thisNote++) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
    // play the next note:
    tone(buzzer, (winningTones[thisNote]));
    // hold the note:
    delay(winningTonesDuration[thisNote]);
    // stop for the next note:
    noTone(buzzer);
    delay(25);
  }
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
}
