#ifndef CARAMELO_KEYPAD_H
#define CARAMELO_KEYPAD_H

#include <Arduino.h>
#include <Keypad.h>

// -----------------------------------------------------------------------------
// KEYBOARD Simples
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Pinos utilizados como Entradas do KEYPAD
// -----------------------------------------------------------------------------
#define KEYPAD_PIN_1 0
#define KEYPAD_PIN_2 1
#define KEYPAD_PIN_3 2
#define KEYPAD_PIN_4 3
#define KEYPAD_PIN_5 4
#define KEYPAD_PIN_6 5
#define KEYPAD_PIN_7 6
#define KEYPAD_PIN_8 7

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {KEYPAD_PIN_8, KEYPAD_PIN_7, KEYPAD_PIN_6, KEYPAD_PIN_5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {KEYPAD_PIN_4, KEYPAD_PIN_3, KEYPAD_PIN_2, KEYPAD_PIN_1}; //connect to the column pinouts of the k

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

#endif
