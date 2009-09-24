#include <MD.h>
#include <MDPitchEuclidSketch.h>

uint8_t patData[4096];
class MDPatternEuclid : 
public MDPitchEuclid {
public:
  MDPattern pattern;

  MDPatternEuclid() : 
  MDPitchEuclid() {
    track.setEuclid(3, 8, 0);
    randomizePitches();
  }

  void makeTrack(uint8_t trackNum) {
    pattern.clearTrack(trackNum);
    for (uint8_t i = 0; i < pattern.patternLength; i++) {
      if (track.isHit(i)) {
        pattern.setTrig(trackNum, i);
        if (MD.isMelodicTrack(trackNum)) {
          uint8_t pitch = basePitch + pitches[pitches_idx];
          pitches_idx = (pitches_idx + 1) % pitches_len;
          uint8_t realPitch = MD.trackGetPitch(trackNum, pitch);
          pattern.addLock(trackNum, i, 0, realPitch);
        }
      }
    }
    uint16_t len = pattern.toSysex(patData, sizeof(patData));
    MidiUart.puts(patData, len);
  }
};

class RandomPatternPage : 
public EncoderPage, MDCallback {
public:
  MDTrackFlashEncoder trackEncoder;
  MDPatternEuclid *euclid;

  RandomPatternPage(MDPatternEuclid *_euclid) : 
  euclid(_euclid),
  trackEncoder("TRK") {
    setEncoders(&trackEncoder);
  }

  void setup() {
    MDTask.addOnPatternChangeCallback(this, (md_callback_ptr_t)&RandomPatternPage::onPatternChange);
    MDSysexListener.addOnPatternMessageCallback(this, (md_callback_ptr_t)&RandomPatternPage::onPatternMessage);
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      MD.requestPattern(MD.currentPattern);
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      euclid->makeTrack(trackEncoder.getValue());
    }
    return false;
  }

  void onPatternChange() {
    MD.requestPattern(MD.currentPattern);
  }

  void onPatternMessage() {
    if (euclid->pattern.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
      char name[5];
      MD.getPatternName(euclid->pattern.origPosition, name);
      GUI.flash_strings_fill("PATTERN", name);
    }
  }
};

class RandomPatternSketch : 
public Sketch {
public:
  RandomPatternPage page;
  MDPatternEuclid euclid;
  MDPitchEuclidConfigPage1 configPage1;
  MDPitchEuclidConfigPage2 configPage2;
  SwitchPage switchPage;

  RandomPatternSketch() : 
  page(&euclid), configPage1(&euclid), configPage2(&euclid) {
  }

  void setup() {
    page.setup();
    page.setShortName("PAT");
    configPage1.setShortName("EUC");
    configPage2.setShortName("SCL");

    switchPage.initPages(&page, &configPage1, &configPage2);
    switchPage.parent = this;

    setPage(&page);
  }
  
  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&switchPage);
      return true;
    } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&switchPage);
      return true;
    } 
    
    return false;
  }

};

RandomPatternSketch sketch;

void setup() {
  initMDTask();
  MDTask.verbose = false;

  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}

