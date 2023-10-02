#include <map>
#include <vector>

#define NUM_PADS 9

// Somente entrada 34, 35, 36, 39
// Pinos sem Pull UP 13, 25, 26, 27, 32 e 33
// Pinos com Pull UP 14, 16, 17, 18, 19, 21, 22 e 23
// Pinos ADC_1 32, 33, 34, 36, 37, 38, 39
// Pinos ADC_2 0, 2, 4, 12, 13, 14, 15, 25, 26, 27
// -----------------------------------------------------------------------------
// Pinos utilizados como Saída para os Leds dos Pads
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// Cores disponíveis para os LEDS
// -----------------------------------------------------------------------------
uint8_t padBrightness = 255;

uint32_t vermelho = 0xFF0000;
uint32_t roxo = 0x800080;
uint32_t rosaEscuro = 0xD80213;
uint32_t amareloClaro = 0xFF4500;
uint32_t laranja = 0x910b00;
uint32_t verdeClaro = 0x7FFF00;
uint32_t amareloEscuro = 0xFFD700;
uint32_t verde = 0x00FF00;
uint32_t azulClaro = 0x007AA3;
uint32_t azul = 0x0000FF;
uint32_t branco = 0xFFFFFF;

const uint32_t colors[11] = { vermelho, azulClaro, amareloClaro, rosaEscuro, verde', azul, amareloEscuro, roxo, laranja, branco };
const int colorsSize = *(&colors + 1) - colors; 
uint8_t colorsIndex = 0;

uint32_t offColor = 0x000000;
uint32_t backlight = 0xFFE42D;
uint32_t colorPr = vermelho; // Cor Primaria
uint32_t colorSc = verde; // Cor Secundaria

// -----------------------------------------------------------------------------

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

std::map<char, char> notes = {
  { PIEZO_PIN_1, 36 },
  { PIEZO_PIN_2, 37 },
  { PIEZO_PIN_3, 38 },
  { PIEZO_PIN_4, 39 },
  { PIEZO_PIN_5, 40 },
  { PIEZO_PIN_6, 45 },
  { PIEZO_PIN_7, 42 },
  { PIEZO_PIN_8, 43 },
  { PIEZO_PIN_9, 44 }
};

// -----------------------------------------------------------------------------
// FASTLED with OctoWS2811
// -----------------------------------------------------------------------------
byte pinList[NUM_PADS] = { LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4, LED_PIN_5, LED_PIN_6, LlD_PIN_7, LED_PIN_8, LED_PIN_9};
const int ledsPerStrip = 252;
CRGB rgbarray[NUM_PADS * ledsPerStrip];

// These buffers need to be large enough for all the pixels.
// The total number of pixels is "ledsPerStrip * NUM_PADS".
// Each pixel needs 3 bytes, so multiply by 3.  An "int" is
// 4 bytes, so divide by 4.  The array is created using "int"
// so the compiler will align it to 32 bit memory.
DMAMEM int displayMemory[ledsPerStrip * NUM_PADS * 3 / 4];
int drawingMemory[ledsPerStrip * NUM_PADS * 3 / 4];
OctoWS2811 octo(ledsPerStrip, displayMemory, drawingMemory, WS2811_RGB | WS2811_800kHz, NUM_PADS, pinList);

template <EOrder RGB_ORDER = RGB,
          uint8_t CHIP = WS2812_800kHz>
class CTeensy4Controller : public CPixelLEDController<RGB_ORDER, 8, 0xFF>
{
    OctoWS2811 *pocto;

public:
    CTeensy4Controller(OctoWS2811 *_pocto)
        : pocto(_pocto){};

    virtual void init() {}
    virtual void showPixels(PixelController<RGB_ORDER, 8, 0xFF> &pixels)
    {

        uint32_t i = 0;
        while (pixels.has(1))
        {
            uint8_t r = pixels.loadAndScale0();
            uint8_t g = pixels.loadAndScale1();
            uint8_t b = pixels.loadAndScale2();
            pocto->setPixel(i++, r, g, b);
            pixels.stepDithering();
            pixels.advanceData();
        }

        pocto->show();
    }
};

CTeensy4Controller<RGB, WS2811_800kHz> *pcontroller;

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

vector<LedStripeNeoPixel> ledStripes = {
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