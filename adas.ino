#include <Arduino.h>
#include <Encoder.h>
#include <Keyboard.h>

//Settings------------------------START
const int numButtons = 2;
const int numLevers = 4;
const int numEncoders = 3;

const int leverPins[numLevers] = {
    9,  // <- 
    8,  // <- 
    7,  // <- 
    6   // <- 
};
const int buttonPins[numButtons] = {
    5, // <- 
    4  // <- 
};
const int encoderPins[numEncoders][2] = {
    {10, 16}, // <- 
    {14, 15}, // <- 
    {18, 19}  // <- 
};
//Settings-------------------------END

class Lever {
public:
    Lever(int pin, bool hold_tap) : _pin(pin), hold(hold_tap), change(false) {
        pinMode(_pin, INPUT_PULLUP);
    }

    bool getValue() {
        bool current_state = digitalRead(_pin) == LOW;

        if (!hold) {
            if (current_state != change) {
                change = current_state;
                return true;
            }
        } else {
            change = false;
            return current_state;
        }

        return false;
    }

private:
    const int _pin;
    bool hold;
    bool change;
    bool value;
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
    Button(int pin, bool hold_tap) : _pin(pin), hold(hold_tap), change(false) {
        pinMode(_pin, INPUT_PULLUP);
    }

    bool getValue() {
        bool current_state = digitalRead(_pin) == LOW;

        if (!hold) {
            if (current_state != change && !change) {
                change = true;
                return true;
            }
        } else {
            change = false;
            return current_state;
        }

        return false;
    }

private:
    const int _pin;
    bool hold;
    bool change;
};

Lever levers[numLevers] = {
    Lever(leverPins[0]),
    Lever(leverPins[1]),
    Lever(leverPins[2]),
    Lever(leverPins[3])
};

Button buttons[numButtons] = {
    Button(buttonPins[0]),
    Button(buttonPins[1])
};

RotaryEncoder encoder[numEncoders] = {
  RotaryEncoder(encoderPins[0][0],encoderPins[0][1]),
  RotaryEncoder(encoderPins[1][0],encoderPins[1][1]),
  RotaryEncoder(encoderPins[2][0],encoderPins[2][1])
};

void setup() {
    Keyboard.begin();
}

void loop() {
  doEncoders();
  doButtons();
  doLevers();
  
  delay(20);
  Keyboard.releaseAll();
}

void doLevers(){
    for(int i = 0; i<numLevers; i++){
        if(levers[i].getValue()){
            switch (i)
            {
            case 0:
                Keyboard.press(KEY_KP_0);
            break;
            case 1:
                Keyboard.press(KEY_KP_1);
            break;
            case 2:
                Keyboard.press(KEY_KP_2);
            break;
            case 3:
                Keyboard.press(KEY_KP_3);
            break;
            }
        }
    }
}

void doButtons(){
    for(int i = 0; i<numButtons; i++){
        if(buttons[i].getValue()){
            switch (i)
            {
            case 0:
                Keyboard.press(KEY_KP_MULTIPLY);
            break;
            case 1:       
                Keyboard.press(KEY_KP_DIVIDE);
            break;
            }
        }
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
                Keyboard.press(KEY_KP_7);
            }else if(value<0){
                Keyboard.press(KEY_KP_4);
            }
        break;
        case 1:
            if(value>0){
                Keyboard.press(KEY_KP_8);
            }else if(value<0){
                Keyboard.press(KEY_KP_5);
            }
        break;
        case 2:
          if(value>0){
                Keyboard.press(KEY_KP_9);
            }else if(value<0){
                Keyboard.press(KEY_KP_6);
            }
        break;
        }
    }
  }
}