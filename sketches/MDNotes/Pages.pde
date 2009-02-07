class ChannelConfigPage : public Page {
    RangeEncoder configEncoder;
  public:
    uint8_t configChannel;
    ChannelConfigPage() : configEncoder(0, 3, "TYP") {
      configChannel = 128;
    }

    virtual void handle() {
      if (configEncoder.hasChanged() && configChannel != 128) {
        if (configEncoder.getValue() != inputChannels[configChannel].type) {
          inputChannels[configChannel].type = configEncoder.getValue();
          display(true);
        }
      }
    }
     
    virtual void display(bool redisplay = false) {
      if (redisplay) {
        GUI.setLine(GUI.LINE1);
        GUI.put_p_string(0, PSTR("CFG CHN"));
        
        if (configChannel == 128) {
          GUI.setLine(GUI.LINE2);
          GUI.put_p_string(0, PSTR("PRESS A TRACK"));
        } else {
          GUI.put_value(2, configChannel + 1);
          switch (inputChannels[configChannel].type) {
            case InputChannel::NORMAL_CHANNEL_TYPE:
               GUI.put_p_string(3, PSTR("NML"));
               break;
               
            case InputChannel::POLY_CHANNEL_TYPE:
              GUI.put_p_string(3, PSTR("PLY"));
              break;
           
           case InputChannel::MULTI_CHANNEL_TYPE:
             GUI.put_p_string(3, PSTR("MLT"));
             break;
           }
           
           GUI.setLine(GUI.LINE2);
           if (inputChannels[configChannel].type != InputChannel::NORMAL_CHANNEL_TYPE) {
             char mask[17];
             uint8_t i;
             for (i = 0; i < 16; i++) {
               if (IS_BIT_SET(inputChannels[configChannel].trackmask, i))
                 mask[i] = 'X';
	       else
	         mask[i] = '.';
             }
             mask[16] = 0;
             GUI.put_string(0, mask);
           } else {
             GUI.clearLine();
           }
        }
      }

    } 
};

ChannelConfigPage configPage;

class MDNotesPage : public Page {
  public:
  MDNotesPage() {
  }
};

MDNotesPage normalPage;

void setupPages() {
  GUI.setPage(&normalPage);
}

void handleGuiPages() {
  if (MD.currentKit != -1) {
    if (BUTTON_PRESSED(Buttons.BUTTON2)) {
      if (GUI.page == &configPage) {
        GUI.setPage(&normalPage);
      } else {
        GUI.setPage(&configPage);
      }
    }
  }
}

void handleConfigChannelSelectNote(uint8_t *msg) {
}

void handleConfigChannelNote(uint8_t *msg) {
}
