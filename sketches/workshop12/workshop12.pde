void on16Callback() {
  if (MidiClock.div16th_counter % 4 == 0) {
    OCR3A = 255;
//    setLed();
  } else {
    OCR3A = 80;
//    clearLed();
  }
  if (MidiClock.div16th_counter % 4 == 2) {
//    setLed2();
  } else {
//    clearLed2();
  }
  if (MidiClock.div16th_counter % 32 == 0) {
    GUI.setLine(GUI.LINE1);
    GUI.flash_string_fill("YEAH");
  }
  if (MidiClock.div16th_counter % 32 == 16) {
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill("WOW" );
  }

}

void onNoteOnCallback(uint8_t *msg) {
  if (msg[2] == 0) {
    return onNoteOffCallback(msg);
  }
  
  if (msg[1] == 72) {
    setLed();
  }
  
   if (msg[1] == 74) {
    setLed2();
  }

}

void onNoteOffCallback(uint8_t *msg) {
  if (msg[1] == 72) {
    clearLed();
  }
   if (msg[1] == 74) {
    clearLed2();
  }
}

void loop() {
  GUI.update();
}

void setup() {
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
  
  Midi.setOnNoteOnCallback(onNoteOnCallback);
  Midi.setOnNoteOffCallback(onNoteOffCallback);
}

void handleGui() {
}
