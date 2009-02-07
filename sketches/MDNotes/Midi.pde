void setupMidi() {
  Midi.setOnControlChangeCallback(onControlChangeCallback);
  Midi2.setOnNoteOnCallback(onNoteOnCallback);
}

void onControlChangeCallback(uint8_t *msg) {
}

void handleNoteOn(uint8_t *msg) {
}

void onNoteOnCallback(uint8_t *msg) {
  switch (mdNotesStatus) {
    case MD_NOTES_STATUS_NORMAL:
      handleNoteOn(msg);
      break;
    
    case MD_NOTES_STATUS_CONFIG_CHANNEL_WAIT_NOTE:
      if (MIDI_VOICE_CHANNEL(msg[0]) == MD.baseChannel)
        handleConfigChannelSelectNote(msg);
      break;
    
    case MD_NOTES_STATUS_CONFIG_CHANNEL:
      if (MIDI_VOICE_CHANNEL(msg[0]) == MD.baseChannel)
        handleConfigChannelNote(msg);
      break;
  }
}

