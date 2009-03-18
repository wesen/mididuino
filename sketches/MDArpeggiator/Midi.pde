void on16Callback() {
  if (triggerRecording && (MidiClock.div16th_counter % 16) == 0) {
    triggerRecording = false;
    recording = true;
    recordStart = MidiClock.div16th_counter;
    for (int i = 0; i < 64; i++) {
      arpeggiator.recordPitches[i] = 128;
    }
  }
  
  if (recording || endRecording) {
    int pos = MidiClock.div16th_counter - recordStart;
    if (pos >= (recordLength * 3)) {
      endRecording = false;
    } else if (pos >= (recordLength * 2)) {
      recording = false;
      endRecording = true;
      return;
    }
  }
  
  if (!triggerRecording) {
    arpeggiator.playNext(recording);
  }
}

void setupMidi() {
  Midi.setOnNoteOnCallback(onNoteOnCallbackMD);
  Midi2.setOnNoteOnCallback(onNoteOnCallbackKeyboard);
  Midi2.setOnNoteOffCallback(onNoteOffCallbackKeyboard);
  
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
}

void handleNoteOn(uint8_t *msg) {
  arpeggiator.addNote(msg[1], msg[2]);
}

void handleNoteOff(uint8_t *msg) {
  arpeggiator.removeNote(msg[1]);
}

void onNoteOnCallbackMD(uint8_t *msg) {
}

void onNoteOffCallbackKeyboard(uint8_t *msg) {
  handleNoteOff(msg);
}

void onNoteOnCallbackKeyboard(uint8_t *msg) {
  if (msg[2] != 0) {
    handleNoteOn(msg);
  } else {
    handleNoteOff(msg);
  }
}
