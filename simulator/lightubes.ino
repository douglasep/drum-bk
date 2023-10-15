#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include "Button.h"

#define LED_PIN_1 8 // Connect to the data wires on the pixel strips
#define LED_PIN_2 7
#define LED_PIN_3 6
#define LED_PIN_4 5
#define LED_PIN_5 4
#define LED_PIN_6 3
#define LED_PIN_7 2
#define LED_PIN_8 1
#define LED_PIN_9 0

enum EffectState
{
  NONE_EFFECT,
  STROBE,
  CORTINA_DIREITA,
  CORTINA_ESQUERDA,
  ENCHER,
  ESVAZIAR,
  CYLON_BOUNCE,
  FIRE
};

EffectState effectState = NONE_EFFECT;

int flashDelay = 20;
int returnDelay = 20;
int eyeSize = 5;

// commands
int vermelho = 0xFF0000;
int roxo = 0x800080;
int rosaEscuro = 0xD80213;
int laranja = 0x910b00;
int amarelo = 0xff6400;
int amareloEscuro = 0xFFD700;
int amareloClaro = 0xFF4500;
int verdeClaro = 0x7FFF00;
int verde = 0x00FF00;
int azulClaro = 0x007AA3;
int azul = 0x0000FF;
int branco = 0xFFFFFF;
int offColor = 0x000000;
int colors[] = {
    vermelho,
    roxo,
    rosaEscuro,
    laranja,
    amarelo,
    amareloEscuro,
    amareloClaro,
    verdeClaro,
    verde,
    azulClaro,
    azul,
    branco,
    offColor};

int shadowvermelho = 0xEE0000;
int shadowroxo = 0x600060;
int shadowrosaEscuro = 0xb80213;
int shadowlaranja = 0x710b00;
int shadowamarelo = 0xdf6400;
int shadowamareloEscuro = 0xdFD700;
int shadowamareloClaro = 0xdF2500;
int shadowverdeClaro = 0x5FdF00;
int shadowverde = 0x00dF00;
int shadowazulClaro = 0x005A83;
int shadowazul = 0x0000dF;
int shadowbranco = 0xdFdFdF;
int shadowoffColor = 0x202020;
int shadowcolors[] = {
    shadowvermelho,
    shadowroxo,
    shadowrosaEscuro,
    shadowlaranja,
    shadowamarelo,
    shadowamareloEscuro,
    shadowamareloClaro,
    shadowverdeClaro,
    shadowverde,
    shadowazulClaro,
    shadowazul,
    shadowbranco,
    shadowoffColor};

const int pixelW = 15;
const int pixelH = 15;
const int pixelGap = 5;
const int margin = 60;

class Tubo
{
public:
  int w;
  int h;
  int virtual_w;
  int virtual_h;
  // String memory[42][42];
  int index;
  Adafruit_NeoPixel strip;

  Tubo(int pin, int w, int h, int virtual_w, int virtual_h)
  {
    this->w = w;
    this->h = h;
    this->virtual_w = virtual_w;
    this->virtual_h = virtual_h;
    this->strip = Adafruit_NeoPixel(w * h, pin, NEO_GRB + NEO_KHZ800);
  }

  void drawMemory(int x, int y, int color)
  {
    // this->memory[x][y] = color;
    this->strip.setPixelColor((x * this->w) + y, color);
  }

  void setAll(int color)
  {
    for (int i = 0; i < this->h; i++)
    {
      for (int j = 0; j < this->w; j++)
      {
        this->drawMemory(i, j, color);
      }
    }
  }

  void setPixel(int pixelX, int pixelY, int pixelColor)
  {
    this->drawMemory(pixelX, pixelY, pixelColor);
  }

  void show()
  {
    this->strip.show();
  }

  void showStatic(int color)
  {
    this->setAll(color);
    this->show();
  }
};

