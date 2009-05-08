// Einbinden der MIDI Library
#include <MidiUart.h>
#include <Midi.h>
#include <MD.h>
MidiClass Midi;

byte mididuino_sysex_data[128];

int ledPin = 13;
int ledStatus = HIGH;

void onNoteOnCallback(byte msg[3]) {
  switch (msg[1]) {
    case 36: // Bassdrum
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
    break;
    
    case 50: // CLap
    digitalWrite(3, HIGH);
    delay(50);
    digitalWrite(3, LOW);
    break;
    
    case 52: // Hihat
    digitalWrite(4, HIGH);
    delay(50);
    digitalWrite(4, LOW);
    break;
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  MidiUart.init();
  Midi.setOnNoteOnCallback(onNoteOnCallback);
}

void loop() {
  while (MidiUart.avail()) {
    byte c = MidiUart.getc();
    Midi.handleByte(c);
  }
}
