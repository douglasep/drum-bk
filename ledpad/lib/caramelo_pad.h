#ifndef caramelo_pad_h
#define caramelo_pad_h

#include <Arduino.h>
#include <EEPROM.h>
#include <MIDIUSB.h>
#include <map>

// -----------------------------------------------------------------------------
// Declaração de variáveis e constantes para a classe Pad
// -----------------------------------------------------------------------------

#define NUM_PADS 9
#define initialHitReadDuration 850    // In microseconds. Shorter times will mean less latency, but less velocity-accuracy
#define midiVelocityScaleDownAmount 2 // Number of halvings that will be applied to MIDI velocity
#define tailRecordResolution 68

const uint16_t triggerThresholds[NUM_PADS] = { 350, 350, 350, 350, 350, 350, 350, 350, 350 }; // Threshold iniciais {pad1, pad2, pad3, pad2}

uint32_t lastKickTime = 0;
uint32_t kickStartTime = 0;
// Compares times without being prone to problems when the micros() counter overflows, every ~70 mins
boolean timeGreaterOrEqual(uint32_t lhs, uint32_t rhs) {
  return (((lhs - rhs) & 2147483648) == 0);
}

class Pad {
  Pad();
  void init();
  void tick();
}

Pad pads[NUM_PADS]; // utiliza a classe "Pad" para o número de 'numPads' pads[4] ou [  ,  ,  , ] pode mudar para quantos pads forem necessários

