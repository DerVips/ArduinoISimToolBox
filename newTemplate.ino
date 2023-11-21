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
        if (event.type == type) {
            if (event.count != 0xFF) {
                event.count++;
            }
        } else {
            event.type = type;
            event.count = 1;
        }
    }

public:
    Encoder(int pinA, int pinB, int maxAngleValue = 360) : 
        lastPinStates(0), angle(0), maxAngle(maxAngleValue) {
        pinMode(pinA, INPUT);
        pinMode(pinB, INPUT);
    }

    void checkPins(bool pinAState, bool pinBState) {
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
};

// Pin-Definitionen
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

void setup() {
    Serial.begin(9600);
}

void loop() {
    for (int i = 0; i < numEncoders; ++i) {
        encoders[i].checkPins(digitalRead(encoderPins[i][0]), digitalRead(encoderPins[i][1]));

        EncoderEvent event;
        if (encoders[i].read(&event)) {
            encoders[i].processEvent(event);

            Serial.print("Winkel Encoder ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(encoders[i].getAngle());
        }
    }
}
