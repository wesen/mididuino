void setupMidi() {
  Midi.setOnControlChangeCallback(onControlChangeCallback);
  Midi.setOnNoteOnCallback(onNoteOnCallbackMD);
  Midi2.setOnNoteOnCallback(onNoteOnCallbackKeyboard);
}

void onControlChangeCallback(uint8_t *msg) {
}

void handleNoteOn(uint8_t *msg) {
  InputChannel *inputChannel = &inputChannels[MIDI_VOICE_CHANNEL(msg[0])];
  switch (inputChannel->type) {
    case InputChannel::NORMAL_CHANNEL_TYPE:
    break;
    
    case InputChannel::POLY_CHANNEL_TYPE: {
    }
    break;
    
    case InputChannel::MULTI_CHANNEL_TYPE: {
    }
    break;
  }
}

void onNoteOnCallbackMD(uint8_t *msg) {
  if (msg[2] == 0)
    return;
  switch (mdNotesStatus) {
    case MD_NOTES_STATUS_CONFIG_CHANNEL_WAIT_NOTE:
      if (MIDI_VOICE_CHANNEL(msg[0]) == MD.baseChannel)
        handleConfigChannelSelectNote(msg);
      break;
    
    case MD_NOTES_STATUS_CONFIG_CHANNEL:
      if (MIDI_VOICE_CHANNEL(msg[0]) == MD.baseChannel)
        handleConfigChannelNote(msg);
      break;
      
    default:
      break;
  }
}

void onNoteOnCallbackKeyboard(uint8_t *msg) {
  switch (mdNotesStatus) {
    case MD_NOTES_STATUS_NORMAL:
      handleNoteOn(msg);
      break;

    default:
      break;    
  }
}

