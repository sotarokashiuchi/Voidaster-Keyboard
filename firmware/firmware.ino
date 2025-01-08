#include "USB.h"
#include "USBHIDKeyboard.h"

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
int ColPin[COL_SIZE] = {2, 1, 13, 12, 11, 10, 9};

#define ROW_SIZE 4
typedef enum {
  ROW0,
  ROW1,
  ROW2,
  ROW3,
} row_t;
row_t Row;
int RowPin[ROW_SIZE] = {38, 39, 40, 41};

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

uint8_t KeyMapping[LAYER_SIZE][ROW_SIZE][COL_SIZE] = {
  {
    {HID_KEY_SHIFT_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T},
    {HID_KEY_SHIFT_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G},
    {HID_KEY_SHIFT_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B},
    {HID_KEY_SHIFT_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_Q, HID_KEY_W, HID_KEY_E, LAYER(NUMBER), HID_KEY_T},
  },
  {
    {HID_KEY_Q, HID_KEY_Q, HID_KEY_Q, HID_KEY_Q, },
  },
};

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

    if(keycode < 0xE8){
      Keyboard.pressRaw(keycode);
    } else {
      Layer = (layer_t)(keycode - 0xE8);
    }

    Serial.print("Press -> ");
    Serial.println(keycode, DEC);
    KeyState[Row][Col] = 1;
  }
}

void generate_keycode_release(void){
  for(int i=0; i<6; i++){
    if(KeyPress[i].keycode != 0x00 && KeyPress[i].col == Col && digitalRead(RowPin[KeyPress[i].row]) == LOW){
      if(KeyPress[i].keycode < 0xE8){
        Keyboard.releaseRaw(KeyPress[i].keycode);
      } else {
        Layer = MAIN;
      }

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
}

void loop() {
  // put your main code here, to run repeatedly:
  int pin;
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
