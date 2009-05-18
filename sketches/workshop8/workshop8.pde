RangeEncoder ccEncoder(0, 127, "CC1");
EncoderPage page(&ccEncoder);

void setup() {
  GUI.setPage(&page);
}

void loop() {
  GUI.updatePage();

  if (ccEncoder.hasChanged()) {
    MidiUart.sendCC(1, ccEncoder.getValue());
  }  
  GUI.update();
}

void handleGui() {
}
