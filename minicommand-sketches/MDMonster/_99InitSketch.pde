void setSketch(SubSketch &sketch) {
  if (currentSketch != NULL) {
    currentSketch->destroy();
  }
  GUI.flash_p_strings_fill(sketch.getName(), PSTR(""));
  GUI.setPage(NULL);
  currentSketch = &sketch;
  currentSketch->setup();
}

SubSketch *sketches[] = {
  &MDArpeggiator::sketch,
  &MDNotes::sketch,
  &MDWesenLivePatch::sketch,
  &MDRandomize::sketch,
  &MDPitchEuclid::sketch,
  &MDMelodyHelper::sketch,
  &MDSnareNotes::sketch,
  NULL
};

RangeEncoder firmwareEncoder(0, 6);

class FirmwarePage : public EncoderPage {
  public:
  FirmwarePage() {
    encoders[0] = &firmwareEncoder;
  }
  
  virtual void display(bool redisplay = true) {
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("SELECT FIRMWARE:"));
    }

    if (firmwareEncoder.hasChanged() || redisplay) {
      GUI.setLine(GUI.LINE2);
      GUI.put_p_string_fill(sketches[firmwareEncoder.getValue()]->getName());
    }

  }
};

FirmwarePage page;

class InitSketch : public SubSketch {
  public:
  void setup() {
    GUI.flash_p_strings_fill(PSTR("WELCOME TO THE"), PSTR("RW MINICOMMAND"));
    GUI.setLine(GUI.LINE1);
    GUI.setPage(&page);
  }
  
  void loop() {
    GUI.updatePage();
    GUI.update();
  }
  
  void handleGui() {
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      setSketch(*sketches[firmwareEncoder.getValue()]);
    }
  }
  
  void destroy() {
  }
};

InitSketch initSketch;

void setup() {
  setSketch(initSketch);
}
