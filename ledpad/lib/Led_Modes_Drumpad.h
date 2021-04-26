#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void colorWipe(Adafruit_NeoPixel *strip, uint32_t color, int wait, int numLeds);
void theaterChase(Adafruit_NeoPixel *strip, uint32_t color, int wait, int numLeds);
void rainbow(Adafruit_NeoPixel *strip, int wait, int numLeds);
void theaterChaseRainbow(Adafruit_NeoPixel *strip, int wait, int numLeds);