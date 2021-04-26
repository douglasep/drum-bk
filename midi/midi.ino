      #include <Arduino.h>
      #include <SPIFFS.h>
      #include <ESPFlash.h>
      #include <EEPROM.h>
      #include <Adafruit_NeoPixel.h>
      #include <Led_Matrix.h>
      #include <vector>
      
      #define NUM_PADS 10
      
      // -----------------------------------------------------------------------------
      // Pinos utilizados como Saída para os Leds dos Pads
      // -----------------------------------------------------------------------------

      #define LED_PIN_1 4
      #define LED_PIN_2 21
      #define LED_PIN_3 18
      #define LED_PIN_4 22
      #define LED_PIN_5 14
      #define LED_PIN_6 2
      #define LED_PIN_7 16
      #define LED_PIN_8 17
      #define LED_PIN_9 5
      #define LED_PIN_10 13 // descarte
      
      // -----------------------------------------------------------------------------
      // Pinos utilizados como Entrada dos piezos dos Pads
      // -----------------------------------------------------------------------------

      #define PIEZO_PIN_1 27 // ok
      #define PIEZO_PIN_2 26 // ok
      #define PIEZO_PIN_3 25 // ok
      #define PIEZO_PIN_4 33 // ok
      #define PIEZO_PIN_5 32 // ok
      #define PIEZO_PIN_6 35 // ok
      #define PIEZO_PIN_7 34 // ok
      #define PIEZO_PIN_8 39 // ok (vn)
      #define PIEZO_PIN_9 36 // ok mas com disturbios
      #define PIEZO_PIN_10 44 // descarte 
        
      // -----------------------------------------------------------------------------
      // Cores disponíveis para os LEDS
      // -----------------------------------------------------------------------------
      uint8_t colorMode = 0;
      uint32_t colorPr = 0xFF0000; // Cor Primaria
      uint32_t colorSc = 0x0000FF; // Cor Secundaria
      // -----------------------------------------------------------------------------

      uint8_t piezoPadPins[NUM_PADS] = {
        PIEZO_PIN_1, 
        PIEZO_PIN_2, 
        PIEZO_PIN_3,
        PIEZO_PIN_4,
        PIEZO_PIN_5,
        PIEZO_PIN_6,
        PIEZO_PIN_7,
        PIEZO_PIN_8,
        PIEZO_PIN_9,
        PIEZO_PIN_10
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

      // modo 0 => solido secundário
      // modo 1 => solido primario
      // modo 2 => interno primario // bordas em secundario
      Stored_Matrix mtx_stored_t1_m0 = { ledPadType1, MODE_SOLID_PR, "/t1_m0" }; // tipo 1 - modo 0
      Stored_Matrix mtx_stored_t1_m1 = { ledPadType1, MODE_SOLID_PR, "/t1_m1" }; // tipo 1 - modo 1
      Stored_Matrix mtx_stored_t1_m2 = { ledPadType1, MODE_SOLID_PR, "/t1_m2" }; // tipo 1 - modo 2
      Stored_Matrix mtx_stored_t2_m0 = { ledPadType2, MODE_SOLID_PR, "/t2_m0" }; // tipo 2 - modo 0
      Stored_Matrix mtx_stored_t2_m1 = { ledPadType2, MODE_SOLID_PR, "/t2_m1" }; // tipo 2 - modo 1
      Stored_Matrix mtx_stored_t2_m2 = { ledPadType2, MODE_SOLID_PR, "/t2_m2" }; // tipo 2 - modo 2
      Stored_Matrix mtx_stored_t3_m0 = { ledPadType3, MODE_SOLID_PR, "/t3_m0" }; // tipo 3 - modo 0
      Stored_Matrix mtx_stored_t3_m1 = { ledPadType3, MODE_SOLID_PR, "/t3_m1" }; // tipo 3 - modo 1
      Stored_Matrix mtx_stored_t3_m2 = { ledPadType3, MODE_SOLID_PR, "/t3_m2" }; // tipo 3 - modo 2

      // -----------------------------------------------------------------------------
      // Declaração de variáveis e constantes para a classe Pad
      // -----------------------------------------------------------------------------
      // initialReadDuration 850
      // ScaleDownAmount 2
      // tailRecordResolution 128
      #define initialHitReadDuration 850    // In microseconds. Shorter times will mean less latency, but less velocity-accuracy
      #define midiVelocityScaleDownAmount 2 // Number of halvings that will be applied to MIDI velocity
      #define tailRecordResolution 68
      const uint16_t triggerThresholds[NUM_PADS] = {700, 700, 700, 700, 700, 700, 700, 700, 700, 700}; // Threshold iniciais {pad1, pad2, pad3, pad4}
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
      // PADS DO TIPO  => 4 PIEZOS + 4 FITAS LEDS 27X4
      // -----------------------------------------------------------------------------
      struct LedStripeNeoPixel ledStripe1Type1 {
        ledPadType1,
        new Adafruit_NeoPixel(ledPadType1.numLeds, LED_PIN_1, NEO_GRB + NEO_KHZ800)
      };
      struct LedStripeNeoPixel ledStripe2Type1 {
        ledPadType1,
        new Adafruit_NeoPixel(ledPadType1.numLeds, LED_PIN_2, NEO_GRB + NEO_KHZ800)
      };
      struct LedStripeNeoPixel ledStripe3Type1 {
        ledPadType1,
        new Adafruit_NeoPixel(ledPadType1.numLeds, LED_PIN_3, NEO_GRB + NEO_KHZ800)
      };
      struct LedStripeNeoPixel ledStripe4Type1 {
        ledPadType1,
        new Adafruit_NeoPixel(ledPadType1.numLeds, LED_PIN_4, NEO_GRB + NEO_KHZ800)
      };

      // -----------------------------------------------------------------------------

      // -----------------------------------------------------------------------------
      // PADS DO TIPO 2 => 2 PIEZOS + 2 FITAS LEDS 36X4
      // -----------------------------------------------------------------------------
      struct LedStripeNeoPixel ledStripe1Type2 {
        ledPadType2,
        new Adafruit_NeoPixel(ledPadType2.numLeds, LED_PIN_5, NEO_GRB + NEO_KHZ800)
      };
      struct LedStripeNeoPixel ledStripe2Type2 {
        ledPadType2,
        new Adafruit_NeoPixel(ledPadType2.numLeds, LED_PIN_6, NEO_GRB + NEO_KHZ800)
      };
      // -----------------------------------------------------------------------------

      // -----------------------------------------------------------------------------
      // PADS DO TIPO 3 => 4 PIEZOS + 4 FITAS LEDS 45X4
      // -----------------------------------------------------------------------------
      struct LedStripeNeoPixel ledStripe1Type3 {
        ledPadType3,
        new Adafruit_NeoPixel(ledPadType3.numLeds, LED_PIN_7, NEO_GRB + NEO_KHZ800)
      };
      struct LedStripeNeoPixel ledStripe2Type3 {
        ledPadType3,
        new Adafruit_NeoPixel(ledPadType3.numLeds, LED_PIN_8, NEO_GRB + NEO_KHZ800)
      };
      struct LedStripeNeoPixel ledStripe3Type3 {
        ledPadType3,
        new Adafruit_NeoPixel(ledPadType3.numLeds, LED_PIN_9, NEO_GRB + NEO_KHZ800)
        
      };
      struct LedStripeNeoPixel ledStripe4Type3 {
        ledPadType3,
        new Adafruit_NeoPixel(ledPadType3.numLeds, LED_PIN_10, NEO_GRB + NEO_KHZ800)
      };
      // -----------------------------------------------------------------------------

      vector<LedStripeNeoPixel> ledStripes = {
        ledStripe1Type1,
        ledStripe2Type1,
        ledStripe3Type1,
        ledStripe4Type1,
        ledStripe1Type2,
        ledStripe2Type2,
        ledStripe1Type3,
        ledStripe2Type3,
        ledStripe3Type3,
        ledStripe4Type3
      }; // Lista de fitas led

      // -----------------------------------------------------------------------------
      // Salva matrizes de cores na memória Flash
      // Aciona fita led conforme um modo determinado
      // -----------------------------------------------------------------------------
      void initColorSetups() {
        // 0 => Solido cor secundaria, 1 => Solido cor Primaria, 2 => Borda
        Led_Matrix *led_matrix = new Led_Matrix(mtx_stored_t1_m0.ledPadType.rowsSize, mtx_stored_t1_m0.ledPadType.colsSize, colorPr, colorSc);
        vector<uint32_t> mtx;
        
        // Salva em memoria

        // TIPO 1
        ESPFlash<uint32_t> t1_m0(mtx_stored_t1_m0.address);
        mtx = led_matrix->get_matrix_mode(mtx_stored_t1_m0.modo);
        for(int ii = 0; ii < led_matrix->num_leds; ii++)
        {
          t1_m0.append(mtx[ii]); // salva matriz no endereco 
          // Serial.print("colorIndex: " + String(ii) + " hex: " + String(mtx[ii]) + " ");
        }
        uint32_t clrs[led_matrix->num_leds];
        ESPFlash<uint32_t> colorsStores(mtx_stored_t1_m0.address);
        colorsStores.getFrontElements(clrs, mtx_stored_t1_m0.ledPadType.numLeds);
        // Serial.println("");
        for(uint32_t j=0;j<mtx_stored_t1_m0.ledPadType.numLeds;j++) {
          // Serial.print("colorIndex: " + String(j) + " hex: " + String(clrs[j]) + " ");
          //ledStripes[ii].neoPixelStripe->setPixelColor(ii, colors[ii]);
        }
        mtx.clear();

        Serial.println("Número de cores: " + String(led_matrix->num_leds));
        Serial.println("Endereço Valor no Flash: " + String(mtx_stored_t1_m0.address));
        
        ESPFlash<uint32_t> t1_m1(mtx_stored_t1_m1.address);
        mtx = led_matrix->get_matrix_mode(mtx_stored_t1_m1.modo);
        for(int ii = 0; ii < led_matrix->num_leds; ii++)
        {
          t1_m1.append(mtx[ii]); // salva matriz no endereco 
        }
        mtx.clear();

        Serial.println("Número de cores: " + String(led_matrix->num_leds));
        Serial.println("Endereço Valor no Flash: " + String(mtx_stored_t1_m1.address));
        
        ESPFlash<uint32_t> t1_m2(mtx_stored_t1_m2.address);
        mtx = led_matrix->get_matrix_mode(mtx_stored_t1_m2.modo);
        for(int ii = 0; ii < led_matrix->num_leds; ii++)
        {
          t1_m2.append(mtx[ii]); // salva matriz no endereco 
        }
        mtx.clear();

        Serial.println("Número de cores: " + String(led_matrix->num_leds));
        Serial.println("Endereço Valor no Flash: " + String(mtx_stored_t1_m2.address));
        
        // Tipo 2
        // Altera tamanho e cores da matriz para o tipo 2
        led_matrix->set_size(ledPadType2.rowsSize, ledPadType2.colsSize);
        led_matrix->set_primary_color(colorPr);
        led_matrix->set_secondary_color(colorSc);

        // Salva em memoria
        ESPFlash<uint32_t> t2_m0(mtx_stored_t2_m0.address);
        mtx = led_matrix->get_matrix_mode(mtx_stored_t2_m0.modo);
        for(int ii = 0; ii < led_matrix->num_leds; ii++)
        {
          t2_m0.append(mtx[ii]); // salva matriz no endereco 
        }
        mtx.clear();

        Serial.println("Número de cores: " + String(led_matrix->num_leds));
        Serial.println("Endereço Valor no Flash: " + String(mtx_stored_t2_m0.address));

        ESPFlash<uint32_t> t2_m1(mtx_stored_t2_m1.address);
        mtx = led_matrix->get_matrix_mode(mtx_stored_t2_m1.modo);
        for(int ii = 0; ii < led_matrix->num_leds; ii++)
        {
          t2_m1.append(mtx[ii]); // salva matriz no endereco 
        }
        mtx.clear();

        Serial.println("Número de cores: " + String(led_matrix->num_leds));
        Serial.println("Endereço Valor no Flash: " + String(mtx_stored_t2_m1.address));

        ESPFlash<uint32_t> t2_m2(mtx_stored_t2_m2.address);
        mtx = led_matrix->get_matrix_mode(mtx_stored_t2_m2.modo);
        for(int ii = 0; ii < led_matrix->num_leds; ii++)
        {
          t2_m2.append(mtx[ii]); // salva matriz no endereco 
        }
        mtx.clear();

        // TIPO 3
        // Altera tamanho e cores da matriz para o tipo 3
        led_matrix->set_size(ledPadType3.rowsSize, ledPadType3.colsSize);
        led_matrix->set_primary_color(colorPr);
        led_matrix->set_secondary_color(colorSc);  
        // Salva em memoria
        ESPFlash<uint32_t> t3_m0(mtx_stored_t3_m0.address);
        mtx = led_matrix->get_matrix_mode(mtx_stored_t3_m0.modo);
        for(int ii = 0; ii < led_matrix->num_leds; ii++)
        {
          t3_m0.append(mtx[ii]); // salva matriz no endereco 
        }
        mtx.clear();
        
        ESPFlash<uint32_t> t3_m1(mtx_stored_t3_m1.address);
        mtx = led_matrix->get_matrix_mode(mtx_stored_t3_m1.modo);
        for(int ii = 0; ii < led_matrix->num_leds; ii++)
        {
          t3_m1.append(mtx[ii]); // salva matriz no endereco 
        }
        mtx.clear();

        ESPFlash<uint32_t> t3_m2(mtx_stored_t3_m2.address);
        mtx = led_matrix->get_matrix_mode(mtx_stored_t3_m2.modo);
        for(int ii = 0; ii < led_matrix->num_leds; ii++)
        {
          t3_m2.append(mtx[ii]); // salva matriz no endereco 
        }
        mtx.clear();

        // Exclui objeto de tipo de matrizes
        delete led_matrix;
        led_matrix = NULL;

      }
      // -----------------------------------------------------------------------------

      // -----------------------------------------------------------------------------
      // Retorna um stored_matrix com o endereço da matriz salva em memoria flash
      // @param typeId [uint8_t] número do tipo de pad
      // @param modeId [uint8_t] número do modo de operação
      // @return [struct Stored_Matrix] Retorna o endereço salvo na memoria flash
      // -----------------------------------------------------------------------------
      struct Stored_Matrix getStoredMatrix(uint8_t typeId, uint8_t modeId) {
        uint8_t storedMatrixId = pow(typeId, 2) + modeId;
        switch (storedMatrixId)
        {
          case 1: // t1_m0 1^2+0=2
            return mtx_stored_t1_m0;
            break;
          case 2: // t1_m1 1^2+1=2
            return mtx_stored_t1_m1;
            break;
          case 3: // t1_m2 1^2+2=3
            return mtx_stored_t1_m2;
            break;
          case 4: // t2_m0 2^2+0=4
            return mtx_stored_t2_m0;
            break;
          case 5: // t2_m1 2^2+1=5
            return mtx_stored_t2_m1;
            break;
          case 6: // t2_m2
            return mtx_stored_t2_m2;
            break;
          case 9: // t3_m0
            return mtx_stored_t3_m0;
            break;
          case 10: // t3_m1
            return mtx_stored_t3_m1;
            break;
          case 11: // t3_m2
            return mtx_stored_t3_m2;
            break;
          default:
            break;
        }
      }

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
            uint16_t thresholdNow = 720; //  os Thresholds iniciais
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
                delay(60);
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
                    delay(60);
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
      // Alterar modo de cores
      // -----------------------------------------------------------------------------
      void changeColorMode() {
        switch(colorMode) {
          case 1: 
            colorPr = 0xff0000;
            colorSc = 0x0000ff;
            break;
          case 2:
            colorPr = 0x00ff00;
            colorSc = 0x0f0f00;
            break;
          case 3:
            colorPr = 0x00ffff;
            colorSc = 0xffff00;
            break;
          default:
            colorPr = 0xffffff;
            colorSc = 0x000000;
            break;
        }
        colorMode = colorMode++ > 3 ? 0 : colorMode++;
      }
      // -----------------------------------------------------------------------------
      
      // -----------------------------------------------------------------------------
      // Configurações de inicialização do ESP
      // -----------------------------------------------------------------------------
      void setup()
      {
        
        Serial.begin(115200);
        Serial.println("Iniciando o projeto");
        // delay(500);
        // Serial.println("Iniciando SPIFFS!");
        // Serial.println();
        // SPIFFS.begin();
        // Serial.println("Formatting SPIFFS. Will take a while...");
        // SPIFFS.format();
        // Serial.println("Salvando matrizes de cores...");
        analogReadResolution(10);
        // initColorSetups(); // Inicia matrizes de cores utilizadas durante runtime. Calcula e salva em memória flash
        // fitaLedTeste.begin();
        
        Serial.println("--------------------------------------------------------------");
        Serial.println("triggerThresholds: " + String(triggerThresholds[0]) + " " + String(triggerThresholds[1]));
        Serial.println("--------------------------------------------------------------");
        // Biblioteca de Cores
        for (uint16_t i = 0; i < NUM_PADS; i++)
        {
          ledStripes[i].neoPixelStripe->begin();
          pads[i].init(piezoPadPins[i], ledStripes[i].ledPadType.typeId, i);
          Serial.println("PIEZO_PIN: " + String(piezoPadPins[i] + "LED_PIN: " ));
        }
        
        for (uint16_t i = 0; i < NUM_PADS; i++)
        {
          triggerLeds(MODE_SOLID_PR, ledStripes[i].ledPadType.typeId, i, colorPr);
        }
        Serial.println("Iniciando PADS: ");
        delay(400);
        for (uint16_t i = 0; i < NUM_PADS; i++)
        {
          triggerLeds(MODE_SOLID_SC, ledStripes[i].ledPadType.typeId, i, colorSc);
        }
        Serial.println("Iniciando PADS: ");
        delay(400);
        for (uint16_t i = 0; i < NUM_PADS; i++)
        {
          triggerLeds(MODE_SOLID_PR, ledStripes[i].ledPadType.typeId, i, colorPr);
        }
        
        Serial.println("--------------------------------------------------------------");
        Serial.printf("biggest free block: %i\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
        Serial.println("--------------------------------------------------------------");
      }

      // -----------------------------------------------------------------------------
      // Loop principal do ESP
      // -----------------------------------------------------------------------------
      void loop()
      {
        uint8_t changingColorMode = digitalRead(23);
        Serial.printf("Tentando ler modo de cor para: %i\n", changingColorMode);
        if (changingColorMode == HIGH) {
          Serial.printf("Trocando mo+do de cor para: %i\n", colorMode);
          changeColorMode();
        }
        // Serial.println("loop..." + String(micros()));
        for (uint8_t i = 0; i < NUM_PADS; i++)
        {
          // Serial.println("piezoPadPins " + String(i));
          pads[i].tick();
          
          //Serial.println(learnPad);
          // TESTES
          // -----------------------------------------------------------------------------
          // struct Stored_Matrix sm = getStoredMatrix(ledStripes[i].ledPadType.typeId, MODE_SOLID_SC);
          // uint32_t colors[sm.ledPadType.numLeds];
          // ESPFlash<uint32_t> colorsStores(sm.address);
          // colorsStores.getFrontElements(colors, sm.ledPadType.numLeds);
          // for(uint32_t ii=0;ii<sm.ledPadType.numLeds;ii++) {
          //   Serial.print("colorIndex: " + String(ii) + " hex: " + String(colorsStores.getElementAt(ii)) + " ");
          //   ledStripes[i].neoPixelStripe->setPixelColor(ii, colorsStores.getElementAt(ii));
          // }
          // ledStripes[i].neoPixelStripe->show();
          // Serial.println("-----------------------------------------------------------------");
          // -----------------------------------------------------------------------------
        }

      }
