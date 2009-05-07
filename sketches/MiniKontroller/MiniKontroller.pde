#include <MidiUart.h>
#include <Midi.h>

MidiClass Midi;

int potValues[4];
void setup() {
   MidiUart.init();
}

void loop() {
  int i;
  for (i = 0; i < 4; i++) {
    int value = analogRead(i);
    value = value >> 3;
    if (abs(value - potValues[i]) >= 2) {
      potValues[i] = value;
      MidiUart.sendCC(0, i, value);
    }
  }
}
