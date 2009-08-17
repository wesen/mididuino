#include <MD.h>

class MDLFOPage : 
public EncoderPage {
public:
  MDLFOEncoder lfoEncoders[4];

  void initLFOParams(uint8_t lfo1, uint8_t lfo2, uint8_t lfo3, uint8_t lfo4) {
    lfoEncoders[0].setParam(lfo1);
    lfoEncoders[1].setParam(lfo2);
    lfoEncoders[2].setParam(lfo3);
    lfoEncoders[3].setParam(lfo4);
    redisplay = true;
  }

  MDLFOPage() {  
    initLFOParams(MD_LFO_SHP1, MD_LFO_SHP2, MD_LFO_UPDTE, MD_LFO_SPEED);
    for (uint8_t i = 0; i < 4; i++) {
      encoders[i] = &lfoEncoders[i];
    }
  }

  void setTrack(uint8_t track) {
    for (uint8_t i = 0; i < 4; i++) {
      lfoEncoders[i].track = track;
      lfoEncoders[i].loadFromKit();
    }
  }

  void loadFromKit() {
    for (uint8_t i = 0; i < 4; i++) {
      lfoEncoders[i].loadFromKit();
    }
  }
};

MDLFOPage lfoPage;

class MDLFOConfigPage : 
public EncoderPage {
public:
  EnumEncoder paramEncoders[4];

  MDLFOConfigPage() {
    for (uint8_t i = 0; i < 4; i++) {
      paramEncoders[i].initEnumEncoder(MDLFONames, 8);
      encoders[i] = &paramEncoders[i];
    }
    paramEncoders[0].setName("P1");
    paramEncoders[1].setName("P2");
    paramEncoders[2].setName("P3");
    paramEncoders[3].setName("P4");
  }

  virtual void show() {
    for (uint8_t i = 0; i < 4; i++) {
      paramEncoders[i].setValue(lfoPage.lfoEncoders[i].param);
    }
  }

  virtual void loop() {
    for (uint8_t i = 0 ;i < 4; i++) {
      if (paramEncoders[i].hasChanged()) {
        lfoPage.lfoEncoders[i].setParam(paramEncoders[i].getValue());
      }
    }
  }
};

MDLFOConfigPage lfoConfigPage;

class MDLFOTrackSelectPage : 
public EncoderPage {
public:
  MDTrackFlashEncoder trackEncoder;

  MDLFOTrackSelectPage() : trackEncoder("TRK") {
    encoders[0] = &trackEncoder;
  }  

  virtual void loop() {
    if (trackEncoder.hasChanged()) {
      lfoPage.setTrack(trackEncoder.getValue());
    }
  }
};

MDLFOTrackSelectPage lfoTrackSelectPage;

class MDLFOSketch : 
public Sketch, public MDCallback {
public:
  void setup() {
    MDTask.setup();
    MDTask.autoLoadKit = true;
    MDTask.reloadGlobal = true;
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&MDLFOSketch::onKitChanged);
    GUI.addTask(&MDTask);

    setPage(&lfoPage);
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&lfoTrackSelectPage);
    } 
    else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&lfoTrackSelectPage);
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      pushPage(&lfoConfigPage);
    } 
    else if (EVENT_RELEASED(event, Buttons.BUTTON2)) {
      popPage(&lfoConfigPage);
    }
  }

  void onKitChanged() {
    lfoPage.loadFromKit();
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MD.kit.name);
  }
};

MDLFOSketch sketch;

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}
