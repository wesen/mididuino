void onNoteOnCallback(uint8_t *msg) {
  GUI.setLine(GUI.LINE1);
  GUI.put_value(0, msg[1]);
  GUI.put_value(1, msg[2]);
}

void setup() {
  Midi.setOnNoteOnCallback(onNoteOnCallback);
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

void handleGui() {
}
