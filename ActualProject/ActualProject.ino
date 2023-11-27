#include <Arduino.h>
#include <Encoder.h>
#include <Keyboard.h>

//Settings------------------------START
const int numButtons = 2;
const int numLevers = 5;
const int numEncoders = 3;

const int leverPins[numLevers] = {
    9,  // <- 0
    8,  // <- 1
    7,  // <- 2
    6,  // <- 3
    3   // <- ?
};
const int buttonPins[numButtons] = {
    5, // <- /  <- Hold funktion
    4  // <- *
};
const int encoderPins[numEncoders][2] = {
    {10, 16}, // <- 7 4    Auf joystick umbauen
    {14, 15}, // <- 8 5    Auf joystick umbauen
    {18, 19}  // <- 9 6    Auf joystick umbauen
};
//Settings-------------------------END

class Lever {
public:
  Lever(int pin, char lever) : _pin(pin), clever(lever), change(false) {
    pinMode(_pin, INPUT);
  }

  bool getValue() {
    bool currentValue = digitalRead(_pin) == HIGH;

    if (currentValue && currentValue != change) {
      change = currentValue;
      Keyboard.press(clever);
      return true;
    } else if (!currentValue && currentValue != change) {
      change = false;
    }

    delay(20);
    Keyboard.release(clever);
    return false;
  }

private:
    const int _pin;
    char clever;
    bool change;
};

class RotaryEncoder {
public:
  RotaryEncoder(int pinA, int pinB) : encoder(pinA, pinB), steps(0) {}

  int read() {
    latestpositon = lastPosition;
    int newPosition = encoder.read();
    int delta = newPosition - lastPosition;
    
    if (delta != 0) {
      steps += delta;
      lastPosition = newPosition;
    }
    
    return latestpositon - steps;
  }
private:
  Encoder encoder;
  int lastPosition = 0;
  int steps = 0;
  int latestpositon = 0;
};

class Button {
public:
  Button(int pin, char button) : _pin(pin), cbutton(button), change(false) {
    pinMode(_pin, INPUT);
  }

  bool getValue() {
    bool currentValue = digitalRead(_pin) == HIGH;

    if (currentValue && currentValue != change) {
      change = currentValue;
      Keyboard.press(cbutton);
      return true;
    } else if (!currentValue && currentValue != change) {
      Keyboard.release(cbutton);
      change = false;
    }

    return false;
  }

private:
    const int _pin;
    char cbutton;
    bool change;
};

Lever levers[numLevers] = {
  Lever(leverPins[0],'0'),
  Lever(leverPins[1],'1'),
  Lever(leverPins[2],'2'),
  Lever(leverPins[3],'3'),
  Lever(leverPins[4],'?')
};

Button buttons[numButtons] = {
  Button(buttonPins[0],'/'),
  Button(buttonPins[1],'*')
};

RotaryEncoder encoder[numEncoders] = {
  RotaryEncoder(encoderPins[0][0],encoderPins[0][1]),
  RotaryEncoder(encoderPins[1][0],encoderPins[1][1]),
  RotaryEncoder(encoderPins[2][0],encoderPins[2][1])
};

void setup() {
  Keyboard.begin(KeyboardLayout_de_DE);
}

void loop() {
  doEncoders();
  doButtons();
  doLevers();
}

void doLevers(){
  for(int i = 0; i<numLevers; i++){
    levers[i].getValue();
  }
}

void doButtons(){
  for(int i = 0; i<numButtons; i++){
    buttons[i].getValue();
  }
}

void doEncoders(){
  int value = 0;
  for(int i = 0; i < numEncoders; i++){
    value = encoder[i].read();
    
    if(value != 0){
      switch (i)
      {
      case 0:
          if(value>0){
              Keyboard.press('7');
          }else if(value<0){
              Keyboard.press('4');
          }
      break;
      case 1:
          if(value>0){
              Keyboard.press('8');
          }else if(value<0){
              Keyboard.press('5');
          }
      break;
      case 2:
        if(value>0){
              Keyboard.press('9');
          }else if(value<0){
              Keyboard.press('6');
          }
      break;
      }
      Keyboard.releaseAll();
    }
  }
}