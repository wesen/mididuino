void setup() {
  GUI.put_string_fill("HELO DUDE");
  GUI.addEventHandler(handleEvent);
}

extern void (*jump_to_boot)();

bool handleEvent(gui_event_t *evt) {
  if (EVENT_PRESSED(evt, Buttons.BUTTON1)) {
    start_bootloader();
  } 
  if (EVENT_PRESSED(evt, Buttons.BUTTON2)) {
    GUI.setLine(GUI.LINE2);
    GUI.put_value16(0, (uint16_t)jump_to_boot);
    
  }
}

void loop() {
}
