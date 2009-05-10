void setupMidi() {
  Midi.setOnControlChangeCallback(onControlChangeCallback);
  Midi.setOnNoteOnCallback(onNoteOnCallbackMD);
  Midi2.setOnNoteOnCallback(onNoteOnCallbackKeyboard);
}

void onControlChangeCallback(uint8_t *msg) {
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  if (!BUTTON_DOWN(Buttons.BUTTON4)) {
    uint8_t track, param;
    MD.parseCC(channel, msg[1], &track, &param);
    if (track != 255) {
      for (uint8_t chan = 0; chan < 16; chan++) {
        InputChannel *inputChannel = &inputChannels[chan];
        switch (inputChannel->type) {
          case InputChannel::NORMAL_CHANNEL_TYPE:
            break;
          
          case InputChannel::MULTI_CHANNEL_TYPE:
            break;
            
          case InputChannel::POLY_CHANNEL_TYPE: {
            if (IS_BIT_SET(inputChannel->trackmask, track)) {
              for (uint8_t i = 0; i < 16; i++) {
                if (IS_BIT_SET(inputChannel->trackmask, i) && i != track) {
                  MD.setTrackParam(i, param, msg[2]);
                }
              }
            }
          }
          break;
        }
      }
    }
  }
}

void handleNoteOn(uint8_t *msg) {
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  InputChannel *inputChannel = &inputChannels[channel];
  switch (inputChannel->type) {
    case InputChannel::NORMAL_CHANNEL_TYPE:
      if (MD.isMelodicTrack(channel)) {
        MD.sendNoteOn(channel, msg[1], msg[2]);
      }
    break;
    
    case InputChannel::POLY_CHANNEL_TYPE: {
      uint8_t voice = (inputChannel->last_voice + 1) % inputChannel->voices;
      uint8_t track = inputChannel->getVoiceTrack(voice);
      if (track != 255) {
        MD.sendNoteOn(track, msg[1], msg[2]);
      }
      inputChannel->last_voice = voice;
    }
    break;
    
    case InputChannel::MULTI_CHANNEL_TYPE: {
      for (uint8_t i = 0; i < 16; i++) {
        if (IS_BIT_SET(inputChannel->trackmask, i)) {
          MD.sendNoteOn(i, msg[1], msg[2]);
        }
      }
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
  if (msg[2] != 0) {
    setLed();
    handleNoteOn(msg);
    clearLed();
  }
}

