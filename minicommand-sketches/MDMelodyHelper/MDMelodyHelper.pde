#include <MD.h>

bool loadedKit = false;

void setup() {
  setupMidi();
  setupPages();
  GUI.setLine(GUI.LINE1);
  GUI.put_p_string_fill(PSTR("RELOAD KIT"));
}

void onCurrentKitCallback() {
    loadedKit = true;
    GUI.setLine(GUI.LINE1);
    GUI.put_string_fill(MD.kit.name);
}

void handleGuiSysex() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    loadedKit = false;
    MDSysexListener.getCurrentKit(onCurrentKitCallback);
  }
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

void handleGui() {
  handleGuiSysex();
  handleGuiPages();
}
