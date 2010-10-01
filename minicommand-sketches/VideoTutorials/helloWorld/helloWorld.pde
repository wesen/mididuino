void setup() {
  GUI.setLine(GUI.LINE1);
  GUI.put_string_fill("HELLO WORLD");
}

void handleEvent(gui_event_t *evt) {
  GUI.setLine(GUI.LINE2);
  for (uint8_t i = Buttons.BUTTON1; i <= Buttons.BUTTON4; i++) {
    if (EVENT_PRESSED(evt, i)) {
      GUI.flash_printf_fill("%b PRESSED", i);
    } 
    else if (EVENT_RELEASED(evt, i)) {
      GUI.flash_printf_fill("%b RELEASED", i);
    }
  }
}

