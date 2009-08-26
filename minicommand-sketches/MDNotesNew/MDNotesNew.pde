#include <MD.h>
#include <MidiTools.h>
#include <MDRecorder.h>
#include <Merger.h>
#include <MidiClockPage.h>
#include <SDCard.h>

class ConfigPage_1 : 
public EncoderPage {
public:
  MDMelodicTrackFlashEncoder trackEncoder;
  uint8_t track;

  ConfigPage_1() :
  trackEncoder("TRK")
  {
    track = 255;
    encoders[0] = &trackEncoder;
  }  

  virtual void loop() {
    if (trackEncoder.hasChanged()) {
      track = trackEncoder.getValue();
    }
  }
};

class MDNotesSketch : 
public Sketch, public MDCallback, public ClockCallback, public MidiCallback {
public:
  ConfigPage_1 configPage_1;
  uint8_t track;

  MDNotesSketch() {
    track = 255;
  }

  void setup() {
    MDTask.setup();
    MDTask.autoLoadKit = true;
    MDTask.reloadGlobal = true;
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&MDNotesSketch::onKitChanged);
    GUI.addTask(&MDTask);

    MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&MDNotesSketch::on16Callback);
    Midi2.addOnNoteOnCallback(this, (midi_callback_ptr_t)&MDNotesSketch::onNoteOnCallbackKeyboard);

    setPage(&configPage_1);
  }

  void destroy() {
    GUI.removeTask(&MDTask);
    MDTask.destroy();
  }

  void onNoteOnCallbackKeyboard(uint8_t *msg) {
    if (configPage_1.track != 255) {
      MD.sendNoteOn(configPage_1.track, msg[1], msg[2]);
    }
  }

  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      MDRecorder.startRecord(16, 16);
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      MDRecorder.looping = true;
      MDRecorder.startPlayback(16);
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      MDRecorder.stopPlayback();
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
      MDRecorder.looping = false;
      MDRecorder.startMDPlayback(16);
      return true;
    }
    return false;
  }


  void on16Callback() {
    GUI.setLine(GUI.LINE1);
    GUI.put_value(1, MDRecorder.rec16th_counter);
    GUI.put_value(2, MDRecorder.play16th_counter);
    GUI.put_value(3, MDRecorder.eventList.size());
    GUI.setLine(GUI.LINE2);
    GUI.put_value(2, (uint8_t)(MidiClock.div16th_counter % 32));
  }

  void onKitChanged() {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MD.kit.name);
    configPage_1.trackEncoder.old = 255;
  }
};

MDNotesSketch sketch;

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);
  MDRecorder.setup();
  
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

