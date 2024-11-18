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
Version: V01.02.00
=========================================================*/

/*=========================================================
Updates in this Version:
-Buttons ohne Widerstand Umverkabelung auf Board, Abfrage mit LOW statt High)
=========================================================*/


/*=========================================================
ToDo:
- Button debouncer (Button können teilweise nicht gedrückt werden wegen dem Delay, muss mit Millis gelöst werden)
- Startmelodie
- Fehler Melodie
- Level Up Melodie
- Display mit Levelanzeige
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
  comPlaying = false;
  startupSeq();
  simonPlays();
  checkButtonPressed();
}

//Funktion zum auslesen der Buttons
void checkButtonPressed() {
  if (!comPlaying) {
    for (int i = 0; i < (sizeof(buttonPins) / sizeof(buttonPins[0])); i++) {  //Für alle Buttons
      if (digitalRead(buttonPins[i]) == LOW) {  
        Serial.print("Buttonpressed ");                             //Wenn Button gedrückt
        digitalWrite(ledPins[i], HIGH);
        playTone(buttonTones[i]);
        playerSays[guess] = i;  //Füge dem Array playerSays die Gedrückte Taste hinzu
        Serial.print("PlayerSays: ");
        Serial.println(playerSays[guess]);
        // Irgendwo hier ist noch ein Fehler!!!! Sagt zu früh gewonnen!
        delay(500);
        digitalWrite(ledPins[i], LOW);
        delay(1000);
        if (playerSays[guess] == simonSays[guess] == 1) {  //Abfrage ob der Schritt richtig war
          guess++;
          Serial.println("Gut geraten");
          if (guess == level) { //Abrage ob die richtige Anzahl an Schritten erreicht wurde
            // Ja -> Next Level
            Serial.println("  Next Level  ");
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

void playTone(int note) {
  //tone(buzzer, note, 400);
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
      delay(500);
    }
  }
}

void startupSeq() {
  if (startup) {
    delay(2000);
    Serial.println("Startup");
    startup = false;
    comPlaying = true;
  }
}
