RangeEncoder param1(0, 127, "P1");
RangeEncoder param2(0, 127, "P2");
EncoderPage page(&param1, &param2);

void setup() {
  GUI.setPage(&page);
}

void loop() {
  if (param1.hasChanged()) {
    MidiUart.sendCC(1, param1.getValue());
  }
  if (param2.hasChanged()) {
    MidiUart.sendCC(2, param2.getValue());
  }
}

