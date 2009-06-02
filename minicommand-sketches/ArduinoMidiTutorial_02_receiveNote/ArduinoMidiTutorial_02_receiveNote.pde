#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

int ledPin = 13;

void onNoteOnCallback(byte *msg) {
  digitalWrite(ledPin, HIGH);
}

void onNoteOffCallback(byte *msg) {
  digitalWrite(ledPin, LOW);
}

void setup() {
  MidiUart.init();
  Midi.setOnNoteOnCallback(onNoteOnCallback);
  Midi.setOnNoteOffCallback(onNoteOffCallback);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  while (MidiUart.avail()) {
    Midi.handleByte(MidiUart.getc());
  }
}
