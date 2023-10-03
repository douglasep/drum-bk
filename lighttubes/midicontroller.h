void onControlChange(byte channel, byte control, byte value) {
  if(channel != 0x00) return;
  if(control == 0x00) {
  //   Serial.print("Freq changed to: ");
  //   Serial.println(freq);
    switch(value) {
    // vermelho primario (300hz)
    case 0x00:
        colorSc = colors[0];
        turnOnLeds(colorSc);
        break;
    // azul claro (600hz)
    case 0x01:
        colorSc = colors[1];
        turnOnLeds(colorSc);
        break;
    // rosa escuro (900hz)
    case 0x02:
        colorSc = colors[2];
        turnOnLeds(colorSc);
        break;
    // verde (1200hz)
    case 0x03:
        colorSc = colors[3];
        turnOnLeds(colorSc);
        break;
    // (1500hz)
    case 0x04:
        colorSc = colors[4];
        turnOnLeds(colorSc);
        break;
    // (1.800hz)
    case 0x05:
        colorSc = colors[5];
        turnOnLeds(colorSc);
        break;
    // (2.100hz)
    case 0x06:
        colorSc = colors[6];
        turnOnLeds(colorSc);
        break;
    // (2.400hz)
    case 0x08:
        colorSc = colors[7];
        turnOnLeds(colorSc);
        break;
    // (2.700hz)
    case 0x09:
        colorSc = colors[9];
        turnOnLeds(colorSc);
        break;
    // amarelo primario (6.000hz)
    case 0x0a:
        colorPr = colors[0];
        break;
    // amarelo secundario (6.500hz)
    case 0x0b:
        colorPr = colors[1];
        break;
    // amarelo escuro (7.000hz)
    case 0x0c:
        colorPr = colors[2];
        break;
    // amarelo escuro (7.500hz)
    case 0x0d:
        colorPr = colors[3];
        break;
    // (5.600hz)
    case 0x0e:
        colorPr = colors[4];
        break;
    // (5.600hz)
    case 0x0f:
        colorPr = colors[5];
        break;
    // (6.100hz)
    case 0x10:
        colorPr = colors[6];
        break;
    // (6.600hz)
    case 0x11:
        colorPr = colors[7];
        break;
    // (7.500hz)
    case 0x12:
        randomEffectMijoDoCachorroAlado();
        break;
    // (8.500hz)
    case 0x13:
        cortinaEffect();
        break;
    // (9.500hz)
    case 0x14:
        fadeOut();
        break;
    }
  }
}