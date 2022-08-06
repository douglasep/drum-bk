#include "caramelo_led.h"

// -----------------------------------------------------------------------------
// Cores disponíveis para os LEDS
// -----------------------------------------------------------------------------
uint8_t padBrightness = 255;

uint32_t vermelho = 0xFF0000;
uint32_t roxo = 0xb400ff;
uint32_t rosaEscuro = 0xD80213;
uint32_t laranja = 0x910b00;
uint32_t amarelo = 0xff6400;
uint32_t verde = 0x00FF00;
uint32_t azulClaro = 0x007AA3;
uint32_t azul = 0x0000FF;
uint32_t branco = 0xFFFFFF;
uint32_t offColor = 0x000000;
uint32_t colorPr = vermelho; // Cor Primaria
uint32_t colorSc = verde; // Cor Secundaria

const uint16_t triggerThresholds[NUM_PADS] = { 350, 350, 350, 350, 350, 350, 350, 350, 350 }; // Threshold iniciais {pad1, pad2, pad3, pad2}

const uint32_t colors[9] = { vermelho, azulClaro, rosaEscuro, verde, azul, amarelo, roxo, laranja, branco }; // Threshold iniciais {pad1, pad2, pad3, pad2}

const int colorsSize = *(&colors + 1) - colors; 

uint8_t colorsIndex = 0;

struct Led_Pad_Type {
  Pad_Type typeId;
  uint8_t rowsSize;
  uint8_t colsSize;
  uint8_t numLeds;
  uint32_t primaryColor;
  uint32_t secondaryColor;
};

// Tipo 1 => 27x4
// Tipo 2 => 36x4
// Tipo 3 => 45X4
// grande = 252 / 6 = 42
// medio = 210 / 6 = 35 
// pequeno = 162 / 6 = 27
struct Led_Pad_Type ledPadType1 = { PAD_TYPE_1, 45, 6, 252, colorPr, colorSc };
struct Led_Pad_Type ledPadType2 = { PAD_TYPE_2, 45, 6, 252, colorPr, colorSc };
struct Led_Pad_Type ledPadType3 = { PAD_TYPE_3, 45, 6, 252, colorPr, colorSc };

// -----------------------------------------------------------------------------
// Fitas led da Adafruit Neopixel
// ledPadType => Pode ter 3 tipos de pad
// stripe => Indicador para configurar a stripe
// -----------------------------------------------------------------------------
struct LedStripeNeoPixel {
  Led_Pad_Type ledPadType;
  Adafruit_NeoPixel *neoPixelStripe;
};

// -----------------------------------------------------------------------------
// PADS DO TIPO 1 => 4 PIEZOS + 4 FITAS LEDS 27X4
// -----------------------------------------------------------------------------
struct LedStripeNeoPixel ledStripe1Type1 {
  ledPadType1,
  new Adafruit_NeoPixel(ledPadType1.numLeds, LED_PIN_1, NEO_GRB + NEO_KHZ800)
};
struct LedStripeNeoPixel ledStripe2Type1 {
  ledPadType1,
  new Adafruit_NeoPixel(ledPadType1.numLeds, LED_PIN_2, NEO_GRB + NEO_KHZ800)
};
struct LedStripeNeoPixel ledStripe3Type1 {
  ledPadType1,
  new Adafruit_NeoPixel(ledPadType1.numLeds, LED_PIN_8, NEO_GRB + NEO_KHZ800)
};
struct LedStripeNeoPixel ledStripe4Type1 {
  ledPadType1,
  new Adafruit_NeoPixel(ledPadType1.numLeds, LED_PIN_9, NEO_GRB + NEO_KHZ800)
};

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PADS DO TIPO 2 => 2 PIEZOS + 2 FITAS LEDS 36X4
// -----------------------------------------------------------------------------

struct LedStripeNeoPixel ledStripe1Type2 {
  ledPadType2,
  new Adafruit_NeoPixel(ledPadType2.numLeds, LED_PIN_4, NEO_GRB + NEO_KHZ800)
};
struct LedStripeNeoPixel ledStripe2Type2 {
  ledPadType2,
  new Adafruit_NeoPixel(ledPadType2.numLeds, LED_PIN_6, NEO_GRB + NEO_KHZ800)
};

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PADS DO TIPO 3 => 4 PIEZOS + 4 FITAS LEDS 45X4
// -----------------------------------------------------------------------------
struct LedStripeNeoPixel ledStripe1Type3 {
  ledPadType3,
  new Adafruit_NeoPixel(ledPadType3.numLeds, LED_PIN_3, NEO_GRB + NEO_KHZ800)
};
struct LedStripeNeoPixel ledStripe2Type3 {
  ledPadType3,
  new Adafruit_NeoPixel(ledPadType3.numLeds, LED_PIN_5, NEO_GRB + NEO_KHZ800)
};
struct LedStripeNeoPixel ledStripe3Type3 {
  ledPadType3,
  new Adafruit_NeoPixel(ledPadType3.numLeds, LED_PIN_7, NEO_GRB + NEO_KHZ800)
};
// -----------------------------------------------------------------------------

