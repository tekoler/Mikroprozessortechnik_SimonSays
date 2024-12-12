/*
    /////////////////////////////////////////////////////////////
   //														                              //
  //                        Simon Says                       //
 //														                            	//
/////////////////////////////////////////////////////////////
 */


/*=========================================================
Description: SimonSays Game
Author: M. Mühlethaler
Date: 12.12.2024
Version: V02.02.00
=========================================================*/



#include "arduino_config.h"
#include "noten.h"
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include <toneAC.h>

//LCD Settings
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(rotPin1, rotPin2, RotaryEncoder::LatchMode::TWO03);



void setup() {
  //Activate serial Output
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

  //Used to have a real random
  randomSeed(analogRead(0));

  //Define LCD
  lcd.init();
  lcd.backlight();

  //Define first Rotaryposition that it is not 0:
  encoder.setPosition(rotaryNewPos[actSetNr] / rotarySteps[actSetNr]);
}

// LCD Main updating function used for printing Line 1 and 2 (only when there is an update)
// If Line 2 is not submitted print avtual Level/Record 
void printLCD(String lcdLine1, String lcdLine2 = "none") {
  if ((oldLcdLine1 != lcdLine1) || (oldLcdLine2 != lcdLine2)) {
       Serial.print("PrintLCD lcdline ");
   Serial.println(lcdLine2);
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
    } else {
      lcd.print(lcdLine2);
    }
  } 
}


