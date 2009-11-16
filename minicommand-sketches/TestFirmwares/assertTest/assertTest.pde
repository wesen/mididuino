void setup() {
  GUI.addEventHandler(handleEvent);
}

void loop() {
}

uint8_t cnt = 0;
bool handleEvent(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
    cnt++;
    MidiUart.printfString("HELO DUDE %b", cnt);
    GUI.setLine(GUI.LINE1);
    GUI.put_value16(0, MidiUart.rxRb.size());
  }
}    

