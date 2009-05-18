RangeEncoder encoder(0, 127, "BLA");
EncoderPage page(&encoder);

void setup() {
  GUI.setPage(&page);
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

void handleGui() { 
}
