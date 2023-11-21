#include <arduino.h>

struct EC11Event {
    enum Type { None = 0, StepCW, StepCCW };
    Type type;
    uint8_t count;

    EC11Event() : type(None), count(0) {};
};

class EC11 {
private:
    uint8_t lastPinStates;
    EC11Event event;

    void addEvent(EC11Event::Type type) {
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
    EC11() : lastPinStates(0) {};

    void checkPins(bool pinAState, bool pinBState) {
        uint8_t state = (pinBState << 1) | pinAState;
        if (state != (lastPinStates & 0x3)) {
            lastPinStates = (lastPinStates << 2) | state;
            if (lastPinStates == 0b10000111) {
                addEvent(EC11Event::StepCCW);
            } else if (lastPinStates == 0b01001011) {
                addEvent(EC11Event::StepCW);
            }
        }
    }

    bool read(EC11Event *e) {
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
};

// Pin-Definitionen
const int pinEncoderA = 2;
const int pinEncoderB = 3;
const int maxAngle = 360; // Maximale Winkelposition (Grad)

// Variablen
int angle = 0;

// EC11-Objekt erstellen
EC11 myEncoder;

void setup() {
    pinMode(pinEncoderA, INPUT);
    pinMode(pinEncoderB, INPUT);
    Serial.begin(9600);
}

void loop() {
    // Zustand der Pins überprüfen
    myEncoder.checkPins(digitalRead(pinEncoderA), digitalRead(pinEncoderB));

    // Ereignisse lesen
    EC11Event event;
    if (myEncoder.read(&event)) {
        // Ereignis verarbeiten
        if (event.type == EC11Event::StepCW) {
            angle = (angle + 10) % maxAngle; // Beispiel: Schritte im Uhrzeigersinn erhöhen den Winkel um 10 Grad
        } else if (event.type == EC11Event::StepCCW) {
            angle = (angle - 10 + maxAngle) % maxAngle; // Beispiel: Schritte gegen den Uhrzeigersinn verringern den Winkel um 10 Grad
        }

        // Winkelinformation ausgeben
        Serial.print("Aktueller Winkel: ");
        Serial.println(angle);
    }

    // Hier können weitere Anweisungen stehen...
}
