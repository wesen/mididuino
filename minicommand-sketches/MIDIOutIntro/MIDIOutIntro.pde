#include <MidiUart.h>
#include <Midi.h>

MidiClass Midi;

int led = 0;

void onNoteOnCallback(byte msg[]) {
  digitalWrite(13, led);
  led = !led;
}

void setup() {
  pinMode(13, OUTPUT);
  Midi.setOnNoteOnCallback(onNoteOnCallback);
  MidiUart.init();
}

void loop() {
  while (MidiUart.avail()) {
    Midi.handleByte(MidiUart.getc());
  }
  MidiUart.sendNoteOn(100, 100);
  delay(200);
}
