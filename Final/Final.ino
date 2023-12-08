#include <Arduino.h>
#include <Encoder.h>
#include <Keyboard.h>

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
    bool hold;
    bool isPressed;
    long cooldown;
    char TYPO;

  public:
    Button(int pin, bool hold, char TYPO) {
      this->pin = pin;
      this->hold = hold;
      this->TYPO = TYPO;
      
      cooldown = millis();
      pinMode(pin, INPUT);
    }

    void process(){
      bool buttonState = digitalRead(pin) == HIGH;

      if(buttonState){
        if(hold && !isPressed){
          Keyboard.press(TYPO);
          isPressed = true;
        }else if(!hold && !isPressed){
          Keyboard.press(TYPO);
          isPressed = true;
          cooldown = millis();
        }else if(isPressed && !hold && cooldown + 50 < millis()){
          Keyboard.release(TYPO);
        }
      }else if(isPressed){
        if(!buttonState && hold){
          Keyboard.release(TYPO);
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
    bool isActive;
    long cooldown;
    char TYPO;
  
  public:
    Lever(int pin, char TYPO) {
      this->pin = pin;
      this->TYPO = TYPO;
      cooldown = millis();

      pinMode(pin, INPUT);
      this->isActive = digitalRead(pin) == HIGH;
    }

    void process(){
      bool leverState = digitalRead(pin) == HIGH;

      if(leverState != isActive){
        cooldown = millis();
        isActive = leverState;
        Keyboard.press(TYPO);
      }else if(cooldown + 50 < millis()){
        Keyboard.release(TYPO);
      }
    }
};

class RotaryEncoder {
  private:
    Encoder encoder;
    int lastPosition;
    char TYPO_UP;
    char TYPO_DOWN;
    int PinA;
    int PinB;

  public:
    RotaryEncoder(int pinA, int pinB, char TYPO_UP, char TYPO_DOWN) : encoder(pinA, pinB) {
      this->PinA = pinA;
      this->PinB = pinB;
      this->TYPO_UP = TYPO_UP;
      this->TYPO_DOWN = TYPO_DOWN;
      lastPosition = 0;
    }

    void process()
    {
      int Position = encoder.read();
      Position /= 4;
      
      if(lastPosition<Position){
        while (lastPosition++<Position)
        {
          Keyboard.write(TYPO_UP);
        }
      }else{
        while (lastPosition-->Position)
        {
          Keyboard.write(TYPO_DOWN);
        }
      }
    }
};

Lever levers[numLevers] = {
  Lever(leverPins[0],'ÿ'),
  Lever(leverPins[1],'þ'),
  Lever(leverPins[3],'ý'),
  Lever(leverPins[4],'ü')
};

Button buttons[numButtons] = {
  Button(buttonPins[0],true,'û'),
  Button(buttonPins[1],false,'ú'),
  Button(leverPins[2],false,'ù')
};

RotaryEncoder encoder[numEncoders] = {
  RotaryEncoder(encoderPins[0][0],encoderPins[0][1],'ø','÷'),
  RotaryEncoder(encoderPins[1][0],encoderPins[1][1],'ö','õ'),
  RotaryEncoder(encoderPins[2][0],encoderPins[2][1],'ô','ó')
};

void setup(){
  Keyboard.begin();
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
}