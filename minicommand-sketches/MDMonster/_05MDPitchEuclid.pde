namespace MDPitchEuclid {
RangeEncoder pitchLengthEncoder(1, 32, "PTC");
RangeEncoder pulseEncoder(1, 32, "PLS");
RangeEncoder lengthEncoder(2, 32, "LEN");
RangeEncoder offsetEncoder(0, 32, "OFF");
RangeEncoder trackEncoder(0, 15, "TRK");

EuclidDrumTrack track(3, 8);
EncoderPage page;
EncoderPage trackPage;
uint8_t arpPitches[] = { 0, 3, 7, 10, 12, 15, 19, 22 };

uint8_t pitches[32];
uint8_t pitches_len = 0;
uint8_t pitches_idx = 0;

void setPitchLength(uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    pitches[i] = 60 + arpPitches[random(8)];
  }
  pitches_len = len;
}

void on16Callback() {
  if (track.isHit(MidiClock.div16th_counter)) {
    /*
    uint8_t decay = random(40, 120);
    MD.setTrackParam(trackEncoder.getValue(), 1, decay);
    if (decay < 80) {
      MD.setTrackParam(trackEncoder.getValue(), 15, 100);
    } else {
      MD.setTrackParam(trackEncoder.getValue(), 15, 0);
    }
    */
    MD.sendNoteOn(trackEncoder.getValue(), pitches[pitches_idx], random(40, 110));
    pitches_idx = (pitches_idx + 1) % pitches_len;
  }
}

void onCurrentKitCallback() {
  GUI.setLine(GUI.LINE1);
  GUI.flash_string_fill(MD.name);
}

class MDPitchEuclidSketch : public SubSketch {
  public:
void setup() {
  page.encoders[0] = &pitchLengthEncoder;
  page.encoders[1] = &pulseEncoder;
  page.encoders[2] = &lengthEncoder;
  page.encoders[3] = &offsetEncoder;
  setPitchLength(4);
  
  trackPage.encoders[0] = &trackEncoder;
  trackEncoder.setValue(0);
  
  pulseEncoder.setValue(3);
  lengthEncoder.setValue(8);
  offsetEncoder.setValue(0);
  pitchLengthEncoder.setValue(4);

  GUI.setPage(&page);
  
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
}

void loop() {
  GUI.updatePage();

  if (pulseEncoder.hasChanged() || lengthEncoder.hasChanged() || offsetEncoder.hasChanged()) {
    track.setEuclid(pulseEncoder.getValue(),
		    lengthEncoder.getValue(),
		    offsetEncoder.getValue());
  }
  if (pitchLengthEncoder.hasChanged()) {
    setPitchLength(pitchLengthEncoder.getValue());
  }
  if (trackEncoder.hasChanged() && GUI.page == &trackPage) {
    GUI.setLine(GUI.LINE2);
    uint8_t track = trackEncoder.getValue();
    if (MD.isMelodicTrack(track)) {
      GUI.put_p_string_at_fill(5, getMachineName(MD.trackModels[track]));
    } else {
      GUI.put_p_string_at_fill(5, PSTR("XXX"));
    }
  }
  
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(0)) {
    setPitchLength(pitchLengthEncoder.getValue());
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    GUI.setPage(&trackPage);
  } else if (BUTTON_RELEASED(Buttons.BUTTON2)) {
    GUI.setPage(&page);
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    MDSysex.getCurrentKit(onCurrentKitCallback);
  }
}

void destroy() {
  MidiClock.setOn16Callback(NULL);
  MidiClock.stop();
}

PGM_P getName() {
  return PSTR("MD PITCH EUCLID");
}
};

MDPitchEuclidSketch sketch;
}
