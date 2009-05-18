void setup() {
}

void loop() {
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    MidiUart.sendNoteOn(60, 100);
  }
}