std::vector<LedStripeNeoPixel> ledStripes = {
  ledStripe1Type1,
  ledStripe2Type1,
  ledStripe1Type3,
  ledStripe1Type2,
  ledStripe2Type3,
  ledStripe2Type2,
  ledStripe3Type3,
  ledStripe3Type1,
  ledStripe4Type1
}; // Lista de fitas led

//------------------------------------------------------------------------------
// Aciona fita led conforme um modo determinado
// @param modo [int] Modo para alteração [0 => Desligado, 1 => Modo 1 (Todos leds acionados em uma cor), 2 => Modo 2 (Bordas coloridas) ]
// @param setupPad [LedPadConjunto] Configurações de cores para cada led da fita
// @param ledPad [Adafruit_NeoPixel] Fita led
//------------------------------------------------------------------------------
void triggerLeds(uint8_t modeId, uint8_t typePadId, uint8_t ledPadIndex, uint32_t color)
{
  for (int i = 0; i < ledStripes[ledPadIndex].ledPadType.numLeds; i++)
  {
      ledStripes[ledPadIndex].neoPixelStripe->setPixelColor(i, color); //  Set pixel's color (in RAM)
  }
  ledStripes[ledPadIndex].neoPixelStripe->show();                  //  Update strip to match
  // Serial.println("--------------------------------------------");
  // Serial.println(String(ledPadIndex));
  // Serial.println("tipo: " + String(ledStripes[ledPadIndex].ledPadType.typeId));
  // Serial.println("--------------------------------------------");
}

// -----------------------------------------------------------------------------
// Liga os leds com a cor primária
// -----------------------------------------------------------------------------
void turnOnLeds(uint32_t color, uint32_t timeDelay) {
  delay(timeDelay);

  for (uint16_t i = 0; i < NUM_PADS; i++)
  {
    triggerLeds(MODE_SOLID_PR, ledStripes[i].ledPadType.typeId, i, color);
  }
}

// -----------------------------------------------------------------------------
// Alterar modo de cores
// -----------------------------------------------------------------------------
void changeColors(char colorMode) {
  switch(colorMode) {
    case '0': 
      break;
    case '1':
      colorPr = colors[0];
      break;
    case '2':
      colorPr = colors[1];
      break;
    case '3':
      colorPr = colors[2];
      break;
    case '4':
      colorPr = colors[3];
      break;
    case '5':
      colorPr = colors[4];
      break;
    case '6':
      colorPr = colors[5];
      break;
    case '7':
      colorPr = colors[6];
      break;
    case '8':
      colorPr = colors[7];
      break;
    case '9':
      colorPr = colors[8];
      break;
    case 'A':
      turnOff(); 
      break;
    case 'B':
      randomEffectMijoDoCachorroAlado();
      break;
    case 'C':
      colorPr = laranja;
      turnOnLeds(colorSc, 0);
      break;
    case 'D':
      setBrightness();
      break;
    case '*':
      changeSecondaryColor();
      turnOnLeds(colorSc, 0);
      break;
    case '#':
      break;
    default:
      break;
  }
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Modos Iniciais
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Liga os leds com a cor primária
// Liga os leds com a cor secundária
// Liga os leds com a cor primária
// -----------------------------------------------------------------------------
void ledPadInit(uint8_t ledPadIndex) {
  ledStripes[ledPadIndex].neoPixelStripe->begin();
}

// -----------------------------------------------------------------------------
// EFEITOS
// -----------------------------------------------------------------------------
void turnOff() {
  turnOnLeds(offColor, 0);
}

void setBrightness() {
  padBrightness -= 25;
  if(padBrightness < 0) { 
    padBrightness = 255;
  }
  for (uint16_t i = 0; i < NUM_PADS; i++)
  {
    ledStripes[i].neoPixelStripe->setBrightness(padBrightness);
    ledStripes[i].neoPixelStripe->show();
  }
  Serial.println("Alterando Brightness: " + String(padBrightness));
}

void changeSecondaryColor() {
  colorsIndex += 1;
  
  if(colorsIndex > (colorsSize - 1)) {
    colorsIndex = 0;
  }
  colorSc = colors[colorsIndex];
}

void randomEffectMijoDoCachorroAlado() {
  uint32_t ultimaCor = vermelho;
  if(colorPr == vermelho) {
    ultimaCor = roxo; 
  }
  turnOnLeds(rosaEscuro,150);
  turnOnLeds(verde, 150);
  turnOnLeds(ultimaCor, 150);
  turnOnLeds(colorPr, 150);
}

// Liga os leds com a cor primária
// Liga os leds com a cor secundária
// Liga os leds com a cor primária
// -----------------------------------------------------------------------------
void turnOnLedsModePrSc(uint32_t firstDelay, uint32_t secondDelay) {
  for (uint16_t i = 0; i < NUM_PADS; i++)
  {
    ledStripes[i].neoPixelStripe->begin();
  }
  turnOnLeds(colorPr, 0);
  turnOnLeds(colorSc, 200);
}
