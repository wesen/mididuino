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
    }
  }
}
