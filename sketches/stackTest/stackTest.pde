Stack<uint8_t, 8> intStack;
RangeEncoder enc(0, 31, "NUM");
EncoderPage page;

void setup() {
  page.encoders[0] = &enc;
  GUI.setPage(&page);
}

void loop() {
  GUI.updatePage();
  
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    if(intStack.push(enc.getValue())) {
      GUI.flash_p_string_clear(PSTR("PUSH"));
    } else {
      GUI.flash_p_string_clear(PSTR("PUSH ERROR"));
    }
  }

  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    uint8_t val;
    if (intStack.pop(&val)) {
      enc.setValue(val);
      page.display(true);
      GUI.flash_p_string_clear(PSTR("POP"));
    } else {
      GUI.flash_p_string_clear(PSTR("POP ERROR"));
    }
  }
}
