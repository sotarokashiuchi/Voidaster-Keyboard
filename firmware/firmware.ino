#include "USB.h"
#include "USBHIDKeyboard.h"
#define LEFT

// 0x01 - 0x03 3
// 0x74 - 0x86 19
// 0x8D - 0x8F 3
// 0x95 - 0xDF 75 shifted
// 0xE8 - 0xFF 18 layer
#define SHIFTED_KEY_A								0x95 //a
#define SHIFTED_KEY_B								0x96 //b
#define SHIFTED_KEY_C								0x97 //b
#define SHIFTED_KEY_D								0x98 //b
#define SHIFTED_KEY_E								0x99 //b
#define SHIFTED_KEY_F								0x9A //b
#define SHIFTED_KEY_G								0x9B //b
#define SHIFTED_KEY_H								0x9C //b
#define SHIFTED_KEY_I								0x9D //b
#define SHIFTED_KEY_J								0x9E //b
#define SHIFTED_KEY_K								0x9F //b
#define SHIFTED_KEY_L								0xA0 //b
#define SHIFTED_KEY_M								0xA1 //b
#define SHIFTED_KEY_N								0xA2 //b
#define SHIFTED_KEY_O								0xA3 //b
#define SHIFTED_KEY_P								0xA4 //b
#define SHIFTED_KEY_Q								0xA5 //b
#define SHIFTED_KEY_R								0xA6 //b
#define SHIFTED_KEY_S								0xA7 //b
#define SHIFTED_KEY_T								0xA8 //b
#define SHIFTED_KEY_U								0xA9 //b
#define SHIFTED_KEY_V								0xAA //b
#define SHIFTED_KEY_W								0xAB //b
#define SHIFTED_KEY_X								0xAC //b
#define SHIFTED_KEY_Y								0xAD //b
#define SHIFTED_KEY_Z								0xAE //b
#define SHIFTED_KEY_EXCLAIM					    0xAF //!
#define SHIFTED_KEY_AT									0xB0 //@
#define SHIFTED_KEY_HASH								0xB1 //#
#define SHIFTED_KEY_DOLLAR							0xB2 //$
#define SHIFTED_KEY_PERCENT							0xB3 //%
#define SHIFTED_KEY_CIRCUMFLEX					0xB4 //^
#define SHIFTED_KEY_AMPERSAND						0xB5 //&
#define SHIFTED_KEY_ASTERISK					  0xB6 //*
#define SHIFTED_KEY_LEFT_PAREN					0xB7 //(
#define SHIFTED_KEY_RIGHT_PAREN					0xB8 //)
#define SHIFTED_KEY_UNDERSCORE					0xB9 //_
#define SHIFTED_KEY_PLUS								0xBA //+
#define SHIFTED_KEY_LEFT_CURLY_BRACE		0xBB //{
#define SHIFTED_KEY_RIGHT_CURLY_BRACE		0xBC //}
#define SHIFTED_KEY_PIPE								0xBD //|
#define SHIFTED_KEY_TILDE								0xBE //~
#define SHIFTED_KEY_COLON								0xBF //:
#define SHIFTED_KEY_DOUBLE_QUOTE				0xC0 //"
#define SHIFTED_KEY_LEFT_ANGLE_BRACKET	0xC1 //<
#define SHIFTED_KEY_RIGHT_ANGLE_BRACKET	0xC2 //>
#define SHIFTED_KEY_QUESTION						0xC2 //?

#define COL_SIZE 7
typedef enum {
  COL0,
  COL1,
  COL2,
  COL3,
  COL4,
  COL5,
  COL6,
} col_t;
col_t Col;

#define ROW_SIZE 4
typedef enum {
  ROW0,
  ROW1,
  ROW2,
  ROW3,
} row_t;
row_t Row;

// ! Max of LAYER_SIZE is 18
#define LAYER_SIZE 2
#define LAYER(x) ((x)+0xE8)
typedef enum {
  MAIN,
  NUMBER,
} layer_t;
layer_t Layer;

typedef struct {
  row_t row;
  col_t col;
  uint8_t keycode;
} keyinfo_t;
keyinfo_t KeyPress[6];
KeyReport KeyReports;
USBHIDKeyboard Keyboard;

unsigned int KeyState[ROW_SIZE][COL_SIZE] = {0};

#ifdef LEFT
int ColPin[COL_SIZE] = {2, 1, 13, 12, 11, 10, 9};
int RowPin[ROW_SIZE] = {38, 39, 40, 41};
uint8_t KeyMapping[LAYER_SIZE][ROW_SIZE][COL_SIZE] = {
  {
    {HID_KEY_SHIFT_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T},
    {HID_KEY_SHIFT_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G},
    {HID_KEY_SHIFT_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B},
    {HID_KEY_SHIFT_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, LAYER(NUMBER), HID_KEY_NONE},
  },
  {
    {HID_KEY_Q, HID_KEY_Q, HID_KEY_Q, HID_KEY_Q, },
  },
};
#endif
#ifdef RIGHT
int ColPin[COL_SIZE] = {9, 10, 11, 12, 13, 1, 2};
int RowPin[ROW_SIZE] = {38, 39, 40, 41};
uint8_t KeyMapping[LAYER_SIZE][ROW_SIZE][COL_SIZE] = {
  {
    {HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_NONE, HID_KEY_NONE},
    {HID_KEY_H, HID_KEY_J, HID_KEY_K, HID_KEY_L, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE},
    {HID_KEY_N, HID_KEY_M, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE},
    {HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE},
  },
  {
    {HID_KEY_Q, HID_KEY_Q, HID_KEY_Q, HID_KEY_Q, },
  },
};
#endif

