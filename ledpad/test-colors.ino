#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <map>
#include <vector>
#include <string>
#include <Keypad.h>

#define KEYPAD_PIN_1 0
#define KEYPAD_PIN_2 1
#define KEYPAD_PIN_3 2
#define KEYPAD_PIN_4 3
#define KEYPAD_PIN_5 4
#define KEYPAD_PIN_6 5
#define KEYPAD_PIN_7 6
#define KEYPAD_PIN_8 7

#define LED_PIN_1 24
#define LED_PIN_2 25
#define LED_PIN_3 26
#define LED_PIN_4 27
#define LED_PIN_5 28
#define LED_PIN_6 29
#define LED_PIN_7 30
#define LED_PIN_8 31
#define LED_PIN_9 32

uint32_t colorsArr[] = {
    0xF0F8FF,
    0x9966CC,
    0xFAEBD7,
    0x00FFFF,
    0x7FFFD4,
    0xF0FFFF,
    0xF5F5DC,
    0xFFE4C4,
    0x000000,
    0xFFEBCD,
    0x0000FF,
    0x8A2BE2,
    0xA52A2A,
    0xDEB887,
    0x5F9EA0,
    0x7FFF00,
    0xD2691E,
    0xFF7F50,
    0x6495ED,
    0xFFF8DC,
    0xDC143C,
    0x00FFFF,
    0x00008B,
    0x008B8B,
    0xB8860B,
    0xA9A9A9,
    0xA9A9A9,
    0x006400,
    0xBDB76B,
    0x8B008B,
    0x556B2F,
    0xFF8C00,
    0x9932CC,
    0x8B0000,
    0xE9967A,
    0x8FBC8F,
    0x483D8B,
    0x2F4F4F,
    0x2F4F4F,
    0x00CED1,
    0x9400D3,
    0xFF1493,
    0x00BFFF,
    0x696969,
    0x696969,
    0x1E90FF,
    0xB22222,
    0xFFFAF0,
    0x228B22,
    0xFF00FF,
    0xDCDCDC,
    0xF8F8FF,
    0xFFD700,
    0xDAA520,
    0x808080,
    0x808080,
    0x008000,
    0xADFF2F,
    0xF0FFF0,
    0xFF69B4,
    0xCD5C5C,
    0x4B0082,
    0xFFFFF0,
    0xF0E68C,
    0xE6E6FA,
    0xFFF0F5,
    0x7CFC00,
    0xFFFACD,
    0xADD8E6,
    0xF08080,
    0xE0FFFF,
    0xFAFAD2,
    0x90EE90,
    0xD3D3D3,
    0xFFB6C1,
    0xFFA07A,
    0x20B2AA,
    0x87CEFA,
    0x778899,
    0x778899,
    0xB0C4DE,
    0xFFFFE0,
    0x00FF00,
    0x32CD32,
    0xFAF0E6,
    0xFF00FF,
    0x800000,
    0x66CDAA,
    0x0000CD,
    0xBA55D3,
    0x9370DB,
    0x3CB371,
    0x7B68EE,
    0x00FA9A,
    0x48D1CC,
    0xC71585,
    0x191970,
    0xF5FFFA,
    0xFFE4E1,
    0xFFE4B5,
    0xFFDEAD,
    0x000080,
    0xFDF5E6,
    0x808000,
    0x6B8E23,
    0xFFA500,
    0xFF4500,
    0xDA70D6,
    0xEEE8AA,
    0x98FB98,
    0xAFEEEE,
    0xDB7093,
    0xFFEFD5,
    0xFFDAB9,
    0xCD853F,
    0xFFC0CB,
    0xCC5533,
    0xDDA0DD,
    0xB0E0E6,
    0x800080,
    0xFF0000,
    0xBC8F8F,
    0x4169E1,
    0x8B4513,
    0xFA8072,
    0xF4A460,
    0x2E8B57,
    0xFFF5EE,
    0xA0522D,
    0xC0C0C0,
    0x87CEEB,
    0x6A5ACD,
    0x708090,
    0x708090,
    0xFFFAFA,
    0x00FF7F,
    0x4682B4,
    0xD2B48C,
    0x008080,
    0xD8BFD8,
    0xFF6347,
    0x40E0D0,
    0xEE82EE,
    0xF5DEB3,
    0xFFFFFF,
    0xF5F5F5,
    0xFFFF00,
    0x9ACD32,
    // LED RGB color that roughly approximates
    // the color of incandescent fairy lights,
    // assuming that you're using FastLED
    // color correction on your LEDs (recommended),
    0xFFE42D,
    // If you are using no color correction, use this
    0xFF9D2A
};
std::map<std::string, uint32_t> colors = {
    { "AliceBlue", 0xF0F8FF },
    { "Amethyst", 0x9966CC },
    { "AntiqueWhite", 0xFAEBD7 },
    { "Aqua", 0x00FFFF },
    { "Aquamarine", 0x7FFFD4 },
    { "Azure", 0xF0FFFF },
    { "Beige", 0xF5F5DC },
    { "Bisque", 0xFFE4C4 },
    { "Black", 0x000000 },
    { "BlanchedAlmond", 0xFFEBCD },
    { "Blue", 0x0000FF },
    { "BlueViolet", 0x8A2BE2 },
    { "Brown", 0xA52A2A },
    { "BurlyWood", 0xDEB887 },
    { "CadetBlue", 0x5F9EA0 },
    { "Chartreuse", 0x7FFF00 },
    { "Chocolate", 0xD2691E },
    { "Coral", 0xFF7F50 },
    { "CornflowerBlue", 0x6495ED },
    { "Cornsilk", 0xFFF8DC },
    { "Crimson", 0xDC143C },
    { "Cyan", 0x00FFFF },
    { "DarkBlue", 0x00008B },
    { "DarkCyan", 0x008B8B },
    { "DarkGoldenrod", 0xB8860B },
    { "DarkGray", 0xA9A9A9 },
    { "DarkGrey", 0xA9A9A9 },
    { "DarkGreen", 0x006400 },
    { "DarkKhaki", 0xBDB76B },
    { "DarkMagenta", 0x8B008B },
    { "DarkOliveGreen", 0x556B2F },
    { "DarkOrange", 0xFF8C00 },
    { "DarkOrchid", 0x9932CC },
    { "DarkRed", 0x8B0000 },
    { "DarkSalmon", 0xE9967A },
    { "DarkSeaGreen", 0x8FBC8F },
    { "DarkSlateBlue", 0x483D8B },
    { "DarkSlateGray", 0x2F4F4F },
    { "DarkSlateGrey", 0x2F4F4F },
    { "DarkTurquoise", 0x00CED1 },
    { "DarkViolet", 0x9400D3 },
    { "DeepPink", 0xFF1493 },
    { "DeepSkyBlue", 0x00BFFF },
    { "DimGray", 0x696969 },
    { "DimGrey", 0x696969 },
    { "DodgerBlue", 0x1E90FF },
    { "FireBrick", 0xB22222 },
    { "FloralWhite", 0xFFFAF0 },
    { "ForestGreen", 0x228B22 },
    { "Fuchsia", 0xFF00FF },
    { "Gainsboro", 0xDCDCDC },
    { "GhostWhite", 0xF8F8FF },
    { "Gold", 0xFFD700 },
    { "Goldenrod", 0xDAA520 },
    { "Gray", 0x808080 },
    { "Grey", 0x808080 },
    { "Green", 0x008000 },
    { "GreenYellow", 0xADFF2F },
    { "Honeydew", 0xF0FFF0 },
    { "HotPink", 0xFF69B4 },
    { "IndianRed", 0xCD5C5C },
    { "Indigo", 0x4B0082 },
    { "Ivory", 0xFFFFF0 },
    { "Khaki", 0xF0E68C },
    { "Lavender", 0xE6E6FA },
    { "LavenderBlush", 0xFFF0F5 },
    { "LawnGreen", 0x7CFC00 },
    { "LemonChiffon", 0xFFFACD },
    { "LightBlue", 0xADD8E6 },
    { "LightCoral", 0xF08080 },
    { "LightCyan", 0xE0FFFF },
    { "LightGoldenrodYellow", 0xFAFAD2 },
    { "LightGreen", 0x90EE90 },
    { "LightGrey", 0xD3D3D3 },
    { "LightPink", 0xFFB6C1 },
    { "LightSalmon", 0xFFA07A },
    { "LightSeaGreen", 0x20B2AA },
    { "LightSkyBlue", 0x87CEFA },
    { "LightSlateGray", 0x778899 },
    { "LightSlateGrey", 0x778899 },
    { "LightSteelBlue", 0xB0C4DE },
    { "LightYellow", 0xFFFFE0 },
    { "Lime", 0x00FF00 },
    { "LimeGreen", 0x32CD32 },
    { "Linen", 0xFAF0E6 },
    { "Magenta", 0xFF00FF },
    { "Maroon", 0x800000 },
    { "MediumAquamarine", 0x66CDAA },
    { "MediumBlue", 0x0000CD },
    { "MediumOrchid", 0xBA55D3 },
    { "MediumPurple", 0x9370DB },
    { "MediumSeaGreen", 0x3CB371 },
    { "MediumSlateBlue", 0x7B68EE },
    { "MediumSpringGreen", 0x00FA9A },
    { "MediumTurquoise", 0x48D1CC },
    { "MediumVioletRed", 0xC71585 },
    { "MidnightBlue", 0x191970 },
    { "MintCream", 0xF5FFFA },
    { "MistyRose", 0xFFE4E1 },
    { "Moccasin", 0xFFE4B5 },
    { "NavajoWhite", 0xFFDEAD },
    { "Navy", 0x000080 },
    { "OldLace", 0xFDF5E6 },
    { "Olive", 0x808000 },
    { "OliveDrab", 0x6B8E23 },
    { "Orange", 0xFFA500 },
    { "OrangeRed", 0xFF4500 },
    { "Orchid", 0xDA70D6 },
    { "PaleGoldenrod", 0xEEE8AA },
    { "PaleGreen", 0x98FB98 },
    { "PaleTurquoise", 0xAFEEEE },
    { "PaleVioletRed", 0xDB7093 },
    { "PapayaWhip", 0xFFEFD5 },
    { "PeachPuff", 0xFFDAB9 },
    { "Peru", 0xCD853F },
    { "Pink", 0xFFC0CB },
    { "Plaid", 0xCC5533 },
    { "Plum", 0xDDA0DD },
    { "PowderBlue", 0xB0E0E6 },
    { "Purple", 0x800080 },
    { "Red", 0xFF0000 },
    { "RosyBrown", 0xBC8F8F },
    { "RoyalBlue", 0x4169E1 },
    { "SaddleBrown", 0x8B4513 },
    { "Salmon", 0xFA8072 },
    { "SandyBrown", 0xF4A460 },
    { "SeaGreen", 0x2E8B57 },
    { "Seashell", 0xFFF5EE },
    { "Sienna", 0xA0522D },
    { "Silver", 0xC0C0C0 },
    { "SkyBlue", 0x87CEEB },
    { "SlateBlue", 0x6A5ACD },
    { "SlateGray", 0x708090 },
    { "SlateGrey", 0x708090 },
    { "Snow", 0xFFFAFA },
    { "SpringGreen", 0x00FF7F },
    { "SteelBlue", 0x4682B4 },
    { "Tan", 0xD2B48C },
    { "Teal", 0x008080 },
    { "Thistle", 0xD8BFD8 },
    { "Tomato", 0xFF6347 },
    { "Turquoise", 0x40E0D0 },
    { "Violet", 0xEE82EE },
    { "Wheat", 0xF5DEB3 },
    { "White", 0xFFFFFF },
    { "WhiteSmoke", 0xF5F5F5 },
    { "Yellow", 0xFFFF00 },
    { "YellowGreen", 0x9ACD32 },
    // LED RGB color that roughly approximates
    // the color of incandescent fairy lights,
    // assuming that you're using FastLED
    // color correction on your LEDs (recommended),
    { "FairyLight", 0xFFE42D },
    // If you are using no color correction, use this
    { "FairyLightNCC", 0xFF9D2A }
};
// -----------------------------------------------------------------------------
// KEYBOARD Simples
// -----------------------------------------------------------------------------
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

