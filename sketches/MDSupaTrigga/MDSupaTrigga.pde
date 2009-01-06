void on16Callback() {
  uint8_t val = MidiClock.div16th_counter % 32;

  GUI.setLine(GUI.LINE2);
  GUI.put_value(0, val);
  
  if ((val % 8) == 0) {
    if (random(100) > 30) {
      uint8_t from = 0, to = 0;
      if (random(100) > 50) {
        from = random(0, 6);
        to = random(from + 2, 8);
      } else {
        from = random(2, 8);
        to = random(0, from - 2);
      }
      MD::sliceTrack32(15, from * 4, to * 4);
    } else {
      MD::sliceTrack32(15, val, val + 8);
    }
  }
}

void setup() {
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
}

void loop() {
}

void handleGui() {
}
