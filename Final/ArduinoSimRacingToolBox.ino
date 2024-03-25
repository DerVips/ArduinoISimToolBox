#include <Arduino.h>
#include <Encoder.h>
#include <Keyboard.h>

//Settings------------------------START
const int numButtons = 3;
const int numLevers = 4;
const int numEncoders = 3;
const int numKeys = 7;

const int multiplier[numEncoders] = {1,1,1};

const int leverPins[numLevers+1] = {
  9,  // <- 0
  8,  // <- 1
  7,  // <- 2
  6,  // <- 3
  3   // <- 4
};
const int buttonPins[numButtons-1] = {
  5, // <- 5 
  4  // <- 6
};
const int encoderPins[numEncoders][2] = {
  {10, 16},
  {14, 15},
  {18, 19}
};

const char leverChars[numLevers+1] = {
  '0',  // <- 0
  '1',  // <- 1
  '2',  // <- 2
  '3',  // <- 3
  '4'   // <- 4
};
const char buttonChars[numButtons-1] = {
  '5', // <- 5 
  '6'  // <- 6
};
const char encoderChars[numEncoders][2] = {
  {'7', '8'},
  {'g', 'h'},
  {'j', 'k'}
};
//Settings-------------------------END

class Button {
  private:
    int pin;
    int keyNum;
    bool hold;
    bool isPressed;
    long cooldown;
    int latestState;

  public:
    Button(int pin, int keyNum, bool hold) {
      this->pin = pin;
      this->keyNum = keyNum;
      this->hold = hold;
      this->isPressed = false;
      latestState = 0;
      
      cooldown = millis();

      pinMode(pin, INPUT);
    }

    int process(){
      bool buttonState = digitalRead(pin) == HIGH;

      if(buttonState){
        if(hold && !isPressed){
          latestState = 1;
          isPressed = true;
        }else if(!hold && !isPressed){
          latestState = 1;
          isPressed = true;
          cooldown = millis();
        }else if(isPressed && !hold && cooldown + 50 < millis()){
          latestState = 0;
        }
      }
      else if(isPressed)
      {
        if(!buttonState && hold){
          latestState = 0;
          isPressed = false;
        }else if(!buttonState && !hold){
          isPressed = false;
        }
      }

      if(!buttonState && cooldown + 50 < millis()){
        latestState = 0;
      }

      return latestState;
    }
};

class Lever {
  private:
    int pin;
    int keyNum;
    bool isActive;
    long cooldown;
    int latestState;
  
  public:
    Lever(int pin, int keyNum) {
      this->pin = pin;
      this->keyNum = keyNum;
      latestState = 0;
      cooldown = millis();

      pinMode(pin, INPUT);
      this->isActive = digitalRead(pin) == HIGH;
    }

    int process(){
      bool leverState = digitalRead(pin) == HIGH;

      if(leverState != isActive){
        cooldown = millis();
        isActive = leverState;
        latestState = 1;
      }else if(cooldown + 50 < millis()){
        latestState = 0;
      }
      return latestState;
    }
};

class RotaryEncoder {
  private:
    Encoder encoder;
    int PinA;
    int PinB;

  public:
    RotaryEncoder(int pinA, int pinB) : encoder(pinA, pinB) {
      this->PinA = pinA;
      this->PinB = pinB;
    }

    int process()
    {
      int Position = encoder.read() / 4;
      return Position;
    }
};

Lever levers[numLevers] = {
  Lever(leverPins[0],0),
  Lever(leverPins[1],1),
  Lever(leverPins[3],3),
  Lever(leverPins[4],4)
};

Button buttons[numButtons] = {
  Button(buttonPins[0],5,true),
  Button(buttonPins[1],6,false),
  Button(leverPins[2],2,false)
};

RotaryEncoder encoder[numEncoders] = {
  RotaryEncoder(encoderPins[0][0],encoderPins[0][1]),
  RotaryEncoder(encoderPins[1][0],encoderPins[1][1]),
  RotaryEncoder(encoderPins[2][0],encoderPins[2][1])
};

bool lastLeverValue[numLevers];
bool lastButtonValue[numButtons];
int lastEncoderValue[numEncoders];

void setup(){
  Keyboard.begin(KeyboardLayout_de_DE);

  for(int i = 0; i < numLevers; i++){
    lastLeverValue[i] = false;
  }
  for(int i = 0; i < numButtons; i++){
    lastButtonValue[i] = false;
  }
  for(int i = 0; i < numEncoders; i++){
    lastEncoderValue[i] = 0;
  }
}

void loop() {
  for(int i = 0; i < numLevers; i++){
    if(levers[i].process()){
      if(!lastLeverValue[i]){
        Keyboard.press(leverChars[i]);
        lastLeverValue[i] = true;
      }
    }else if(lastLeverValue[i]){
      delay(100);
      Keyboard.release(leverChars[i]);
      lastLeverValue[i] = false;
    }
  }

  for(int i = 0; i < numButtons; i++){
    if(buttons[i].process()){
      if(!lastButtonValue[i]){
        Keyboard.press(buttonChars[i]);
        lastButtonValue[i] = true;
      }
    }else if(lastButtonValue[i]){
      delay(100);
      Keyboard.release(buttonChars[i]);
      lastButtonValue[i] = false;
    }
  }

  for(int i = 0; i < numEncoders; i++){
    int tempValue = encoder[i].process() * multiplier[i];

    if(tempValue != lastEncoderValue[i]){
      for(; lastEncoderValue[i] < tempValue; lastEncoderValue[i]++){
        Keyboard.write(encoderChars[i][0]);
      }

      for(; lastEncoderValue[i] > tempValue; lastEncoderValue[i]--){
        Keyboard.write(encoderChars[i][1]);
      }
    }
  }
}
