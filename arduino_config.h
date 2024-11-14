//Uebung_01: Stammdaten


/*=========================================================
Description: Stammdaten: einfaches Blink-Signal auf internes LED Platine
Author: M. Mühlethaler copy from C. Meier
Date: 21.10.2024
Version: V01.00.00
=========================================================*/

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
