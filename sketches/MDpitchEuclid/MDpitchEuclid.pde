RangeEncoder pitchLengthEncoder(1, 32);
RangeEncoder pulseEncoder(1, 32);
RangeEncoder lengthEncoder(2, 32);
RangeEncoder offsetEncoder(0, 32);

#define ROM_TRACK 4

EuclidDrumTrack track(3, 8);
EncoderPage page;
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

void handleGui() {
  if (BUTTON_PRESSED(0)) {
    setPitchLength(pitchLengthEncoder.getValue());
  }

  page.update();
}


void on16Callback() {
  if (track.isHit(MidiClock.div16th_counter)) {
    MD::sendNoteOn(ROM_TRACK, pitches[pitches_idx], 100);
    pitches_idx = (pitches_idx + 1) % pitches_len;
  }
}

void setup() {
  MD::trackModels[ROM_TRACK] = ROM_MODEL;
  
  page.encoders[0] = &pitchLengthEncoder;
  page.encoders[1] = &pulseEncoder;
  page.encoders[2] = &lengthEncoder;
  page.encoders[3] = &offsetEncoder;
  
  setPitchLength(4);
  
  MidiClock.mode = MidiClock.EXTERNAL;
  pulseEncoder.setValue(3);
  lengthEncoder.setValue(8);
  offsetEncoder.setValue(0);
  pitchLengthEncoder.setValue(4);
  
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();

  GUI.setLine(GUI.LINE1);
  page.display(true);
}

void loop() {
  cli();
  if (pulseEncoder.hasChanged() || lengthEncoder.hasChanged() || offsetEncoder.hasChanged()) {
    track.setEuclid(pulseEncoder.getValue(), lengthEncoder.getValue(), offsetEncoder.getValue());
  }
  if (pitchLengthEncoder.hasChanged()) {
    setPitchLength(pitchLengthEncoder.getValue());
  }
  GUI.setLine(GUI.LINE1);
  page.display();

  page.handle();
  sei();
}
