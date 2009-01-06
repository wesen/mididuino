TempoEncoder tempoEncoder;
RangeEncoder pitchLengthEncoder(1, 32);
RangeEncoder pulseEncoder(1, 32);
RangeEncoder lengthEncoder(2, 32);
RangeEncoder offsetEncoder(0, 32);

EuclidDrumTrack track(3, 8);
PitchTrack pitchTrack(&track, 4);

EncoderPage page;
uint8_t arpPitches[] = { 0, 3, 7, 10, 12, 15, 19, 22 };

void setPitchLength(uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    pitchTrack.pitches[i] = 60 + arpPitches[random(8)];
  }
  pitchTrack.len = len;
}

void handleGui() {
  if (BUTTON_PRESSED(4))  
    MidiClock.pause();
    
  if (BUTTON_PRESSED(0)) {
    setPitchLength(pitchLengthEncoder.getValue());
  }

  page.update();
}


void on16Callback() {
  pitchTrack.playHit(MidiClock.div16th_counter);
}

void setup() {
  page.encoders[0] = &pitchLengthEncoder;
  page.encoders[1] = &pulseEncoder;
  page.encoders[2] = &lengthEncoder;
  page.encoders[3] = &offsetEncoder;
  page.encoders[4] = &tempoEncoder;
  
  setPitchLength(4);
  
  MidiClock.mode = MidiClock.INTERNAL;
  MidiClock.setTempo(100);
  tempoEncoder.setValue(MidiClock.tempo);
  pulseEncoder.setValue(3);
  lengthEncoder.setValue(8);
  offsetEncoder.setValue(0);
  pitchLengthEncoder.setValue(4);
  
  MidiClock.transmit = true;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
}

void loop() {
  if (pulseEncoder.hasChanged() || lengthEncoder.hasChanged() || offsetEncoder.hasChanged()) {
    track.setEuclid(pulseEncoder.getCurValue(), lengthEncoder.getCurValue(), offsetEncoder.getCurValue());
  }
  if (pitchLengthEncoder.hasChanged()) {
    setPitchLength(pitchLengthEncoder.getValue());
  }
  if (tempoEncoder.hasChanged()) {
    GUI.setLine(GUI.LINE1);
    GUI.put_value(0, tempoEncoder.getValue());
  }

  page.handle();

  GUI.setLine(GUI.LINE2);
  page.display();
}