class LightTubes
{
public:
  Tubo tubos[9] = {
      Tubo(LED_PIN_1, 6, 27, 6, 42),
      Tubo(LED_PIN_2, 6, 27, 6, 42),
      Tubo(LED_PIN_3, 6, 35, 6, 42),
      Tubo(LED_PIN_4, 6, 42, 6, 42),
      Tubo(LED_PIN_5, 6, 35, 6, 42),
      Tubo(LED_PIN_6, 6, 42, 6, 42),
      Tubo(LED_PIN_7, 6, 35, 6, 42),
      Tubo(LED_PIN_8, 6, 27, 6, 42),
      Tubo(LED_PIN_9, 6, 27, 6, 42)};
  int primaryColor = vermelho;
  int secundaryColor = verde;
  int transitionColor = this->secundaryColor;
  bool mergedTubos = false;
  String mode = "estatico";
  String modeTick = "estatico";

  void initialDraw()
  {
    this->setAll(this->primaryColor);
    this->showLeds();
  }

  void setPixelToColumns(int pixelX, int color, int columns[], int lengthColumns)
  {
    for (int i = 0; i < lengthColumns; i++)
    {
      this->setPixel(pixelX, columns[i], color);
    }
  }

  int getWDefault()
  {
    return 6;
    // int wDefault = 0;
    // for (int i = 0; i < 9; i++) {
    //   if (this->tubos[i].w > wDefault) {
    //     wDefault = this->tubos[i].w;
    //   }
    // }
    // return wDefault;
  }

  void setPixel(int pixelY, int pixelX, int color)
  {
    if (this->mergedTubos)
    {
      int tuboIdx = pixelX / this->getWDefault();
      int pixelXPosition = pixelX - (tuboIdx * this->getWDefault());
      this->tubos[tuboIdx].setPixel(pixelY, pixelXPosition, color);
    }
    else
    {
      for (int i = 0; i < 9; i++)
      {
        this->tubos[i].setPixel(pixelY, pixelX, color);
      }
    }
  }

  void tick(int idx)
  {
    if (this->modeTick == "subida-linha")
    {
      int columns[] = {0, 1, 2, 3, 4, 5};
      // rightToLeft(this->secundaryColor, 4, 1, 2, columns);
    }
    else if (this->modeTick == "encher")
    {
      int columns[] = {0, 1, 2, 3, 4, 5};
      // encher(this->secundaryColor, 1, 1, 2, columns);
    }
    else
    {
      this->tubos[idx].showStatic(this->secundaryColor);
    }
    this->turnOnLeds(this->primaryColor);
  }

  void turnOnLeds(int color)
  {
    this->setPrimaryMode(color);
    this->showLeds();
  }

  void setPrimaryMode(int color)
  {
    this->transitionColor = this->primaryColor;
    if (this->mode == "escudo")
    {
      this->setAll(color);
      this->primaryColor = color;
      // escudoDraw();
    }
    else
    {
      this->setAll(color);
    }
    this->primaryColor = this->transitionColor;
  }

  void setAll(int color)
  {
    for (int i = 0; i < 9; i++)
    {
      this->tubos[i].setAll(color);
    }
  }

  void showLeds()
  {
    for (int i = 0; i < 9; i++)
    {
      this->tubos[i].show();
    }
  }
};

LightTubes lightTubes;
boolean runningLeftToRightEffect = false;
boolean runningRightToLeftEffect = false;
unsigned long leftToRightEffectTime = 0;
unsigned long rightToLeftEffectTime = 0;
unsigned long fireEffectTime = 0;
unsigned long esvaziarEffectTime = 0;
unsigned long encherEffectTime = 0;
unsigned int pixelRightToLeftEffect = 0;
unsigned int pixelParaEsvaziar = 0;
unsigned int pixelParaEncher = 0;

