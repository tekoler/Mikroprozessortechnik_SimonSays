

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
Version: V01.04.00
=========================================================*/

/*=========================================================
Updates in this Version:
-Buttonpressing verbessert. Es ist nun möglich schneller zu drücken
-richtiges Random mit randomSeed
-Display mit Levelanzeige integriert
-Multiplayer beginn

=========================================================*/


/*=========================================================
ToDo:
- Code check / vereinfachen / kommentieren
- Multiplayer Modus vervollständigen
=========================================================*/

#include "arduino_config.h"
#include "noten.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);



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
  randomSeed(analogRead(0));

  //Define LCD
  lcd.init();
  lcd.backlight();
}

void loop() {
  simonSays[guess] = random(0, 4);

  if (multiPlayer) {
    playerOne = !playerOne;
  }

  
  startupSeq();
  guess++;
  simonPlays();

  if (!checkUserSequence()) {
    losing();
  }

  if (record < guess) {
    record = guess;
  }

  delay(300);
  if (guess > 0) {
    winning();
    delay(300);
  }
}

byte readButtons() {
  while (true) {

    for (byte i = 0; i < 4; i++) {
      byte buttonPin = buttonPins[i];
      if (digitalRead(buttonPin) == LOW) {
        return i;
      }
    }
    delay(50);
  }
}

bool checkUserSequence() {

  if (multiPlayer) {
    if (guess % 2 == 0) {
      printLCD("Spieler 2 am Zug");
    } else {
      printLCD("Spieler 1 am Zug");
    }

  } else {
    Serial.println("Spieler am Zug");
   //printLCD("Spieler am Zug");
  }


  for (int i = 0; i < guess; i++) {
    byte expectedButton = simonSays[i];
    byte actualButton = readButtons();
    toneLED(actualButton);
    if (expectedButton != actualButton) {
      return false;
      Serial.println("Verloren");
    }
  }
      printLCD("Cool!");
  return true;
}

void toneLED(int button) {
  digitalWrite(ledPins[button], HIGH);
  tone(buzzer, buttonTones[button]);
  delay(300);
  noTone(buzzer);
  digitalWrite(ledPins[button], LOW);
}

void simonPlays() {
  //printLCD("Simon am Zug");
  printLCD("");
  Serial.print("SimonSays: ");
  for (int i = 0; i < guess; i++) {
    Serial.print(simonSays[i]);
    Serial.print(" : ");
  }
  Serial.println();
  for (int i = 0; i < guess; i++) {
    toneLED(simonSays[i]);
    delay(100);
  }
}

void startupSeq() {
  if(guess == 0){
  delay(2000);
    printLCD("Game starting");
  for (int thisNote = 0; thisNote < 13; thisNote++) {  // Ton und LED Sequenz
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
  delay(200);
  winning();
  delay(1000);
  startup = false;
  }
}


void losing() {
  //Wird gespielt wenn verloren
  guess = 0;
  printLCD("Verloren");
  for (int thisNote = 0; thisNote < 6; thisNote++) {  //Flashes lights for failure

    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
    tone(buzzer, (losingTones[thisNote]), losingTonesDuration[thisNote]);
    delay(losingTonesDuration[thisNote] / 2 - 50);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    delay(losingTonesDuration[thisNote] / 2 - 50);
    noTone(buzzer);
  }
  
  Serial.println(guess);
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

void printLCD(String lcdLine1) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcdLine1);
  lcd.setCursor(0, 1);
  if (guess > 0) {
    lcd.print("Lvl ");
    lcd.print(guess);
      lcd.print("     Rec ");
      if (record > 0) {
      lcd.print(record);
    }
  }else {
    lcd.print("");
  }
}
