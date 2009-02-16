class ConfigPage_1 : public EncoderPage {
  public:
  ConfigPage_1() {
    encoders[0] = &trackEncoder;
    encoders[2] = &speedEncoder;
    encoders[3] = &octavesEncoder;
  }  
};

RangeEncoder styleEncoder(0, ARP_STYLE_CNT - 1, "STY");
RangeEncoder retrigEncoder(0, RETRIG_CNT - 1, "TRG");

class ConfigPage_2 : public EncoderPage {
  public:
  
  ConfigPage_2() {
    encoders[0] = &styleEncoder;
    encoders[3] = &retrigEncoder;
    styleEncoder.setValue(0);
    arpStyle = (arp_style_t)styleEncoder.getValue();
    retrigEncoder.setValue(0);
    arpRetrig = (arp_retrig_type_t)retrigEncoder.getValue();
    calculateArp();
  }
  
  virtual void display(bool redisplay = false) {
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("STYLE    RETRIG"));
      displayStyle();
      displayRetrig();
    }
  }
  
  void displayStyle() {
    GUI.setLine(GUI.LINE2);
    GUI.put_p_string_at(0, arp_names[styleEncoder.getValue()]);
  }
  
  void displayRetrig() {
    GUI.setLine(GUI.LINE2);
    GUI.put_p_string_at(8, retrig_names[retrigEncoder.getValue()]);
  }
  
  virtual void handle() {
    bool changed = false;
    if (styleEncoder.hasChanged()) {
      arpStyle = (arp_style_t)styleEncoder.getValue();
      displayStyle();
      changed = true;
    }
    styleEncoder.checkHandle();
    if (retrigEncoder.hasChanged()) {
      arpRetrig = (arp_retrig_type_t)retrigEncoder.getValue();
      displayRetrig();
      changed = true;
    }
    retrigEncoder.checkHandle();
    if (changed) {
      calculateArp();
    }
  }
};

ConfigPage_1 configPage_1;
ConfigPage_2 configPage_2;

void loopPages() {
  if (trackEncoder.hasChanged()) {
    GUI.setLine(GUI.LINE2);
    GUI.clearFlash();
    GUI.flash_put_value(0, trackEncoder.getValue());
    GUI.flash_p_string_at_fill(8, getMachineName(MD.trackModels[trackEncoder.getValue()]));
  }
}

void setupPages() {
  GUI.setPage(&configPage_1);
}

void onCurrentKitCallback() {
  GUI.flash_strings_fill("LOADED KIT", MD.name);
  GUI.setLine(GUI.LINE1);
  loadedKit = true;
}

void handleGuiPages() {
  if (GUI.page == &configPage_1) {
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      loadedKit = false;
      MDSysex.getCurrentKit(onCurrentKitCallback);
    } else if (BUTTON_PRESSED(Buttons.BUTTON2)) {
      GUI.setPage(&configPage_2);
    }
  } else if (GUI.page == &configPage_2) {
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      GUI.setPage(&configPage_1);
    }
  }
}