void nextColor()
{
  int index = 0;
  for (int i = 0; i < sizeof(colors); i++)
  {
    if (colors[i] == lightTubes.primaryColor)
    {
      index = i;
      break;
    }
  }
  if (index < sizeof(colors))
  {
    lightTubes.primaryColor = colors[index + 1];
  }
  else
  {
    lightTubes.primaryColor = colors[0];
  }
  lightTubes.turnOnLeds(lightTubes.primaryColor);
}

void nextSecundaryColor()
{
  int index = 0;
  for (int i = 0; i < sizeof(colors); i++)
  {
    if (colors[i] == lightTubes.secundaryColor)
    {
      index = i;
      break;
    }
  }
  if (index < sizeof(colors))
  {
    lightTubes.secundaryColor = colors[index + 1];
  }
  else
  {
    lightTubes.secundaryColor = colors[0];
  }
}

unsigned long strobeEffectTime;
bool strobeLightState = true;
void strobeTick()
{
  strobeEffectTime = millis();
  strobeLightState = true;
  effectState = STROBE;
}

void initCortinaDireitaTick()
{
  effectState = CORTINA_DIREITA;
  leftToRightEffectTime = millis();
  runningLeftToRightEffect = true;
}

void initCortinaEsquerdaTick()
{
  effectState = CORTINA_ESQUERDA;
  leftToRightEffectTime = millis();
  runningLeftToRightEffect = true;
}

int cortinaColumns = 0;
void cortinaTick()
{
  int maxCortinaColumns = 5;
  int columns[2];
  int columnsLength = 1;
  if (lightTubes.mergedTubos)
  {
    maxCortinaColumns = 52;
    columnsLength = 2;
    if (cortinaColumns <= maxCortinaColumns)
    {
      if (effectState == CORTINA_ESQUERDA)
      {
        columns[0] = maxCortinaColumns + 1 - cortinaColumns;
        columns[1] = maxCortinaColumns - cortinaColumns;
      }
      else
      {
        columns[0] = cortinaColumns;
        columns[1] = cortinaColumns + 1;
      }
    }
  }
  else
  {
    if (effectState == CORTINA_ESQUERDA)
    {
      columns[0] = maxCortinaColumns - cortinaColumns;
    }
    else
    {
      columns[0] = cortinaColumns;
    }
  }

  leftToRight(lightTubes.secundaryColor, 39, flashDelay, returnDelay, columns, columnsLength, true);
  Serial.println(cortinaColumns);
  Serial.println(maxCortinaColumns);
  if (cortinaColumns <= maxCortinaColumns)
  {
    Serial.println(cortinaColumns);
    if (runningLeftToRightEffect == false)
    {
      runningLeftToRightEffect = true;
      cortinaColumns++;
      if (lightTubes.mergedTubos)
      {
        cortinaColumns++;
      }
    }
  }
  else
  {
    effectState = NONE_EFFECT;
    runningLeftToRightEffect = false;
    cortinaColumns = 0;
    int color = lightTubes.secundaryColor;
    lightTubes.secundaryColor = lightTubes.primaryColor;
    lightTubes.primaryColor = color;
  }
}

int cylonCurrentColumns[2] = {2, 3};
void cylonInit()
{
  leftToRightEffectTime = millis();
  rightToLeftEffectTime = millis();
  effectState = CYLON_BOUNCE;
  cylonCurrentColumns[0] = 2;
  cylonCurrentColumns[1] = 3;
}

void cylonTickColumns(int columns[])
{
  leftToRightEffectTime = millis();
  rightToLeftEffectTime = millis();
  effectState = CYLON_BOUNCE;
  cylonCurrentColumns[0] = columns[0];
  cylonCurrentColumns[1] = columns[1];
  if (leftToRightEffectTime == 0 && rightToLeftEffectTime == 0)
  {
    effectState = NONE_EFFECT;
  }
}

