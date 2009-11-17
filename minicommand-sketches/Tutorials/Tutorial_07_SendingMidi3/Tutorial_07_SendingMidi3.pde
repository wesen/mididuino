CCEncoder param1(1, 0, "P1");
CCEncoder param2(2, 0, "P2");
EncoderPage page(&param1, &param2);

void setup() {
  GUI.setPage(&page);
}

bool handleEvent(gui_event_t *evt) {
  for (uint8_t i = Buttons.BUTTON1; i <= Buttons.BUTTON4; i++) {
    uint8_t buttonIdx = i - Buttons.BUTTON1;
    if (EVENT_PRESSED(evt, i)) {
      MidiUart.sendNoteOn(1 + buttonIdx, 100);
      return true;
    } 
    else if (EVENT_RELEASED(evt, i)) {
      MidiUart.sendNoteOff(1 + buttonIdx);
      return true;
    }
  }

  return false;
}


