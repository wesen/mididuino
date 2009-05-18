void on16thNoteCallback() {
  GUI.setLine(GUI.LINE1);
  GUI.put_value(0, MidiClock.div16th_counter % 16);
}

void setup() {
  MidiClock.mode = MidiClock.INTERNAL;
  MidiClock.transmit = true;
  MidiClock.setOn16Callback(on16thNoteCallback);
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1))
    MidiClock.start();
  if (BUTTON_PRESSED(Buttons.BUTTON2)) 
    MidiClock.stop();
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

