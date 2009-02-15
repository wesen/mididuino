RangeEncoder trackEncoder(0, 15, "TRK");
RangeEncoder speedEncoder(1, 16, "SPD");
RangeEncoder octavesEncoder(0, 5, "OCT");
  
class ConfigPage_1 : public EncoderPage {
  public:
  ConfigPage_1() {
    encoders[0] = &trackEncoder;
    encoders[2] = &speedEncoder;
    encoders[3] = &octavesEncoder;
  }  
};

class ConfigPage_2 : public Page {
};

ConfigPage_1 configPage_1;

void on16Callback() {
  speedEncoder.getValue();
}

void loopPages() {
  if (trackEncoder.hasChanged()) {
    GUI.setLine(GUI.LINE2);
    GUI.flash_p_string_fill(getMachineName(MD.trackModels[trackEncoder.getValue()]));
  }
}

void setupPages() {
  GUI.setPage(&configPage_1);
}

void onCurrentKitCallback() {
  GUI.flash_strings_fill("LOADED KIT", MD.name);
  loadedKit = true;
}

void handleGuiPages() {
  if (GUI.page == &configPage_1) {
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      loadedKit = false;
      MDSysex.getCurrentKit(onCurrentKitCallback);
    }
  }
}
