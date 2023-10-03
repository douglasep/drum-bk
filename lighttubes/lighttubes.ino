#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <Led_Matrix.h>
#include <Keypad.h>
#include <map>
#include <vector>
#include <TeensyThreads.h> 
#include <configs.h>

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
  Serial.println("--------------------------------------------------------------");
  Serial.println("triggerThresholds: " + String(triggerThresholds[0]) + " " + String(triggerThresholds[1]));
  Serial.println("--------------------------------------------------------------");
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
  turnOnLeds(colorSc, 150);
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

void keypadClicked() {
  char key = keypad.getKey();// Read the key
  if (key){
    // Serial.print("Key Pressed: ");
    // Serial.println(key);
    changeColors(key);
  }
}

boolean ledsInChangedState[NUM_PADS] = { false, false, false, false, false, false, false, false, false };
elapsedMillis ledsTimeout[NUM_PADS];

void padLoop1() {
  triggerLeds(MODE_SOLID_PR, ledStripes[0].ledPadType.typeId, 0, colorPr);
  usbMIDI.sendNoteOn(36, 127, 1);
  ledsTimeout[0] = 0;
  ledsInChangedState[0] = true;
}

void padLoop2() {
  triggerLeds(MODE_SOLID_PR, ledStripes[1].ledPadType.typeId, 1, colorPr);
  usbMIDI.sendNoteOn(37, 127, 1);
  ledsTimeout[1] = 0;
  ledsInChangedState[1] = true;
}

void padLoop3() {
  triggerLeds(MODE_SOLID_PR, ledStripes[2].ledPadType.typeId, 2, colorPr);
  usbMIDI.sendNoteOn(38, 127, 1);
  ledsTimeout[2] = 0;
  ledsInChangedState[2] = true;
}

void padLoop4() {
  triggerLeds(MODE_SOLID_PR, ledStripes[3].ledPadType.typeId, 3, colorPr);
  usbMIDI.sendNoteOn(39, 127, 1);
  ledsTimeout[3] = 0;
  ledsInChangedState[3] = true;
}

void padLoop5() {
  triggerLeds(MODE_SOLID_PR, ledStripes[4].ledPadType.typeId, 4, colorPr);
  usbMIDI.sendNoteOn(40, 127, 1);
  ledsTimeout[4] = 0;
  ledsInChangedState[4] = true;
}

void padLoop6() {
  triggerLeds(MODE_SOLID_PR, ledStripes[5].ledPadType.typeId, 5, colorPr);
  usbMIDI.sendNoteOn(45, 127, 1);
  ledsTimeout[5] = 0;
  ledsInChangedState[5] = true;
}

void padLoop7() {
triggerLeds(MODE_SOLID_PR, ledStripes[6].ledPadType.typeId, 6, colorPr);
usbMIDI.sendNoteOn(42, 127, 1);
ledsTimeout[6] = 0;
ledsInChangedState[6] = true;
}

void padLoop8() {
  triggerLeds(MODE_SOLID_PR, ledStripes[7].ledPadType.typeId, 7, colorPr);
  usbMIDI.sendNoteOn(43, 127, 1);
  ledsTimeout[7] = 0;
  ledsInChangedState[7] = true;
}

void padLoop9() {
  triggerLeds(MODE_SOLID_PR, ledStripes[8].ledPadType.typeId, 8, colorPr);
  usbMIDI.sendNoteOn(44, 127, 1);
  ledsTimeout[8] = 0;
  ledsInChangedState[8] = true;
}

// -----------------------------------------------------------------------------
// Configurações de inicialização do ESP
// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
//   while (!Serial) ; // wait for Arduino Serial Monitor
//   delay(10);
  Serial.println("Iniciando o projeto");
  Serial.println("alive");
  delay(10);
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
  usbMIDI.setHandleControlChange(onControlChange);

  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_1), padLoop1, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_2), padLoop2, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_3), padLoop3, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_4), padLoop4, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_5), padLoop5, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_6), padLoop6, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_7), padLoop7, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_8), padLoop8, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIEZO_PIN_9), padLoop9, HIGH);
}

int ledDelay = 60;
void ledsToSecundary(int ledIndex) {
  boolean changed = changeLedsWithDelay(MODE_SOLID_SC, ledStripes[ledIndex].ledPadType.typeId, ledIndex, colorSc);
  if(changed) { ledsInChangedState[ledIndex] = false; }
}

boolean changeLedsWithDelay(uint8_t modeId, int ledIndex, uint32_t color, int delay) {
  if (ledsTimeout[ledIndex] > delay) {
    triggerLeds(modeId, ledStripes[ledIndex].ledPadType.typeId, ledIndex, color);
    return true;
  }
  return false;
}

void loop() {
  // freqMeasure();
  keypadClicked();
  for (int i = 0; i < NUM_PADS; i++) {
    if(ledsInChangedState[i]) {
      ledsToSecundary(i);
    }
  }

  usbMidi.read();
}