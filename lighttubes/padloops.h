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

