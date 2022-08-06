#include "caramelo_pad.h"

uint32_t lastKickTime = 0;
uint32_t kickStartTime = 0;

// Compares times without being prone to problems when the micros() counter overflows, every ~70 mins
boolean timeGreaterOrEqual(uint32_t lhs, uint32_t rhs) {
  return (((lhs - rhs) & 2147483648) == 0);
}

Pad pads[NUM_PADS]; // utiliza a classe "Pad" para o número de 'numPads' pads[4] ou [  ,  ,  , ] pode mudar para quantos pads forem necessários

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

void padInit(uint8_t padIndex, std::vector<uint16_t> _triggerThresholds) {
  pads[padIndex].init(piezoPadPins[padIndex], triggerThresholds, notes, ledStripes[padIndex].ledPadType.typeId, padIndex);
}

// -----------------------------------------------------------------------------
// Classificação dos Pads
// -----------------------------------------------------------------------------
Pad::Pad() {
  hitOccurredRecently = false;
  previousHitStillRecent = false;
  inInitialListenPhase = false;
};

  void Pad::init(uint8_t _padNo, std::vector<uint16_t> _triggerThresholds, std::map<char, char> _notes, uint8_t _typePadId, uint8_t _ledPadIndex)
  {
    triggerThresholds = _triggerThresholds;
    notes = _notes;
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
  };

  void Pad::tick()
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
              // Serial.println("Tocado padNo: " + String(padNo));  
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
  };
// -----------------------------------------------------------------------------
// FIM Classificação dos Pads
// -----------------------------------------------------------------------------
