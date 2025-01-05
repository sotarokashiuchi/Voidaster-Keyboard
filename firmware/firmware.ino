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
  int keycode;
  struct keyinfo_tag *next;
} keyinfo_t;
keyinfo_t PressKeycodeList = {ROW0, COL0, 0, NULL};

USBHIDKeyboard Keyboard;
esp_task_wdt_config_t config = {10000, true};

unsigned int KeyState[ROW_SIZE][COL_SIZE] = {0};

byte KeyMapping[LAYER_SIZE][ROW_SIZE][COL_SIZE] = {
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
  p->row = p->next->row;
  p->col = p->next->col;
  p->keycode = p->next->keycode;
  free(temp);
}

void generate_keycode_press(void){
  Serial.println("generate_keycode_press");
  if(KeyState[Row][Col] == LOW){
    // Keyboard.press(KeyMapping[Layer][Row][Col]);
    Serial.print("Press -> ");
    Serial.println(KeyMapping[Layer][Row][Col], DEC);
    KeyState[Row][Col] = HIGH;
    //new_press_keycode();
  }
}

void generate_keycode_release(void){
  for(keyinfo_t* p=PressKeycodeList.next; p!=NULL;p=p->next){
    if(p->col == Col && digitalRead(p->row) == LOW){
      // Keyboard.release(p->keycode);
      Serial.print("Release -> ");
      Serial.println(KeyMapping[Layer][Row][Col]);
      KeyState[Row][Col] = LOW;
      delete_press_keycode(p);
    }
  }
}


void check_row0(){
  Serial.println("check_row0");
  Row = ROW0;
  generate_keycode_press();
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

  attachInterrupt(digitalPinToInterrupt(RowPin[ROW0]), check_row0, RISING);
  Layer = MAIN;

  Keyboard.println("Completed Setup");
  Serial.begin(115200);
  Serial.println("SETUP!");

  // esp_task_wdt_deinit();
  // esp_task_wdt_init(&config);
  // esp_task_wdt_add(NULL);
}



void loop() {
  // put your main code here, to run repeatedly:
  int pin;
  for(Col=COL0; Col<COL_SIZE; Col = (col_t)((int)Col + 1)){
    Serial.println(Col);
    digitalWrite(ColPin[Col], HIGH);
    //generate_keycode_release();
    delay(500);
    digitalWrite(ColPin[Col], LOW);
    delay(500);
  }


  // if (digitalRead(ROW2) == HIGH) {
  //   Keyboard.press('a');
  //   keystate['a'] = 1;
  //   delay(10);
  // }

  // if (keystate['a'] == 1) {
  //   if (digitalRead(ROW2) == LOW) {
  //     Keyboard.release('a');
  //     keystate['a'] = 0;
  //   }
  // }
}
