#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

void setup() {
  MidiUart.init(115200);
}

int oldPotWert = -10;
void loop() {
  int potWert = map(analogRead(0), 0, 1023, 0, 127);
  if (abs(potWert - oldPotWert) >= 2) {
    MidiUart.sendCC(1, potWert);
    oldPotWert = potWert;
  }
}