void add_key_press(uint8_t keycode){
  for(int i=0; i<6; i++){
    if(KeyPress[i].keycode==0x00){
      KeyPress[i].row = Row;
      KeyPress[i].col = Col;
      KeyPress[i].keycode = keycode;
      return;
    }
  }
}

void update_key_report(bool sw_flag, uint8_t keycode){
  uint8_t key;
  uint8_t modifier;
  if(keycode < 0x95){
    modifier=0x0;
    key=keycode;
  } else if(keycode <= 0xDF){//sifted
    modifier = 0b00000010;
    if(keycode < 0xB9){
      key = keycode-0x91;
    } else {
      key = keycode-0x8C;
    }
  }
  
  if(sw_flag==true){
    for(int i=0; i<6; i++){
      if(KeyReports.keys[i]==0x00){
        KeyReports.modifiers |= modifier;
        KeyReports.keys[i] = key;
        break;
      }
    }
  } else {
    for(int i=0; i<6; i++){
      if(KeyReports.keys[i]==key){
        KeyReports.modifiers &= ~modifier;
        KeyReports.keys[i] = 0;
        break;
      }
    }
  }
  Keyboard.sendReport(&KeyReports);
}

void generate_keycode_press(void){
  uint8_t keycode;
  if(KeyState[Row][Col] == 0){
    keycode = KeyMapping[Layer][Row][Col];
    if(keycode==0x00){
      return;
    }
    add_key_press(keycode);

#ifdef LEFT
    if(keycode < 0xE8){
      update_key_report(true, keycode);
    } else {
      Layer = (layer_t)(keycode - 0xE8);
      Serial1.write(keycode);
    }
#endif
#ifdef RIGHT
    Serial1.write(0b00000001);
    Serial1.write(keycode);
    if(keycode >= 0xE8){
      Layer = (layer_t)(keycode - 0xE8);
    }
#endif
    Serial.print("Press -> ");
    Serial.println(keycode, DEC);
    KeyState[Row][Col] = 1;
  }
}

void generate_keycode_release(void){
  for(int i=0; i<6; i++){
    if(KeyPress[i].keycode != 0x00 && KeyPress[i].col == Col && digitalRead(RowPin[KeyPress[i].row]) == LOW){
#ifdef LEFT
      if(KeyPress[i].keycode < 0xE8){
        update_key_report(false, KeyPress[i].keycode);
      } else {
        Layer = MAIN;
        Serial1.write(LAYER(MAIN));
      }
#endif
#ifdef RIGHT
      Serial1.write(0b00000000);
      Serial1.write(KeyPress[i].keycode);
      if(KeyPress[i].keycode >= 0xE8){
        Layer = MAIN;
      }
#endif
      Serial.print("Release -> ");
      Serial.println(KeyPress[i].keycode);
      KeyState[KeyPress[i].row][Col] = 0;
      KeyPress[i].keycode = 0x00;
    }
  }
}

void setup() {
  // put your Hello world Hello world setup code here, to run once:
  for(int i=0; i<COL_SIZE; i++){
    pinMode(ColPin[i], OUTPUT);
  }
  for(int i=0; i<ROW_SIZE; i++){
    pinMode(RowPin[i], INPUT_PULLDOWN);
  }

  Keyboard.begin();
  USB.begin();

  Layer = MAIN;

  for(int i=0; i<6; i++){
    KeyPress[i].keycode = 0x00;
  }

  Serial.begin(115200);
  Serial.println("SETUP!");
#ifdef LEFT
  Serial1.begin(115200, SERIAL_8N1, 18, 17);
#endif
#ifdef RIGHT
  Serial1.begin(115200, SERIAL_8N1, 17, 18);
#endif

}

void loop() {
  // put your main code here, to run repeatedly:
#ifdef LEFT
  if(Serial1.available() >= 2){
    uint8_t control = Serial1.read();
    uint8_t keycode = Serial1.read();

    if(control == 0b00000001){
      // press
      if(keycode < 0xE8){
        update_key_report(true, keycode);
      } else {
        Layer = (layer_t)(keycode - 0xE8);
      }
      Serial.print("Press -> ");
      Serial.println(keycode, DEC);
    } else {
      // release
      if(keycode < 0xE8){
        update_key_report(false, keycode);
      } else {
        Layer = MAIN;
      }

      Serial.print("Release -> ");
      Serial.println(keycode, DEC);
    }
  }
#endif
#ifdef RIGHT
  if(Serial1.available() >= 1){
    Layer = (layer_t)(Serial1.read() - 0xE8);
  }
#endif

  for(Col=COL0; Col<COL_SIZE; Col = (col_t)((int)Col+1)){
    digitalWrite(ColPin[Col], HIGH);

    for(Row=ROW0; Row<ROW_SIZE; Row = (row_t)((int)Row+1)){
        if(digitalRead(RowPin[Row]) == HIGH){
          Serial.print(Layer);
          Serial.print(Row);
          Serial.println(Col);
          generate_keycode_press();
        }
    }
    generate_keycode_release();
    
    digitalWrite(ColPin[Col], LOW);
  }
}
