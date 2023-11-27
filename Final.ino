#include <Keyboard.h>
#include <Encoder.h>
#include <Joystick.h>

class Button {
private:
    int pin;
    char keyboardKey;
    bool hold;
    bool isPressed;
    unsigned long releaseButton;
    const bool invert = false;
    const unsigned long debounceDelay = 50;

public:
    Button(int pin, char keyboardKey, bool hold) {
        this->pin = pin;
        this->keyboardKey = keyboardKey;
        this->hold = hold;
        this->releaseButton = 0;
        this->isPressed = false;

        pinMode(pin, INPUT);
    }

    void process(){
        if(invert){
            bool buttonState = digitalRead(pin) == LOW;
        }else{
            bool buttonState = digitalRead(pin) == HIGH;
        }

        if(buttonState){
            if(hold && isPressed){
                releaseButton = millis() + debounceDelay;
                Keyboard.press(KeyboardKey);
                isPressed = true;
            }else if(!hold){
                Keyboard.write(keyboardKey);
            }
        }else if(isPressed && !buttonstate && millis() > releaseButton && hold){
            Keyboard.release(keyboardKey);
            isPressed = false;
        }
    }
}

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
                Keyboard.write(keyboardKey)
            }
        }
}

class RotaryEncoder {
    private:
        Encoder encoder;
        Joystick_ joystick;
        int multiplier;
        int value;
        int offset;

        const int max = 255;
        const int min = 0;

    int calculate(int position){
        if (position < min + offset)
            {
                if(position + min < offset)
                {
                    offset = position + min;
                    return min;
                }
            }
            else if (position > max + offset)
            {
                if (position + min > offset)
                {
                    offset = position - max;
                    return max;
                }
            }
            return position - offset;
    }

    public:
        RotaryEncoder(int pinA, int pinB) {
            encoder = Encoder(pinA, pinB);
            multiplier = 1;
            offset = 0;
            value = (max + min) / 2;

            joystick.begin();
        }

        void process()
        {
            int position = encoder.read();
            joystick.setThrottle(calculate(position*multiplier));
        }
};