// ----------------
#define NUM_PADS 9
std::vector<Adafruit_NeoPixel> ledStripes;

void triggerLeds(uint8_t ledPadIndex, uint32_t color)
{
  for (int i = 0; i < 255; i++)
    {
    ledStripes[ledPadIndex].setPixelColor(i, color); //  Set pixel's color (in RAM)
  }
  ledStripes[ledPadIndex].show();                  //  Update strip to match
}
uint8_t colorsIndex = 0;
const uint16_t colorsSize = *(&colorsArr + 1) - colorsArr; 

void mode_1() {
  std::map<std::string, uint32_t>::iterator it;

  for (it = colors.begin(); it != colors.end(); it++)
  {
      for(int i = 0; i < NUM_PADS; i++) {
        triggerLeds(i, it->second);
      };
      delay(1000);
  }
}

void mode_2() {
  std::map<std::string, uint32_t>::iterator it;
  Serial.println("Mode 2");
  for (it = colors.begin(); it != colors.end(); it++)
  {
      for(int i = 0; i < NUM_PADS; i++) {
        triggerLeds(i, it->second);
      Serial.println(String(it->second));
      }
      
      for(int i = 0; i < NUM_PADS; i++) {
        triggerLeds(i, 0xFFE42D);
      }
      delay(1000);
  }
}

