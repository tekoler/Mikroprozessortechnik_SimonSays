

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
Date: 09.12.2024
Version: V02.00.00
=========================================================*/

/*=========================================================
Updates in this Version:
-Multiplayer funktionstüchtig
-Einstellungen Helligkeit, Lautstärke und Speed möglich

=========================================================*/


/*=========================================================
ToDo:
- Ausgabe Einstellungswert auf Display
=========================================================*/

#include "arduino_config.h"
#include "noten.h"
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include "Volume3.h"
#include <toneAC.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(rotPin1, rotPin2, RotaryEncoder::LatchMode::TWO03);



void setup() {
  //Serielle Schnittstelle aktivieren
  Serial.begin(9600);
  //Define pinModes Buttons
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(b4, INPUT_PULLUP);
  pinMode(bPlayMode, INPUT_PULLUP);
  pinMode(bRotary, INPUT_PULLUP);

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

  //Define first Rotaryposition that it is not 0:
  encoder.setPosition(rotaryNewPos[actSetNr] / rotarySteps[actSetNr]);
}

void printLCD(String lcdLine1, String lcdLine2 = "none") {
  if ((oldLcdLine1 != lcdLine1) || oldLcdLine2 != lcdLine2) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lcdLine1);
    lcd.setCursor(0, 1);
    oldLcdLine1 = lcdLine1;
    oldLcdLine2 = lcdLine2;
    if (lcdLine2 == "none") {
      lcd.print("Lvl ");
      lcd.print(guess);
      lcd.print("     Rec ");
      if (record > 0) {
        lcd.print(record);
      }
    }
  } else lcd.print(lcdLine2);
}

void test(String lcdLine1, String lcdLine2 = "none") {
  Serial.println(lcdLine1);
  Serial.println(lcdLine2);
  if (lcdLine2 == "none") {
    Serial.print("funktioniert");
  }
}

