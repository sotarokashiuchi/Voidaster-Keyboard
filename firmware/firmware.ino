#include "USB.h"
#include "USBHIDKeyboard.h"

#define col0 2
#define col1 1
#define col2 13
#define col3 12
#define col4 11
#define col5 10
#define col6 9
#define row0 38
#define row1 39
#define row2 40
#define row3 41

USBHIDKeyboard Keyboard;

int row, col;

int keystate[7*4] = {0};
// row * 7 + col
//   0  1  2  3  4  5  6 
// 0
// 1
// 2
// 3

int keymapping[1][7*4] = {
  {
    'a', 'a', 'q', 'w', 'e', 'r', 't',
        // KC.A, KC.A, KC.Q, KC.W, KC.E, KC.R, KC.T,
        // KC.A, KC.A, KC.A, KC.S, KC.D, KC.F, KC.G,
        // KC.A, KC.A, KC.Z, KC.X, KC.C, KC.V, KC.B,
        // KC.A, KC.A, KC.A, KC.A, KC.A, KC.A, KC.A,
  }
};


void CheckRow0(){
  Keyboard.press('a');
}

void setup() {
  // put your Hello world Hello world setup code here, to run once:
  pinMode(col0, OUTPUT);
  pinMode(col1, OUTPUT);
  pinMode(col2, OUTPUT);
  pinMode(col3, OUTPUT);
  pinMode(col4, OUTPUT);
  pinMode(col5, OUTPUT);
  pinMode(col6, OUTPUT);
  pinMode(row0, INPUT);
  pinMode(row1, INPUT);
  pinMode(row2, INPUT);
  pinMode(row3, INPUT);

  Keyboard.begin();
  USB.begin();

  attachInterrupt(digitalPinToInterrupt(row0), CheckRow0, CHANGE);

  Keyboard.println("Completed Setup");
}



void loop() {
  // put your main code here, to run repeatedly:
  for(col=0; col<7; col++){
    switch(col){
      case 0:  digitalWrite(col0, HIGH);  break;
      case 1:  digitalWrite(col1, HIGH);  break;
      case 2:  digitalWrite(col2, HIGH);  break;
      case 3:  digitalWrite(col3, HIGH);  break;
      case 4:  digitalWrite(col4, HIGH);  break;
      case 5:  digitalWrite(col5, HIGH);  break;
      case 6:  digitalWrite(col6, HIGH);  break;
    }
    delay(1000);
    switch(col){
      case 0:  digitalWrite(col0, LOW);  break;
      case 1:  digitalWrite(col1, LOW);  break;
      case 2:  digitalWrite(col2, LOW);  break;
      case 3:  digitalWrite(col3, LOW);  break;
      case 4:  digitalWrite(col4, LOW);  break;
      case 5:  digitalWrite(col5, LOW);  break;
      case 6:  digitalWrite(col6, LOW);  break;
    }
  }
}