void mode_4() {
  Serial.println("Mode 4");
  
  for (uint16_t i=0;i <= colorsSize; i++)
  {
      for(int i = 0; i < NUM_PADS; i++) {
        triggerLeds(i, colorsArr[i]);
      Serial.println(String(colorsArr[i]));
      }
      
      for(int i = 0; i < NUM_PADS; i++) {
        triggerLeds(i, 0xFFE42D);
      }
      delay(1000);
  }
}


void mode_3() {
  std::map<std::string, uint32_t>::iterator it;

  for (it = colors.begin(); it != colors.end(); it++)
  {
      for(int i = 0; i < NUM_PADS; i++) {
        triggerLeds(i, it->second);
      }
      delay(5000);
  }
}

void mode_keyboard(char key) {
  if(key == "A") {
    if(colorsIndex > (colorsSize - 1)) {
      colorsIndex = 0;
    } else {
      colorsIndex++;
    }
  }

  if(key == "B") {
    if(colorsIndex <= 0) {
      colorsIndex = colorsSize;
    } else {
      colorsIndex--;
    }
  }

  if(key == "C") {
    if(colorsIndex > (colorsSize - 1)) {
      colorsIndex = 0;
    } else {
      colorsIndex += 10;
    }
  }

  if(key == "D") {
    if(colorsIndex <= 0) {
      colorsIndex = colorsSize;
    } else {
      colorsIndex -= 10;
    }
  }
  Serial.println("Cor alterada: " + String(colorsArr[colorsIndex]));
  changeAllColors(colorsArr[colorsIndex]);
}