void cylonTick()
{
  cylonBounce(lightTubes.secundaryColor, eyeSize, flashDelay, returnDelay, cylonCurrentColumns, 2);
  if (leftToRightEffectTime == 0 && rightToLeftEffectTime == 0)
  {
    effectState = NONE_EFFECT;
    lightTubes.turnOnLeds(lightTubes.primaryColor);
  }
}

void fireTick()
{
  fireEffectTime = millis();
  effectState = FIRE;
}

void initEsvaziar()
{
  effectState = ESVAZIAR;
  esvaziarEffectTime = millis();
}

void esvaziarTick()
{
  if (lightTubes.mergedTubos)
  {
    int columns[] = {12, 13, 14, 15, 16, 17, 36, 37, 38, 39, 40, 41};
    esvaziar(lightTubes.secundaryColor, flashDelay, returnDelay, columns, 12);
    if (esvaziarEffectTime <= 0)
    {
      esvaziarEffectTime = millis();
      int columns[] = {18, 19, 20, 21, 22, 23, 30, 31, 32, 33, 34, 35};
      esvaziar(lightTubes.secundaryColor, flashDelay, returnDelay, columns, 12);
    }
    if (esvaziarEffectTime <= 0)
    {
      effectState = NONE_EFFECT;
    }
  }
  else
  {
    int columns[] = {0, 1, 2, 3, 4, 5};
    esvaziar(lightTubes.secundaryColor, flashDelay, returnDelay, columns, 6);
    if (esvaziarEffectTime <= 0)
    {
      int color = lightTubes.secundaryColor;
      lightTubes.secundaryColor = lightTubes.primaryColor;
      lightTubes.primaryColor = color;
      effectState = NONE_EFFECT;
    }
  }
}

void initEncher()
{
  effectState = ENCHER;
  encherEffectTime = millis();
}

void encherTick()
{
  if (lightTubes.mergedTubos)
  {
    int columns[] = {12, 13, 14, 15, 16, 17, 36, 37, 38, 39, 40, 41};
    encher(lightTubes.secundaryColor, flashDelay, returnDelay, columns, 12);
    if (encherEffectTime <= 0)
    {
      encherEffectTime = millis();
      int columns[] = {18, 19, 20, 21, 22, 23, 30, 31, 32, 33, 34, 35};
      encher(lightTubes.secundaryColor, flashDelay, returnDelay, columns, 12);
    }
    if (encherEffectTime <= 0)
    {
      effectState = NONE_EFFECT;
    }
  }
  else
  {
    int columns[] = {0, 1, 2, 3, 4, 5};
    encher(lightTubes.secundaryColor, flashDelay, returnDelay, columns, 6);
    if (encherEffectTime <= 0)
    {
      int color = lightTubes.secundaryColor;
      lightTubes.secundaryColor = lightTubes.primaryColor;
      lightTubes.primaryColor = color;
      effectState = NONE_EFFECT;
    }
  }
}

void strobe(int color, int strobeCount, int flashDelay, int endPause)
{
  if (millis() - strobeEffectTime > flashDelay)
  {
    for (int j = 0; j < strobeCount; j++)
    {
      if (strobeLightState)
      {
        lightTubes.turnOnLeds(offColor);
      }
      else
      {
        lightTubes.turnOnLeds(color);
      }
    }
  }

  if (millis() - strobeEffectTime > ((flashDelay * strobeCount) + endPause))
  {
    lightTubes.turnOnLeds(lightTubes.primaryColor);
    strobeEffectTime = 0;
    strobeLightState = true;
    effectState = NONE_EFFECT;
  }
  strobeLightState = !strobeLightState;
}

int findShadowColor(int color)
{
  int colorIndex = 0;
  for (int i = 0; i < sizeof(colors); i++)
  {
    if (colors[i] == color)
    {
      colorIndex = i;
      break;
    }
  }
  return shadowcolors[colorIndex];
}

