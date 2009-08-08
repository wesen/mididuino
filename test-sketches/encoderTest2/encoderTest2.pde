RangeEncoder enc1(0, 127, "TST");
RangeEncoder enc2(0, 127, "TST");
RangeEncoder enc3(0, 127, "TST");
RangeEncoder enc4(0, 127, "TST");

void onNoteCallback(uint8_t *msg) {
  GUI.setLine(GUI.LINE1);
  GUI.flash_string_fill("NOTE RECV");
}

EncoderPage page(&enc1, &enc2, &enc3, &enc3);
void setup() {
  LCD.line1("LED BLINK");
  for (int i = 0; i < 5; i++) {
    setLed();
    clearLed2();
    delay(200);
    setLed2();
    clearLed();
    delay(200);
  }
  clearLed2();

  page.encoders[0] = &enc1;
  page.encoders[1] = &enc2;
  page.encoders[2] = &enc3;
  page.encoders[3] = &enc4;
  GUI.setPage(&page);
  Midi2.setOnNoteOnCallback(onNoteCallback);
}

void loop() {
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    GUI.flash_string_fill("B1");
    setLed();
  } else if (BUTTON_RELEASED(Buttons.BUTTON1)) {
    clearLed();
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    GUI.flash_string_fill("B2");
    setLed2();
  } else if (BUTTON_RELEASED(Buttons.BUTTON2)) {
    clearLed2();
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON3)) {
    GUI.flash_string_fill("B3");
    OCR3A = 100;
  } else if (BUTTON_RELEASED(Buttons.BUTTON3)) {
    OCR3A = 200;
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON4)) {
    GUI.flash_string_fill("B4");
  }
  
 if (BUTTON_PRESSED(Buttons.ENCODER1)) {
   GUI.setLine(GUI.LINE2);
   GUI.flash_string_fill("E1");
 }
  if (BUTTON_PRESSED(Buttons.ENCODER2)) {
    GUI.flash_string_fill("E2");
    MidiUart.sendNoteOn(100, 100);
  }
    
  if (BUTTON_PRESSED(Buttons.ENCODER3))
    GUI.flash_string_fill("E3");
  if (BUTTON_PRESSED(Buttons.ENCODER4))
    GUI.flash_string_fill("E4");
}
