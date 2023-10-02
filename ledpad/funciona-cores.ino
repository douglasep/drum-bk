#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <Led_Matrix.h>
#include <Keypad.h>
#include <FreqMeasureMulti.h>
#include <map>
#include <vector>
 
#define NUM_PADS 9

// -----------------------------------------------------------------------------
// Pinos utilizados como Saída para os Leds dos Pads
// -----------------------------------------------------------------------------
// PAD 1 - 04, 26 - Pequeno
// PAD 3 - 16, 25 - Grande
// PAD 5 - 17, 33 - Grande
// PAD 7 - 05, 32 - Grande
// PÁD 9 - 18, 35 - Pequeno
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
// PAD 1 - 04, 26 - Pequeno
// PAD 3 - 16, 25 - Grande
// PAD 5 - 17, 33 - Grande
// PAD 7 - 05, 32 - Grande
// PÁD 9 - 18, 35 - Pequeno
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

// Somente entrada 34, 35, 36, 39
// Pinos sem Pull UP 13, 25, 26, 27, 32 e 33
// Pinos com Pull UP 14, 16, 17, 18, 19, 21, 22 e 23
// Pinos ADC_1 32, 33, 34, 36, 37, 38, 39
// Pinos ADC_2 0, 2, 4, 12, 13, 14, 15, 25, 26, 27
// -----------------------------------------------------------------------------
// Cores disponíveis para os LEDS
// -----------------------------------------------------------------------------
uint8_t padBrightness = 255;

uint32_t vermelho = 0xFF0000;
uint32_t roxo = 0x800080;
uint32_t rosaEscuro = 0xD80213;
uint32_t laranja = 0x910b00;
uint32_t amarelo = 0xff6400;
uint32_t amareloEscuro = 0xFFD700;
uint32_t amareloClaro = 0xFF4500;
uint32_t verdeClaro = 0x7FFF00;
uint32_t verde = 0x00FF00;
uint32_t azulClaro = 0x007AA3;
uint32_t azul = 0x0000FF;
uint32_t branco = 0xFFFFFF;

const uint32_t colors[11] = { vermelho, azulClaro, rosaEscuro, verde, azul, amarelo, amareloEscuro, amareloClaro, roxo, laranja, branco }; // Threshold iniciais {pad1, pad2, pad3, pad2}
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
// Declaração de variáveis e constantes para a classe Padf
// -----------------------------------------------------------------------------
#define initialHitReadDuration 850    // In microseconds. Shorter times will mean less latency, but less velocity-accuracy
#define midiVelocityScaleDownAmount 2 // Number of halvings that will be applied to MIDI velocity
#define tailRecordResolution 68

// \/-\/-\/-\/-\/ COLOCAR AS NOTAS AQUI \/-\/-\/-\/-\/-\/
//const uint16_t notes[NUM_PADS] = {36, 37, 38, 39, 40, 41, 42, 43, 44}; // Notas {pad1, pad2, pad3, pad4}
const uint16_t triggerThresholds[NUM_PADS] = {350, 350, 350, 350, 350, 350, 350, 350, 350}; // Threshold iniciais {pad1, pad2, pad3, pad2}

uint32_t lastKickTime = 0;
uint32_t kickStartTime = 0;
// Compares times without being prone to problems when the micros() counter overflows, every ~70 mins
boolean timeGreaterOrEqual(uint32_t lhs, uint32_t rhs) {
  return (((lhs - rhs) & 2147483648) == 0);
}

// -----------------------------------------------------------------------------

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
// Classificação dos Pads
// -----------------------------------------------------------------------------
class Pad
{
public:
  uint16_t highestYet;
  uint32_t startReadingTime;
  uint32_t highestValueTime;
  boolean hitOccurredRecently;
  boolean inInitialListenPhase;

  boolean previousHitStillRecent;
  uint16_t previousHitPeak;
  uint32_t previousHitTime;

  uint16_t tailLength;
  boolean newRecordSet;
  uint8_t typePadId;
  uint8_t ledPadIndex;
  uint8_t padNo;
  uint16_t defaultThreshold;

  Pad()
  {
    hitOccurredRecently = false;
    previousHitStillRecent = false;
    inInitialListenPhase = false;
  }

  void init(uint8_t _padNo, uint8_t _typePadId, uint8_t _ledPadIndex)
  {
    padNo = _padNo;
    typePadId = _typePadId;
    ledPadIndex = _ledPadIndex;
    defaultThreshold = triggerThresholds[_ledPadIndex];
    tailLength = 0;
    for (uint16_t i = 0; i < tailRecordResolution; i++)
    {
      if (EEPROM.read(i + padNo * tailRecordResolution) != 0)
        tailLength = i + 1;
    }
    tailLength = tailRecordResolution;
  }

