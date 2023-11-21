#include <Arduino.h>

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

    bool read(EncoderEvent *e) {
        noInterrupts();
        if (event.count == 0) {
            interrupts();
            return false;
        }
        *e = event;
        event.count = 0;
        interrupts();
        return true;
    }

    int getAngle() const {
        return angle;
    }

    void processEvent(const EncoderEvent& event) {
        if (event.type == EncoderEvent::StepCW) {
            angle = (angle + 10) % maxAngle;
        } else if (event.type == EncoderEvent::StepCCW) {
            angle = (angle - 10 + maxAngle) % maxAngle;
        }
    }
private:
    const int pinA;
    const int pinB;
};

class Lever {
public:
    Lever(int pin, bool hold_tap) : _pin(pin), hold(hold_tap), change(false), value(false) {
        pinMode(_pin, INPUT);
    }

    bool getValue() {
        bool current_state = digitalRead(_pin) == HIGH;

        if (!hold) {
            value = current_state != change;
            change = current_state;
        } else {
            value = current_state;
        }

        return value;
    }

private:
    const int _pin;
    bool hold;
    bool change;
    bool value;
};

// Pin-Definitionen
const int numLevers = 4;
const int numEncoders = 3;
const int encoderPins[numEncoders][2] = {
    {2, 3},
    {4, 5},
    {6, 7}
};

// Encoder-Objekte erstellen
Encoder encoders[numEncoders] = {
    Encoder(encoderPins[0][0], encoderPins[0][1]),
    Encoder(encoderPins[1][0], encoderPins[1][1]),
    Encoder(encoderPins[2][0], encoderPins[2][1])
};

Lever levers[numLevers] = {
    Lever(8, false),
    Lever(9, false),
    Lever(10, false),
    Lever(11, false)
};

void setup() {
    Serial.begin(9600);
}

void loop() {
    for (int i = 0; i < numEncoders; i++) {
        encoders[i].checkPins();

        EncoderEvent event;
        if (encoders[i].read(&event)) {
            encoders[i].processEvent(event);

            Serial.print("Winkel Encoder ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(encoders[i].getAngle());
        }
    }

    for (int i = 0; i < numLevers; i++) {
        Serial.print("Lever ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(levers[i].getValue());
    }
}
