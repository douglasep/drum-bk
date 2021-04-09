#include <EEPROM.h>

#define initialHitReadDuration 850 // In microseconds. Shorter times will mean less latency, but less velocity-accuracy
#define midiVelocityScaleDownAmount 2 // Number of halvings that will be applied to MIDI velocity
#define firstInputPin A0 // os outros pinos serão somados a este por meio do (numPads) ex: se numPads = 2 // pinos usados seRão A0 e A1
#define kickPin A0

#define learnPad 99 // Set this to the index of the pad you want to learn; set it back to 99 to stop learning from happening
#define numPads 10 // Número de pads que irá utilizar
#define numLeds 10 // Número de leds para acender em sequencia com os pads

// #define tailRecordResolution 128 // Define um tempo de recepção para cada pad (pad 1 = 128 / pad2 = 128*2 / pad3 = 128*3) assim ele diferencia cada pad
#define tailRecordResolution 68

const uint16_t triggerThresholds[numPads] = {500, 500, 500, 500, 500, 500, 500, 500, 500}; // Threshold iniciais {pad1, pad2, pad3, pad4}

// \/-\/-\/-\/-\/ COLOCAR AS NOTAS AQUI \/-\/-\/-\/-\/-\/
const uint16_t notes[numPads] = {36, 37, 38, 39, 40, 42, 43, 44, 41}; // Notas {pad1, pad2, pad3, pad4}

uint32_t lastKickTime = 0;
uint32_t kickStartTime = 0;
// Compares times without being prone to problems when the micros() counter overflows, every ~70 mins
boolean timeGreaterOrEqual(uint32_t lhs, uint32_t rhs) {
  return (((lhs - rhs) & 2147483648) == 0);
}

class Pad {
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

    Pad() {
      hitOccurredRecently = false;
      previousHitStillRecent = false;
      inInitialListenPhase = false;
    }

    void init(uint8_t padNo, boolean learning) {
      if (learning) {
        for (int i = 0; i < tailRecordResolution; i++) {
          EEPROM.write(i + padNo * tailRecordResolution, 0);
        }
      }
      else {
        tailLength = 0;
        for (int i = 0; i < tailRecordResolution; i++) {
          if (EEPROM.read(i + padNo * tailRecordResolution) != 0) tailLength = i + 1;
        }
        tailLength = tailRecordResolution;
      }
    }

    void blinkLed(uint8_t padNo, int velocity, int timeTurnedOn) {
      uint8_t ledN0 = padNo + numPads;
      int maxVelocity = 127;
      int brightness = (velocity / maxVelocity) * 255;
      int fadeAmount = brightness;
      analogWrite(ledN0, brightness);
      Serial.println("ledno = " + String(ledN0));
      Serial.println(brightness);
      while(brightness > 0) {
        // change the brightness for next time through the loop:
        brightness = brightness + fadeAmount;
        // reverse the direction of the fading at the ends of the fade:
        fadeAmount = -fadeAmount;
        // wait for 30 milliseconds to see the dimming effect
              Serial.println(brightness);
        delay(timeTurnedOn);
      }
    }

