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
  5, // <- 5 
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
      }else if(isPressed){
        if(!buttonState && hold){
          latestState = 0;
          isPressed = false;
        }else if(!buttonState && !hold){
          isPressed = false;
        }
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
    const int max = 1023;
    const int min = 0;

    Encoder encoder;
    int newPosition;
    int lastPosition;
    int steps;
    int PinA;
    int PinB;
    int slider;

  public:
    RotaryEncoder(int pinA, int pinB, int slider) : encoder(pinA, pinB) {
      steps = (max+min)/2;
      newPosition = 0;
      lastPosition = 0;
      this->PinA = pinA;
      this->PinB = pinB;
      this->slider = slider;
    }

    int process()
    {
      int newPosition = encoder.read()/4;
      newPosition = newPosition<-127 ? -127 : newPosition;
      int delta = newPosition - lastPosition;

      if (delta != 0) {
        steps += delta;
        steps = steps>127 ? 127 : steps;
        lastPosition = newPosition;
      }

      return steps;
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
  RotaryEncoder(encoderPins[0][0],encoderPins[0][1],0),
  RotaryEncoder(encoderPins[1][0],encoderPins[1][1],1),
  RotaryEncoder(encoderPins[2][0],encoderPins[2][1],2)
};

#include <Joystick.h>

Joystick_ Joystick(
0x05,                   /* HID Report ID: */ 
JOYSTICK_TYPE_GAMEPAD,  /* Joystick Typ: */ 
numButtons+numLevers,   /* Anzahl der Buttons: */ 
0,                      /* Anzahl der Hat Switches: */
false,                  /* X-Achse: */ 
false,                  /* Y-Achse: */ 
false,                  /* Z-Achse: */ 
false,                  /* X-Achsenrotation: */ 
false,                  /* Y-Achsenrotation: */ 
false,                  /* Z-Achsenrotation: */ 
true,                   /* Rudder: */
true,                   /* Throttle: */ 
true,                   /* Accelerator: */ 
false,                  /* Brake: */ 
false                   /* Steering: */ 
);

void setup(){
  Joystick.begin(true);
}

void loop() {
  for(int i = 0; i < numLevers; i++){
    Joystick.setButton(i+1, levers[i].process());
  }

  for(int i = numLevers; i < numButtons; i++){
    Joystick.setButton(i+1 + numLevers, buttons[i].process());
  }

  Joystick.setRudder(encoder[0].process());
  Joystick.setThrottle(encoder[1].process());
  Joystick.setAccelerator(encoder[2].process());
}