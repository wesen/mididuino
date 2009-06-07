namespace MDNotes {
  
enum {
  MD_NOTES_STATUS_NORMAL = 0,
  MD_NOTES_STATUS_CONFIG_CHANNEL_WAIT_NOTE,
  MD_NOTES_STATUS_CONFIG_CHANNEL
} mdNotesStatus = MD_NOTES_STATUS_NORMAL;

bool loadedKit = false;

class InputChannel {
  public:
    static const uint8_t NORMAL_CHANNEL_TYPE = 0;
    static const uint8_t POLY_CHANNEL_TYPE   = 1;
    static const uint8_t MULTI_CHANNEL_TYPE  = 2;
    
    uint8_t type;
    uint16_t trackmask;
    uint8_t voices;
    uint8_t last_voice;
    
    InputChannel() {
      type = NORMAL_CHANNEL_TYPE;
      trackmask = 0;
      voices = 0;
      last_voice = 0;
    }
    
    uint8_t getVoices() {
      uint8_t result = 0;
      for (uint8_t i = 0; i < 16; i++) {
        if (IS_BIT_SET(trackmask, i))
          result++;
      }
      return result;
    }
    
    uint8_t getVoiceTrack(uint8_t voice) {
      uint8_t cnt = 0;
      for (uint8_t i = 0; i < 16; i++) {
        if (IS_BIT_SET(trackmask, i)) {
          if (cnt++ == voice)
            return i;
        }
      }
    }
};

InputChannel inputChannels[16];

void onCurrentKitCallback() {
  loadedKit = true;
}

bool isConfigPageActive();

void handleGuiSysex() {
  if (BUTTON_PRESSED(Buttons.BUTTON1) && !isConfigPageActive()) {
    loadedKit = false;
    MDSysexListener.getCurrentKit(onCurrentKitCallback);
  }
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

void handleConfigChannelSelectNote(uint8_t *msg);
void handleConfigChannelNote(uint8_t *msg);

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

void setupMidi() {
  Midi.setOnControlChangeCallback(onControlChangeCallback);
  Midi.setOnNoteOnCallback(onNoteOnCallbackMD);
  Midi2.setOnNoteOnCallback(onNoteOnCallbackKeyboard);
}


class ChannelConfigPage : public Page {
    RangeEncoder configEncoder;
  public:
    uint8_t configChannel;
    uint8_t oldConfigChannel;
    
    ChannelConfigPage() : configEncoder(0, 2, "TYP") {
      configChannel = 128;
      oldConfigChannel = 255;
    }
  
    void setConfigChannel(uint8_t _configChannel) {
      configChannel = _configChannel;
      configEncoder.setValue(inputChannels[configChannel].type);
      display(true);
    }
    
    virtual void update() {
      uint8_t tmp = SREG;
      cli();
      configEncoder.update(&Encoders.encoders[0]);
      Encoders.clearEncoders();
      SREG = tmp;
    }

    virtual void handle() {
      if (configEncoder.hasChanged() && configChannel != 128) {
        uint8_t value = configEncoder.getValue();
        inputChannels[configChannel].type = value;
        display(true);
      }
      configEncoder.checkHandle();
    }
     
    virtual void display(bool redisplay = false) {
      if (redisplay) {
        GUI.setLine(GUI.LINE1);
        GUI.put_p_string_fill(PSTR("CFG CHN"));
      }
      
      if (redisplay || oldConfigChannel != configChannel) {
        
        if (configChannel == 128) {
          GUI.setLine(GUI.LINE1);
          GUI.put_p_string_fill(PSTR("CONFIG CHANNEL"));
          GUI.setLine(GUI.LINE2);
          GUI.put_p_string_fill(PSTR("SELECT WITH PADS"));
        } else {
          GUI.setLine(GUI.LINE1);
          uint8_t model = MD.kit.machines[configChannel].model;
          GUI.put_p_string(PSTR("CHANNEL "));
          GUI.put_value(2, configChannel + 1);
          GUI.lines[GUI.curLine].data[11] = ':';
          switch (inputChannels[configChannel].type) {
            case InputChannel::NORMAL_CHANNEL_TYPE:
               GUI.put_p_string(3, PSTR(" NML"));
               GUI.setLine(GUI.LINE2);
               GUI.clearLine();
               GUI.put_p_string_at(0, MD.getMachineName(model));
               if (MD.isMelodicTrack(configChannel)) {
               } else {
                 GUI.put_p_string_at(7, PSTR("XXX"));
               }
               break;
               
            case InputChannel::POLY_CHANNEL_TYPE:
              GUI.put_p_string(3, PSTR(" PLY"));
              displayTracks();
              break;
           
           case InputChannel::MULTI_CHANNEL_TYPE:
             GUI.put_p_string(3, PSTR(" MLT"));
             displayTracks();           
             break;
             
           default:
             break;
          }
        }
        oldConfigChannel = configChannel;
      }

    } 

    char mask[17];
    
    void displayTracks() {
      GUI.setLine(GUI.LINE2);
      if (inputChannels[configChannel].type != InputChannel::NORMAL_CHANNEL_TYPE) {
        uint8_t i;
        for (i = 0; i < 16; i++) {
          if (MD.isMelodicTrack(i)) {
            if (IS_BIT_SET(inputChannels[configChannel].trackmask, i))
              mask[i] = 'X';
    	    else
  	      mask[i] = '.';
          } else {
            mask[i] = '_';
          }
        }
        mask[16] = 0;
        GUI.put_string_fill(mask);
      }
    }
    
    void toggleTrack(uint8_t track) {
      if (MD.isMelodicTrack(track)) {
        TOGGLE_BIT(inputChannels[configChannel].trackmask, track);
        inputChannels[configChannel].voices = inputChannels[configChannel].getVoices();
        displayTracks();
      }
    }
};

ChannelConfigPage configPage;

class MDNotesPage : public Page {
  int oldKit;
  public:
  MDNotesPage() {
    oldKit = -1;
  }
  
  virtual void display(bool redisplay = false) {
    if (redisplay && (loadedKit == false || MD.currentKit == -1) ) {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("RELOAD KIT"));
    }

    if ((MD.currentKit != oldKit || redisplay) && loadedKit == true) {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("LOADED KIT"));
      GUI.setLine(GUI.LINE2);
      GUI.put_string_fill(MD.kit.name);
      oldKit = MD.currentKit;
    }
  }
};

