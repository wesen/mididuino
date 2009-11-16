#include <MD.h>
#include <MidiClockPage.h>

class PatternRandomizationPage : public EncoderPage, ClockCallback, MDCallback {
  public:
  RangeEncoder lengthEncoder;
  
  uint8_t randomPatterns[16];
  uint8_t len;
  uint8_t idx;
  
  PatternRandomizationPage() : lengthEncoder(1, 16, "LEN") {
    setEncoders(&lengthEncoder);
    lengthEncoder.pressmode = true;
    for (uint8_t i = 0; i < countof(randomPatterns); i++) {
      randomPatterns[i] = 0;
    }
    len = 1;
    idx = 0;
  }
  
  void setRandomLength(uint8_t _len) {
    len = _len;
  }
  
  void setup() {
    MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&PatternRandomizationPage::on16Callback);
    MDTask.addOnPatternChangeCallback(this, (md_callback_ptr_t)&PatternRandomizationPage::onPatternChange);
  }
  
  virtual void loop() {
    if (lengthEncoder.hasChanged()) {
      setRandomLength(lengthEncoder.getValue());
    }
  }

  void advancePattern() {
    #if 0
    GUI.setLine(GUI.LINE1);
    GUI.flash_put_value(1, idx);
    GUI.flash_put_value(2, randomPatterns[idx]);
    GUI.setLine(GUI.LINE2);
    GUI.flash_put_value(1, len);
    GUI.flash_put_value(2, MD.currentPattern);
    #endif
    
    if (MD.currentPattern == randomPatterns[idx]) {
      idx++;
      if (idx >= len) {
        idx = 0;
      }
      MD.loadPattern(randomPatterns[idx]);
    } else {
      MD.loadPattern(randomPatterns[idx]);
    }
  }

  void onPatternChange() {
  }
  
  void on16Callback(uint32_t counter) {
    if (counter == 0) {
      randomizePatterns();
    }
    if ((counter % 16) == 0) {
      MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_PATTERN_REQUEST);
    }
    if ((counter % 16) == 8) {
      advancePattern();
    }
  }
  
  void flashPatternNames() {
    for (uint8_t i = 0; i < countof(randomPatterns); i++) {
      char name[5];
      MD.getPatternName(randomPatterns[i], name);
      if (i < 4) {
        GUI.setLine(GUI.LINE1);
        GUI.flash_string_at(i << 2, name);
      } else if (i < 8) {
        GUI.setLine(GUI.LINE2);
        GUI.flash_string_at((i - 4) << 2, name);
      }
    }
  }
  
  void randomizePatterns() {
    for (uint8_t i = 0; i < countof(randomPatterns); i++) {
      randomPatterns[i] = random(127);
    }
    
    flashPatternNames();
    
    idx = 0;
    MD.loadPattern(randomPatterns[0]);
  }
  
  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
      randomizePatterns();
      return true;
    }
    return false;
  }
};

class LeoSketch : public Sketch {
  public:
  PatternRandomizationPage randomPage;

  void setup() {
    randomPage.setup();
    setPage(&randomPage);
  }
};

LeoSketch sketch;

void setup() {
  initMDTask();
  MDTask.verbose = false;
  
  sketch.setup();
  GUI.setSketch(&sketch);
  
  initClockPage();
}
  
  void loop() {
  }