void cylonBounce(int color, int eyeSize, int speedDelay, int returnDelay, int columns[], int lengthColumns)
{
  if (leftToRightEffectTime > 0)
  {
    leftToRight(color, eyeSize, speedDelay, returnDelay, columns, lengthColumns, true);
  }
  else if (rightToLeftEffectTime > 0)
  {
    rightToLeft(color, eyeSize, speedDelay, returnDelay, columns, lengthColumns, true);
  }
}

int pixelLeftToRightEffect = 0;
void leftToRight(int color, int eyeSize, int speedDelay, int returnDelay, int columns[], int lengthColumns, bool fill)
{
  int shadowColor = findShadowColor(color);
  const int numLeds = 42;
  int sizeLoop = numLeds - eyeSize - 2;
  if (pixelLeftToRightEffect <= sizeLoop)
  {
    if (millis() - leftToRightEffectTime < millis() + speedDelay)
    {
      if (!fill)
      {
        lightTubes.setPrimaryMode(lightTubes.primaryColor);
      }
      lightTubes.setPixelToColumns(pixelLeftToRightEffect, shadowColor, columns, lengthColumns);
      for (int j = 1; j <= eyeSize; j++)
      {
        lightTubes.setPixelToColumns(pixelLeftToRightEffect + j, color, columns, lengthColumns);
      }
      lightTubes.setPixelToColumns(pixelLeftToRightEffect + eyeSize + 1, shadowColor, columns, lengthColumns);
      lightTubes.showLeds();

      pixelLeftToRightEffect++;
      leftToRightEffectTime = millis();
    }
  }
  else
  {
    if (millis() - leftToRightEffectTime < millis() + returnDelay)
    {
      leftToRightEffectTime = 0;
      pixelLeftToRightEffect = 0;
      runningLeftToRightEffect = false;
    }
  }
}

void rightToLeft(int color, int eyeSize, int speedDelay, int returnDelay, int columns[], int lengthColumns, bool fill)
{
  int shadowColor = findShadowColor(color);
  const int numLeds = 42;
  int sizeLoop = numLeds - eyeSize - 2;
  if (pixelRightToLeftEffect <= sizeLoop)
  {
    int i = sizeLoop - pixelRightToLeftEffect;
    if (millis() - rightToLeftEffectTime < millis() + speedDelay)
    {
      if (!fill)
      {
        lightTubes.setPrimaryMode(lightTubes.primaryColor);
      }
      lightTubes.setPixelToColumns(i, shadowColor, columns, lengthColumns);
      for (int j = 1; j <= eyeSize; j++)
      {
        lightTubes.setPixelToColumns(i + j, color, columns, lengthColumns);
      }
      lightTubes.setPixelToColumns(i + eyeSize + 1, shadowColor, columns, lengthColumns);
      lightTubes.showLeds();

      pixelRightToLeftEffect++;
      rightToLeftEffectTime = millis();
    }
  }
  else
  {
    if (millis() - rightToLeftEffectTime < millis() + returnDelay)
    {
      rightToLeftEffectTime = 0;
      pixelRightToLeftEffect = 0;
      runningRightToLeftEffect = false;
    }
  }
}

void esvaziar(int color, int speedDelay, int returnDelay, int columns[], int lengthColumns)
{
  const int numLeds = 42;
  if (pixelParaEsvaziar < 42)
  {
    if (millis() - esvaziarEffectTime > speedDelay)
    {
      lightTubes.setPixelToColumns(pixelParaEsvaziar, color, columns, lengthColumns);
      lightTubes.showLeds();
      pixelParaEsvaziar++;
      esvaziarEffectTime = millis();
    }
  }
  else
  {
    if (millis() - esvaziarEffectTime > returnDelay)
    {
      esvaziarEffectTime = 0;
      pixelParaEsvaziar = 0;
      return;
    }
  }
}

