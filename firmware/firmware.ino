#include "USB.h"
#include "USBHIDKeyboard.h"
#include "esp_task_wdt.h"

#define COL_SIZE 7
typedef enum {
  COLNULL=0,
  COL0=2,
  COL1=1,
  COL2=13,
  COL3=12,
  COL4=11,
  COL5=10,
  COL6=9,
} col_t;
col_t Col;

#define ROW_SIZE 4
typedef enum {
  ROWNULL=0,
  ROW0=38,
  ROW1=39,
  ROW2=40,
  ROW3=41,
} row_t;
row_t Row;

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
keyinfo_t PressKeycodeList = {ROWNULL, COLNULL, 0, NULL};

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
  // Keyboard.press('a');
  Serial.println("check_row0");
  Row = ROW0;
  generate_keycode_press();
}

void setup() {
  // put your Hello world Hello world setup code here, to run once:
  pinMode(COL0, OUTPUT);
  pinMode(COL1, OUTPUT);
  pinMode(COL2, OUTPUT);
  pinMode(COL3, OUTPUT);
  pinMode(COL4, OUTPUT);
  pinMode(COL5, OUTPUT);
  pinMode(COL6, OUTPUT);
  pinMode(ROW0, INPUT);
  pinMode(ROW1, INPUT);
  pinMode(ROW2, INPUT);
  pinMode(ROW3, INPUT);

  Keyboard.begin();
  USB.begin();

  attachInterrupt(digitalPinToInterrupt(ROW0), check_row0, RISING);
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
  int col;
  for(col=0; col<7; col++){
    switch(col){
      case 0: Col=COL0; break;
      case 1: Col=COL1; break;
      case 2: Col=COL2; break;
      case 3: Col=COL3; break;
      case 4: Col=COL4; break;
      case 5: Col=COL5; break;
      case 6: Col=COL6; break;
    }
    Serial.println(Col);
    digitalWrite(Col, HIGH);
    //generate_keycode_release();
    delay(500);
    digitalWrite(Col, LOW);
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