void changeAllColors(uint32_t color) {
  for(int i = 0; i < NUM_PADS; i++) {
    triggerLeds(i, color);
  }
}

void setup() {
Serial.begin(115200);
  Serial.println("Iniciando o projeto");
  analogReadResolution(10);
  pinMode(KEYPAD_PIN_1, INPUT);
  pinMode(KEYPAD_PIN_2, INPUT);
  pinMode(KEYPAD_PIN_3, INPUT);
  pinMode(KEYPAD_PIN_4, INPUT);
  pinMode(KEYPAD_PIN_5, INPUT);
  pinMode(KEYPAD_PIN_6, INPUT);
  pinMode(KEYPAD_PIN_7, INPUT);
  pinMode(KEYPAD_PIN_8, INPUT);
 
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(LED_PIN_5, OUTPUT);
  pinMode(LED_PIN_6, OUTPUT);
  pinMode(LED_PIN_7, OUTPUT);
  pinMode(LED_PIN_8, OUTPUT);
  pinMode(LED_PIN_9, OUTPUT);
  Adafruit_NeoPixel *pad_1 = new Adafruit_NeoPixel(255, LED_PIN_1, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel *pad_2 = new Adafruit_NeoPixel(255, LED_PIN_2, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel *pad_3 = new Adafruit_NeoPixel(255, LED_PIN_3, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel *pad_4 = new Adafruit_NeoPixel(255, LED_PIN_4, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel *pad_5 = new Adafruit_NeoPixel(255, LED_PIN_5, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel *pad_6 = new Adafruit_NeoPixel(255, LED_PIN_6, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel *pad_7 = new Adafruit_NeoPixel(255, LED_PIN_7, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel *pad_8 = new Adafruit_NeoPixel(255, LED_PIN_8, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel *pad_9 = new Adafruit_NeoPixel(255, LED_PIN_9, NEO_GRB + NEO_KHZ800);
  ledStripes.push_back(*pad_1);
  ledStripes.push_back(*pad_2);
  ledStripes.push_back(*pad_3);
  ledStripes.push_back(*pad_4);
  ledStripes.push_back(*pad_5);
  ledStripes.push_back(*pad_6);
  ledStripes.push_back(*pad_7);
  ledStripes.push_back(*pad_8);
  ledStripes.push_back(*pad_9);
}

void loop() {
  //mode_1(); // Sem backlight

  // char key = keypad.getKey();// Read the key
  // if (key){
  //   mode_keyboard(key);
  // }
  Serial.println("loop");
  mode_4(); // Com backlight

  // mode_3() // lento
}
