RangeEncoder trackEncoder(0, 15, "TRK");
RangeEncoder speedEncoder(1, 16, "SPD");
RangeEncoder octavesEncoder(0, 5, "OCT");
RangeEncoder lenEncoder(0, 16, "LEN");
  
class ConfigPage_1 : public EncoderPage {
  public:
  ConfigPage_1() {
    encoders[0] = &trackEncoder;
    encoders[1] = &speedEncoder;
    encoders[2] = &octavesEncoder;
    encoders[3] = &lenEncoder;
    trackEncoder.setValue(arpeggiator.arpTrack);
    speedEncoder.setValue(arpeggiator.arpSpeed);
  }  
  
  virtual void display(bool redisplay = false) {
    EncoderPage::display(redisplay);
    if (redisplay || lenEncoder.hasChanged()) {
      if (lenEncoder.getValue() == 0)
        GUI.put_p_string_at(12, PSTR("INF"));
    }
    if (lenEncoder.hasChanged()) {
      arpeggiator.arpTimes = lenEncoder.getValue();
    }
    if (speedEncoder.hasChanged()) {
      arpeggiator.arpSpeed = speedEncoder.getValue();
    }
    if (octavesEncoder.hasChanged()) {
      arpeggiator.arpOctaves = octavesEncoder.getValue();
      arpeggiator.arpOctaveCount = 0;
    }
    if (trackEncoder.hasChanged()) {
      uint8_t track = trackEncoder.getValue();
      GUI.setLine(GUI.LINE2);
      GUI.clearFlash();
      GUI.flash_put_value(0, track);
      if (MD.isMelodicTrack(track)) {
        arpeggiator.arpTrack = track;
        GUI.flash_p_string_at_fill(8, MD.getMachineName(MD.kit.machines[track].model));
      } else {
        GUI.flash_p_string_at_fill(8, PSTR("XXX"));
      }
    }
  }
};

RangeEncoder styleEncoder(0, ARP_STYLE_CNT - 1, "STY");
RangeEncoder retrigEncoder(0, RETRIG_CNT - 1, "TRG");
RangeEncoder retrigSpeedEncoder(1, 32, "SPD");

class ConfigPage_2 : public EncoderPage {
  public:
  
  ConfigPage_2() {
    encoders[0] = &styleEncoder;
    encoders[2] = &retrigEncoder;
    encoders[3] = &retrigSpeedEncoder;
    styleEncoder.setValue(0);
    arpeggiator.arpStyle = (arp_style_t)styleEncoder.getValue();
    retrigEncoder.setValue(0);
    arpeggiator.arpRetrig = (arp_retrig_type_t)retrigEncoder.getValue();
    arpeggiator.calculateArp();
  }
  
  virtual void display(bool redisplay = false) {
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("STYLE   TRG  SPD "));
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
    if (arpeggiator.arpRetrig == RETRIG_BEAT) {
      GUI.put_value_at(13, retrigSpeedEncoder.getValue());
    }
  }
  
  virtual void handle() {
    bool changed = false;
    if (styleEncoder.hasChanged()) {
      arpeggiator.arpStyle = (arp_style_t)styleEncoder.getValue();
      displayStyle();
      changed = true;
    }
    styleEncoder.checkHandle();
    if (retrigEncoder.hasChanged()) {
      arpeggiator.arpRetrig = (arp_retrig_type_t)retrigEncoder.getValue();
      displayRetrig();
      changed = true;
    }
    retrigEncoder.checkHandle();
    if (changed) {
      arpeggiator.calculateArp();
    }
    if (retrigSpeedEncoder.hasChanged()) {
      arpeggiator.retrigSpeed = retrigSpeedEncoder.getValue();
      displayRetrig();
    }
    retrigSpeedEncoder.checkHandle();
  }
};

ConfigPage_1 configPage_1;
ConfigPage_2 configPage_2;

void loopPages() {
}

void setupPages() {
  GUI.setPage(&configPage_1);
}

void onCurrentKitCallback() {
  GUI.flash_strings_fill("LOADED KIT", MD.kit.name);
  GUI.setLine(GUI.LINE1);
  loadedKit = true;
}

void handleGuiPages() {
  if (GUI.page == &configPage_1) {
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      loadedKit = false;
      MDSysexListener.getCurrentKit(onCurrentKitCallback);
    } else if (BUTTON_PRESSED(Buttons.BUTTON2)) {
      GUI.setPage(&configPage_2);
    }
  } else if (GUI.page == &configPage_2) {
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      GUI.setPage(&configPage_1);
    }
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON3)) {
    triggerRecording = true;
    recording = false;
    endRecording = false;
  }
}
