//Uebung_01: Stammdaten


/*=========================================================
Description: Stammdaten: einfaches Blink-Signal auf internes LED Platine
Author: M. Mühlethaler copy from C. Meier
Date: 21.10.2024
Version: V01.00.00
=========================================================*/
#include "noten.h"

//PIN Belegung Buttons
const int buttonPins[] = {50, 51, 52, 53};
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
const int b1 = 50;
const int b2 = 51;
const int b3 = 52;
const int b4 = 53;

//PIN Belegung LED's
const int ledPins[] = {30, 31, 32, 33};
const int led1 = 30;
const int led2 = 31;
const int led3 = 32;
const int led4 = 33;

//PIN Belegung Buzzer
const int buzzer = 48;

//Tones and lights
const int buttonTones[] = {NOTE_C4, NOTE_F4, NOTE_G4, NOTE_E4};

//Millis
unsigned long currentMillis;
unsigned long buttonMillis;
const unsigned long buttonPeriod = 10;


//Spielvariablen
bool startup = true;
bool comPlaying = true; 
int guess = 0;
int level = 1;
int simonSays[100] ={};
int playerSays[100] = {};
int simonSpeed = 800; //Delay zwischen den Schritten von Simon

//Hardware
int ledTime = 400; //Millisekunden wie lange die LED leuchten soll

//Sounds
int startupTones[] = {NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_F4, NOTE_G4};
int startupTonesDuration[] = {100, 200, 100, 200, 100, 400, 100, 100, 100, 100, 200, 100, 500};
int winningTones[] = {NOTE_C4, NOTE_C4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_C5};
int winningTonesDuration[] = {100, 100, 100, 300, 100, 300};
int losingTones[] = {NOTE_G3, NOTE_C3,NOTE_G3, NOTE_C3,NOTE_G3, NOTE_C3};
int losingTonesDuration[] = {300, 300,300, 300,300, 300};
