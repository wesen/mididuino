#include <MD.h>

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

const char *selectNames[13] = {
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

class RandomizePage : 
public EncoderPage {
public:
  RangeEncoder trackEncoder;
  RangeEncoder amtEncoder;
  EnumEncoder selectEncoder;

  Stack<uint8_t [24], 8> undoStack;

  RandomizePage() :
  trackEncoder(0, 15, "TRK"),
  amtEncoder(0, 128, "AMT"),
  selectEncoder(selectNames, 13, "SEL") {
    encoders[0] = &trackEncoder;
    encoders[1] = &amtEncoder;
    encoders[2] = &selectEncoder;
  }

  void trackHandle() {
    undoStack.reset();
    if (MD.loadedKit) {
      GUI.setLine(GUI.LINE2);
      GUI.flash_put_value(0, trackEncoder.getValue());
      GUI.flash_p_string_at(4, MD.getMachineName(MD.kit.machines[trackEncoder.getValue()].model));
    }
  }    

  virtual void loop() {
    if (trackEncoder.hasChanged()) {
      trackHandle();
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
    } 
    else {
      GUI.flash_p_string_fill(PSTR("UNDO XXX"));
    }
  }

  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      randomize();
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      undo();
      return true;
    }

    return false;
  }
};

RandomizePage randomizePage;

class MDRandomizeSketch : 
public Sketch {
public:

  void setup() {
    MDTask.setup();
    MDTask.autoLoadKit = true;
    MDTask.reloadGlobal = true;
    MDTask.addOnKitChangeCallback(_onKitChanged);
//    MDTask.addOnGlobalChangeCallback(_onGlobalChanged);
    GUI.addTask(&MDTask);
    
    setPage(&randomizePage);
  }
};

MDRandomizeSketch sketch;

void onControlChangeCallback(uint8_t *msg) {
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  uint8_t track, param;
  MD.parseCC(channel, msg[1], &track, &param);
  if (track != 255) {
    MD.kit.machines[track].params[param] = msg[2];
  }
}

void setup() {
  GUI.flash_p_string_clear(PSTR("MD RANDOMIZE"));
  sketch.setup();
  GUI.setSketch(&sketch);
  Midi.setOnControlChangeCallback(onControlChangeCallback);
}

void loop() {
}

void _onKitChanged() {
  randomizePage.trackHandle();
  randomizePage.redisplayPage();
}
