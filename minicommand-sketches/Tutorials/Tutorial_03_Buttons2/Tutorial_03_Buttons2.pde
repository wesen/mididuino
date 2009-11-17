void setup() {
  GUI.setLine(GUI.LINE1);
  GUI.put_string_fill("BUTTONS TEST 2");
}

bool handleEvent(gui_event_t *evt) {
  GUI.setLine(GUI.LINE2);
  if (EVENT_PRESSED(evt, Buttons.BUTTON1)) {
    GUI.flash_string_fill("PRESSED");
    return true;
  }
  if (EVENT_RELEASED(evt, Buttons.BUTTON1)) {
    GUI.flash_string_fill("RELEASED");
    return true;
  }
  
  return false;
}

