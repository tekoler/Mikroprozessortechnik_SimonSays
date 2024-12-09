//Uebung_01: Stammdaten


/*=========================================================
Description: Stammdaten: einfaches Blink-Signal auf internes LED Platine
Author: M. Mühlethaler copy from C. Meier
Date: 21.10.2024
Version: V01.00.00
=========================================================*/
#include "noten.h"

//PIN Belegung Buttons
const int buttonPins[] = {50, 51, 52, 53, 44, 7};
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
const int b1 = 50;
const int b2 = 51;
const int b3 = 52;
const int b4 = 53;
const int bPlayMode = 44;


//PIN Belegung Rotary
#define rotPin1 8
#define rotPin2 9
const int bRotary = 7;

//PIN Belegung LED's
const int ledPins[] = {3, 4, 5, 6};
const int led1 = 3;
const int led2 = 4;
const int led3 = 5;
const int led4 = 6;

//PIN Belegung Buzzer
const int buzzer = 40;

//Tones and lights
const int buttonTones[] = {NOTE_C4, NOTE_F4, NOTE_G4, NOTE_E4};

//Millis
unsigned long currentMillis;
unsigned long speedSettingMillis;
unsigned long rotaryMillis;
unsigned long bPlayModeMillis;
const unsigned long buttonPeriod = 10;
const unsigned long speedSettingPeriod = 1000;
const unsigned long bPlayModePeriod = 3000;
const unsigned long rotaryPeriod = 5000;


//Spielvariablen
bool startup = false;
bool comPlaying = true; 
int guess = 0;
int simonSays[100] ={};
bool multiPlayer = false;
int record = 0;
bool settingsMade = false;


//Settings
// Rotary LED / Speaker / Speed
int rotarySteps[] = { 4, 1, 5 };
int rotaryMin[] = { 0, 0, 0 };
int rotaryMax[] = { 255, 9, 100 };
int rotaryNewPos[] = { 110, 5, 0 };
int rotaryLastPos[] = { 110, 5, 0 };
int actSetNr = 0;
int simonSpeed = -2.5*rotaryNewPos[2]+300;
bool testSpeed = false;
String  oldLcdLine1 = "";
String  oldLcdLine2 = "";

String actSetText[] = {"Helligkeit", "Lautstärke", "Speed"};




//Sounds
int startupTones[] = {NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_F4, NOTE_G4};
int startupTonesDuration[] = {100, 200, 100, 200, 100, 400, 100, 100, 100, 100, 200, 100, 500};
int winningTones[] = {NOTE_C4, NOTE_C4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_C5};
int winningTonesDuration[] = {100, 100, 100, 300, 100, 300};
int losingTones[] = {NOTE_G3, NOTE_C3,NOTE_G3, NOTE_C3,NOTE_G3, NOTE_C3};
int losingTonesDuration[] = {300, 300,300, 300,300, 300};