RangeEncoder param1(0, 127, "P1");
RangeEncoder param2(0, 127, "P2");
RangeEncoder param3(0, 127, "P3");
RangeEncoder param4(0, 127, "P4");

EncoderPage page(&param1, &param2);
EncoderPage page2(&param3, &param4, &param2);

void setup() {
  GUI.setPage(&page);
}

bool handleEvent(gui_event_t *evt) {
  if (EVENT_PRESSED(evt, Buttons.BUTTON1)) {
    GUI.setPage(&page);
    return true;
  } else if (EVENT_PRESSED(evt, Buttons.BUTTON2)) {
    GUI.setPage(&page2);
    return true;
  }
  
  return false;
}

