#ifndef CARAMELO_LED_H
#define CARAMELO_LED_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <vector>

// -----------------------------------------------------------------------------
// Pinos utilizados como Saída para os Leds dos Pads
// -----------------------------------------------------------------------------
// PAD 1 - 24, A0 - Pequeno
// PAD 2 - 25, A1 - Pequeno
// PAD 3 - 26, A2 - Grande
// PAD 4 - 27, A3 - Medio
// PAD 5 - 28, A4 - Grande
// PAD 6 - 29, A5 - Medio
// PAD 7 - 30, A6 - Grande
// PAD 8 - 31, A7 - Pequeno
// PÁD 9 - 31, A8 - Pequeno
#define LED_PIN_1 24
#define LED_PIN_2 25
#define LED_PIN_3 26
#define LED_PIN_4 27
#define LED_PIN_5 28
#define LED_PIN_6 29
#define LED_PIN_7 30
#define LED_PIN_8 31
#define LED_PIN_9 32

// -----------------------------------------------------------------------------
// Pinos utilizados como Entrada dos piezos dos Pads
// -----------------------------------------------------------------------------
#define PIEZO_PIN_1 A0
#define PIEZO_PIN_2 A1
#define PIEZO_PIN_3 A2
#define PIEZO_PIN_4 A3
#define PIEZO_PIN_5 A4
#define PIEZO_PIN_6 A5
#define PIEZO_PIN_7 A6
#define PIEZO_PIN_8 A7
#define PIEZO_PIN_9 A8
                                                                              //
#define NUM_PADS 9

enum Pad_Type {
  PAD_TYPE_1 = 1,
  PAD_TYPE_2,
  PAD_TYPE_3
};

enum Led_Stripe_Mode {
  MODE_SOLID_SC,
  MODE_SOLID_PR,
  MODE_BORDER
};

uint8_t piezoPadPins[NUM_PADS] = {
  PIEZO_PIN_1, 
  PIEZO_PIN_2,
  PIEZO_PIN_3,
  PIEZO_PIN_4,
  PIEZO_PIN_5,
  PIEZO_PIN_6,
  PIEZO_PIN_7,
  PIEZO_PIN_8,
  PIEZO_PIN_9
};

void ledPadInit(uint8_t ledPadIndex);
void triggerLeds(uint8_t modeId, uint8_t typePadId, uint8_t ledPadIndex, uint32_t color);
void turnOnLeds(uint32_t color, uint32_t timeDelay);
void changeColors(char colorMode);
void turnOff();
void setBrightness();
void changeSecondaryColor();
void randomEffectMijoDoCachorroAlado();
void turnOnLedsModePrSc(uint32_t firstDelay, uint32_t secondDelay);

#endif
