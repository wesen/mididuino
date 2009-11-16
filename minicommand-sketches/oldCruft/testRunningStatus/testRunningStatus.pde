void setup() {
  MidiUart.useRunningStatus = true;
  MidiClock.mode = MidiClock.INTERNAL;
  MidiClock.setTempo(100);
  MidiClock.transmit = true;
  sei();
  MidiUart.sendNoteOn(100, 100);
  MidiClock.start();
}

void loop() {
}

void handleGui() {
}
