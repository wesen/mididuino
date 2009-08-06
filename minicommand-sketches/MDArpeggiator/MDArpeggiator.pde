#include <MD.h>
#include <MidiTools.h>
#include <Arpeggiator.hh>
#include <MidiClockPage.h>
#include <SDCard.h>

MDArpeggiatorClass arpeggiator;

class ConfigPage_1 : public EncoderPage {
  public:
  RangeEncoder trackEncoder;
  RangeEncoder speedEncoder;
  RangeEncoder octavesEncoder;
  RangeEncoder lenEncoder;
  
  ConfigPage_1() :
    trackEncoder(0, 15, "TRK"),
    speedEncoder(1, 16, "SPD"),
    octavesEncoder(0, 5, "OCT"),
    lenEncoder(0, 16, "LEN")
  {
    encoders[0] = &trackEncoder;
    encoders[1] = &speedEncoder;
    encoders[2] = &octavesEncoder;
    encoders[3] = &lenEncoder;
    trackEncoder.setValue(arpeggiator.arpTrack);
    speedEncoder.setValue(arpeggiator.arpSpeed);
  }  
  
  virtual void display() {
    EncoderPage::display();
    if (redisplay || lenEncoder.hasChanged()) {
      if (lenEncoder.getValue() == 0)
        GUI.put_p_string_at(12, PSTR("INF"));
    }
  }
  
  virtual void loop() {
    if (lenEncoder.hasChanged()) {
      arpeggiator.arpTimes = lenEncoder.getValue();
    }
    if (speedEncoder.hasChanged()) {
      arpeggiator.arpSpeed = speedEncoder.getValue();
    }
    if (octavesEncoder.hasChanged()) {
      arpeggiator.arpOctaves = octavesEncoder.getValue();
      arpeggiator.arpOctaveCount = 0;
    }
    if (trackEncoder.hasChanged()) {
      uint8_t track = trackEncoder.getValue();
      GUI.setLine(GUI.LINE2);
      GUI.clearFlash();
      GUI.flash_put_value(0, track);
      if (MD.isMelodicTrack(track)) {
        arpeggiator.arpTrack = track;
        GUI.flash_p_string_at_fill(8, MD.getMachineName(MD.kit.machines[track].model));
      } else {
        GUI.flash_p_string_at_fill(8, PSTR("XXX"));
      }
    }
  }
};

class ConfigPage_2 : public EncoderPage {
  public:
  EnumEncoder styleEncoder;
  EnumEncoder retrigEncoder;
  RangeEncoder retrigSpeedEncoder;
  
  ConfigPage_2() :
    styleEncoder(arp_names, (int)ARP_STYLE_CNT, "STY"),
    retrigEncoder(retrig_names, (int)RETRIG_CNT, "TRG"),
    retrigSpeedEncoder(1, 32, "SPD")
  {
    encoders[0] = &styleEncoder;
    encoders[2] = &retrigEncoder;
    encoders[3] = &retrigSpeedEncoder;
    styleEncoder.setValue(0);
    arpeggiator.arpStyle = (arp_style_t)styleEncoder.getValue();
    retrigEncoder.setValue(0);
    arpeggiator.arpRetrig = (arp_retrig_type_t)retrigEncoder.getValue();
    arpeggiator.calculateArp();
  }
  
  virtual void loop() {
    bool changed = false;
    if (styleEncoder.hasChanged()) {
      arpeggiator.arpStyle = (arp_style_t)styleEncoder.getValue();
      changed = true;
    }
    if (retrigEncoder.hasChanged()) {
      arpeggiator.arpRetrig = (arp_retrig_type_t)retrigEncoder.getValue();
      changed = true;
    }
    if (changed) {
      arpeggiator.calculateArp();
    }
    if (retrigSpeedEncoder.hasChanged()) {
      arpeggiator.retrigSpeed = retrigSpeedEncoder.getValue();
    }
  }
};

ConfigPage_1 configPage_1;
ConfigPage_2 configPage_2;

class ArpeggiatorSketch : public Sketch {
public:
  ConfigPage_1 configPage_1;
  ConfigPage_2 configPage_2;

  bool loadedKit;
  bool triggerRecording;
  bool recording;
  bool endRecording;

  ArpeggiatorSketch() {
    loadedKit = false;
    triggerRecording = false;
    recording = false;
    endRecording = false;
  }

  void setup() {
    MDTask.setup();
    MDTask.autoLoadKit = true;
    MDTask.reloadGlobal = true;
    MDTask.addOnKitChangeCallback(_onKitChanged);
    GUI.addTask(&MDTask);

    Midi2.setOnNoteOnCallback(_onNoteOnCallbackKeyboard);
    Midi2.setOnNoteOffCallback(_onNoteOffCallbackKeyboard);
    
    MidiClock.setOn16Callback(_on16Callback);
    setPage(&configPage_1);
  }

  void destroy() {
    GUI.removeTask(&MDTask);
    MDTask.destroy();
  }

  bool handleEvent(gui_event_t *event) {
    if (currentPage() == &configPage_1) {
      if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
	setPage(&configPage_2);
	return true;
      }
    } else if (currentPage() == &configPage_2) {
      if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
	setPage(&configPage_1);
	return true;
      }
    }

    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      triggerRecording = true;
      recording = false;
      endRecording = false;
    }
  }

  void onKitChanged() {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MD.kit.name);
  }

  void on16Callback() {
    if (triggerRecording && (MidiClock.div16th_counter % 16) == 0) {
      triggerRecording = false;
      recording = true;
      arpeggiator.recordStart = MidiClock.div16th_counter;
      for (int i = 0; i < 64; i++) {
	arpeggiator.recordPitches[i] = 128;
      }
    }
    
    if (recording || endRecording) {
      int pos = MidiClock.div16th_counter - arpeggiator.recordStart;
      if (pos >= (arpeggiator.recordLength * 3)) {
	endRecording = false;
      } else if (pos >= (arpeggiator.recordLength * 2)) {
	recording = false;
	endRecording = true;
	return;
      }
    }
    
    if (!triggerRecording) {
      arpeggiator.playNext(recording);
    }
  }
};

ArpeggiatorSketch sketch;

void setup() {
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

void _onKitChanged() {
  sketch.onKitChanged();
}
void _onNoteOffCallbackKeyboard(uint8_t *msg) {
  arpeggiator.removeNote(msg[1]);
}

void _onNoteOnCallbackKeyboard(uint8_t *msg) {
  if (msg[2] != 0) {
    arpeggiator.addNote(msg[1], msg[2]);
  } else {
    arpeggiator.removeNote(msg[1]);
  }
}

void _on16Callback() {
  sketch.on16Callback();
}
