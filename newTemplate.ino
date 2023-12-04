#include <HID-Project.h>

void setup() {
    Joystick.begin();
}

void loop() {
  for(int i = 0; i < numLevers; i++){
    Joystick.button(i+1, levers[i].process());
  }

  for(int i = numLevers; i < numButtons; i++){
    Joystick.button(i+1 + numLevers, buttons[i].process());
  }

  for(int i = 0; i < numEncoders; i++){
    Joystick.slider(encoder[i].process(), i+1); <- Erwartet -127 bis 127
  }
}