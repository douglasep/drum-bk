#ifndef CARAMELO_PAD_H
#define CARAMELO_PAD_H

#include <Arduino.h>
#include <EEPROM.h>
#include <MIDIUSB.h>
#include <map>
#include <vector>

// -----------------------------------------------------------------------------
// Declaração de variáveis e constantes para a classe Pad
// -----------------------------------------------------------------------------

#define NUM_PADS 9
#define initialHitReadDuration 850    // In microseconds. Shorter times will mean less latency, but less velocity-accuracy
#define midiVelocityScaleDownAmount 2 // Number of halvings that will be applied to MIDI velocity
#define tailRecordResolution 68

void padInit(uint8_t padIndex);
class Pad {
public:
  std::vector<uint16_t> triggerThresholds;
  std::map<char, char> notes;
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
  
  Pad();
  void init(uint8_t _padNo, std::vector<uint16_t> _triggerThresholds, std::map<char, char> _notes, uint8_t _typePadId, uint8_t _ledPadIndex);
  void tick();
};

#endif