void encher(int color, int speedDelay, int returnDelay, int columns[], int lengthColumns)
{
  const int numLeds = 42;
  if (pixelParaEncher < 42)
  {
    if (millis() - encherEffectTime > speedDelay)
    {
      lightTubes.setPixelToColumns(numLeds - pixelParaEncher - 1, color, columns, lengthColumns);
      lightTubes.showLeds();
      pixelParaEncher++;
      encherEffectTime = millis();
    }
  }
  else
  {
    if (millis() - encherEffectTime > returnDelay)
    {
      encherEffectTime = 0;
      pixelParaEncher = 0;
      return;
    }
  }
}

void fire(int cooling, int sparking, int speedDelay, int columns[], int lengthColumns)
{
  if (millis() - fireEffectTime > speedDelay)
  {
    int numLeds = 42;
    int margin = random(8, 15);
    int cooldown;
    int heat[numLeds];
    for (int i = 0; i < numLeds; i++)
    {
      heat[i] = 10;
    }

    for (int i = margin; i < numLeds; i++)
    {
      cooldown = random(0, cooling * 10 / numLeds + 2);
      if (cooldown > heat[i])
      {
        heat[i] = 0;
      }
      else
      {
        heat[i] = heat[i] - cooldown;
      }
    }

    for (int k = numLeds - (1 + margin); k >= 2; k--)
    {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    if (random(255) < sparking)
    {
      int y = random(7);
      heat[y] = heat[y] + random(160, 255);
    }

    for (int j = margin; j < numLeds; j++)
    {
      if (heat[j])
      {
        setPixelHeatColor(j, columns, lengthColumns, heat[j]);
      }
    }

    lightTubes.showLeds();
    fireEffectTime = millis();
  }
}

void setPixelHeatColor(int pixel, int columns[], int lengthColumns, int temperature)
{
  int t192 = round((temperature / 255.0) * 191);
  int heatramp = t192 & 0x3F;
  heatramp <<= 2;
  if (t192 > 0x80)
  {
    //   lightTubes.setPixelToColumns(pixel, rgbToHex(255, 255, heatramp), columns);
  }
  else if (t192 > 0x40)
  {
    // lightTubes.setPixelToColumns(pixel, rgbToHex(255, heatramp, 0), columns);
  }
  else
  {
    //  lightTubes.setPixelToColumns(pixel, rgbToHex(heatramp, 0, 0), columns);
  }
}

int valueToHex(int c)
{
  String hex = String(c, HEX);
  // if (hex.length() == 1) {
  //   hex = "0" + hex;
  // }
  // return hex;
  return CRGB::Red;
}

Button tick_a0(A0);
Button tick_a1(A1);
void setup()
{
  Serial.begin(115200);
  Serial.println("Iniciando o projeto");

  tick_a0.begin();
  tick_a1.begin();

  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(LED_PIN_5, OUTPUT);
  pinMode(LED_PIN_6, OUTPUT);
  pinMode(LED_PIN_7, OUTPUT);
  pinMode(LED_PIN_8, OUTPUT);
  pinMode(LED_PIN_9, OUTPUT);
  lightTubes.initialDraw();
}

void loop()
{
  if (tick_a0.pressed())
  {
    initCortinaDireitaTick();
  }

  if (tick_a1.pressed())
  {
    initCortinaEsquerdaTick();
  }
  if (effectState == STROBE)
  {
    strobe(lightTubes.secundaryColor, 10, flashDelay, returnDelay);
  }
  else if (effectState == CORTINA_DIREITA)
  {
    cortinaTick();
  }
  else if (effectState == CORTINA_ESQUERDA)
  {
    cortinaTick();
  }
  else if (effectState == ENCHER)
  {
    encherTick();
  }
  else if (effectState == ESVAZIAR)
  {
    esvaziarTick();
  }
  else if (effectState == CYLON_BOUNCE)
  {
    cylonTick();
  }
  else if (effectState == FIRE)
  {
    int columns[2] = {2, 3};
    fire(55, 120, flashDelay, columns, 2);
  }
}
