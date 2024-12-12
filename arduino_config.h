/*
    /////////////////////////////////////////////////////////////
   //														                              //
  //                        Simon Says                       //
 //														                            	//
/////////////////////////////////////////////////////////////
 */


/*=========================================================
Description: SimonSays Game Variables
Author: M. Mühlethaler
Date: 12.12.2024
Version: V02.02.00
=========================================================*/

#include "noten.h"

//PIN Belegung Buttons
const int buttonPins[] = {50, 51, 52, 53, 44, 7};
const int b1 = 50;
const int b2 = 51;
const int b3 = 52;
const int b4 = 53;
const int bPlayMode = 44;


//PIN Belegung Rotary Button
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
const int buzzer = 11;

//Tones and lights
const int buttonTones[] = {NOTE_C4, NOTE_F4, NOTE_G4, NOTE_E4};

//Millis
unsigned long currentMillis;
unsigned long speedSettingMillis;
unsigned long rotaryMillis;
unsigned long bPlayModeMillis;
const unsigned long speedSettingPeriod = 1000;
const unsigned long bPlayModePeriod = 3000;
const unsigned long rotaryPeriod = 5000;


//Spielvariablen
bool startup = true;
int guess = 0;
int simonSays[100] ={};
bool multiPlayer = false;
int record = 0;
bool playerModeChanged = false; //needed to check if restart is required after changing PlayerMode


//Settings Rotary LED / Speaker / Speed
int rotarySteps[] = { 2, 10, 5 }; //Steps
int rotaryMin[] = { 0, 0, 0 }; //Min Value
int rotaryMax[] = { 100, 100, 100 }; //Max Value
int rotaryNewPos[] = { 50, 60, 0 }; //Start Value
int rotaryLastPos[] = { 50, 60, 0 }; //Value for checking changes
int actSetNr = 0; //Which Setting is on
String actSetText[] = {"Helligkeit", "Volume", "Speed"}; //Display text for settings

//Setting Values from Rotary Button
int brightness = 2 * rotaryNewPos[0];
int volume = 0.1* (rotaryNewPos[1]-1);
int simonSpeed = -2.5*rotaryNewPos[2]+300;

bool testSpeed = false; //Needed for visualizing the Speed
String  oldLcdLine1 = "";
String  oldLcdLine2 = "";
char settingValue[15]; //Helper for changing INT into String with sprintf



//Sounds
int startupTones[] = {NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_F4, NOTE_G4};
int startupTonesDuration[] = {100, 200, 100, 200, 100, 400, 100, 100, 100, 100, 200, 100, 500};
int winningTones[] = {NOTE_C4, NOTE_C4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_C5};
int winningTonesDuration[] = {100, 100, 100, 300, 100, 300};
int losingTones[] = {NOTE_G3, NOTE_C3,NOTE_G3, NOTE_C3,NOTE_G3, NOTE_C3};
int losingTonesDuration[] = {300, 300,300, 300,300, 300};