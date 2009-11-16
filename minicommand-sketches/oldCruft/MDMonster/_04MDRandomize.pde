namespace MDRandomize {
  
  bool loadedKit = false;

void onControlChangeCallback(uint8_t *msg) {
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  uint8_t track, param;
  MD.parseCC(channel, msg[1], &track, &param);
  if (track != 255) {
    MD.kit.machines[track].params[param] = msg[2];
  }
}

#define SELECT_FILTER 0
#define SELECT_AMD    1
#define SELECT_EQ     2
#define SELECT_EFFECT 3
#define SELECT_LOWSYN 4
#define SELECT_UPSYN  5
#define SELECT_SYN    6
#define SELECT_LFO    7
#define SELECT_SENDS  8
#define SELECT_DIST   9
#define SELECT_FX_LOWSYN 10
#define SELECT_FX_SYN    11
#define SELECT_ALL       12

uint32_t paramSelectMask[13] = {
  // SELECT_FILTER
  _BV(MODEL_FLTF) | _BV(MODEL_FLTW) | _BV(MODEL_FLTQ), 

  // SELECT_AMD
  _BV(MODEL_AMD) | _BV(MODEL_AMF), 

  // SELECT_EQ
  _BV(MODEL_EQF) | _BV(MODEL_EQG), 

  // SELECT_EFFECT
  _BV(MODEL_AMD) | _BV(MODEL_AMF) | _BV(MODEL_EQF) | _BV(MODEL_EQG) |
  _BV(MODEL_FLTF) | _BV(MODEL_FLTW) | _BV(MODEL_FLTQ), 
  
  // SELECT_LOWSYN
  _BV(MODEL_P5) | _BV(MODEL_P6) | _BV(MODEL_P7) | _BV(MODEL_P8),
  
  // SELECT_UPSYN
  _BV(MODEL_P2) | _BV(MODEL_P3) | _BV(MODEL_P4),
  
  // SELECT_SYN
  _BV(MODEL_P2) | _BV(MODEL_P3) | _BV(MODEL_P4) | _BV(MODEL_P5) |
  _BV(MODEL_P6) | _BV(MODEL_P7),

  // SELECT_LFO  
  _BV(MODEL_LFOS) | _BV(MODEL_LFOD) | _BV(MODEL_LFOM),
  
  // SELECT_SENDS
  _BV(MODEL_DEL) | _BV(MODEL_REV),
  
  // SELECT_DIST
  _BV(MODEL_SRR) | _BV(MODEL_DIST),
  
  // SELECT FX_LOWSYN
  _BV(MODEL_AMD) | _BV(MODEL_AMF) | _BV(MODEL_EQF) | _BV(MODEL_EQG) |
  _BV(MODEL_FLTF) | _BV(MODEL_FLTW) | _BV(MODEL_FLTQ) |
  _BV(MODEL_P5) | _BV(MODEL_P6) | _BV(MODEL_P7) | _BV(MODEL_P8),
  
  // SELECT_FX_SYN
  _BV(MODEL_AMD) | _BV(MODEL_AMF) | _BV(MODEL_EQF) | _BV(MODEL_EQG) |
  _BV(MODEL_FLTF) | _BV(MODEL_FLTW) | _BV(MODEL_FLTQ) |
  _BV(MODEL_P2) | _BV(MODEL_P3) | _BV(MODEL_P4) |
  _BV(MODEL_P5) | _BV(MODEL_P6) | _BV(MODEL_P7) | _BV(MODEL_P8),
  
  // SELECT_ALL
  _BV(MODEL_AMD) | _BV(MODEL_AMF) | _BV(MODEL_EQF) | _BV(MODEL_EQG) |
  _BV(MODEL_FLTF) | _BV(MODEL_FLTW) | _BV(MODEL_FLTQ) | _BV(MODEL_SRR) |
  _BV(MODEL_P1) | _BV(MODEL_P2) | _BV(MODEL_P3) | _BV(MODEL_P4) |
  _BV(MODEL_P5) | _BV(MODEL_P6) | _BV(MODEL_P7) | _BV(MODEL_P8) |
  _BV(MODEL_DIST) | _BV(MODEL_VOL) | _BV(MODEL_PAN) | _BV(MODEL_DEL) |
  _BV(MODEL_REV) | _BV(MODEL_LFOS) | _BV(MODEL_LFOD) | _BV(MODEL_LFOM)
};

PROGMEM char selectNames[13][7] = {
  "FILTER",
  "AMD   ",
  "EQ    ",
  "EFFECT",
  "LOWSYN",
  "UP SYN",
  "SYN   ",
  "LFO   ",
  "SENDS ",
  "DIST  ",
  "FX LOW",
  "FX SYN",
  "ALL   "
};

RangeEncoder trackEncoder(0, 15, "TRK");
RangeEncoder amtEncoder(0, 128, "AMT");
RangeEncoder selectEncoder(0, 12, "SEL");

class RandomizePage : public Page {
public:
  Stack<uint8_t [24], 8> undoStack;
  
  RandomizePage() {
  }
  
  virtual void update() {
    uint8_t tmp = SREG;
    cli();
    selectEncoder.update(&Encoders.encoders[0]);
    amtEncoder.update(&Encoders.encoders[2]);
    trackEncoder.update(&Encoders.encoders[3]);
    Encoders.clearEncoders();
    SREG = tmp;
  }

  void trackHandle() {
      displayTrack();
      undoStack.reset();
      if (loadedKit) {
        GUI.setLine(GUI.LINE2);
        GUI.flash_p_string_at(9, MD.getMachineName(MD.kit.machines[trackEncoder.getValue()].model));
      }
  }    
  
  virtual void handle() {
    if (trackEncoder.hasChanged()) {
      trackHandle();
    }
    trackEncoder.checkHandle();
    if (selectEncoder.hasChanged()) {
      displaySelect();
    }
    selectEncoder.checkHandle();
    if (amtEncoder.hasChanged()) {
      GUI.setLine(GUI.LINE2);
      GUI.put_value_at(9, amtEncoder.getValue());
    }
    amtEncoder.checkHandle();
    
  }
  
  void displaySelect() {
    GUI.setLine(GUI.LINE2);
    GUI.put_p_string(selectNames[selectEncoder.getValue()]);
  }

  void displayKit() {
    GUI.setLine(GUI.LINE1);
    if (!loadedKit) {
      GUI.put_p_string(PSTR("NO KIT     "));
    } else {
      GUI.put_string(MD.kit.name);
    }
  }
  
  void displayTrack() {
    GUI.setLine(GUI.LINE1);
    GUI.put_value_at(13, trackEncoder.getValue());
  }
  
  virtual void display(bool redisplay = false) {
    if (redisplay) {
      displayTrack();
      displayKit();
      displaySelect();
      GUI.setLine(GUI.LINE2);
      GUI.put_value_at(9, amtEncoder.getValue());
    }
  }
  
  void randomize() {
    uint32_t trackMask = paramSelectMask[selectEncoder.getValue()];
    uint8_t track = trackEncoder.getValue();
    int amount = (int)amtEncoder.getValue();

    GUI.setLine(GUI.LINE2);
    undoStack.push(&MD.kit.machines[track].params);
  
    for (uint8_t i = 0; i < 24; i++) {
      if (IS_BIT_SET(trackMask, i)) {
        int param = MD.kit.machines[track].params[i];
        int rand = random(-amount, amount);
        param += rand;
        if (param > 127) 
          param = 127;
        if (param < 0)
          param = 0;
        MD.kit.machines[track].params[i] = param;
        MD.setTrackParam(track, i, param);
      }
    }
  }

  void undo() {
    GUI.setLine(GUI.LINE1);
    uint8_t track = trackEncoder.getValue();
    if (undoStack.pop(&MD.kit.machines[track].params)) {
      GUI.flash_p_string_fill(PSTR("UNDO"));
      for (uint8_t i = 0; i < 24; i++) {
        MD.setTrackParam(track, i, MD.kit.machines[track].params[i]);
      }
    } else {
      GUI.flash_p_string_fill(PSTR("UNDO XXX"));
    }
  }
};

RandomizePage randomizePage;

void setupPages() {
  GUI.setPage(&randomizePage);
}

void handleGuiPages() {
  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    randomizePage.randomize();
  }
  if (BUTTON_PRESSED(Buttons.BUTTON3)) {
    randomizePage.undo();
  }
}

void onCurrentKitCallback() {
  loadedKit = true;
  randomizePage.trackHandle();
  randomizePage.display(true);
}

class MDRandomizeSketch : public SubSketch {
  public:
void setup() {
  GUI.flash_p_string_clear(PSTR("MD RANDOMIZE"));
  setupPages();
  Midi.setOnControlChangeCallback(onControlChangeCallback);
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    loadedKit = false;
    MDSysexListener.getCurrentKit(onCurrentKitCallback);
  }
  handleGuiPages();  
}

void destroy() {
  Midi.setOnControlChangeCallback(NULL);
}

PGM_P getName() {
  return PSTR("RANDOMIZER");
}
};

MDRandomizeSketch sketch;

}
