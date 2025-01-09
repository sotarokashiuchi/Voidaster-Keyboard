#include "USB.h"
#include "USBHIDKeyboard.h"
#define RIGHT

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

void add_key_press(){
  for(int i=0; i<6; i++){
    if(KeyPress[i].keycode==0x00){
      KeyPress[i].row = Row;
      KeyPress[i].col = Col;
      KeyPress[i].keycode = KeyMapping[Layer][Row][Col];
      return;
    }
  }
}

void generate_keycode_press(void){
  uint8_t keycode;
  if(KeyState[Row][Col] == 0){
    keycode = KeyMapping[Layer][Row][Col];
    if(keycode==0x00){
      return;
    }
    add_key_press();

#ifdef LEFT
    if(keycode < 0xE8){
      Keyboard.pressRaw(keycode);
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
        Keyboard.releaseRaw(KeyPress[i].keycode);
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
        Keyboard.pressRaw(keycode);
      } else {
        Layer = (layer_t)(keycode - 0xE8);
      }
      Serial.print("Press -> ");
      Serial.println(keycode, DEC);
    } else {
      // release
      if(keycode < 0xE8){
        Keyboard.releaseRaw(keycode);
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
