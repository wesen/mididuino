#include <MD.h>
#include <SDCard.h>
#include <MidiClockPage.h>
#include <Sequencer.h>
#include <Scales.h>

#define NUM_SCALES 7

scale_t *scales[NUM_SCALES] = {
  &ionianScale,
  &aeolianScale,
  &bluesScale,
  &majorPentatonicScale,
  &majorMaj7Arp,
  &majorMin7Arp,
  &minorMin7Arp
};

scale_t *scale = &majorScale;

class PitchEuclid {
public:
  EuclidDrumTrack track;
  scale_t *currentScale;

  uint8_t pitches[32];
  uint8_t pitches_len;
  uint8_t pitches_idx;

  uint8_t mdTrack;

  uint8_t octaves;
  uint8_t basePitch;

  PitchEuclid() : 
  track(3, 8, 0) {
    currentScale = scales[0];
    octaves = 0;

    pitches_len = 0;
    pitches_idx = 0;
    setPitchLength(4);
    mdTrack = 0;
  }

  void setPitchLength(uint8_t len) {
    pitches_len = len;
    randomizePitches();
  }
  
  void randomizePitches() {
    for (uint8_t i = 0; i < pitches_len; i++) {
      pitches[i] = randomScalePitch(currentScale, octaves);
    }
  }

  void on16Callback() {
    if (track.isHit(MidiClock.div16th_counter)) {
      uint8_t pitch = basePitch + pitches[pitches_idx];
      if (pitch <= 127) {
        MD.sendNoteOn(mdTrack, pitch, 100);
      }
      pitches_idx = (pitches_idx + 1) % pitches_len;
    }
  }
};

PitchEuclid pitchEuclid;

class PitchEuclidConfigPage1 : 
public EncoderPage {
public:
  RangeEncoder pitchLengthEncoder;
  RangeEncoder pulseEncoder;
  RangeEncoder lengthEncoder;
  RangeEncoder offsetEncoder;

  PitchEuclidConfigPage1() :
  pitchLengthEncoder(1, 32, "PTC", 4),
  pulseEncoder(1, 32, "PLS", 3),
  lengthEncoder(2, 32, "LEN", 8),
  offsetEncoder(0, 32, "OFF", 0) {
    encoders[0] = &pitchLengthEncoder;
    encoders[1] = &pulseEncoder;
    encoders[2] = &lengthEncoder;
    encoders[3] = &offsetEncoder;
  }

  void loop() {
    if (pulseEncoder.hasChanged() || lengthEncoder.hasChanged() || offsetEncoder.hasChanged()) {
      pitchEuclid.track.setEuclid(pulseEncoder.getValue(), lengthEncoder.getValue(),
      offsetEncoder.getValue());
    }
    if (pitchLengthEncoder.hasChanged()) {
      pitchEuclid.setPitchLength(pitchLengthEncoder.getValue());
    }
  }
};

class PitchEuclidConfigPage2 : 
public EncoderPage {
public:
  RangeEncoder trackEncoder;
  RangeEncoder scaleEncoder;
  RangeEncoder octavesEncoder;
  NotePitchEncoder basePitchEncoder;

  PitchEuclidConfigPage2() :
  trackEncoder(0, 15, "TRK", 0),
  scaleEncoder(0, NUM_SCALES, "SCL", 0),
  basePitchEncoder("BAS"),
  octavesEncoder(0, 4, "OCT")
  {
    encoders[0] = &trackEncoder;
    encoders[3] = &basePitchEncoder;
    encoders[2] = &octavesEncoder;
    encoders[1] = &scaleEncoder;
  }

  void loop() {
    if (scaleEncoder.hasChanged()) {
      pitchEuclid.currentScale = scales[scaleEncoder.getValue()];
      pitchEuclid.randomizePitches();
    }
    if (basePitchEncoder.hasChanged()) {
      pitchEuclid.basePitch = basePitchEncoder.getValue();
    }
    if (octavesEncoder.hasChanged()) {
      pitchEuclid.octaves = octavesEncoder.getValue();
      pitchEuclid.randomizePitches();
    }
  }

  void display() {
    if (trackEncoder.hasChanged()) {
      GUI.setLine(GUI.LINE2);
      uint8_t track = trackEncoder.getValue();
        GUI.flash_put_value(0, track);
      if (MD.isMelodicTrack(track)) {
        GUI.flash_p_string_at_fill(4, MD.getMachineName(MD.kit.machines[track].model));
        pitchEuclid.mdTrack = track;
      } 
      else {
        GUI.flash_p_string_at_fill(4, PSTR("XXX"));
        pitchEuclid.mdTrack = 255;
      }
    }
    EncoderPage::display();
  }    
};

class PitchEuclidSketch : 
public Sketch {
  public:
  PitchEuclidConfigPage1 page1;
  PitchEuclidConfigPage2 page2;

  PitchEuclidSketch() {
  }

  void setup() {
    MDTask.setup();
    MDTask.autoLoadKit = true;
    MDTask.reloadGlobal = true;
    MDTask.addOnKitChangeCallback(_onKitChanged);
//    MDTask.addOnGlobalChangeCallback(_onGlobalChanged);
    GUI.addTask(&MDTask);

    MidiClock.setOn16Callback(_on16Callback);

    setPage(&page1);
  }

  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      GUI.setPage(&page1);
      return true;
    } else if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      GUI.setPage(&page2);
      return true;
    } else if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
      pitchEuclid.randomizePitches();
    }
  }

  void onKitChanged() {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MD.kit.name);
  }  

};

PitchEuclidSketch sketch;

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
  } 
  else {
    midiClockPage.setup();
    if (BUTTON_DOWN(Buttons.BUTTON1)) {
      GUI.pushPage(&midiClockPage);
    }
  }
}

void loop() {
}

void _on16Callback() {
  pitchEuclid.on16Callback();
}

void _onKitChanged() {
  sketch.onKitChanged();
}

