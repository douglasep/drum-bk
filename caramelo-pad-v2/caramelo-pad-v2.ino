// Arduino Libs
#include <Adafruit_NeoPixel.h>

// Cpp libs
#include <map>
#include <vector>

// -----------------------------------------------------------------------------
// Variáveis para inicialização
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Internal Libs
#include "caramelo_keypad.h"
#include "caramelo_pad.h"
#include "caramelo_led.h"

static const uint16_t thresholds[NUM_PADS] = { 350, 350, 350, 350, 350, 350, 350, 350, 350 }; // Threshold iniciais {pad1, pad2, pad3, pad2};
const std::vector<uint16_t> triggerThresholds(&thresholds[0], &thresholds[0]+NUM_PADS);

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
  Serial.println("triggerThresholds: " + String(triggerThresholds.front()));
  Serial.println("--------------------------------------------------------------");
  // Biblioteca de Cores

 for (uint16_t i = 0; i < NUM_PADS; i++)
  {
    ledPadInit(i);
    padInit(i, triggerThresholds);
    Serial.println("PIEZO_PIN: " + String(piezoPadPins[i] + "LED_PIN: " ));
  }
  turnOnLedsModePrSc(200, 200);
  Serial.println("--------------------------------------------------------------");
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
    Serial.print("Key Pressed : ");
    Serial.println(key);
    changeColors(key);
  }
}
