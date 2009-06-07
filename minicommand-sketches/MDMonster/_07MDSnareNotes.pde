namespace MDSnareNotes {
  class MDSnareNotesSketch : public SubSketch {
    public:
    void setup() {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("SNARE NOTES"));
      GUI.setLine(GUI.LINE2);
      GUI.put_p_string_fill(PSTR(""));
  MD.kit.machines[0].model = EFM_SD_MODEL;
  MD.kit.machines[1].model = ROM_MODEL;
  MD.kit.machines[2].model = EFM_SD_MODEL;
  MD.kit.machines[3].model = ROM_MODEL;
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
