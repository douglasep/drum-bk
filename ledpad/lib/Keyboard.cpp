class Keyboard {
  #define ROW_NUM     4 // four rows
  #define COLUMN_NUM  4 // three columns

  char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
  };

  Keyboard(byte pin_rows, byte pin_column) {
    Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
    return 0;
  }

  int keyPressed() {

    return 0;
  }

};