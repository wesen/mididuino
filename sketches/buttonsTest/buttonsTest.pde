#include <GUI.h>

void setup() {
}

void loop() {
  GUI.update();
}

void handleGui() {
  if (BUTTON_CLICKED(Buttons.BUTTON1)) {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("CLICKED"));
  }
  if (BUTTON_DOUBLE_CLICKED(Buttons.BUTTON1)) {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("DBL CLICKED"));
  }
  if (BUTTON_LONG_CLICKED(Buttons.BUTTON1)) {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("LONG CLICKED"));
  }
}
