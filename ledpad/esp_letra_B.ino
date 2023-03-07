        #include <Arduino.h>
        #include <ESPFlash.h>
        #include <EEPROM.h>
        #include <Adafruit_NeoPixel.h>
        #include <Led_Matrix.h>
        #include <vector>
        
        #define NUM_PADS 5

        // -----------------------------------------------------------------------------
        // Pinos utilizados como Saída para os Leds dos Pads
        // -----------------------------------------------------------------------------
        // PINO     -> L,  P
        // PINO 2   -> 12, 32
        // PINO 4   -> 15, 36
        // PINO 6   -> 2, 39
        // PINO 8   -> 4, 34
        // PINO 10  -> 13, 35
        #define LED_PIN_1 4   // P
        #define LED_PIN_2 12  // P
        #define LED_PIN_3 15  // M pad 4
        #define LED_PIN_4 2   // M
        #define LED_PIN_5 13  // G

        // -----------------------------------------------------------------------------
        // Pinos utilizados como Entrada dos   piezos dos Pads
        // -----------------------------------------------------------------------------
        // PINO     -> L,  P
        // PINO 2   -> 12, 32
        // PINO 4   -> 15, 36
        // PINO 6   -> 2, 39
        // PINO 8   -> 4, 34
        // PINO 10  -> 13, 35

        #define PIEZO_PIN_1 34 // P
        #define PIEZO_PIN_2 32 // P
        #define PIEZO_PIN_3 36 // M pad 4
        #define PIEZO_PIN_4 39 // M
        #define PIEZO_PIN_5 35 // G
 
      // -----------------------------------------------------------------------------
      // Cores disponíveis para os LEDS
      // -----------------------------------------------------------------------------
      
      uint8_t colorMode = 0;
      
      uint32_t vermelho =   0xFF0000; // ok
      uint32_t roxo =       0xb400ff; // a311a6
      uint32_t rosaEscuro = 0xD80213; // ok
      uint32_t laranja =    0x910b00; // ok
      uint32_t amarelo =    0xff6400;
      uint32_t verde =      0x00FF00; // ok
      uint32_t azulClaro =  0x007AA3; // ok
      uint32_t azul =       0x0000FF; // ok

      uint32_t colorPr = vermelho; // Cor Primaria
      uint32_t colorSc = verde; // Cor Secundaria

      // -----------------------------------------------------------------------------

      uint8_t piezoPadPins[NUM_PADS] = {
        PIEZO_PIN_1, 
        PIEZO_PIN_2, 
        PIEZO_PIN_3,
        PIEZO_PIN_4,
        PIEZO_PIN_5,
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

      // Tipo 1 => 27x4
      // Tipo 2 => 36x4
      // Tipo 3 => 45X4
      struct Led_Pad_Type {
        Pad_Type typeId;
        uint8_t rowsSize;
        uint8_t colsSize;
        uint8_t numLeds;
        uint32_t primaryColor;
        uint32_t secondaryColor;
      };

      struct Led_Pad_Type ledPadType1 = { PAD_TYPE_1, 45, 4, 108, colorPr, colorSc };
      struct Led_Pad_Type ledPadType2 = { PAD_TYPE_2, 45, 4, 144, colorPr, colorSc };
      struct Led_Pad_Type ledPadType3 = { PAD_TYPE_3, 45, 4, 180, colorPr, colorSc };

      // Pad em armazenamento
      // Tipo 2 => 36x4
      // Tipo 3 => 45X4
      struct Stored_Matrix {
        Led_Pad_Type ledPadType;
        uint8_t modo;
        char address[7];
      };

      // -----------------------------------------------------------------------------
      // Declaração de variáveis e constantes para a classe Pad
      // -----------------------------------------------------------------------------
      // initialReadDuration 850
      // ScaleDownAmount 2
      // tailRecordResolution 128
      #define initialHitReadDuration 850    // In microseconds. Shorter times will mean less latency, but less velocity-accuracy
      #define midiVelocityScaleDownAmount 2 // Number of halvings that will be applied to MIDI velocity
      #define tailRecordResolution 68
      const uint16_t triggerThresholds[NUM_PADS] = {500, 500, 500, 500, 500}; // Threshold iniciais {pad1, pad2, pad3, pad4}
      // const uint8_t triggerThresholds[NUM_PADS] = {500,500,500,500}; // Threshold iniciais {pad1, pad2, pad3, pad4}

      uint32_t lastKickTime = 0;
      uint32_t kickStartTime = 0;
      // Compares times without being prone to problems when the micros() counter overflows, every ~70 mins
      boolean timeGreaterOrEqual(uint32_t lhs, uint32_t rhs)
      {
        return (((lhs - rhs) & 2147483648) == 0);
      }
      // -----------------------------------------------------------------------------

      // -----------------------------------------------------------------------------
      // Variáveis para utilizar como teste
      // -----------------------------------------------------------------------------
      // #define PIN 2
      // #define NUMLED 6
      // Adafruit_NeoPixel fitaLedTeste(NUMLED, PIN, NEO_GRB + NEO_KHZ800);

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

      // -----------------------------------------------------------------------------

      // -----------------------------------------------------------------------------
      // PADS DO TIPO 2 => 2 PIEZOS + 2 FITAS LEDS 36X4
      // -----------------------------------------------------------------------------
      struct LedStripeNeoPixel ledStripe1Type2 {
        ledPadType2,
        new Adafruit_NeoPixel(ledPadType2.numLeds, LED_PIN_3, NEO_GRB + NEO_KHZ800)
      };
      struct LedStripeNeoPixel ledStripe2Type2 {
        ledPadType2,
        new Adafruit_NeoPixel(ledPadType2.numLeds, LED_PIN_4, NEO_GRB + NEO_KHZ800)
      };
      // -----------------------------------------------------------------------------

      // -----------------------------------------------------------------------------
      // PADS DO TIPO 3 => 4 PIEZOS + 4 FITAS LEDS 45X4
      // -----------------------------------------------------------------------------
      struct LedStripeNeoPixel ledStripe1Type3 {
        ledPadType3,
        new Adafruit_NeoPixel(ledPadType3.numLeds, LED_PIN_5, NEO_GRB + NEO_KHZ800)
      };
      // -----------------------------------------------------------------------------

      vector<LedStripeNeoPixel> ledStripes = {
        ledStripe1Type1,
        ledStripe2Type1,
        ledStripe1Type2,
        ledStripe2Type2,
        ledStripe1Type3
      }; // Lista de fitas led

      //------------------------------------------------------------------------------
      // Aciona fita led conforme um modo determinado
      // @param modo [int] Modo para alteração [0 => Desligado, 1 => Modo 1 (Todos leds acionados em uma cor), 2 => Modo 2 (Bordas coloridas) ]
      // @param setupPad [LedPadConjunto] Configurações de cores para cada led da fita
      // @param ledPad [Adafruit_NeoPixel] Fita led
      //------------------------------------------------------------------------------
      void triggerLeds(uint8_t modeId, uint8_t typePadId, uint8_t ledPadIndex, uint32_t color)
      {
        // struct Stored_Matrix sm = getStoredMatrix(typePadId, modeId);
        // uint32_t colors[sm.ledPadType.numLeds];
        // ESPFlash<uint32_t> colorsStores(sm.address);
        // colorsStores.getFrontElements(colors, sm.ledPadType.numLeds);

        // Serial.println("Número de cores: " + String(sm.ledPadType.numLeds));
        // Serial.println("Endereço Valor no Flash: " + String(sm.address));
        // Serial.println("modeID: " + String(modeId));
        // Serial.println("Última cor: " + String(colors[sm.ledPadType.numLeds]));
        for (int i = 0; i < ledStripes[ledPadIndex].ledPadType.numLeds; i++)
        {                                  // For each pixel in strip->..
            ledStripes[ledPadIndex].neoPixelStripe->setPixelColor(i, color); //  Set pixel's color (in RAM)
        }
        ledStripes[ledPadIndex].neoPixelStripe->show();                  //  Update strip to match
        // for (uint8_t i = 0; i < sm.ledPadType.numLeds; i++)
        // {
        //   // Serial.println("--------------------------------------------");
        //   // Serial.print("colorIndex: " + String(i+1) + " hex: " + String(colorsStores.getElementAt(i)) + " ");
        //   // Serial.println("--------------------------------------------");
        //   ledStripes[ledPadIndex].neoPixelStripe->setPixelColor(i, colors[i]);
        // }
        // ledStripes[ledPadIndex].neoPixelStripe->show();
        Serial.println("--------------------------------------------");
        Serial.println(String(ledPadIndex));
        Serial.println("tipo: " + String(ledStripes[ledPadIndex].ledPadType.typeId));
        Serial.println("--------------------------------------------");
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
            // Serial.println("--");
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
              Serial.println("--------------------------------------------");
              Serial.println("Tocado padNo: " + String(padNo));  
              // Serial.println("Tocado ledPadIndex: " + String(ledPadIndex));
              // Serial.println("Tocado highestYet: " + String(highestYet));
              Serial.println("--------------------------------------------");
              triggerLeds(MODE_SOLID_PR, typePadId, ledPadIndex, colorPr);

              hitOccurredRecently = true;
              newRecordSet = false;
              inInitialListenPhase = false;
            }
          }

          else {
            // Assume the normal hit-threshold
            uint16_t thresholdNow = 673; //  os Thresholds iniciais
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
            // Serial.println("--");
            // Serial.println("thresholdNow " + String(thresholdNow) + "value: " + String(value) + " tocada valida?: " + String(value >= thresholdNow));
            // Serial.println("--");
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
      // Modos Iniciais
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
      void changeColorMode1(uint32_t modeSwitcher) {
        int16_t j=0, i=0;
        for (i = 0; i <= modeSwitcher; i += (1023 / 8)) {
          j++;
          colorMode = j;
        }
        switch(colorMode) {
          case 1: 
            colorPr = verde;
            colorSc = vermelho;
            break;
          case 2:
            colorPr = laranja;
            colorSc = azul;
            break;
          case 3:
            colorPr = azulClaro;
            colorSc = amarelo;
            break;
          case 4:
            colorPr = laranja;
            colorSc = rosaEscuro;
            break;
          case 5:
            colorPr = azul;
            colorSc = vermelho;
            break;
          case 6:
            colorPr = rosaEscuro;
            colorSc = azul;
            break;
          case 7:
            colorPr = roxo;
            colorSc = amarelo;
            break;
          case 8:
            colorPr = verde;
            colorSc = rosaEscuro;
            break;
          default:
            colorPr = verde;
            colorSc = vermelho;
            break;
        }
        turnOnLeds(colorSc, 100);
      }

      void changeColorMode2(uint32_t modeSwitcher) {
        uint16_t j=8, i=0;
        for (i = 0; i <= modeSwitcher; i += 127) {
          j++;
          colorMode = j;
        }
        switch(colorMode) {
          case 9:
            colorPr = vermelho;
            colorSc = verde;
            break;
          case 10:
            colorPr = azulClaro;
            colorSc = roxo;
            break;
          case 11:
            colorPr = rosaEscuro;
            colorSc = azulClaro;
            break;
          case 12:
            colorPr = azul;
            colorSc = laranja;
            break;
          case 13:
            colorPr = laranja;
            colorSc = verde;
            break;
          case 14:
            colorPr = amarelo;
            colorSc = roxo;
            break;
          case 15:
            colorPr = laranja;
            colorSc = azulClaro;
            break;
          case 16:
            colorPr = rosaEscuro;
            colorSc = laranja;
            break;
          default:
            colorPr = vermelho;
            colorSc = verde;
            break;
        }
        turnOnLeds(colorSc, 100);
      }
      // -----------------------------------------------------------------------------
      

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
        turnOnLeds(colorPr, 200);
      }

      // -----------------------------------------------------------------------------
      // Configurações de inicialização do ESP
      // -----------------------------------------------------------------------------
      void setup()
      {
        Serial.begin(115200);
        Serial.println("Iniciando o projeto");
        analogReadResolution(10);
        pinMode(14, INPUT);
        for (uint8_t i = 0; i < NUM_PADS; i++)
        {
          ledStripes[i].neoPixelStripe->setBrightness(190);
        }
        Serial.println("--------------------------------------------------------------");
        Serial.println("triggerThresholds: " + String(triggerThresholds[0]) + " " + String(triggerThresholds[1]));
        Serial.println("--------------------------------------------------------------");
        // Biblioteca de Cores
        turnOnLedsModePrSc(200, 200);
        
        Serial.println("--------------------------------------------------------------");
        Serial.printf("biggest free block: %i\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
        Serial.println("--------------------------------------------------------------");
      }
      // -----------------------------------------------------------------------------
      // Potenciometros para alteraçao de modo de cores (16 cores 8 em cada potenciometro)
      // -----------------------------------------------------------------------------
      int32_t potColorMode1 = 0;
      int32_t potColorMode2 = 0;
      int32_t potBrightness = 0;
      int16_t safetyRange = 75;
      // int16_t safetyRangeBrightness = 20;
      // -----------------------------------------------------------------------------
      // Loop principal do ESP
      // -----------------------------------------------------------------------------
      void loop()
      {
        int16_t switcherMode1 = analogRead(14);
        int16_t switcherMode2 = analogRead(27);
        // int16_t switchBrightness = analogRead(26);
        for (uint8_t i = 0; i < NUM_PADS; i++)
        {
          pads[i].tick();
        }
        if ((potColorMode1 < (switcherMode1 - safetyRange)) || (potColorMode1 > (switcherMode1 + safetyRange))) {
          Serial.println("Potenciometro 1: " + String(switcherMode1));
          potColorMode1 = switcherMode1;
          changeColorMode1(potColorMode1);
        }
      }