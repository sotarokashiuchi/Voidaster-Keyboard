#include "USB.h"
#include "USBHIDKeyboard.h"

#define COL_SIZE 7
typedef enum {
  COL0=2,
  COL1=1,
  COL2=13,
  COL3=12,
  COL4=11,
  COL5=10,
  COL6=9,
} col_t;
unsigned int Col;

#define ROW_SIZE 4
typedef enum {
  ROW0 =38,
  ROW1=39,
  ROW2=40,
  ROW3=41,
} row_t;
unsigned int Row;

#define LAYER_SIZE 1
typedef enum {
  MAIN,
} layer_t;
layer_t Layer;

USBHIDKeyboard Keyboard;

unsigned int KeyState[COL_SIZE*ROW_SIZE] = {0};
// row * 7 + col
//   0  1  2  3  4  5  6 
// 0
// 1
// 2
// 3

unsigned int KeyMapping[LAYER_SIZE][COL_SIZE*ROW_SIZE] = {
  {
    'a', 'a', 'q', 'w', 'e', 'r', 't',
        // KC.A, KC.A, KC.Q, KC.W, KC.E, KC.R, KC.T,
        // KC.A, KC.A, KC.A, KC.S, KC.D, KC.F, KC.G,
        // KC.A, KC.A, KC.Z, KC.X, KC.C, KC.V, KC.B,
        // KC.A, KC.A, KC.A, KC.A, KC.A, KC.A, KC.A,
  },
};


void check_row0(){
  // Keyboard.press('a');
  Row = 2;
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

  attachInterrupt(digitalPinToInterrupt(ROW0), check_row0, CHANGE);

  Keyboard.println("Completed Setup");
}



void loop() {
  // put your main code here, to run repeatedly:
  for(Col=0; Col<7; Col++){
    switch(Col){
      case 0:  digitalWrite(COL0, HIGH);  break;
      case 1:  digitalWrite(COL1, HIGH);  break;
      case 2:  digitalWrite(COL2, HIGH);  break;
      case 3:  digitalWrite(COL3, HIGH);  break;
      case 4:  digitalWrite(COL4, HIGH);  break;
      case 5:  digitalWrite(COL5, HIGH);  break;
      case 6:  digitalWrite(COL6, HIGH);  break;
    }
    delay(1000);
    switch(Col){
      case 0:  digitalWrite(COL0, LOW);  break;
      case 1:  digitalWrite(COL1, LOW);  break;
      case 2:  digitalWrite(COL2, LOW);  break;
      case 3:  digitalWrite(COL3, LOW);  break;
      case 4:  digitalWrite(COL4, LOW);  break;
      case 5:  digitalWrite(COL5, LOW);  break;
      case 6:  digitalWrite(COL6, LOW);  break;
    }
  }
}
