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
    if ((MD.kit.origPosition != oldKit || redisplay) && MD.loadedKit == true ) {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("LOADED KIT"));
      GUI.setLine(GUI.LINE2);
      GUI.put_string_fill(MD.kit.name);
      oldKit = MD.kit.origPosition;
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
  if (channel == MD.global.baseChannel) {
    uint8_t track = MD.noteToTrack(msg[1]);
    if (track != 128) {
      configPage.setConfigChannel(track);
      mdNotesStatus = MD_NOTES_STATUS_CONFIG_CHANNEL;
    }
  }
}

void handleConfigChannelNote(uint8_t *msg) {
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  if (channel == MD.global.baseChannel) {
    uint8_t track = MD.noteToTrack(msg[1]);
    if (track != 128) {
      configPage.toggleTrack(track);
    }
  }
}

bool isConfigPageActive() {
  return GUI.page == &configPage;
}
