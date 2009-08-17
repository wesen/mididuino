#include <MD.h>
#include <MDRandomizer.h>
#include <SDCard.h>
#include <MidiClockPage.h>
#include <Merger.h>

MDRandomizerClass MDRandomizer;

class RandomizePage : 
public EncoderPage {
public: 
  MDTrackFlashEncoder trackEncoder;
  RangeEncoder amtEncoder;
  EnumEncoder selectEncoder;

  RandomizePage() :
  trackEncoder("TRK"),
  amtEncoder(0, 128, "AMT"),
  selectEncoder(MDRandomizerClass::selectNames, countof(MDRandomizerClass::selectNames), "SEL") {
    encoders[0] = &trackEncoder;
    encoders[1] = &amtEncoder;
    encoders[2] = &selectEncoder;

    MDRandomizer.setTrack(trackEncoder.getValue());
  }

  virtual void loop() {
    if (trackEncoder.hasChanged()) {
      MDRandomizer.setTrack(trackEncoder.getValue());
    }
  }

  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      MDRandomizer.randomize(amtEncoder.getValue(), selectEncoder.getValue());
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      GUI.setLine(GUI.LINE1);
      if (MDRandomizer.undo()) {
        GUI.flash_p_string_fill(PSTR("UNDO"));
      } 
      else {
        GUI.flash_p_string_fill(PSTR("UNDO XXX"));
      }
      return true;
    }

    return false;
  }
};

RandomizePage randomizePage;

class MDRandomizeSketch : 
public Sketch, public MDCallback {
public:

  void setup() {
    MDRandomizer.setup();

    MDTask.setup();
    MDTask.autoLoadKit = true;
    MDTask.reloadGlobal = true;
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&MDRandomizeSketch::onKitChanged);
    GUI.addTask(&MDTask);

    setPage(&randomizePage);
    
  }


  void onKitChanged() {
    MDRandomizer.onKitChanged();
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MD.kit.name);
  }

};

MDRandomizeSketch sketch;

void setup() {
  GUI.flash_p_string_clear(PSTR("MD RANDOMIZE"));
  sketch.setup();
  GUI.setSketch(&sketch);

  if (SDCard.init() != 0) {
    GUI.flash_strings_fill("SDCARD ERROR", "");
    GUI.display();
    delay(800);
    MidiClock.mode = MidiClock.EXTERNAL_MIDI;
    MidiClock.transmit = true;
    MidiClock.start();
  } else {
    midiClockPage.setup();
    if (BUTTON_DOWN(Buttons.BUTTON1)) {
      GUI.pushPage(&midiClockPage);
    }
  }
}

void loop() {
}

