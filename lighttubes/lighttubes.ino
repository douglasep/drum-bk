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
        Serial.println("--------------------------------------------");
        Serial.println("Tocado padNo: " + String(padNo) + ' ' + String(analogRead(padNo)));  
        // Serial.println("Tocado ledPadIndex: " + String(ledPadIndex));
        // Serial.println("Tocado highestYet: " + String(highestYet));
        Serial.println("--------------------------------------------");  
        Serial.println("nota: " + String(nota)); // Send the unscaled velocity value to the serial monitor too, for debugging / fine-tuning
        //Serial.println("pad: " + String((int)padNo));
        Serial.println("velocity: " + String(highestYet)); // Send the unscaled velocity value to the serial monitor too, for debugging / fine-tuning
        Serial.println("peak time: " + String(highestValueTime - startReadingTime)); // Send the unscaled velocity value to the serial monitor too, for debugging / fine-tuning

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
              // Serial.println("Tocado padNo: " + String(padNo));  
              // Serial.println("Tocado ledPadIndex: " + String(ledPadIndex));
              // Serial.println("Tocado tailLength: " + String(tailLength));
              // Serial.println("Tocado msPrevious: " + String(msPrevious));
              // Serial.println("modo: 1");
              // Serial.println("--------------------------------------------");
              previousHitStillRecent = false;               // Se o gap entre o ultimo hit for maior que 128ms, desliga a flag -> "previousHitStillRecent"
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
          // Serial.println("-----------------------------hitOccurredRecently---------------------------------");
          // Serial.println("pin: " + String(padNo));
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

// -----------------------------------------------------------------------------
// Configurações de inicialização do ESP
// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  Serial.println("Iniciando o projeto");
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

  pinMode(KEYPAD_PIN_1, INPUT_PULLUP);
  pinMode(KEYPAD_PIN_2, INPUT_PULLUP);
  pinMode(KEYPAD_PIN_3, INPUT_PULLUP);
  pinMode(KEYPAD_PIN_4, INPUT_PULLUP);
  pinMode(KEYPAD_PIN_5, INPUT_PULLUP);
  pinMode(KEYPAD_PIN_6, INPUT_PULLUP);
  pinMode(KEYPAD_PIN_7, INPUT_PULLUP);
  pinMode(KEYPAD_PIN_8, INPUT_PULLUP);
 
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(LED_PIN_5, OUTPUT);
  pinMode(LED_PIN_6, OUTPUT);
  pinMode(LED_PIN_7, OUTPUT);
  pinMode(LED_PIN_8, OUTPUT);
  pinMode(LED_PIN_9, OUTPUT);

  octo.begin();
  pcontroller = new CTeensy4Controller<RGB, WS2811_800kHz>(&octo);

  FastLED.setBrightness(255);
  FastLED.addLeds(pcontroller, rgbarray, NUM_PADS * ledsPerStrip);

  Serial.println("--------------------------------------------------------------");
  Serial.println("triggerThresholds: " + String(triggerThresholds[0]) + " " + String(triggerThresholds[1]));
  Serial.println("--------------------------------------------------------------");
  // Biblioteca de Cores
  turnOnLedsModePrSc(200, 200);
  Serial.println("--------------------------------------------------------------");
  //threads.addThread(thread_pads_func);
  //threads.addThread(thread_keyboard_func);
}

// -----------------------------------------------------------------------------
// Loop principal do ESP
// -----------------------------------------------------------------------------

void loop() { 
  for (uint8_t i = 0; i < NUM_PADS; i++)
  {
    pads[i].tick();
  }
  
  char key = keypad.getKey();// Read the key
  if (key){
    changeColors(key);
  }
}