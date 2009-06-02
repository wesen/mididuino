#include <MidiUart.h>
#include <Midi.h>

void setup() {
  MidiUart.init();
}

void loop() {
  MidiUart.sendNoteOn(100, 100);
  delay(800);
}