void loop() {

  playerModeChanged = false;
  currentMillis = millis();

  //Set a random Value for the actual level to be played from Simon
  simonSays[guess] = random(0, 4);

  startupSeq();
  guess++;
  simonPlays();

  if (!checkUserSequence()) {
    if (!playerModeChanged) {  
      losing();
    }
    startup = true;
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

//Check if and which Button was pressed
byte readButtons() {
  Serial.println("User input requested");
  //If Playermode was changed restart required, therfore return
  if (playerModeChanged) {
    return;
  }
  while (true) {

    for (byte i = 0; i < 6; i++) {
      byte buttonPin = buttonPins[i];
      if (digitalRead(buttonPin) == LOW) {
        if (buttonPin == bPlayMode) {
          settings(buttonPin);
          return;
        } else if (buttonPin == bRotary) {
          settings(buttonPin);
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

//checking if user entered sequence is correct
bool checkUserSequence() {

  printPlayerStatus();

  for (int i = 0; i < guess; i++) {
    byte actualButton = readButtons();
    byte expectedButton = simonSays[i];
    if (!playerModeChanged) {
      toneLED(actualButton);
      delay(150);
    }
    if (expectedButton != actualButton) {
      return false;
      Serial.println("Game Over");
    }
  }
  return true;
}

//helping function for printing difference between Multiplayer and Singleplayer
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

//function for the settings menu
void settings(int button) {
  if (button == bRotary) {
    Serial.println("SettingsRotary");
    actSetNr = 0;
    rotary();
    return;
  }
  if (button == bPlayMode) {
    playerModeChanged = true;
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

//Navigation and changing values with the rotary Button
//Millis is always used to have a timer to wait for more user input and to return if not
void rotary() {
  Serial.println("entering rotary menu");
  delay(150);
  currentMillis = millis();
  rotaryMillis = millis();
  while (currentMillis - rotaryMillis < rotaryPeriod) {
    sprintf(settingValue, "%d %%", rotaryNewPos[actSetNr]);
    printLCD(actSetText[actSetNr], settingValue);
    currentMillis = millis();
    encoder.tick();
    if (digitalRead(bRotary) == LOW) {
      rotaryMillis = millis();
      testSpeed = false;
      if (actSetNr < 3) {
        actSetNr++;
        if (actSetNr == 1) {
          toneAC(NOTE_F4, volume, 200);
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

    //Get the new Rotary Position
    rotaryNewPos[actSetNr] = encoder.getPosition() * rotarySteps[actSetNr];

    //checking if Rotaryposition is too high, too low or different than the last one 
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

    //some things needed to visualize and sonificate the actual setting
      if (actSetNr == 1) {
        toneAC(NOTE_F4, volume, 200);
      }
      if (actSetNr == 2) {
        simonSpeed = -2.5 * rotaryNewPos[2] + 300;
        speedSettingMillis = millis();
        Serial.println(simonSpeed);
      }
    }
    if (actSetNr == 0) {
      brightness = 2 * rotaryNewPos[0];
      Serial.println(rotaryNewPos[0]);
      Serial.println(brightness);
      noTone(buzzer);
      allLED(true);
    }

    if (actSetNr == 1) {
      volume = 0.1* (rotaryNewPos[1]-1);
      allLED(false);
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
      noTone(buzzer);
      if (testSpeed) {
        testSpeed = false;
      }
    }
  }
  allLED(false);
  noTone(buzzer);
  return;
}

//changing only from Multiplayer to Singleplayer and vice versa
void changePlayerMode() {
  multiPlayer = !multiPlayer;
  if (multiPlayer) {
    printLCD("2 Player Mode", "");
  } else {
    printLCD("1 Player Mode", "");
  }
  delay(150);
}

//activating LED and the assigned sound
void toneLED(int button) {
  analogWrite(ledPins[button], brightness);
  toneAC(buttonTones[button], volume);
  delay(simonSpeed);
  noTone(buzzer);
  digitalWrite(ledPins[button], LOW);
}

//Playing the computer sequence
void simonPlays() {
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
  Serial.println("StartupSeq entered");
  if (guess == 0) {
    delay(2000);
    printLCD("Game starting", "");
    for (int thisNote = 0; thisNote < 13; thisNote++) {  // Ton und LED Sequenz
      // play the next note:
      toneAC(startupTones[thisNote], volume);
      //tone(buzzer, (startupTones[thisNote]));

      if (thisNote == 0 || thisNote == 2 || thisNote == 4 || thisNote == 6) {
        analogWrite(led1, brightness);
      }
      if (thisNote == 1 || thisNote == 3 || thisNote == 5 || thisNote == 7 || thisNote == 9 || thisNote == 11) {
        analogWrite(led2, brightness);
      }
      if (thisNote == 8 || thisNote == 12) {
        analogWrite(led3, brightness);
      }
      if (thisNote == 10) {
        analogWrite(led4, brightness);
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

//Turn on and off all LED (for visualizing in Settings)
void allLED(bool on) {
  if (on) {
    for (int i = 0; i < 4; i++) {
      analogWrite(ledPins[i], brightness);
    }
  } else {
    for (int i = 0; i < 4; i++) {
      analogWrite(ledPins[i], LOW);
    }
  }
}


//Losing sequence of audio and LED
void losing() {
  //Wird gespielt wenn verloren
Serial.println(multiPlayer);
  if (multiPlayer) {

    if (guess % 2 == 0) {
      printLCD("Game Over", "Player 1 wins");
    } else {
      printLCD("Game Over", "Player 2 wins");
    }

  } else {
    printLCD("Game Over", "");
  }
  for (int thisNote = 0; thisNote < 6; thisNote++) {  //Flashes lights for failure

    allLED(true);
    toneAC(losingTones[thisNote], volume);
    //tone(buzzer, (losingTones[thisNote]), losingTonesDuration[thisNote]);
    delay(losingTonesDuration[thisNote] / 2 - 50);
    allLED(false);
    delay(losingTonesDuration[thisNote] / 2 - 50);
    noTone(buzzer);
  }

  guess = 0;
  delay(500);
}

//Winning sequence of audio and LED
void winning() {  // Wird gespielt wenn Game gewonnen
if (!startup){
  printLCD("Cool!","");
}
  
  for (int thisNote = 0; thisNote < 6; thisNote++) {
    allLED(true);
    // play the next note:
    toneAC(winningTones[thisNote], volume);
    //tone(buzzer, (winningTones[thisNote]));
    // hold the note:
    delay(winningTonesDuration[thisNote]);
    // stop for the next note:
    noTone(buzzer);
    delay(25);
  }
  allLED(false);
}