void loop() {

  settingsMade = false;
  currentMillis = millis();
  simonSays[guess] = random(0, 4);


  //startupSeq();
  guess++;
  Serial.println("nextRound");
  simonPlays();

  if (!checkUserSequence()) {
    if (!settingsMade) {
      losing();
    }
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
  if (settingsMade) {
    return;
  }
  while (true) {

    for (byte i = 0; i < 6; i++) {
      byte buttonPin = buttonPins[i];
      if (digitalRead(buttonPin) == LOW) {
        if (buttonPin == bPlayMode) {
          Serial.println("enteringSettings");
          settings(buttonPin);
          Serial.println("ReadButtons verlassen");
          return;
        } else if (buttonPin == bRotary) {
          Serial.println("enteringSettings");
          settings(buttonPin);
          Serial.println("ReadButtons verlassen");
          printPlayerStatus();
        }

        else {
          return i;
        }
      }
    }

    delay(50);
  }
}


bool checkUserSequence() {
  Serial.print("SettingsMade: ");
  Serial.println(settingsMade);


  printPlayerStatus();


  for (int i = 0; i < guess; i++) {
    byte actualButton = readButtons();
    Serial.println(actualButton);
    byte expectedButton = simonSays[i];
    Serial.println(actualButton);
    Serial.println(actualButton);
    Serial.println("  Where do You play?");
    if (!settingsMade) {
      toneLED(actualButton);
      delay(100);
    }
    if (expectedButton != actualButton) {
      return false;
      Serial.println("Verloren");
    }
  }
  if (!settingsMade) {
  printLCD("Cool!");
  }
  return true;
}

void printPlayerStatus() {
  if (multiPlayer) {
    if (guess % 2 == 0) {
      printLCD("Spieler 2 am Zug");
    } else {
      printLCD("Spieler 1 am Zug");
    }

  } else {
    Serial.println("Spieler am Zug");
    printLCD("Spieler am Zug");
  }
}

void settings(int button) {
  if (button == bRotary) {
    Serial.println("SettingsRotary");
    actSetNr = 0;
    rotary();
    return;
  }
  if (button == bPlayMode) {
    settingsMade = true;
    currentMillis = millis();
    bPlayModeMillis = millis();
    changePlayerMode();
    Serial.println("timer updated");
    Serial.println(currentMillis - bPlayModeMillis);

    while (currentMillis - bPlayModeMillis < bPlayModePeriod) {
      currentMillis = millis();
      if (digitalRead(bPlayMode) == LOW) {
        settings(bPlayMode);
      }
      delay(30);
      Serial.println("inSchleife");
    }

    guess = 0;


    Serial.println("Settings verlassen");
    return;
  }




  delay(5000);
}

void displaySetting() {
}

void rotary() {

  Serial.println("enteringRotary");
  delay(150);
  currentMillis = millis();
  rotaryMillis = millis();
  while (currentMillis - rotaryMillis < rotaryPeriod) {
    printLCD(actSetText[actSetNr], "");
    currentMillis = millis();
    encoder.tick();
    if (digitalRead(bRotary) == LOW) {
      rotaryMillis = millis();
      testSpeed = false;
      if (actSetNr < 3) {
        actSetNr++;
        if (actSetNr == 1) {
          toneAC(NOTE_F4, rotaryNewPos[1], 200);
        }
        if (actSetNr == 2) {
          testSpeed = true;
        }
      }
      if (actSetNr == 3) {
        actSetNr = 0;
      }

      encoder.setPosition(rotaryNewPos[actSetNr] / rotarySteps[actSetNr]);

      delay(200);
      Serial.print(actSetText[actSetNr]);
      Serial.print(" LastPos: ");
      Serial.println(rotaryLastPos[actSetNr]);
    }


    rotaryNewPos[actSetNr] = encoder.getPosition() * rotarySteps[actSetNr];

    if (rotaryNewPos[actSetNr] < rotaryMin[actSetNr]) {
      encoder.setPosition(rotaryMin[actSetNr] / rotarySteps[actSetNr]);
      rotaryNewPos[actSetNr] = rotaryMin[actSetNr];
      Serial.print("newPos below min");
    } else if (rotaryNewPos[actSetNr] > rotaryMax[actSetNr]) {
      encoder.setPosition(rotaryMax[actSetNr] / rotarySteps[actSetNr]);
      rotaryNewPos[actSetNr] = rotaryMax[actSetNr];
      Serial.print("newPos higher than max");
    }
    if (rotaryLastPos[actSetNr] != rotaryNewPos[actSetNr]) {
      currentMillis = millis();
      rotaryMillis = millis();
      Serial.print(actSetText[actSetNr]);
      Serial.print(" NewPos: ");
      Serial.print(rotaryNewPos[actSetNr]);
      Serial.println();
      Serial.println(currentMillis - rotaryMillis);
      (rotaryLastPos[actSetNr] = rotaryNewPos[actSetNr]);
      if (actSetNr == 1) {
        toneAC(NOTE_F4, rotaryNewPos[1], 200);
      }
      if (actSetNr == 2) {
        simonSpeed = -2.5 * rotaryNewPos[2] + 300;
        speedSettingMillis = millis();
        Serial.println(simonSpeed);
      }
    }
    if (actSetNr == 0) {
      noTone(buzzer);
      allLED(true);
      // printLCD(actSetText[actSetNr], "");
    }

    if (actSetNr == 1) {
      allLED(false);
      //printLCD(actSetText[actSetNr], "");
    }

    if ((currentMillis - speedSettingMillis > 1000) && (currentMillis - speedSettingMillis < 1500)) {
      Serial.print(currentMillis - speedSettingMillis);
      Serial.println("  testspeed true");
      for (int i = 0; i < 4; i++) {
        toneLED(i);
      }

      testSpeed = true;
    }


    if (actSetNr == 2) {
      // printLCD(actSetText[actSetNr]);
      noTone(buzzer);
      //if (currentMillis - speedSettingMillis > 2000) {
      if (testSpeed) {

        testSpeed = false;
      }
    }
  }
  allLED(false);
  noTone(buzzer);
  return;
}

void changePlayerMode() {
  multiPlayer = !multiPlayer;
  if (multiPlayer) {
    printLCD("2 Player Mode", "");
  } else {
    printLCD("1 Player Mode", "");
  }
  delay(150);
}

void toneLED(int button) {
  Serial.println(simonSpeed);
  Serial.println(" Speed in toneLED");
  analogWrite(ledPins[button], rotaryNewPos[0]);
  toneAC(buttonTones[button], rotaryNewPos[1]);
  delay(simonSpeed);
  noTone(buzzer);
  digitalWrite(ledPins[button], LOW);
}

void simonPlays() {
  //printLCD("Simon am Zug");
  //printLCD("", "");
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
  if (guess == 0) {
    delay(2000);
    printLCD("Game starting", "");
    for (int thisNote = 0; thisNote < 13; thisNote++) {  // Ton und LED Sequenz
      // play the next note:
      toneAC(startupTones[thisNote], rotaryNewPos[1]);
      //tone(buzzer, (startupTones[thisNote]));

      if (thisNote == 0 || thisNote == 2 || thisNote == 4 || thisNote == 6) {
        analogWrite(led1, rotaryNewPos[0]);
      }
      if (thisNote == 1 || thisNote == 3 || thisNote == 5 || thisNote == 7 || thisNote == 9 || thisNote == 11) {
        analogWrite(led2, rotaryNewPos[0]);
      }
      if (thisNote == 8 || thisNote == 12) {
        analogWrite(led3, rotaryNewPos[0]);
      }
      if (thisNote == 10) {
        analogWrite(led4, rotaryNewPos[0]);
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

void allLED(bool on) {
  if (on) {
    for (int i = 0; i < 4; i++) {
      analogWrite(ledPins[i], rotaryNewPos[0]);
    }
  } else {
    for (int i = 0; i < 4; i++) {
      analogWrite(ledPins[i], LOW);
    }
  }
}



void losing() {
  //Wird gespielt wenn verloren

  if (multiPlayer) {

    if (guess % 2 == 0) {
      printLCD("Game Over", "Player 1 wins");
    } else {
      printLCD("Game Over", "Player 2 wins");
    }

  } else {
    printLCD("Game Over", "");
    Serial.println("multi check");
  }
  for (int thisNote = 0; thisNote < 6; thisNote++) {  //Flashes lights for failure

    allLED(true);
    toneAC(losingTones[thisNote], rotaryNewPos[1]);
    //tone(buzzer, (losingTones[thisNote]), losingTonesDuration[thisNote]);
    delay(losingTonesDuration[thisNote] / 2 - 50);
    allLED(false);
    delay(losingTonesDuration[thisNote] / 2 - 50);
    noTone(buzzer);
  }

  guess = 0;
  delay(500);
}

void winning() {  // Wird gespielt wenn Game gewonnen
  for (int thisNote = 0; thisNote < 6; thisNote++) {
    allLED(true);
    // play the next note:
    toneAC(winningTones[thisNote], rotaryNewPos[1]);
    //tone(buzzer, (winningTones[thisNote]));
    // hold the note:
    delay(winningTonesDuration[thisNote]);
    // stop for the next note:
    noTone(buzzer);
    delay(25);
  }
  allLED(false);
}
