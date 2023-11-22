#include <Arduino.h>

//Settings------------------------START
const int numButtons = 2;
const int numLevers = 4;
const int numEncoders = 3;

const int leverPins[numLevers] = {
    8, 
    9, 
    10, 
    11
};
const int buttonPins[numButtons] = {
    12, 
    13
};
const int encoderPins[numEncoders][2] = {
    {2, 3},
    {4, 5},
    {6, 7}
};
//Settings-------------------------END
struct EncoderEvent {
    enum Type { None = 0, StepCW, StepCCW };
    Type type;
    uint8_t count;

    EncoderEvent() : type(None), count(0) {};
};

class Encoder {
private:
    uint8_t lastPinStates;
    EncoderEvent event;
    int angle;
    const int maxAngle;

    void addEvent(EncoderEvent::Type type) {
        if (event.type == EncoderEvent::None || event.type == type) {
            event.type = type;
            event.count++;
        }
    }

public:
    Encoder(int pinA, int pinB, int maxAngleValue = 360) :
        lastPinStates(0), angle(0), maxAngle(maxAngleValue) {
        pinMode(pinA, INPUT);
        pinMode(pinB, INPUT);
    }

    void checkPins() {
        bool pinAState = digitalRead(pinA);
        bool pinBState = digitalRead(pinB);

        uint8_t state = (pinBState << 1) | pinAState;
        if (state != (lastPinStates & 0x3)) {
            lastPinStates = (lastPinStates << 2) | state;
            if (lastPinStates == 0b10000111) {
                addEvent(EncoderEvent::StepCCW);
            } else if (lastPinStates == 0b01001011) {
                addEvent(EncoderEvent::StepCW);
            }
        }
    }

    int getAngle() const {
        return angle;
    }

    void processEvent(const EncoderEvent &event) {
        if (event.type == EncoderEvent::StepCW) {
            angle = (angle + 10) % maxAngle;
        } else if (event.type == EncoderEvent::StepCCW) {
            angle = (angle - 10 + maxAngle) % maxAngle;
        }
    }

    int getValue() const {
        return angle;
    }

private:
    const int pinA;
    const int pinB;
};

class Lever {
public:
    Lever(int pin, bool hold_tap) : _pin(pin), hold(hold_tap), change(false), value(false), changed(false) {
        pinMode(_pin, INPUT_PULLUP);
    }

    bool getValue() {
        bool current_state = digitalRead(_pin) == HIGH;

        if (!hold) {
            if (current_state != change && !changed) {
                changed = true;
                return true;
            } else if (current_state == change) {
                changed = false;
            }
        } else {
            changed = false;
            return current_state;
        }

        return false;
    }

private:
    const int _pin;
    bool hold;
    bool change;
    bool value;
    bool changed;
};

class Button {
public:
    Button(int pin, bool hold_tap) : _pin(pin), hold(hold_tap), change(false), value(false), changed(false) {
        pinMode(_pin, INPUT_PULLUP);
    }

    bool getValue() {
        bool current_state = digitalRead(_pin) == HIGH;

        if (!hold) {
            if (current_state != change && !changed) {
                changed = true;
                return true;
            } else if (current_state == change) {
                changed = false;
            }
        } else {
            changed = false;
            return current_state;
        }

        return false;
    }

private:
    const int _pin;
    bool hold;
    bool change;
    bool value;
    bool changed;
};

//Objekte erstellen
Encoder encoders[numEncoders];
Lever levers[numLevers];
Button buttons[numButtons];

void setup() {
    Serial.begin(9600);

    // Initialisiere Encoder-Objekte
    for (int i = 0; i < numEncoders; i++) {
        encoders[i] = Encoder(encoderPins[i][0], encoderPins[i][1]);
    }

    // Initialisiere Lever-Objekte
    for (int i = 0; i < numLevers; i++) {
        levers[i] = Lever(leverPins[i], false);
    }

    // Initialisiere Button-Objekte
    for (int i = 0; i < numButtons; i++) {
        buttons[i] = Button(buttonPins[i], true);
    }
}

void loop() {
    for (int i = 0; i < numEncoders; i++) {
        encoders[i].checkPins();

        EncoderEvent event;
        if (encoders[i].read(&event)) {
            encoders[i].processEvent(event);
        }
    }

    for (int i = 0; i < numLevers; i++) {
        if(levers[i].getValue()){
            if(i==0){
                Keyboard.write('L');
            }else if(i==1){
                Keyboard.write('K');
            }else if(i==2){
                Keyboard.write('P');
            }
        }

        Serial.print("Lever ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(levers[i].getValue());
    }

    for (int i = 0; i < numButtons; i++) {
        if(levers[i].getValue()){
            if(i==0){
                Keyboard.write('J');
            }else if(i==1){
                Keyboard.write('N');
            }
        }

        Serial.print("Button ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(buttons[i].getValue());
    }
}
