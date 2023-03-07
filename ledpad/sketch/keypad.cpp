#include <OnewireKeypad.h>

#include <LiquidCrystal.h>

LiquidCrystal lcd(2,3,4,5,6,7);

char KEYS[] = {

  '1', '2', '3', 'A',

  '4', '5', '6', 'B',

  '7', '8', '9', 'C',

  '*', '0', '#', 'D'

};

OnewireKeypad <Print, 16 > KP2(Serial, KEYS, 4, 4, A0, 4700, 1000, ExtremePrec );

void setup () {

  lcd.begin(16, 2);

  Serial.begin(9600);

  KP2.SetKeypadVoltage(5.0);

}

void loop()

{

  char Key;

  byte KState = KP2.Key_State();

  if (KState == PRESSED)

  {

    if ( Key = KP2.Getkey() )

    {

      Serial << "Key: " << Key << "\n";

      lcd.setCursor(0, 0);

      lcd.clear();

      lcd.print(Key);

    }

  }

}