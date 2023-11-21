#include <Encoder.h>

const int rotaryCount = 3;
const int timeout = 100;

Encoder encoders[rotaryCount];
long R360[rotaryCount];

void setup() {
    encoders[0] = Encoder(A0, A1);
    encoders[1] = Encoder(A2, A3);
    encoders[2] = Encoder(A4, A5);
}

void loop() {
    long temp[] = R360;
    bool changes = false;
    Read();

    for (int i = 0; i < rotaryCount; i++) {
        if(temp[i] != R360[i]){
            changes = true;
            break;
        }
    }

    if(changes){
        Write();
    }else{ Sleep(timeout) }
}

void Read(){
    for (int i = 0; i < rotaryCount; i++) {
        R360[i] = encoders[i].read();
    }
}

void Write(){
    
}