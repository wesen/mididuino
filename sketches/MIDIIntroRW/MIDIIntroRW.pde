// Einbinden der Ruinwesen MIDI Library
#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

void setup() {
  // Initialisieren der MIDI Schnittstelle
  MidiUart.init();
}

void loop() {
  // Zuerst senden einer NOTE ON Nachricht
  MidiUart.sendNoteOn(100, 100);
  delay(200);
  // Dann senden einer NOTE OFF Nachricht
  MidiUart.sendNoteOff(100);
  delay(2000);
}
