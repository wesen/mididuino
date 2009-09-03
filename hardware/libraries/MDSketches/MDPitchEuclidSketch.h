#ifndef MDPITCHEUCLIDSKETCH_H__
#define MDPITCHEUCLIDSKETCH_H__

#include <MD.h>
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

class MDPitchEuclid {
public:
  EuclidDrumTrack track;
  scale_t *currentScale;

  uint8_t pitches[32];
  uint8_t pitches_len;
  uint8_t pitches_idx;

  uint8_t mdTrack;

  uint8_t octaves;
  uint8_t basePitch;

  bool muted;

  MDPitchEuclid() : track(3, 8, 0) {
    currentScale = scales[0];
    octaves = 0;
    muted = false;

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
	if (!muted)
	  MD.sendNoteOn(mdTrack, pitch, 100);
      }
      pitches_idx = (pitches_idx + 1) % pitches_len;
    }
  }
};

MDPitchEuclid pitchEuclid;

class MDPitchEuclidConfigPage1 : 
public EncoderPage {
public:
  RangeEncoder pitchLengthEncoder;
  RangeEncoder pulseEncoder;
  RangeEncoder lengthEncoder;
  RangeEncoder offsetEncoder;

  MDPitchEuclidConfigPage1() :
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

class MDPitchEuclidConfigPage2 : 
public EncoderPage {
public:
  MDMelodicTrackFlashEncoder trackEncoder;
  RangeEncoder scaleEncoder;
  RangeEncoder octavesEncoder;
  NotePitchEncoder basePitchEncoder;

  MDPitchEuclidConfigPage2() :
  trackEncoder("TRK", 0),
  scaleEncoder(0, NUM_SCALES - 1, "SCL", 0),
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
    if (trackEncoder.hasChanged()) {
      pitchEuclid.mdTrack = trackEncoder.getValue();
    }
  }
};

class MDPitchEuclidSketch : 
public Sketch, public MDCallback, public ClockCallback {
public:
  MDPitchEuclidConfigPage1 page1;
  MDPitchEuclidConfigPage2 page2;

  MDPitchEuclidSketch() {
  }

    void getName(char *n1, char *n2) {
    m_strncpy_p(n1, PSTR("EUC "), 5);
    m_strncpy_p(n2, PSTR("LID "), 5);
  }



  void setup() {
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&MDPitchEuclidSketch::onKitChanged);
    //    MDTask.addOnGlobalChangeCallback(_onGlobalChanged);

    MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&MDPitchEuclidSketch::on16Callback);
  }

  virtual void show() {
    if (currentPage() == NULL)
      setPage(&page1);
  }

  virtual void doExtra(bool pressed) {
    if (pressed) {
      pitchEuclid.randomizePitches();
    }
  }

  virtual void mute(bool pressed) {
    if (pressed) {
      pitchEuclid.muted = !pitchEuclid.muted;
      if (pitchEuclid.muted) {
	GUI.flash_strings_fill("EUCLID", "MUTED");
      } else {
	GUI.flash_strings_fill("EUCLID", "UNMUTED");
      }
    }
  }

  virtual Page *getPage(uint8_t i) {
    if (i == 0) {
      return &page1;
    } else if (i == 1) {
      return &page2;
    } else {
      return NULL;
    }
  }

  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      GUI.setPage(&page1);
      return true;
    } 
    else if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      GUI.setPage(&page2);
      return true;
    } 
    else if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
      pitchEuclid.randomizePitches();
    }
  }

  void onKitChanged() {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MD.kit.name);
  }  

  void on16Callback() {
    pitchEuclid.on16Callback();
  }


};


#endif /* MDPITCHEUCLIDSKETCH_H__ */
