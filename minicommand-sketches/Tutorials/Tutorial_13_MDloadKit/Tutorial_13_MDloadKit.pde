#include <MD.h>

void onCurrentKitCallback() {
  GUI.setLine(GUI.LINE2);
  GUI.put_string_fill(MD.name);
}

void setup() {
  GUI.setLine(GUI.LINE1);
  GUI.put_string_fill("PRESS BUTTON1");
}

void loop() {
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    MDSysex.getCurrentKit(onCurrentKitCallback);
  }
}
