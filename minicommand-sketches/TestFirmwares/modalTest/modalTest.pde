RangeEncoder testEnc(0, 127, "TST");
EncoderPage page(&testEnc);

bool handleEvent(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
    GUI.setLine(GUI.LINE1);
    GUI.put_string_fill("WORLD");
    return true;
  }
  if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
    GUI.setLine(GUI.LINE1);
    GUI.put_string_fill("HELO");
    return true;
  }
  if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
    int i = showModalGui("PRESS 1 OR 2", "", Buttons.BUTTON1_MASK | Buttons.BUTTON2_MASK);
    GUI.setLine(GUI.LINE1);
    GUI.flash_string_fill("MODAL");
    GUI.setLine(GUI.LINE2);
    GUI.flash_put_value(0, i);
  }
  if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
    char *name = getNameModalGui("GIMME NAME", "DEFAULT");
    GUI.flash_strings_fill("NAME:", name);
  }
  return false;
}

void setup() {
  GUI.setPage(&page);
  GUI.put_string_fill("HELO");
  GUI.addEventHandler(handleEvent);
}

void loop() {
}
