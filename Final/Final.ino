#include <Arduino.h>
#include <Encoder.h>
#include <Joystick.h>

//Settings------------------------START
const int numButtons = 3;
const int numLevers = 4;
const int numEncoders = 3;
const int numKeys = 7;

const int leverPins[numLevers+1] = {
  9,  // <- 0
  8,  // <- 1
  7,  // <- 2
  6,  // <- 3
  3   // <- 4
};
const int buttonPins[numButtons-1] = {
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
    int keyNum;
    bool hold;
    bool isPressed;
    long cooldown;
    Joystick_& joystick;

  public:
    Button(int pin, int keyNum, bool hold, Joystick_& joystick) : joystick(joystick) {
      this->pin = pin;
      this->keyNum = keyNum;
      this->hold = hold;
      this->isPressed = false;
      cooldown = millis();

      pinMode(pin, INPUT);
    }

    void process(){
      bool buttonState = digitalRead(pin) == HIGH;

      if(buttonState){
        if(hold && !isPressed){
          joystick.setButton(keyNum, true);
          isPressed = true;
        }else if(!hold && !isPressed){
          joystick.setButton(keyNum, true);
          isPressed = true;
          cooldown = millis();
        }else if(isPressed && !hold && cooldown + 50 < millis()){
          joystick.setButton(keyNum, false);;
        }
      }else if(isPressed){
        if(!buttonState && hold){
          joystick.setButton(keyNum, false);
          isPressed = false;
        }else if(!buttonState && !hold){
          isPressed = false;
        }
      }
    }
};

class Lever {
  private:
    int pin;
    int keyNum;
    bool isActive;
    long cooldown;
    Joystick_& joystick;
  
  public:
    Lever(int pin, int keyNum, Joystick_& joystick) : joystick(joystick) {
      this->pin = pin;
      this->keyNum = keyNum;
      cooldown = millis();

      pinMode(pin, INPUT);
      this->isActive = digitalRead(pin) == HIGH;
    }

    void process(){
      bool leverState = digitalRead(pin) == HIGH;

      if(leverState != isActive){
        cooldown = millis();
        isActive = leverState;
        joystick.setButton(keyNum, true);
      }else if(cooldown + 50 < millis()){
        joystick.setButton(keyNum, false);
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
    int slider;
    Joystick_& joystick;

  public:
    RotaryEncoder(int pinA, int pinB, int slider, Joystick_& joystick) : encoder(pinA, pinB),  joystick(joystick){
      steps = 0;
      newPosition = 0;
      lastPosition = 0;
      this->PinA = pinA;
      this->PinB = pinB;
      this->slider = slider;
    }

    void process()
    {
      int newPosition = encoder.read()/4;
      newPosition = newPosition<0 ? 0 : newPosition;
      int delta = newPosition - lastPosition;

      if (delta != 0) {
        steps += delta;
        steps = steps>1023 ? 1023 : steps;
        lastPosition = newPosition;
      }

      joystick.setSlider(slider, sliderValue);
    }
};

Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  7,
  3,
  false, false, false, false, false, false,
  false, false, false, false, false, false,
  false, false, false, false, false, false
);

Lever levers[numLevers] = {
  Lever(leverPins[0],0,Joystick),
  Lever(leverPins[1],1,Joystick),
  Lever(leverPins[3],3,Joystick),
  Lever(leverPins[4],4,Joystick)
};

Button buttons[numButtons] = {
  Button(buttonPins[0],5,true,Joystick),
  Button(buttonPins[1],6,false,Joystick),
  Button(leverPins[2],2,false,Joystick)
};

RotaryEncoder encoder[numEncoders] = {
  RotaryEncoder(encoderPins[0][0],encoderPins[0][1],0),
  RotaryEncoder(encoderPins[1][0],encoderPins[1][1],1),
  RotaryEncoder(encoderPins[2][0],encoderPins[2][1],2)
};

void setup(){
  Joystick.begin();
}

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
  Joystick.send();
}