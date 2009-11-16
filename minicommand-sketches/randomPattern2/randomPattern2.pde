#include <MD.h>
#include <MDPitchEuclidSketch.h>

extern "C" uint8_t *__data_end;

uint8_t patData[8190];
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
          uint8_t pitch = pitches[pitches_idx] * 5;
          pitches_idx = (pitches_idx + 1) % pitches_len;
          if (pitch < 128) {
            pattern.addLock(trackNum, i, 12, pitches[pitches_idx]);
          }
        }
      }
    }
    uint16_t len = pattern.toSysex(patData, sizeof(patData));
    GUI.flash_strings_fill("SYSEX", "");
    GUI.flash_put_value16(0, len); 
    MidiUart.sendRaw(patData, len);
  }
};

class RandomPatternSketch : 
public Sketch, MDCallback {
public:
  MDPatternEuclid euclid;
  MDPitchEuclidConfigPage1 configPage1;
  MDPitchEuclidConfigPage2 configPage2;
  SwitchPage switchPage;
  MDTrackFlashEncoder trackEncoder;

  RandomPatternSketch() : 
  configPage1(&euclid), configPage2(&euclid), trackEncoder("TRK") {
    configPage2.encoders[0] = &trackEncoder; // HACK HACK
  }

  void setup() {
    configPage1.setShortName("EUC");
    configPage2.setShortName("SCL");

    switchPage.initPages(&configPage1, &configPage2);
    switchPage.parent = this;

    MDTask.addOnPatternChangeCallback(this, (md_callback_ptr_t)&RandomPatternSketch::onPatternChange);
    MDSysexListener.addOnPatternMessageCallback(this, (md_callback_ptr_t)&RandomPatternSketch::onPatternMessage);

    setPage(&configPage1);
  }
  
  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&switchPage);
      return true;
    } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&switchPage);
      return true;
    } 
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      MD.requestPattern(MD.currentPattern);
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      GUI.flash_string_fill("RANDOM TRACK");
      euclid.makeTrack(trackEncoder.getValue());
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
      euclid.randomizePitches();
    }

    return false;
  }

  void onPatternChange() {
    MD.requestPattern(MD.currentPattern);
  }

  void onPatternMessage() {
    if (euclid.pattern.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
      char name[5];
      MD.getPatternName(euclid.pattern.origPosition, name);
      GUI.flash_strings_fill("PATTERN", name);
    } else {
      GUI.flash_strings_fill("SYSEX", "ERROR");
    }
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