MDNotesPage normalPage;

void setupPages() {
  GUI.setPage(&normalPage);
}

void handleGuiPages() {
  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    if (MD.currentKit != -1) {
      if (GUI.page == &configPage) {
        mdNotesStatus = MD_NOTES_STATUS_NORMAL;
        GUI.setPage(&normalPage);
      }
    }
  } 
  if (BUTTON_PRESSED(Buttons.BUTTON3)) {
    if (MD.currentKit != -1) {
      mdNotesStatus = MD_NOTES_STATUS_CONFIG_CHANNEL_WAIT_NOTE;
      configPage.configChannel = 128;
      GUI.setPage(&configPage);
    }
  }
}

void handleConfigChannelSelectNote(uint8_t *msg) {
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  if (channel == MD.baseChannel) {
    uint8_t track = MD.noteToTrack(msg[1]);
    if (track != 128) {
      configPage.setConfigChannel(track);
      mdNotesStatus = MD_NOTES_STATUS_CONFIG_CHANNEL;
    }
  }
}

void handleConfigChannelNote(uint8_t *msg) {
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  if (channel == MD.baseChannel) {
    uint8_t track = MD.noteToTrack(msg[1]);
    if (track != 128) {
      configPage.toggleTrack(track);
    }
  }
}

bool isConfigPageActive() {
  return GUI.page == &configPage;
}

class MDNotesSketch : public SubSketch {
public:
void setup() {
  setupMidi();
  setupPages();
}

void loop() {
  GUI.updatePage();
  
//  if (!MidiUart.avail())
    GUI.update();
}

void handleGui() {
  handleGuiSysex();
  handleGuiPages();
}

void destroy() {
  Midi.setOnControlChangeCallback(NULL);
  Midi.setOnNoteOnCallback(NULL);
  Midi2.setOnNoteOnCallback(NULL);
}

PGM_P getName() {
  return PSTR("MD NOTES");
}


};

MDNotesSketch sketch;

}