  void tick()
  {
    // Read the piezo
    uint16_t value = analogRead(padNo);
    // Serial.println("padNo: " + String(padNo) + "value: " + String(value));
    if (inInitialListenPhase) {
      initialListen:
      // For the next few milliseconds, look out for the highest "spike" in the reading from the piezo. Its height is representative of the hit's velocity
      // Serial.println("value " + String(value) + "highestYet: " + String(highestYet) + " salvar como highest?: " + String(value > highestYet));
      if (value > highestYet)
      {
        highestYet = value;
        highestValueTime = micros();
      }
      // If we've spent enough time reading...
      if (timeGreaterOrEqual(startReadingTime + initialHitReadDuration, micros()))
      {
        // Serial.println("--");
        // Serial.println("startReadingTime " + String(startReadingTime + initialHitReadDuration) + "micros(): " + String(micros()) + " timeGreaterOrEqual?: " + String(timeGreaterOrEqual(startReadingTime + initialHitReadDuration, micros())));

        // Send the MIDI note
        uint8_t midiVelocity = min(127, ((highestYet >> midiVelocityScaleDownAmount) + 1));
        uint16_t nota = notes[padNo];
        usbMIDI.sendNoteOn(nota, midiVelocity, 1); // We add 1 onto the velocity so that the result is never 0, which would mean the same as a note-off
        // Send the MIDI note
        // Serial.println("--------------------------------------------");
        // Serial.println("Tocado padNo: " + String(padNo) + ' ' + String(analogRead(padNo)));  
        // Serial.println("Tocado ledPadIndex: " + String(ledPadIndex));
        // Serial.println("Tocado highestYet: " + String(highestYet));
        // Serial.println("--------------------------------------------");  
        // Serial.println("nota: " + String(nota)); // Send the unscaled velocity value to the serial monitor too, for debugging / fine-tuning
        //Serial.println("pad: " + String((int)padNo));
        // Serial.println("velocity: " + String(highestYet)); // Send the unscaled velocity value to the serial monitor too, for debugging / fine-tuning
        // Serial.println("peak time: " + String(highestValueTime - startReadingTime)); // Send the unscaled velocity value to the serial monitor too, for debugging / fine-tuning

        triggerLeds(MODE_SOLID_PR, typePadId, ledPadIndex, colorPr);

        hitOccurredRecently = true;
        newRecordSet = false;
        inInitialListenPhase = false;
      }
    }

    else {
      // Assume the normal hit-threshold
      uint16_t thresholdNow = defaultThreshold; // 673 os Thresholds iniciais
      // Serial.println("--");
      // Serial.println("thresholdNow " + String(thresholdNow) + "triggerThresholds[padNo] :" + String(triggerThresholds[padNo]));
      // Serial.println("--");
      uint32_t msPassed; // declara váriavel para gravar quanto tempo passou

      // But, if a hit occurred very recently, we need to set a higher threshold for triggering another hit, otherwise the dissipating vibrations
      // of the previous hit would trigger another one now
      if (hitOccurredRecently)
      {
        uint32_t usPassed = micros() - highestValueTime; // Agora - última batida
        msPassed = usPassed >> 10;                       // valor em milisegundos
        
        // espera tailLength até próxima leitura (68ms)
        if (msPassed >= tailLength)
        {
          // Serial.println("msPassed >= tailLength");
          hitOccurredRecently = false; // Se o tempo que passou desde a último hit for maior que 128ms ele desliga a flag -> hitOcurredRecently
          delay(30);
          uint8_t midiVelocity = min(127, ((highestYet >> midiVelocityScaleDownAmount) + 1));
          uint16_t nota = notes[padNo];
          usbMIDI.sendNoteOff(nota, midiVelocity, 1); // We add 1 onto the velocity so that the result is never 0, which would mean the same as a note-off 
          triggerLeds(MODE_SOLID_SC, typePadId, ledPadIndex, colorSc); // teste
        }
        else
        {
          // Work out how high a reading we'd need to see right now in order to conclude that another hit has occurred
          uint32_t currentDynamicThreshold; // variável para novo threshold
          if (usPassed < initialHitReadDuration)
            //enquanto não tiver passado o tempo initial de leitura (initialHitReadDuration) desde a última batida o threshold dinamico será igual ao último valor de leitura
            currentDynamicThreshold = highestYet;
          else
            // msPassed + padNo * tailRecordResolution => 0 
            currentDynamicThreshold = ((uint32_t)EEPROM.read(msPassed + padNo * tailRecordResolution) * highestYet) >> 7;

          thresholdNow += currentDynamicThreshold;
          // If previous hit is still "sounding" too...
          if (previousHitStillRecent)
          {
            uint32_t usPrevious = micros() - previousHitTime; // identificará qual foi o tempo que ocorreu o ultimo hit
            uint32_t msPrevious = usPrevious >> 10;
            if (msPrevious >= tailLength)
            {
              // Serial.println("--------------------------------------------");
              Serial.println("Tocado padNo: " + String(padNo));  
              // Serial.println("Tocado ledPadIndex: " + String(ledPadIndex));
              // Serial.println("Tocado tailLength: " + String(tailLength));
              // Serial.println("Tocado msPrevious: " + String(msPrevious));
              // Serial.println("modo: 1");
              // Serial.println("--------------------------------------------");
              previousHitStillRecent = false;               // Se o gap entre o ultimo hit for maior que 128ms, desliga a flag -> "previousHitStillRecent"
              // delay(30);
              // triggerLeds(MODE_SOLID_SC, typePadId, ledPadIndex, colorSc);
            }
            else
            {
              thresholdNow += ((uint32_t)EEPROM.read(msPrevious + padNo * tailRecordResolution) * previousHitPeak) >> 7; // se for recente, ele modifica o Threshold
              // value >> 7 #=> value / 128
            }
          }
        }
      }
      // If we've breached the threshold, it means we've got a hit!
      if (value >= thresholdNow)
      { // Se o sinal do piezo ultrapassar o threshold, tem hit
        if (hitOccurredRecently)
        { // Serve para acionar as flags de hit anterior
          previousHitStillRecent = true;
          previousHitPeak = highestYet;
          previousHitTime = highestValueTime;
        }

        if (hitOccurredRecently)
        {
          //Serial.println("-----------------------------hitOccurredRecently---------------------------------");
          //Serial.println("pin: " + String(padNo));
          // Serial.println("value: " + String(value));
          // Serial.println("thresholdNow " + String(thresholdNow));
          // Serial.println("gap: " + String(msPassed));
          // Serial.println("thresh: " + String(thresholdNow));
          // Serial.println("stored max for now: " + String((uint32_t)EEPROM.read(msPassed + padNo * tailRecordResolution)));
          // Serial.println("value we would store for this: " + String(ceil((float)value / (float)highestYet * 128)));
          // Serial.println("--------------------------------------------------------------");
        }

        startReadingTime = micros(); // tempo onde entrou o sinal do piezo
        highestYet = 0;              // zera qualquer valor de leitura anterior do piezo
        inInitialListenPhase = true; // Aciona a flag -> "inInitialListenPhase" para começar a tratar os valores que o piezo está enviando
        goto initialListen;
      }
    }
  }
};
// -----------------------------------------------------------------------------
// FIM Classificação dos Pads
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Variáveis para inicialização do ESP
// -----------------------------------------------------------------------------
Pad pads[NUM_PADS]; // utiliza a classe "Pad" para o número de 'numPads' pads[4] ou [  ,  ,  , ] pode mudar para quantos pads forem necessários
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Liga os leds com a cor primária
// -----------------------------------------------------------------------------
void turnOnLeds(uint32_t color, uint32_t timeDelay) {
  Serial.println("--------------------------------------------------------------");
  Serial.println("triggerThresholds: " + String(triggerThresholds[0]) + " " + String(triggerThresholds[1]));
  Serial.println("--------------------------------------------------------------");
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

// Liga os leds com a cor primária
// Liga os leds com a cor secundária
// Liga os leds com a cor primária
// -----------------------------------------------------------------------------
void turnOnLedsModePrSc(uint32_t firstDelay, uint32_t secondDelay) {
  // Serial.println("--------------------------------------------------------------");
  // Serial.println("triggerThresholds: " + String(triggerThresholds[0]) + " " + String(triggerThresholds[1]));
  // Serial.println("--------------------------------------------------------------");
  for (uint16_t i = 0; i < NUM_PADS; i++)
  {
    ledStripes[i].neoPixelStripe->begin();
    pads[i].init(piezoPadPins[i], ledStripes[i].ledPadType.typeId, i);
    Serial.println("PIEZO_PIN: " + String(piezoPadPins[i] + "LED_PIN: " ));
  }
  turnOnLeds(colorPr, 0);
  turnOnLeds(colorSc, 200);
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

void randomEffectMijoDoCachorroAlado(){
  uint32_t ultimaCor = vermelho;
  if(colorPr == vermelho) {
    ultimaCor = roxo; 
  }
  turnOnLeds(rosaEscuro,150);
  turnOnLeds(verde, 150);
  turnOnLeds(ultimaCor, 150);
  turnOnLeds(colorPr, 150);
}

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

byte rowPins[ROWS] = { KEYPAD_PIN_5, KEYPAD_PIN_6, KEYPAD_PIN_7, KEYPAD_PIN_8 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { KEYPAD_PIN_1, KEYPAD_PIN_2, KEYPAD_PIN_3, KEYPAD_PIN_4 }; //connect to the column pinouts of the k

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// -----------------------------------------------------------------------------
// FREQUENCY Measure
// -----------------------------------------------------------------------------
FreqMeasureMulti freqInput;
double freqSum=0;
int freqCount=0;
float frequencyColorGap = 100; // Diferenca de uma cor para a outra
int freqIntervalTime = 500; // atualiza as cores a cada 0,5 segundos
float frequencyMeasurement;


void changedFrequency(float frequency) {
  int freq = frequency;
  Serial.print("Freq changed to: ");
  Serial.println(freq);
  switch(freq) {
  // vermelho (1000 a 2000)
  case 100 ... 199:
      colorPr = colors[0];
      break;
  case 200 ... 299:
      colorSc = colors[0];
      turnOnLeds(colorSc, 0);
      break;
  // azul claro (2000 a 3000)
  case 300 ... 399:
      colorPr = colors[1];
      break;
  case 400 ... 499:
      colorSc = colors[1];
      turnOnLeds(colorSc, 0);
      break;
  // rosa escuro (3000 a 4000)
  case 500 ... 599:
      colorPr = colors[2];
      break;
  case 600 ... 699:
      colorSc = colors[2];
      turnOnLeds(colorSc, 0);
      break;
  // verde (4000 a 5000)
  case 700 ... 799:
      colorPr = colors[3];
      break;
  case 800 ... 899:
      colorSc = colors[3];
      turnOnLeds(colorSc, 0);
      break;
  // azul (5000 a 6000)
  case 900 ... 999:
      colorPr = colors[4];
      break;
  case 1000 ... 1099:
      colorSc = colors[4];
      turnOnLeds(colorSc, 0);
      break;
  // amarelo (6000 a 7000)
  case 1100 ... 1199:
      colorPr = colors[5];
      break;
  case 1200 ... 1299:
      colorSc = colors[5];
      turnOnLeds(colorSc, 0);
      break;
  // amarelo escuro (7000 a 8000)
  case 1300 ... 1399:
      colorPr = colors[6];
      break;
  case 1400 ... 1499:
      colorSc = colors[6];
      turnOnLeds(colorSc, 0);
      break;
  // amarelo claro (8000 a 9000)
  case 1500 ... 1599:
      colorPr = colors[7];
      break;
  case 1600 ... 1699:
      colorSc = colors[7];
      turnOnLeds(colorSc, 0);
      break;
  // roxo (9000 a 10000)
  case 1700 ... 1799:
      colorPr = colors[8];
      break;
  case 1800 ... 1899:
      colorSc = colors[8];
      turnOnLeds(colorSc, 0);
      break;
  // laranja (9000 a 1000)
  case 1900 ... 1999:
      colorPr = colors[9];
      break;
  case 2000 ... 2099:
      colorSc = colors[9];
      turnOnLeds(colorSc, 0);
      break;
  }
}
// -----------------------------------------------------------------------------
// Loop principal do ESP
// -----------------------------------------------------------------------------

//void freqMeasure() {
//  if (freqMeasurement.available()) {
//    freqSum = freqSum + freqMeasurement.read();
//    freqCount = freqCount + 1;
//  }
//  // print results every half second
//  if (freqIntervalTime > 300) {
//    if (freqCount > 0) {
//      Serial.println(freqMeasurement.countToFrequency(freqSum / freqCount));
//      int freq = (int)(((freqMeasurement.countToFrequency(freqSum / freqCount) / 2.67)+30)/100);
//      currentFreq = freq * 100;
//      Serial.print(currentFreq, DEC);
////      Serial.print(int((((freqMeasurement.countToFrequency(freqSum / freqCount) / 2.67)+50) / 100))*100);
//      if(currentFreq != prevFreq) {
//        freqChangeCount++;
//      }
//      if(freqChangeCount > 3) {
//         prevFreq = currentFreq;
//         Serial.print("changed: ");
//         Serial.print(currentFreq);
//         if(freqChangeCount > 10) {
//          freqChangeCount = 0;
//         }
//       }
//    } else {
//      Serial.print("(no pulses)");
//    }
//    Serial.println();
//    freqSum = 0;
//    freqCount = 0;
//    freqIntervalTime = 0;
//  }
//} 

void keypadClicked() {
  char key = keypad.getKey();// Read the key
  if (key){
    Serial.print("Key Pressed: ");
    Serial.println(key);
    changeColors(key);
  }
}

void padLoop() {
  while(1) {
    for (uint8_t i = 0; i < NUM_PADS; i++) {
      pads[i].tick();
    }
    // threads.yield();
  }
}

void padLoop1() {
//  pads[0].tick();
  // while(1) {
     Serial.print("digitalRead(PIEZO_PIN_1): ");
     Serial.println(digitalRead(PIEZO_PIN_1));
    // if(digitalRead(PIEZO_PIN_1) == HIGH){
  // if(analogRead(PIEZO_PIN_1) > threshold){
      triggerLeds(MODE_SOLID_PR, 0, 0, colorPr);
      usbMIDI.sendNoteOn(36, 127, 1);
      delay(100);
      triggerLeds(MODE_SOLID_PR, 0, 0, colorSc);
    // }
    // threads.delay(1000);
  // }
}
void padLoop2() {
//  pads[1].tick();
  // while(1) {
    // if(digitalRead(PIEZO_PIN_2) == HIGH){
    // if(analogRead(PIEZO_PIN_2) > threshold){
      Serial.print("digitalRead(PIEZO_PIN_2): ");
      Serial.println(digitalRead(PIEZO_PIN_2));
      triggerLeds(MODE_SOLID_PR, 1, 1, colorPr);
      usbMIDI.sendNoteOn(37, 127, 1);
      delay(100);
      triggerLeds(MODE_SOLID_PR, 1, 1, colorSc);
    // } 
    // threads.delay(1000);
  // }
}
void padLoop3() {
//  pads[2].tick();
  // while(1) {
    // if(digitalRead(PIEZO_PIN_3) == HIGH){
    // if(analogRead(PIEZO_PIN_3) > threshold){
      Serial.print("digitalRead(PIEZO_PIN_3): ");
      Serial.println(digitalRead(PIEZO_PIN_3));
      triggerLeds(MODE_SOLID_PR, 2, 2, colorPr);
      usbMIDI.sendNoteOn(38, 127, 1);
      delay(100);
      triggerLeds(MODE_SOLID_PR, 2, 2, colorSc);
    // }
    // threads.delay(1000);
  // }
}
void padLoop4() {
//  pads[3].tick();
  // while(1) {
    // if(digitalRead(PIEZO_PIN_4) == HIGH){
    // if(analogRead(PIEZO_PIN_4) > threshold){
      Serial.print("digitalRead(PIEZO_PIN_4): ");
      Serial.println(digitalRead(PIEZO_PIN_4));
      triggerLeds(MODE_SOLID_PR, 3, 3, colorPr);
      usbMIDI.sendNoteOn(39, 127, 1);
      delay(100);
      triggerLeds(MODE_SOLID_PR, 3, 3, colorSc);
    // }
    // threads.delay(1000);
  // }
}
void padLoop5() {
//  pads[4].tick();
  // while(1) {
    // if(digitalRead(PIEZO_PIN_5) == HIGH){
    // if(analogRead(PIEZO_PIN_5) > threshold){
      Serial.print("digitalRead(PIEZO_PIN_5): ");
      Serial.println(digitalRead(PIEZO_PIN_5));
      triggerLeds(MODE_SOLID_PR, 4, 4, colorPr);
      usbMIDI.sendNoteOn(40, 127, 1);
      delay(100);
      triggerLeds(MODE_SOLID_PR, 4, 4, colorSc);
    // }
    // threads.delay(1000);
  // }
}
void padLoop6() {
//  pads[5].tick();
  // while(1) {
  //   if(digitalRead(PIEZO_PIN_6) == HIGH){
    // if(analogRead(PIEZO_PIN_6) > threshold){
      Serial.print("digitalRead(PIEZO_PIN_6): ");
      Serial.println(digitalRead(PIEZO_PIN_6));
      triggerLeds(MODE_SOLID_PR, 5, 5, colorPr);
      usbMIDI.sendNoteOn(45, 127, 1);
      delay(100);
      triggerLeds(MODE_SOLID_PR, 5, 5, colorSc);
    // }
    // threads.delay(1000);
  // }
}
void padLoop7() {
//  pads[6].tick();
  // while(1) {
  //   if(digitalRead(PIEZO_PIN_7) == HIGH){
    // if(analogRead(PIEZO_PIN_7) > threshold){
      Serial.print("digitalRead(PIEZO_PIN_7): ");
      Serial.println(digitalRead(PIEZO_PIN_7));
      triggerLeds(MODE_SOLID_PR, 6, 6, colorPr);
      usbMIDI.sendNoteOn(42, 127, 1);
      delay(100);
      triggerLeds(MODE_SOLID_PR, 6, 6, colorSc);
    // }
    // threads.delay(1000);
  // }
}
void padLoop8() {
//  pads[7].tick();
  // while(1) {
  //   if(digitalRead(PIEZO_PIN_8) == HIGH){
    // if(analogRead(PIEZO_PIN_8) > threshold){
      triggerLeds(MODE_SOLID_PR, 7, 7, colorPr);
      usbMIDI.sendNoteOn(43, 127, 1);
      delay(100);
      triggerLeds(MODE_SOLID_PR, 7, 7, colorSc);
    // }
    // threads.delay(1000);
  // }
}
void padLoop9() {
  //  pads[8].tick();
  // while(1) {
  //   if(digitalRead(PIEZO_PIN_9) == HIGH){
    // if(analogRead(PIEZO_PIN_9) > threshold){
      triggerLeds(MODE_SOLID_PR, 8, 8, colorPr);
      usbMIDI.sendNoteOn(44, 127, 1);
      // threads.delay(1000);
      triggerLeds(MODE_SOLID_PR, 8, 8, colorSc);
    // }
    // threads.delay(1000);
  // }
}

void padColors() {
  // while(1) 
//    freqMeasure();
    // threads.delay(1000);
     keypadClicked();
    // threads.yield();
  // }
}

// -----------------------------------------------------------------------------
// Configurações de inicialização do ESP
// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println("Iniciando o projeto");
  Serial.println("alive");
  analogReadResolution(10);
  pinMode(PIEZO_PIN_1, INPUT);
  pinMode(PIEZO_PIN_2, INPUT);
  pinMode(PIEZO_PIN_3, INPUT);
  pinMode(PIEZO_PIN_4, INPUT);
  pinMode(PIEZO_PIN_5, INPUT);
  pinMode(PIEZO_PIN_6, INPUT);
  pinMode(PIEZO_PIN_7, INPUT);
  pinMode(PIEZO_PIN_8, INPUT);
  pinMode(PIEZO_PIN_9, INPUT);

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

  Serial.println("--------------------------------------------------------------");
  Serial.println("triggerThresholds: " + String(triggerThresholds[0]) + " " + String(triggerThresholds[1]));
  Serial.println("--------------------------------------------------------------");
  // Biblioteca de Cores
  turnOnLedsModePrSc(200, 200);
  Serial.println("--------------------------------------------------------------");
  // freqInput.begin(36);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_1), padLoop1, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_2), padLoop2, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_3), padLoop3, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_4), padLoop4, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_5), padLoop5, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_6), padLoop6, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_7), padLoop7, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_8), padLoop8, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_9), padLoop9, HIGH);

  // attachInterrupt(digitalPinToInterrupt(KEYPAD_PIN_1), keypadClicked, HIGH);
  // attachInterrupt(digitalPinToInterrupt(KEYPAD_PIN_2), keypadClicked, HIGH);
  // attachInterrupt(digitalPinToInterrupt(KEYPAD_PIN_3), keypadClicked, HIGH);
  // attachInterrupt(digitalPinToInterrupt(KEYPAD_PIN_4), keypaClicked, HIGH);
  // attachInterrupt(digitalPinToInterrupt(KEYPAD_PIN_5), keypadClicked, HIGH);
  // attachInterrupt(digitalPinToInterrupt(KEYPAD_PIN_6), keypadClicked, HIGH);
  // attachInterrupt(digitalPinToInterrupt(KEYPAD_PIN_7), keypadClicked, HIGH);
  // attachInterrupt(digitalPinToInterrupt(KEYPAD_PIN_8), keypadClicked, HIGH);
}

void loop() {
   padColors();
}