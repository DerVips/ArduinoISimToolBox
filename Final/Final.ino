#include <Arduino.h>
#include <Keyboard.h>
#include <Encoder.h>

//Settings------------------------START
const int numButtons = 2;
const int numLevers = 5;
const int numEncoders = 3;
const int numKeys = 7;

const char keys[] = {'0','1','2','3','4','5','6'};

const int leverPins[numLevers] = {
  9,  // <- 0
  8,  // <- 1
  7,  // <- 2
  6,  // <- 3
  3   // <- 4
};
const int buttonPins[numButtons] = {
  5, // <- 5  <- Hold funktion
  4  // <- 6
};
const int encoderPins[numEncoders][2] = {
  {10, 16},
  {14, 15},
  {18, 19}
};
//Settings-------------------------END

class Button {
  private:
    int pin;
    char keyboardKey;
    bool hold;
    bool isPressed;

  public:
    Button(int pin, char keyboardKey, bool hold) {
      this->pin = pin;
      this->keyboardKey = keyboardKey;
      this->hold = hold;
      this->isPressed = false;

      pinMode(pin, INPUT);
    }

    void process(){
      bool buttonState = digitalRead(pin) == HIGH;

      if(buttonState){
        if(hold && !isPressed){
          Keyboard.press(keyboardKey);
          isPressed = true;
        }else if(!hold){
          Keyboard.write(keyboardKey);
        }
      }else if(isPressed){
        if(!buttonState && hold){
          Keyboard.release(keyboardKey);
          isPressed = false;
        }
      }
    }
};

class Lever {
  private:
    int pin;
    char keyboardKey;
    bool isActive;
  
  public:
    Lever(int pin, char keyboardKey){
      this->pin = pin;
      this->keyboardKey = keyboardKey;

      pinMode(pin, INPUT);
      this->isActive = digitalRead(pin) == HIGH;
    }

    void process(){
      bool leverState = digitalRead(pin) == HIGH;

      if(leverState != isActive){
        isActive = leverState;
        Keyboard.write(keyboardKey);
      }
    }
};

class RotaryEncoder {
  private:
    Encoder encoder;
    int newPosition;
    int lastPosition;
    int steps;
    int PinA;
    int PinB;

  public:
    RotaryEncoder(int pinA, int pinB) : encoder(pinA, pinB) {
      steps = 0;
      newPosition = 0;
      lastPosition = 0;
      this->PinA = pinA;
      this->PinB = pinB;
    }

    void process()
    {
      int newPosition = encoder.read();
      int delta = newPosition - lastPosition;

      if (delta != 0) {
        steps += delta;
        lastPosition = newPosition;
      }

      Serial.print(String(PinA) + String(PinB) + ":");
      Serial.print(steps);
      Serial.println(",");
    }
};

Lever levers[numLevers] = {
  Lever(leverPins[0],keys[0]),
  Lever(leverPins[1],keys[1]),
  Lever(leverPins[2],keys[2]),
  Lever(leverPins[3],keys[3]),
  Lever(leverPins[4],keys[4])
};

Button buttons[numButtons] = {
  Button(buttonPins[0],keys[5],true),
  Button(buttonPins[1],keys[6],false)
};

RotaryEncoder encoder[numEncoders] = {
  RotaryEncoder(encoderPins[0][0],encoderPins[0][1]),
  RotaryEncoder(encoderPins[1][0],encoderPins[1][1]),
  RotaryEncoder(encoderPins[2][0],encoderPins[2][1])
};

void setup(){Serial.begin(9600);}

void loop() {
  for(int i = 0; i < numLevers; i++){
    levers[i].process();
  }

  for(int i = 0; i < numButtons; i++){
    buttons[i].process();
  }

  for(int i = 0; i < numEncoders; i++){
    encoder[i].process();
  }
}