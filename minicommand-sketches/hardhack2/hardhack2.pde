#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

void setup() {
  MidiUart.useRunningStatus = false;
  MidiUart.init(115200);
}

void loop() {
  MidiUart.sendNoteOn(100, 100);
  delay(1000);
}
