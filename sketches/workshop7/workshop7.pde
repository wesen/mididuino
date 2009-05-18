void on16thNoteCallback() {
  GUI.setLine(GUI.LINE1);
  GUI.put_value(0, MidiClock.div16th_counter % 16);
}

void setup() {
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16thNoteCallback);
  MidiClock.start();
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

void handleGui() {
}
