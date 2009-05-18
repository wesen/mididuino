RangeEncoder encoder(0, 127,
                     "BLA");
RangeEncoder encoder2(0, 127,
                      "FOO");
EncoderPage page(&encoder);
EncoderPage page2(&encoder2, &encoder);

void setup() {
  GUI.setPage(&page);
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

void handleGui() { 
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    GUI.setPage(&page);
  } else if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    GUI.setPage(&page2);
  }
}
