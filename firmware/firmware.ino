#include "USB.h"
#include "USBHIDKeyboard.h"
#include "esp_task_wdt.h"

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

#define LAYER_SIZE 1
typedef enum {
  MAIN,
} layer_t;
layer_t Layer;

struct keyinfo_tag;
typedef struct keyinfo_tag {
  row_t row;
  col_t col;
  uint8_t keycode;
  struct keyinfo_tag *next;
} keyinfo_t;
keyinfo_t PressKeycodeList = {ROW0, COL0, 0, NULL};

USBHIDKeyboard Keyboard;
esp_task_wdt_config_t config = {10000, true};

unsigned int KeyState[ROW_SIZE][COL_SIZE] = {0};

uint8_t KeyMapping[LAYER_SIZE][ROW_SIZE][COL_SIZE] = {
  {
    {'a', 'a', 'q', 'w', 'e', 'r', 't',},
    {'a', 'a', 'q', 'w', 'e', 'r', 't',},
    {'a', 'a', 'q', 'w', 'e', 'r', 't',},
    {'a', 'a', 'q', 'w', 'e', 'r', 't',},
        // KC.A, KC.A, KC.Q, KC.W, KC.E, KC.R, KC.T,
        // KC.A, KC.A, KC.A, KC.S, KC.D, KC.F, KC.G,
        // KC.A, KC.A, KC.Z, KC.X, KC.C, KC.V, KC.B,
        // KC.A, KC.A, KC.A, KC.A, KC.A, KC.A, KC.A,
  },
};

void new_press_keycode(){
  keyinfo_t* p = (keyinfo_t*)malloc(sizeof(keyinfo_t));
  p->row = Row;
  p->col = Col;
  p->keycode = KeyMapping[Layer][Row][Col];
  p->next = PressKeycodeList.next;
  PressKeycodeList.next = p;
}

void delete_press_keycode(keyinfo_t* p){
  keyinfo_t* temp;
  temp = p->next;
  p->next = p->next->next;
  free(temp);
}

void display_press_keycode(){
  Serial.println("display");
  for(keyinfo_t* p = PressKeycodeList.next; p!=NULL; p=p->next){
    Serial.print("row:");
    Serial.print(p->row);
    Serial.print(" col:");
    Serial.print(p->col);
    Serial.print(" keycode:");
    Serial.println(p->keycode);
  }
}

void generate_keycode_press(void){
  if(KeyState[Row][Col] == 0){
    Keyboard.press(KeyMapping[Layer][Row][Col]);
    Serial.print("Press -> ");
    Serial.println(KeyMapping[Layer][Row][Col], DEC);
    KeyState[Row][Col] = 1;
    new_press_keycode();
    display_press_keycode();
  }
}

void generate_keycode_release(void){
  for(keyinfo_t* p=&PressKeycodeList; p!=NULL && p->next!=NULL; p=p->next){
    if(p->next->col == Col && digitalRead(RowPin[p->next->row]) == LOW){
      Keyboard.release(p->next->keycode);
      Serial.print("Release -> ");
      Serial.println(p->next->keycode);
      KeyState[p->next->row][Col] = 0;
      delete_press_keycode(p);
      display_press_keycode();
    }
  }
}

void setup() {
  // put your Hello world Hello world setup code here, to run once:
  for(int i=0; i<COL_SIZE; i++){
    pinMode(ColPin[i], OUTPUT);
  }
  for(int i=0; i<ROW_SIZE; i++){
    pinMode(RowPin[i], INPUT);
  }

  Keyboard.begin();
  USB.begin();

  Layer = MAIN;

  Keyboard.println("Completed Setup");
  Serial.begin(115200);
  Serial.println("SETUP!");
}



void loop() {
  // put your main code here, to run repeatedly:
  int pin;
  for(Col=COL0; Col<COL_SIZE; Col = (col_t)((int)Col+1)){
    digitalWrite(ColPin[Col], HIGH);
    delay(10);
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
    delay(10);
  }
}
