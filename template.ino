#include <Keypad.h>
#include <Joystick.h>

#define ENABLE_PULLUPS
#define NUM_ROTARY_ENCODERS 4
#define NUM_BUTTONS 24
#define NUM_ROWS 5
#define NUM_COLS 5

byte buttonMatrix[NUM_ROWS][NUM_COLS] = {
  {0, 1, 2, 3, 4},
  {5, 6, 7, 8, 9},
  {10, 11, 12, 13, 14},
  {15, 16, 17, 18, 19},
  {20, 21, 22, 23},
};

struct RotaryEncoder {
  byte pin1;
  byte pin2;
  int ccwButton;
  int cwButton;
  volatile unsigned char state;
};

RotaryEncoder rotaryEncoders[NUM_ROTARY_ENCODERS] {
  {0, 1, 24, 25, 0},
  {2, 3, 26, 27, 0},
  {4, 5, 28, 29, 0},
  {6, 7, 30, 31, 0},
};

#define DIR_CCW 0x10
#define DIR_CW 0x20
#define R_START 0x0

#ifdef HALF_STEP
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
  {R_START_M, R_CW_BEGIN, R_CCW_BEGIN, R_START},
  {R_START_M | DIR_CCW, R_START, R_CCW_BEGIN, R_START},
  {R_START_M | DIR_CW, R_CW_BEGIN, R_START, R_START},
  {R_START_M, R_CCW_BEGIN_M, R_CW_BEGIN_M, R_START},
  {R_START_M, R_START_M, R_CW_BEGIN_M, R_START | DIR_CW},
  {R_START_M, R_CCW_BEGIN_M, R_START_M, R_START | DIR_CCW},
};
#else
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char ttable[7][4] = {
  {R_START, R_CW_BEGIN, R_CCW_BEGIN, R_START},
  {R_CW_NEXT, R_START, R_CW_FINAL, R_START | DIR_CW},
  {R_CW_NEXT, R_CW_BEGIN, R_START, R_START},
  {R_CW_NEXT, R_CW_BEGIN, R_CW_FINAL, R_START},
  {R_CCW_NEXT, R_START, R_CCW_BEGIN, R_START},
  {R_CCW_NEXT, R_CCW_FINAL, R_START, R_START | DIR_CCW},
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};
#endif

byte rowPins[NUM_ROWS] = {21, 20, 19, 18, 15};
byte colPins[NUM_COLS] = {14, 16, 10, 9, 8};

Keypad buttonBox = Keypad(makeKeymap(buttonMatrix), rowPins, colPins, NUM_ROWS, NUM_COLS);

Joystick_ virtualJoystick(JOYSTICK_DEFAULT_REPORT_ID,
                          JOYSTICK_TYPE_JOYSTICK, 32, 0,
                          false, false, false, false, false, false,
                          false, false, false, false, false);

void setup() {
  virtualJoystick.begin();
  initializeRotaryEncoders();
}

void loop() {
  checkAllRotaryEncoders();
  checkAllButtons();
}

void checkAllButtons() {
  if (buttonBox.getKeys()) {
    for (int i = 0; i < LIST_MAX; i++) {
      if (buttonBox.key[i].stateChanged) {
        switch (buttonBox.key[i].kstate) {
          case PRESSED:
          case HOLD:
            virtualJoystick.setButton(buttonBox.key[i].kchar, 1);
            break;
          case RELEASED:
          case IDLE:
            virtualJoystick.setButton(buttonBox.key[i].kchar, 0);
            break;
        }
      }
    }
  }
}

void initializeRotaryEncoders() {
  for (int i = 0; i < NUM_ROTARY_ENCODERS; i++) {
    pinMode(rotaryEncoders[i].pin1, INPUT);
    pinMode(rotaryEncoders[i].pin2, INPUT);
#ifdef ENABLE_PULLUPS
    digitalWrite(rotaryEncoders[i].pin1, HIGH);
    digitalWrite(rotaryEncoders[i].pin2, HIGH);
#endif
  }
}

unsigned char processRotaryEncoder(int index) {
  unsigned char pinState = (digitalRead(rotaryEncoders[index].pin2) << 1) | digitalRead(rotaryEncoders[index].pin1);
  rotaryEncoders[index].state = ttable[rotaryEncoders[index].state & 0xf][pinState];
  return (rotaryEncoders[index].state & 0x30);
}

void checkAllRotaryEncoders() {
  for (int i = 0; i < NUM_ROTARY_ENCODERS; i++) {
    unsigned char result = processRotaryEncoder(i);
    if (result == DIR_CCW) {
      virtualJoystick.setButton(rotaryEncoders[i].ccwButton, 1);
      delay(50);
      virtualJoystick.setButton(rotaryEncoders[i].ccwButton, 0);
    };
    if (result == DIR_CW) {
      virtualJoystick.setButton(rotaryEncoders[i].cwButton, 1);
      delay(50);
      virtualJoystick.setButton(rotaryEncoders[i].cwButton, 0);
    };
  }
}
