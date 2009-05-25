namespace MDSnareNotes {
  class MDSnareNotesSketch : public SubSketch {
    public:
    void setup() {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("SNARE NOTES"));
      GUI.setLine(GUI.LINE2);
      GUI.put_p_string_fill(PSTR(""));
  MD.trackModels[0] = EFM_SD_MODEL;
  MD.trackModels[1] = ROM_MODEL;
  MD.trackModels[2] = EFM_SD_MODEL;
  MD.trackModels[3] = ROM_MODEL;
}

void loop() {
}

void handleGui() {
  uint8_t i;
  for (i = 0; i < 4; i++) {
    if (BUTTON_PRESSED(i + 4))
      MD.sendNoteOn(i, scalePitch(random(47, 80), 47, majorScale), 100);
  }
}

    PGM_P getName() {
      return PSTR("SNARE NOTES");
    }
  };
  
  MDSnareNotesSketch sketch;
}
