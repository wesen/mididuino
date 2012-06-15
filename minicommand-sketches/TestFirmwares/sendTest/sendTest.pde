void setup() {
}

void loop() {
  MidiUart.sendNoteOn(MIDI_NOTE_A0, 100);
  delay(100);
}
