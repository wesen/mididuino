#include <MD.h>

MDKit kit;

class MDRandomPage : 
public EncoderPage {
public:
  MDTrackFlashEncoder trackEncoder;

  MDRandomPage() : 
  trackEncoder("TRK"), EncoderPage(&trackEncoder) {
  };  
};

class MDRandomSeqSketch : 
public Sketch, MDCallback, MidiCallback {
  MDRandomPage randomPage;
//  MDPattern pattern;

public:
  void setup() {
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&MDRandomSeqSketch::onKitChange);
    MDTask.addOnGlobalChangeCallback(this, (md_callback_ptr_t)&MDRandomSeqSketch::onGlobalChange);
    MDTask.addOnPatternChangeCallback(this, (md_callback_ptr_t)&MDRandomSeqSketch::onPatternChange);
    MDSysexListener.addOnPatternMessageCallback(this,
    (md_callback_ptr_t)&MDRandomSeqSketch::onPatternMessage);
    Midi.addOnControlChangeCallback(this, (midi_callback_ptr_t)&MDRandomSeqSketch::onControlChange);
  }

  virtual void show() {
    if (currentPage() == NULL)
      setPage(&randomPage);
  }
  
  void onGlobalChange() {
    GUI.flash_printf_fill("CHAN: %b", MD.global.baseChannel);
  }

  void onKitChange() {
    m_memcpy(&kit, &MD.kit, sizeof(kit));
  }

  void onPatternChange() {
    MD.requestPattern(MD.currentPattern);
  }

  void onPatternMessage() {
    if (pattern.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
      char name[5];
      MD.getPatternName(pattern.origPosition, name);
      GUI.flash_strings_fill("PATTERN", name);
    } 
    else {
      GUI.flash_strings_fill("PAT SYSEX", "ERROR");
    }
  }

  void onControlChange(uint8_t *msg) {
    uint8_t track, param;
    track = 0;
    param = 0;
    MD.parseCC(msg[0] & 0xF, msg[1], &track, &param);
    if (track == 255) {
      return;
    }
    kit.params[track][param] = msg[2];
  }
};

MDRandomSeqSketch sketch;

void setup() {
  sketch.setup();
  initMDTask();

  GUI.setSketch(&sketch);
}