    void tick(uint8_t padNo, boolean learning) {
      // Read the piezo
      uint16_t value = analogRead(firstInputPin + padNo);

      if (inInitialListenPhase) {
        initialListen:
        // For the next few milliseconds, look out for the highest "spike" in the reading from the piezo. Its height is representative of the hit's velocity
        if (value > highestYet) {
          highestYet = value;
          highestValueTime = micros();
        }

        // If we've spent enough time reading...
        if (timeGreaterOrEqual(startReadingTime + initialHitReadDuration, micros())) {
          // Send the MIDI note
          uint8_t midiVelocity = min(127, ((highestYet >> midiVelocityScaleDownAmount) + 1));
          uint16_t nota = notes[padNo];
          usbMIDI.sendNoteOn(nota, midiVelocity, 1); // We add 1 onto the velocity so that the result is never 0, which would mean the same as a note-off
          //Serial.println("pad: " + String((int)padNo));
           Serial.println("velocity: " + String(highestYet)); // Send the unscaled velocity value to the serial monitor too, for debugging / fine-tuning
           Serial.println("peak time: " + String(highestValueTime - startReadingTime)); // Send the unscaled velocity value to the serial monitor too, for debugging / fine-tuning
          // Serial.println();
          hitOccurredRecently = true;
          newRecordSet = false;
          inInitialListenPhase = false;
        }
      }

      else {
        // Assume the normal hit-threshold
        uint16_t thresholdNow = triggerThresholds[padNo]; // Aqui ele atribui os Thresholds iniciais [35, 25 , 10 , 6]


        uint32_t msPassed; // declara váriavel para gravar quanto tempo passou

        // But, if a hit occurred very recently, we need to set a higher threshold for triggering another hit, otherwise the dissipating vibrations
        // of the previous hit would trigger another one now
        if (hitOccurredRecently) {

          uint32_t usPassed = micros() - highestValueTime; // A função micros() conta quanto tempo o arduino está rodando o programa - ultima leitura do piezo
          msPassed = usPassed >> 10; // (0b10000000000 = 1024) ou seja 1024us


          if (learning) {
            if (msPassed >= tailRecordResolution) { // Se o tempo da ultima leitura for maior que 128ms 
              hitOccurredRecently = false;
              // if (newRecordSet) {
              // Serial.println("----------------------------------------");
              // for (int i = 0; i < tailRecordResolution; i++) { 
              // if (i % 16 == 0) Serial.println();
              //  Serial.print(String((int)EEPROM.read(i + padNo * tailRecordResolution)) + ", "); 
              //  }
              //}
            }
            else {
              uint16_t nowComparedToHighest = ceil((double)value / (double)highestYet * 128); // ceil faz => Divisão arredondada para o menor número acima /// --> Intensidade piezo / (Maior Intensidade recebida * 128)

              if (value > highestYet) {
                Serial.println("higher than original found, " + String((uint32_t)nowComparedToHighest) + ", at " + String(msPassed) + "ms");
                Serial.println("value: " + String(value));
                Serial.println("highestYet: " + String(highestYet));
              }

              uint8_t previousRecord = EEPROM.read(msPassed + padNo * tailRecordResolution); // Função EEPROM.read()
              if (nowComparedToHighest > previousRecord) {
                newRecordSet = true;
                EEPROM.write(msPassed + padNo * tailRecordResolution, min(255, nowComparedToHighest)); 
                  // Função min(x, y) calcula o menor valor entre 255 e nowComparedHighest 
                  // EEPROM.write(address, value) vai colocar como endereço o tempo atual com resolução para cada pad, 
                  // O dado será ou 255(8bits) ou nowComparedToHighest
              }
            }
            thresholdNow = 1024;
          }

          else {
            if (msPassed >= tailLength) hitOccurredRecently = false; // Se o tempo que passou desde a último hit for maior que 128ms ele desliga a flag -> hitOcurredRecently
            else {
              // Work out how high a reading we'd need to see right now in order to conclude that another hit has occurred
              uint32_t currentDynamicThreshold; // variável para novo threshold
              if (usPassed < initialHitReadDuration) currentDynamicThreshold = highestYet;
              else currentDynamicThreshold = ((uint32_t)EEPROM.read(msPassed + padNo * tailRecordResolution) * highestYet) >> 7;

              thresholdNow += currentDynamicThreshold;

              // If previous hit is still "sounding" too...
              if (previousHitStillRecent) {
                uint32_t usPrevious = micros() - previousHitTime; // identificará qual foi o tempo que ocorreu o ultimo hit
                uint32_t msPrevious = usPrevious >> 10;
                if (msPrevious >= tailLength) previousHitStillRecent = false; // Se o gap entre o ultimo hit for maior que 128ms, desliga a flag -> "previousHitStillRecent"
                else {
                  thresholdNow += ((uint32_t)EEPROM.read(msPrevious + padNo * tailRecordResolution) * previousHitPeak) >> 7; // se for recente, ele modifica o Threshold
                }
              }
            }
          }
        }


        // If we've breached the threshold, it means we've got a hit!
        if (value >= thresholdNow) { // Se o sinal do piezo ultrapassar o threshold, tem hit
          if (hitOccurredRecently) { // Serve para acionar as flags de hit anterior
            previousHitStillRecent = true;
            previousHitPeak = highestYet;
            previousHitTime = highestValueTime;
          }


          if (hitOccurredRecently) {
            // Serial.println("gap: " + String(msPassed));
            // Serial.println("thresh: " + String(thresholdNow));
            // Serial.println("stored max for now: " + String((uint32_t)EEPROM.read(msPassed + padNo * tailRecordResolution)));
            // Serial.println("value we would store for this: " + String(ceil((float)value / (float)highestYet * 128)));
            // Serial.println("--------------------------------");
          }

          startReadingTime = micros(); // tempo que irá começar a ler o piezo é o tempo atual
          highestYet = 0; // zera a leitura do piezo
          inInitialListenPhase = true; // Aciona a flag -> "inInitialListenPhase" para começar a tratar os valores que o piezo está enviando
          goto initialListen;
        }
      }
    }

};

Pad pads[numPads]; // utiliza a classe "Pad" para o número de 'numPads' pads[4] ou [  ,  ,  , ] pode mudar para quantos pads forem necessários

void setup() {
  for (int i = 0; i < numPads && i != 5; i++) { // loop iniciará a função init()"iniciar" para todos os pads
    pads[i].init(i, i == learnPad);
  }
  //pinMode(kickPin, INPUT_PULLUP);
}

boolean recordingKick = false;
uint32_t kickValuesSum = 0;
uint32_t numKickValues = 0;
uint32_t kickRecordStartTime;

void loop() {

  for (int i = 0; i < numPads; i++) {
    pads[i].tick(i, i == learnPad);
    //Serial.println(learnPad);
  }

  if (micros() - lastKickTime > 100000) { // se tempo atual for maior que 100ms => ACHO QUE é 100ms sem certeza
    int kickValue = analogRead(kickPin);
    if (kickValue > 40) {
      lastKickTime = micros();     
      //usbMIDI.sendNoteOn(0, 36, 1);
      //Serial.println(kickValue);
      // Serial.println("kick");
      // Serial.println("attack-time: " + String(lastKickTime - kickStartTime));
    }
  }

}