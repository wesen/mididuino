void setup() {
  GUI.setLine(GUI.LINE1);
  GUI.put_string_fill("RECV MIDI");
}

void onControlChange(uint8_t *msg) {
  GUI.setLine(GUI.LINE2);
  GUI.printf_fill("C%b CC%b V%b", msg[0] & 0xF, msg[1], msg[2]);
}

void onNoteOn(uint8_t *msg) {
  GUI.setLine(GUI.LINE2);
  GUI.printf_fill("C%b N%b V%b", msg[0] & 0xF, msg[1], msg[2]);
}


